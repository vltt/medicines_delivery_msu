#pragma once

#ifndef EMULATOR_H
#define EMULATOR_H

#include <QObject>

#include <optional>
#include <unordered_set>
#include <vector>

#include "customer.h"
#include "medicine.h"
#include "statistics.h"
#include "warehouse.h"

class MainWindow;

namespace emulator {

class Emulator {
public:
    Emulator(MainWindow* window);

    void AllDayRun();
    void NextDayRun();
    void Init(int extra_charge_coef,
                   int medicine_amount,
                   int usual_customers_amount,
                   int regular_customers_amount,
                   int with_card_customers_amount);

    bool IsEndOfEmulation() const;
    const std::unordered_map<medicine::MedicineName, medicine::Medicine>& GetAllMedicines() const;


private:
    int days_of_emalution_ = 25;
    int cur_day_ = 0;
    double extra_charge_coef_ = 0;
    MainWindow* window_ = nullptr;
    std::unordered_set<medicine::MedicineName> on_sale_medicines_;
    warehouse::Warehouse warehouse_;
    statistics::Statistics total_statistics_;
    std::vector<customer::Customer> customers_;

    void EmulateNextDay();

    int CalculateСouriersNumber(int orders_number) const;
    void MakeOrder(const customer::Customer& customer, const warehouse::OrdersRequest& order_request);
    void MakeOrders();
    void MakeRegularOrders();

    int GenerateNextDeliveryDate(int cur_day) const;
    int GenerateOrdersAmount() const;

    customer::Customer RandomChooseCustomer() const;
    medicine::MedicineName RandomChooseMedicineName() const;
    warehouse::OrdersRequest GenerateOrdersRequest() const;
};

} // emulator

#endif // EMULATOR_H
