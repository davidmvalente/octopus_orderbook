#include <string.h>
#include <iostream>


//#include "engine.h"
#include "limits.h"
#include "order_book.h"
#include "constants.h"


namespace executor {

void execution(t_execution exec) {
  std::cerr << "executor::execution - Filling " << std::string(exec.symbol.data(), OB::kFieldLength) 
    << " Qty. " << std::to_string(exec.size)
    << " @ " << std::to_string(exec.price) << std::endl;
  std::cerr << "@ executor::execution - T, " << std::to_string(exec.buyside) << DELIMITER << std::to_string(exec.buyside_uid) << DELIMITER
    << std::to_string(exec.sellside) << DELIMITER << std::to_string(exec.sellside_uid) << DELIMITER
    << std::to_string(exec.price) << DELIMITER << std::to_string(exec.size)
    << std::endl;
  std::cout << "T, " << std::to_string(exec.buyside) << DELIMITER << std::to_string(exec.buyside_uid) << DELIMITER
    << std::to_string(exec.sellside) << DELIMITER << std::to_string(exec.sellside_uid) << DELIMITER
    << std::to_string(exec.price) << DELIMITER << std::to_string(exec.size)
    << std::endl;
};


/* Report trade execution */
void executeTrade(const Field& symbol, const t_size buyTrader, t_size buyerOrder,
                  const t_size sellTrader, t_size sellerOrder, t_price tradePrice,
                  t_size tradeSize) {
  t_execution exec;

  if (tradeSize == 0) /* Skip orders that have been cancelled */
    return;

  exec.symbol = symbol;

  exec.price = tradePrice;
  exec.size = tradeSize;

  exec.buyside = buyTrader;
  exec.buyside_uid = buyerOrder;
  exec.sellside = sellTrader;
  exec.sellside_uid = sellerOrder;
  execution(exec); /* Report the trade */
}

void publishNewTopAsk(t_price newAskMin, t_size newAskSize) {
    std::cerr << "@ OrderBook - askMin B, S, " << std::to_string(newAskMin) << DELIMITER << std::to_string(newAskSize) << std::endl;  
    std::cout << "B, S, " << std::to_string(newAskMin) << DELIMITER << std::to_string(newAskSize) << std::endl;
}

void publishNewTopBid(t_price newBidMax, t_size newBidSize) {
    std::cerr << "@ OrderBook - askMin B, S, " << std::to_string(newBidMax) << DELIMITER << std::to_string(newBidSize) << std::endl;
    std::cout << "B, B, " << std::to_string(newBidMax) << DELIMITER << std::to_string(newBidSize) << std::endl;
}

}

OrderBook& OrderBook::get() {
  static OrderBook ob;
  return ob;
}

void OrderBook::initialize() {
  /* Initialize the price point array */
  std::cerr << "OrderBook::initialize()" << std::endl;
  pricePoints.resize(OB::kMaxPrice);
  for (auto& el : pricePoints) {
    el.clear();
  }

  arenaBookEntries.resize(OB::kMaxNumOrders);

  curOrderID = 0;
  askMin = OB::kMaxPrice;
  bidMax = OB::kMinPrice;
}

void OrderBook::shutdown() {}

t_orderid OrderBook::market(t_order& order) {
   if (order.side == 0) {
     order.price = askMin;
     return OrderBook::limit(order);
   } else {
     order.price = bidMax;
     return OrderBook::limit(order);
   }
}

