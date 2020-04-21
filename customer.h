#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <unordered_map>

#include <medicine.h>

namespace customer {

struct RegularCutomerInfo {
    const int order_period;
    const std::unordered_map<medicine::MedicineName, int> medicines;
    int prev_order_date;
};

struct DiscountCard {
    const int discount_card_number;
    const double discount_value;
    std::optional<RegularCutomerInfo> regular_customer_info;
};

struct Customer {
    const std::string customer_name;
    const std::string phone_number;
    std::optional<DiscountCard> discount_card;
};

} // namespace customer

#endif // CUSTOMER_H
