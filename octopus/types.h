#pragma once

#include "limits.h"
#include "constants.h"
#include <boost/serialization/strong_typedef.hpp>
#include <array>

/* Ticker Id */
typedef std::array<char, OB::kFieldLength> Field;

typedef unsigned long t_orderid;

/* Price
   0-65536 interpreted as divided by 100
   eg the range is 000.00-655.36
   eg the price 123.45 = 12345
   eg the price 23.45 = 2345
   eg the price 23.4 = 2340 */

/* Order Size */
typedef unsigned long t_size;

/* Side
   Ask=1, Bid=0 */
typedef int t_side;
inline int is_ask(t_side side) { return side; }

/* Limit Order */
typedef struct {
  Field symbol;
  t_side side;
  t_price price;
  t_size size;
  t_size trader;
  t_size uid;
} t_order;

/* Execution Report */
typedef struct {
   Field symbol;
  t_price price;
  t_size size;
  t_size buyside;
  t_size buyside_uid;
  t_size sellside;
  t_size sellside_uid;
} t_execution;

/* Map external <trader_id, uid> to internal t_orderid; */
typedef std::array<t_size, 2> t_externalid;