t_orderid OrderBook::limit(t_order& order) {
  t_price price = order.price;
  t_size orderSize = order.size;

  if (order.side == 0) {/* Buy order */
    /* Look for outstanding sell orders that cross with the incoming order */
    if (price >= askMin) {
        std::cerr << "OrderBook::limit - Bid:" << std::to_string(price) << " askMin:" << std::to_string(askMin) << std::endl;
      auto ppEntry = pricePoints.begin() + askMin;
      do {
        auto bookEntry = ppEntry->begin();
        while (bookEntry != ppEntry->end()) {
          if (bookEntry->size < orderSize) {
            /* Partial fil, our entry is not sufficient */
            executor::executeTrade(order.symbol, order.trader, order.uid, 
                      bookEntry->trader, bookEntry->uid, price, bookEntry->size);
            orderSize -= bookEntry->size;
             ++bookEntry;
          } else {
            executor::executeTrade(order.symbol, order.trader, order.uid, 
                      bookEntry->trader, bookEntry->uid, price,
                         orderSize);
            if (bookEntry->size > orderSize) {
              bookEntry->size -= orderSize;
            } else {
              ++bookEntry;
            }

            ppEntry->erase(ppEntry->begin(), bookEntry);
            executor::publishNewTopAsk(askMin, bookEntry->size); // TODO: This does not reflect the correct TOB price after a trade

            while (ppEntry->begin() != bookEntry) {
              ppEntry->pop_front();
            }

            return ++curOrderID;
          }
        }
        /* We have exhausted all orders at the askMin price point. Move on to
           the next price level. */
        ppEntry->clear();
        // Publish top of book change
        ppEntry++;
        askMin++;
        executor::publishNewTopBid(askMin, bookEntry->size);
      } while (price >= askMin);
    }

    auto entry = arenaBookEntries.begin() + (++curOrderID);
    entry->size = orderSize;
    entry->trader = order.trader;
    entry->uid = order.uid; // add user order id
    pricePoints[price].push_back(*entry);
    if (bidMax < price) {
      // top of book change
      bidMax = price;
      executor::publishNewTopBid(bidMax, entry->size);
    }
    return curOrderID;

  } else {/* Sell order */
    /* Look for outstanding Buy orders that cross with the incoming order */
    if (price <= bidMax) {
        std::cerr << "OrderBook::limit - Ask:" << std::to_string(price) << ", bidMax:" << std::to_string(bidMax) << std::endl;
      auto ppEntry = pricePoints.begin() + bidMax;
      do {
      auto bookEntry = ppEntry->begin();
        while (bookEntry != ppEntry->end()) {
          if (bookEntry->size < orderSize) {
            executor::executeTrade(order.symbol, bookEntry->trader, bookEntry->uid,
                         order.trader, order.uid, price,
                         bookEntry->size);
            orderSize -= bookEntry->size;
            ++bookEntry;
          } else {
            executor::executeTrade(order.symbol, bookEntry->trader, bookEntry->uid,
                         order.trader, order.uid, price,
                         orderSize);
            if (bookEntry->size > orderSize){
              bookEntry->size -= orderSize;
            } else {
              ++bookEntry;
            }

            executor::publishNewTopAsk(bidMax, bookEntry->size);
            while (ppEntry->begin() != bookEntry) {
              ppEntry->pop_front();
            }

            std::cerr << "next order" << std::endl;
            return ++curOrderID;
          }
        }

        /* We have exhausted all orders at the bidMax price point. Move on to
           the next price level. */
        ppEntry->clear();
        ppEntry--;
        bidMax--;
        executor::publishNewTopAsk(bidMax, bookEntry->size);

      } while (price <= bidMax);
    }

    auto entry = arenaBookEntries.begin() + (++curOrderID);
    entry->size = orderSize;
    entry->trader = order.trader;
    entry->uid = order.uid; // add user order id
    pricePoints[price].push_back(*entry);
    if (askMin > price) {
      // top of book change
      askMin = price;
      executor::publishNewTopAsk(askMin,entry->size);
    }
    return curOrderID;
  }
}

void OrderBook::save(t_order order, t_orderid orderid) {
  customerOrders[{order.trader, order.uid}] = orderid;
}

void OrderBook::cancel(t_orderid orderid) {
  arenaBookEntries[orderid].size = 0;
}

void OrderBook::cancel_order(t_size trader, t_size uid) {
  t_orderid orderid = customerOrders[{trader, uid}];
  std::cerr << "OrderBook cancel_order: id=" << std::to_string(orderid) 
  << " trader=" << std::to_string(trader) << " uid=" << std::to_string(uid)
  << std::endl;
  arenaBookEntries[orderid].size = 0;
}


