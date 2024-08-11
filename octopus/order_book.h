#include <vector>
#include <map>

#include <boost/noncopyable.hpp>
#include <boost/intrusive/slist.hpp>
#include <boost/intrusive/list.hpp>

#include "types.h"

class OrderBook : public boost::noncopyable {
 public:

  static OrderBook& get();

  void initialize();

  void shutdown();

  t_orderid limit(t_order& order);
  t_orderid market(t_order& order);

  void save(t_order order, t_orderid orderid);

  void cancel(t_orderid orderid);

  void cancel_order(t_size trade, t_size uid);

 private:
  OrderBook() {}

  struct OrderBookEntry : public boost::intrusive::slist_base_hook<> {
    t_size size{0}; /* Order size * */
    t_size trader{};
    t_size uid; /* add user */
  };

  // pricePoint: describes a single price point in the limit order book.
  typedef boost::intrusive::slist<
      OrderBookEntry, boost::intrusive::cache_last<true> > PricePoint;

  // An array of pricePoint structures representing the entire limit order book
  std::vector<OrderBookEntry> arenaBookEntries;
  std::vector<PricePoint> pricePoints;

  // Monotonically-increasing orderID
  t_orderid curOrderID;
  // Minimum Ask price
  t_price askMin;
  // Maximum Bid price
  t_price bidMax;

  // Store customerOrderId vs internalOrderId mapping
  std::map<t_externalid, t_orderid> customerOrders;

};
