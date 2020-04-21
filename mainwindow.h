#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>

#include "customer.h"
#include "emulator.h"
#include "medicine.h"
#include "statistics.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int med_table_row_cnt, int customer_table_row_cnt, QWidget *parent = nullptr);
    void SetRunButton(emulator::Emulator *emulator);
    void SetAllDaysButton(emulator::Emulator *emulator);
    void UpdateStatistic(const statistics::Statistics &statistics, int cur_day);
    void UpdateDay(int day);
    void UpdateMedicineTable(const std::unordered_map<medicine::MedicineName, medicine::Medicine>& medicines);
    void UpdateOnSaleTable(const std::unordered_set<medicine::MedicineName>& medicines);
    void UpdateOutOfTable(const std::unordered_set<medicine::MedicineName>& medicines);
    void UpdateCustomerTable(const std::vector<customer::Customer>& customers);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    emulator::Emulator *emulator_ = nullptr;
    QtCharts::QLineSeries* chart_series = nullptr;
};
#endif // MAINWINDOW_H
