#ifndef _handler_h
#define _handler_h

#include <ctime>
#include <string>
#include <sstream>
#include <iostream>

#include "order_book.h"

namespace handler{

    namespace utils {
        t_order parse_order(const char *order_message, int message_len);
        t_order parse_cancel(const char *order_message, int message_len);
        void log_order(t_order order);
    };

    void acknowledge(t_order order);

    int process(const char *recv_buffer, int transferred);
    
    /* Initialize or flush order book */
    void init();

    void destroy();

    /* Process an incoming limit order */
    t_orderid limit(t_order order);
    t_orderid market(t_order order);

    /* Cancel an outstanding order */
    void cancel(t_orderid orderid);
    void cancel_order(t_size trader, t_size uid);

    /* Save order id in map */
    void save(t_order order, t_orderid orderid);
};
#else
#endif