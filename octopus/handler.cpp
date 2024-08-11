#include "handler.h"

/**
 * @brief Parse CSV order
 * 
 * @param order_message 
 * @param message_len 
 * @return t_order 
 */
t_order handler::utils::parse_order(const char *order_message, int message_len){
    
    /* Copy buffer and stream through */
    t_order new_order;
    std::string input_msg(order_message, message_len);
    std::stringstream ss(input_msg);
    std::vector<std::string> token_holder;
    std::string token;

    while(std::getline(ss, token, ',')) {
        token_holder.push_back(token.substr(1)); /* lstrip first blank */
    }

    /* basic data validation */
    char _side = token_holder[messageToken::side].data()[0];
    if (_side == BUY_ORDER) {
        new_order.side = false; /* is_ask */
    } else if (_side == SELL_ORDER) {
        new_order.side = true;
    } else {
        std::string msg("Invalid order type: ");
        throw std::runtime_error(msg + _side);
    };

    /* very dirty and unsafe unpacking */
    new_order.trader = std::stoi(token_holder[messageToken::trader]);
    
    for (int i = 0; i < OB::kFieldLength; i++) { 
        new_order.symbol[i] = token_holder[messageToken::symbol].c_str()[i];
    };

    new_order.price = std::stoi(token_holder[messageToken::price]);;
    new_order.size = std::stoi(token_holder[messageToken::size]);
    new_order.uid = std::stoi(token_holder[messageToken::uid]); 

    return new_order;
}; 

t_order handler::utils::parse_cancel(const char *order_message, int message_len){
    
    /* Copy buffer and stream through */
    t_order _order;
    std::string input_msg(order_message, message_len);
    std::stringstream ss(input_msg);
    std::vector<std::string> token_holder;
    std::string token;

    while(std::getline(ss, token, ',')) {
        token_holder.push_back(token.substr(1)); /* lstrip first blank */
    }

    /* very dirty and unsafe unpacking */
    _order.trader = std::stoi(token_holder[cancelToken::traderCancel]);
    _order.uid = std::stoi(token_holder[cancelToken::uidCancel]); 

    return _order;
}; 

/**
 * @brief Trace order
 * 
 * @param new_order 
 */
void handler::utils::log_order(t_order new_order){
    std::cerr << "handler::process - new order  side:" << std::to_string(new_order.side) 
    <<  " price:" << std::to_string(new_order.price) 
    <<  " size:"  << std::to_string(new_order.size)
    <<  std::endl;
};

/**
 * @brief Send order acknowledgement
 * 
 * @param order 
 */
void handler::acknowledge(t_order order){
        std::cout << "A, " << std::to_string(order.trader) << DELIMITER << std::to_string(order.uid) << std::endl;
    };

/**
 * @brief Process buffer
 * 
 * @param recv_buffer received buffer
 * @param transferred buffer len
 * @return int 
 */
int handler::process(const char *recv_buffer, int transferred){
    std::cerr << "handler::process - " << "event_type: " << recv_buffer[0] << std::endl;

    char event = recv_buffer[messageToken::event];

    // switch on action type
    if (event == NEWORDER) {
            t_order new_order = utils::parse_order(recv_buffer, transferred);
            utils::log_order(new_order);
            /* Here there is a problem: we acknowledge the order before
              having the actual new_order_id */ 
            handler::acknowledge(new_order);
            if (new_order.price) {
                std::cerr << "process::limit" << std::endl;
                t_orderid new_order_id = handler::limit(new_order);
                handler::save(new_order, new_order_id);
            } else {
                // market order
                std::cerr << "process::market" << std::endl;
                t_orderid new_order_id = handler::market(new_order);
                handler::save(new_order, new_order_id);
            }
    } else if (event == CANCELORDER) {
        t_order to_cancel = utils::parse_cancel(recv_buffer, transferred);
        handler::cancel_order(to_cancel.trader, to_cancel.uid);
        // What format to publish cancellation? 
        // Should we update Top of Book?

    } else if (event == FLUSHBOOK) {
        handler::destroy();
        handler::init();
    } else {
        std::string msg("Invalid event_type: ");
        throw std::runtime_error(msg + event);
    }
    return 0;
};
/**
 * @brief Cancel order wrapper
 * 
 * @param orderid 
 */
void handler::cancel(t_orderid orderid) { OrderBook::get().cancel(orderid); }
void handler::cancel_order(t_size trader, t_size uid) { OrderBook::get().cancel_order(trader, uid); }

void handler::save(t_order order, t_orderid orderid) { OrderBook::get().save(order, orderid); }

/**
 * @brief Limit order wrapper
 * 
 * @param order 
 * @return t_orderid 
 */
t_orderid handler::limit(t_order order) { return OrderBook::get().limit(order); }

/**
 * @brief Execute market order
 * 
 * @param order 
 * @return t_orderid 
 */
t_orderid handler::market(t_order order) { return OrderBook::get().market(order); }


/**
 * @brief Shutdown order book (Not Implemented)
 * 
 */
void handler::destroy() { OrderBook::get().shutdown(); }

/**
 * @brief Initialize order book
 * 
 */
void handler::init() { OrderBook::get().initialize(); }
