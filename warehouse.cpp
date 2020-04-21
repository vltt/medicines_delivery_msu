#include "warehouse.h"

#include <algorithm>
#include <iostream>

namespace warehouse {

Warehouse::Warehouse(std::vector<medicine::Medicine> medicines, int expiration_date_delta):
    expiration_date_delta_(expiration_date_delta) {
    for(auto& medicine : medicines) {
        medicines_[medicine.uniq_name] = std::move(medicine);
    }
}

void Warehouse::EndDaysChecker(int cur_day) {
    for(auto& [medicine_name, medicine] : medicines_) {
        if(!InStock(medicine)) {
            continue;
        }
        if(medicine.GetExpirationDate() == cur_day) {
            expiration_losses_.amount += medicine.amount;
            expiration_losses_.money += medicine.amount * medicine.price;
            need_new_supply_medicines_.insert(medicine_name);
        } else if(medicine.GetExpirationDate() - expiration_date_delta_ <= cur_day) {
            medicine.on_sale = true;
        }
    }
}

void Warehouse::Refil(int cur_day) {
    for(auto& [_, medicine] : medicines_) {
        if(!InStock(medicine) && *medicine.date_of_new_delivery == cur_day) {
            medicine.amount = medicine.default_amount;
            medicine.production_date = cur_day;
            medicine.on_sale = false;
            medicine.date_of_new_delivery = std::nullopt;
        }
    }
}

void Warehouse::OrderSupply(int next_delivery_date) {
    for(const auto& medicine_name : need_new_supply_medicines_) {
        auto& medicine = medicines_[medicine_name];
        medicine.date_of_new_delivery = next_delivery_date;
        medicine.amount = 0;
    }
    need_new_supply_medicines_.clear();
}

std::unordered_set<medicine::MedicineName> Warehouse::GetOnSaleMedicines() const {
    std::unordered_set<medicine::MedicineName> on_sale_medicines;

    for (const auto& [medicnie_name, medicine] : medicines_) {
        if (medicine.on_sale) {
            on_sale_medicines.insert(medicnie_name);
        }
    }

    return on_sale_medicines;
}

std::unordered_set<medicine::MedicineName> Warehouse::GetOutOfMedicines() const {
    std::unordered_set<medicine::MedicineName> out_of_medicines;

    for (const auto& [medicnie_name, medicine] : medicines_) {
        if (!InStock(medicine)) {
            out_of_medicines.insert(medicnie_name);
        }
    }

    return out_of_medicines;
}

OrderResponse Warehouse::ExecuteOrder(const OrdersRequest& cart) {
    double total_price = 0;
    double lost_profit = 0;
    bool done = false;
    const auto& on_sale_medicines = GetOnSaleMedicines();
    for(const auto& item : cart) {
        auto& medicine = medicines_[item.medicine_name];
        if (medicine.date_of_new_delivery.has_value()) {
            continue;
        }
        int taken_amount = std::min(medicine.amount, item.amount);
        int lost_amount = item.amount - taken_amount;
        medicine.amount -= taken_amount;
        if(medicine.amount == 0) {
            need_new_supply_medicines_.insert(item.medicine_name);
        }
        lost_profit += medicine.price * lost_amount;
        if(taken_amount > 0) {
            done = true;
            if (on_sale_medicines.find(item.medicine_name) != on_sale_medicines.end())  {
                total_price += taken_amount * medicine.price / 2;
            } else {
                total_price += taken_amount * medicine.price;
            }
        }
    }

    OrderResponse response;
    response.total_price = total_price;
    response.done = done;
    response.lost_profit = lost_profit;
    return response;
}

int Warehouse::UniqMedicinesAmounts() const {
    return medicines_.size();
}

const std::unordered_map<medicine::MedicineName, medicine::Medicine>& Warehouse::Medicines() const {
    return medicines_;
}

ExpirationLosses Warehouse::GetExpirationLosses() const {
    return expiration_losses_;
}

bool Warehouse::InStock(const medicine::Medicine &medicine) const {
    return !medicine.date_of_new_delivery.has_value();
}

} // warehouse
