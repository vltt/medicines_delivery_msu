#pragma once

#ifndef STATISTICS_H
#define STATISTICS_H

#include <unordered_map>

namespace statistics {

struct DayStatistics {
    int done_orders_count = 0;
    int not_done_orders_count = 0;
    double revenue = 0;
    double lost_profit = 0;
    int couriers_number = 0;
};

struct TotalStatistics {
    int done_orders_count = 0;
    int not_done_orders_count = 0;
    double revenue = 0;
    double lost_profit = 0;
    double expiration_losses_money = 0;
    int expiration_losses_amount = 0;
};

class Statistics {
public:
    Statistics();

    void InitDaySatistics(int day);
    void UpdateDaySatistics(int day, double order_revenue, double lost_profit, bool done);
    void UpdateСourierNumber(int day, int couriers_number);
    void UpdateExpirationLosses(double total_losses_money, int total_losses_amount);
    const DayStatistics& GetDayStatistics(int day) const;
    TotalStatistics GetTotalStatistics() const ;


private:
    std::unordered_map<int, DayStatistics> days_statistics_;
    double total_expiration_losses_money_ = 0;
    int total_expiration_losses_amount_ = 0;
};

} // namespace statistics

#endif // STATISTICS_H
