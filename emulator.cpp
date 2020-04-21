#include "emulator.h"

#include <unistd.h>

#include <iostream>
#include <random>

#include "mainwindow.h"


namespace emulator {

namespace  {

std::default_random_engine kRandomGenerator;

int EXPIRATION_DATE_DELTA = 2;

int GenerateDefaultAmount() {
    static std::uniform_int_distribution<int> distributoin(10, 20);
    return distributoin(kRandomGenerator);
}

int GenerateShelfLife() {
    static std::uniform_int_distribution<int> distributoin(4, 8);
    return distributoin(kRandomGenerator);
}

int GeneratePrice() {
    double mean = 150;
    double stddev = 20;
    double min = 10;
    double max = 500;

    static std::normal_distribution<double> distribution(mean, stddev);

    while (true) {
        double number = distribution(kRandomGenerator);
        if (number >= min && number <= max) {
            return number;
        }
    }
}

int GenerateOrderPeriod() {
    static std::uniform_int_distribution<int> distributoin(4, 8);
    return distributoin(kRandomGenerator);
}

std::vector<medicine::Medicine> InitMedicines(int medicine_amount) {
    static const std::vector<medicine::MedicineName>
            unique_medicines_types = {
        "Цитрамон",
        "Уголь активированный",
        "Парацетамол",
        "Хлоргексдин",
        "Корвалол",
        "Перекись водорода",
        "Аспирин",
        "Фосфалюгель",
        "Мезим",
        "Стрепсилс",
        "Нурофен"
    };
    const int unique_medicines_types_amount = unique_medicines_types.size();

    std::vector<medicine::Medicine> medicines;
    medicines.reserve(medicine_amount);

    for(int i = 0; i < medicine_amount; i++) {
        auto medicine = medicine::Medicine(
                unique_medicines_types[i / unique_medicines_types_amount] + " " + std::to_string(i),
                GenerateDefaultAmount(),
                0, GenerateShelfLife(), GeneratePrice());
        medicines.push_back(std::move(medicine));
    }
    return medicines;
}

std::unordered_map<medicine::MedicineName, int>
    ChooseMedicines(std::vector<medicine::Medicine> medicines) {

    std::shuffle(medicines.begin(), medicines.end(), kRandomGenerator);

    std::uniform_int_distribution<int> distributoin(1, std::max(1, int((medicines.size() + 9) / 10)));
    int n = distributoin(kRandomGenerator);

    std::unordered_map<medicine::MedicineName, int> result_medicines;
    for(int i = 0; i < n; i ++) {
        const auto& medicne = medicines[i];
        std::uniform_int_distribution<int> distributoin(1, std::max(1, int(medicne.default_amount * 0.2)));
        result_medicines[medicne.uniq_name] = distributoin(kRandomGenerator);
    }

    return result_medicines;
}

std::vector<customer::Customer> InitCustomers(const std::vector<medicine::Medicine>& medicines,
                                               int usual_customers_amount,
                                               int regular_customers_amount,
                                               int with_card_customers_amount) {
    std::vector<customer::Customer>  customers;
    customers.reserve(usual_customers_amount + regular_customers_amount + with_card_customers_amount);

    for(int i = 0; i < regular_customers_amount; i++) {
        int discount_card_number =  123;
        double discount_value = 0.05;
        int order_period = GenerateOrderPeriod();
        int prev_order_date = 0;
        auto medicines_list = ChooseMedicines(medicines);
        customer::RegularCutomerInfo regular_customer = {order_period, std::move(medicines_list), prev_order_date};
        customer::DiscountCard card = {discount_card_number, discount_value, regular_customer};

        customer::Customer customer = {
                    "RegularCustomer_" + std::to_string(i),
                    "+79015071710", card};

        customers.push_back(std::move(customer));
    }

    for(int i = 0; i < with_card_customers_amount; i++) {
        int discount_card_number =  123;
        double discount_value = 0.03;
        customer::DiscountCard card = {discount_card_number, discount_value, std::nullopt};

        customer::Customer customer = {
                    "DiscontCustomer_" + std::to_string(i),
                    "+79015071710", card};

        customers.push_back(std::move(customer));
    }

    for(int i = 0; i < usual_customers_amount; i++) {
        customer::Customer customer = {
                    "Customer_" + std::to_string(i),
                    "+79015071710", std::nullopt};

        customers.push_back(std::move(customer));
    }
    return customers;
}

} // namespace

Emulator::Emulator(MainWindow* window) :
    window_(window)
{}

int Emulator::GenerateNextDeliveryDate(int cur_day) const {
    static std::uniform_int_distribution<int> distribution(1, 3);
    return cur_day + distribution(kRandomGenerator);
}

int Emulator::CalculateСouriersNumber(int orders_number) const {
    return (orders_number + 14) / 15;
}

int Emulator::GenerateOrdersAmount() const {
    // 100% - 2 заказа
    // 0% - 40 заказов
    // уравнение прямой: 38x+100y−4000=0
    // где x - наценка, y - число заказов
    // y = (4000 - 38x) / 100

     return (4000.0 - 38.0 * extra_charge_coef_) / 100;

}

void Emulator::Init(int extra_charge_coef,
                         int medicine_amount,
                         int usual_customers_amount,
                         int regular_customers_amount,
                         int with_card_customers_amount) {

    extra_charge_coef_ = extra_charge_coef / 100;
    auto medicines = InitMedicines(medicine_amount);
    customers_ = InitCustomers(
                medicines, usual_customers_amount,
                regular_customers_amount, with_card_customers_amount);

    warehouse_ = warehouse::Warehouse(std::move(medicines), EXPIRATION_DATE_DELTA);
}

void Emulator::AllDayRun() {
    int n = days_of_emalution_;
    while(n--) {
        NextDayRun();
    }
}

void Emulator::NextDayRun() {
    window_->UpdateMedicineTable(warehouse_.Medicines());
    window_->UpdateOnSaleTable(on_sale_medicines_);
    window_->UpdateOutOfTable(warehouse_.GetOutOfMedicines());
    window_->UpdateCustomerTable(customers_);
    EmulateNextDay();
    window_->UpdateDay(cur_day_);
    window_->UpdateStatistic(total_statistics_, cur_day_);
}

void Emulator::EmulateNextDay() {
    // start of the day
    cur_day_++;
    warehouse_.Refil(cur_day_);
    total_statistics_.InitDaySatistics(cur_day_);

    MakeOrders();

    // end of the day
    int next_delivery_date = GenerateNextDeliveryDate(cur_day_);
    warehouse_.EndDaysChecker(cur_day_);
    warehouse_.OrderSupply(next_delivery_date);
    on_sale_medicines_ = warehouse_.GetOnSaleMedicines();

    const auto& day_statistic = total_statistics_.GetDayStatistics(cur_day_);
    int couriers_number = CalculateСouriersNumber(day_statistic.done_orders_count);
    total_statistics_.UpdateСourierNumber(cur_day_, couriers_number);
    auto expiration_losses = warehouse_.GetExpirationLosses();
    total_statistics_.UpdateExpirationLosses(expiration_losses.money, expiration_losses.amount);
}

medicine::MedicineName Emulator::RandomChooseMedicineName() const {
    // Вероятность выбрать лекартсво на скидке повышается в два раза
    // Отрезок [0, 1) делится на n частей, где n - количество различных лекартсв
    // в соотвествии с вероятностью быть выбраным каждого лекарства
    // Затем генерится число от 0 до 1 лекарство, которому принадлежит этот отрезок
    // Пример: Список лекарств [med_1, med_2, med_3] и med_2 - по скидке
    // med_1 - [0, 0.25), med_2 - [0.25, 0.75),  med_2 - [0.75, 1.0)
    // Выпало 0.36 - выбираем лекарство med_2

    int medicne_amount = warehouse_.UniqMedicinesAmounts();
    int twice_prob_medicne_amount = on_sale_medicines_.size();
    double bucket_step = 1.0 / (twice_prob_medicne_amount + medicne_amount);
    static std::uniform_real_distribution<double> distribution(0, 1.0);
    double chosen_number = distribution(kRandomGenerator);

    double cur_bucket = 0;
    const auto& medicines = warehouse_.Medicines();
    for(const auto& [medicine_name, _] : medicines) {
        if(on_sale_medicines_.find(medicine_name) != on_sale_medicines_.end()) {
            cur_bucket += 2.0 * bucket_step;
        } else {
            cur_bucket += bucket_step;
        }
        if(cur_bucket >= chosen_number) {
            return medicine_name;
        }
    }
    throw std::logic_error("Medicine wasn't randomly chosen");
}

warehouse::OrdersRequest Emulator::GenerateOrdersRequest() const {
    static std::uniform_int_distribution<int> distribution(1, 3);
    int order_position_cnt = distribution(kRandomGenerator);

    warehouse::OrdersRequest orders;
    orders.reserve(order_position_cnt);
    while(order_position_cnt--) {
        medicine::MedicineName medicine_name = RandomChooseMedicineName();
        warehouse::Order order;
        order.medicine_name = medicine_name;
        order.amount = distribution(kRandomGenerator);
        orders.push_back(std::move(order));
    }
    return orders;
}


customer::Customer Emulator::RandomChooseCustomer() const {
    static std::uniform_int_distribution<int> distribution(0, customers_.size() - 1);
    int i = distribution(kRandomGenerator);
    return customers_[i];
}

void Emulator::MakeOrder(const customer::Customer& customer, const warehouse::OrdersRequest& order_request) {
    const auto order_response = warehouse_.ExecuteOrder(order_request);
    double total_cost = 0;
    if(order_response.done) {
        double cost_without_sale = order_response.total_price * (1.0 + extra_charge_coef_);
        double total_discount = 0;
        if(customer.discount_card.has_value()) {
            total_discount += customer.discount_card->discount_value;
        } else if(cost_without_sale > 1000) {
            total_discount += 0.03;
        }
        total_discount = std::min(total_discount, 0.09);
        total_cost = cost_without_sale * (1.0 - total_discount);
    }
    total_statistics_.UpdateDaySatistics(
        cur_day_, total_cost, order_response.lost_profit, order_response.done);
}

void Emulator::MakeRegularOrders() {
    for(auto& customer : customers_) {
        if(customer.discount_card.has_value() && customer.discount_card->regular_customer_info.has_value()) {
            auto& customer_member_info = *customer.discount_card->regular_customer_info;

            if (customer_member_info.prev_order_date + customer_member_info.order_period == cur_day_) {
                warehouse::OrdersRequest order_request;
                order_request.reserve(customer_member_info.medicines.size());

                for(const auto& [medicine_name, amount] : customer_member_info.medicines) {
                    warehouse::Order order;
                    order.medicine_name = medicine_name;
                    order.amount = amount;
                    order_request.push_back(std::move(order));
                }
                MakeOrder(customer, order_request);

                customer_member_info.prev_order_date = cur_day_;
            }
       }
    }
}

void Emulator::MakeOrders() {
    int orders_number = GenerateOrdersAmount();
    while(orders_number--) {
        const auto customer = RandomChooseCustomer();
        const auto order_request = GenerateOrdersRequest();
        MakeOrder(customer, order_request);
    }
    MakeRegularOrders();
}

const std::unordered_map<medicine::MedicineName, medicine::Medicine>& Emulator::GetAllMedicines() const {
    return warehouse_.Medicines();
}

bool Emulator::IsEndOfEmulation() const {
    return cur_day_ == days_of_emalution_;
}

} // emulator
