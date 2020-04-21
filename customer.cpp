#include "customer.h"

namespace customer {

Customer::Customer(const std::string& customer_name, int discount_card_number,
         double discount_value, const std::optional<RegularCutomerInfo>& regular_customer_info):
    customer_name(customer_name), discount_card_number(discount_card_number),
    discount_value(discount_value), regular_customer_info(regular_customer_info) {}

} // customer
