#pragma once

#include "types.h"

#include <stdint.h>
#include <limits>

// Define more stuff here.

#define NEWORDER    'N'
#define FLUSHBOOK   'F'
#define CANCELORDER 'C' 
#define DELIMITER   ", "
#define SELL_ORDER  'S'
#define BUY_ORDER   'B'

/* Comma separated values in new order message */
enum messageToken { 
   event = 0, trader, symbol, price, size, side, uid 
};

enum cancelToken { 
   eventCancel = 0, traderCancel, uidCancel
};

typedef unsigned short t_price;

namespace OB {

constexpr t_price kMaxPrice = std::numeric_limits<t_price>::max();

constexpr int kMaxNumOrders = 101000;

constexpr t_price kMinPrice = 1;

constexpr uint32_t kMaxLiveOrders = std::numeric_limits<t_price>::max();

constexpr size_t kFieldLength = 4;

}
