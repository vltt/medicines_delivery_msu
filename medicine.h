#pragma once

#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
#include <optional>

namespace medicine {

using MedicineName = std::string;

class Medicine {
public:
    Medicine() = default;

    Medicine(const MedicineName& uniq_name, int default_amount, int production_date, int shelf_life, int price);

    MedicineName uniq_name;
    int default_amount;
    int amount;
    int production_date;
    int shelf_life;
    double price;
    bool on_sale = false;
    std::optional<int> date_of_new_delivery;

    int GetExpirationDate() const;
};

} // medicine

#endif // MEDICINE_H
