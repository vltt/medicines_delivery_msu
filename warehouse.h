#pragma once

#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "medicine.h"

namespace warehouse {

struct Order {
    medicine::MedicineName medicine_name;
    int amount;
};

struct OrderResponse {
    double total_price;
    double lost_profit;
    bool done;
};

struct ExpirationLosses {
    int amount = 0;
    double money = 0;
};

using OrdersRequest = std::vector<Order>;

class Warehouse {
public:
    Warehouse() = default;

    Warehouse(std::vector<medicine::Medicine> medicines, int expiration_date_delta);

    void EndDaysChecker(int cur_day);
    void Refil(int cur_day);
    void OrderSupply(int next_delivery_date);
    std::unordered_set<medicine::MedicineName> GetOnSaleMedicines() const;
    std::unordered_set<medicine::MedicineName> GetOutOfMedicines() const;
    OrderResponse ExecuteOrder(const OrdersRequest& cart);
    int UniqMedicinesAmounts() const;
    const std::unordered_map<medicine::MedicineName, medicine::Medicine>& Medicines() const;
    ExpirationLosses GetExpirationLosses() const ;

private:
    bool InStock(const medicine::Medicine &medicine) const;

    std::unordered_map<medicine::MedicineName, medicine::Medicine> medicines_;
    std::unordered_set<medicine::MedicineName> need_new_supply_medicines_;

    int expiration_date_delta_ = 0;
    struct ExpirationLosses expiration_losses_;
};

} // warehouse

#endif // WAREHOUSE_H
