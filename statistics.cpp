#include "statistics.h"

namespace statistics {

Statistics::Statistics() {}

void Statistics::InitDaySatistics(int day) {
    days_statistics_[day] = DayStatistics();
}

void Statistics::UpdateDaySatistics(
        int day, double order_revenue, double lost_profit, bool done) {
    auto& cur_day_statistics = days_statistics_[day];
    cur_day_statistics.revenue += order_revenue;
    cur_day_statistics.lost_profit += lost_profit;
    if (done) {
        cur_day_statistics.done_orders_count++;
    } else {
        cur_day_statistics.not_done_orders_count++;
    }
}

void Statistics::UpdateExpirationLosses(double total_losses_money, int total_losses_amount){
    total_expiration_losses_money_ = total_losses_money;
    total_expiration_losses_amount_ = total_losses_amount;
}

void Statistics::UpdateСourierNumber(int day, int couriers_number) {
    days_statistics_[day].couriers_number = couriers_number;
}

const DayStatistics& Statistics::GetDayStatistics(int day) const {
    return days_statistics_.at(day);
}

TotalStatistics Statistics::GetTotalStatistics() const {
    int total_done_orders_count = 0;
    int total_not_done_orders_count = 0;
    double total_revenue = 0;
    double total_lost_profit = 0;
    for(const auto& day_statistics : days_statistics_) {
        total_done_orders_count += day_statistics.second.done_orders_count;
        total_revenue += day_statistics.second.revenue;
        total_lost_profit += day_statistics.second.lost_profit;
        total_not_done_orders_count += day_statistics.second.not_done_orders_count;
    }
    TotalStatistics total_statistic;
    total_statistic.done_orders_count = total_done_orders_count;
    total_statistic.not_done_orders_count = total_not_done_orders_count;
    total_statistic.revenue = total_revenue;
    total_statistic.lost_profit += total_lost_profit;
    total_statistic.expiration_losses_money = total_expiration_losses_money_;
    total_statistic.expiration_losses_amount = total_expiration_losses_amount_;

    return total_statistic;
}

} // namespace statistics
