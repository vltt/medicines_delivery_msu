#include "medicine.h"

#include <iostream>

namespace medicine {

Medicine::Medicine(
        const MedicineName& uniq_name, int default_amount, int production_date, int shelf_life, int price
):
        uniq_name(uniq_name), default_amount(default_amount),
        amount(default_amount), production_date(production_date),
        shelf_life(shelf_life), price(price) {}

int Medicine::GetExpirationDate() const{
    return production_date + shelf_life;
}

} // medicine
