#include <iostream>

#include <unistd.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(int med_table_row_cnt, int customer_table_row_cnt, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    chart_series = new QLineSeries();

    QChart *chart = new QChart();
    chart->addSeries(chart_series);
    chart->setTitle("Количество курьеров за день");

    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("День");
    chart->addAxis(axisX, Qt::AlignBottom);
    chart_series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Количество курьеров");
    chart->addAxis(axisY, Qt::AlignLeft);
    chart_series->attachAxis(axisY);

    ui->graphicsView->setChart(chart);

    auto* medicines_table = ui->tableWidget;
    medicines_table->setRowCount(med_table_row_cnt);
    medicines_table->setColumnCount(6);
    QStringList headers;
    headers << "Название" << "Количество" << "Цена" << "Время хранения" << "Дата изготовления" << "Дата доставки";
    medicines_table->setHorizontalHeaderLabels(headers);
    medicines_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto* on_sale_table = ui->tableWidget_2;
    on_sale_table->setRowCount(med_table_row_cnt);
    on_sale_table->setColumnCount(1);
    QStringList headers_2;
    headers_2 << "Название";
    on_sale_table->setHorizontalHeaderLabels(headers_2);
    on_sale_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto* out_of_table = ui->tableWidget_3;
    out_of_table->setRowCount(med_table_row_cnt);
    out_of_table->setColumnCount(1);
    QStringList headers_3;
    headers_3 << "Название";
    out_of_table->setHorizontalHeaderLabels(headers_3);
    out_of_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto* customer_table = ui->tableWidget_4;
    customer_table->setRowCount(customer_table_row_cnt);
    customer_table->setColumnCount(7);
    customer_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::SetRunButton(emulator::Emulator *emulator) {
    emulator_ = emulator;
    bool is_set = false;
    QObject::connect(ui->pushButton_2,
                     &QPushButton::clicked,
                     [&](){
        if(!is_set) {
            emulator_->Init(
                        ui->spinBox->value(),
                        ui->spinBox_2->value(),
                        ui->spinBox_3->value(),
                        ui->spinBox_4->value(),
                        ui->spinBox_5->value());
            ui->spinBox->setReadOnly(true);
            ui->spinBox_2->setReadOnly(true);
            ui->spinBox_3->setReadOnly(true);
            ui->spinBox_4->setReadOnly(true);
            ui->spinBox_5->setReadOnly(true);
            ui->pushButton_3->setEnabled(false);
            is_set = true;
            sleep(2);
        }
        emulator_->NextDayRun();

        if(emulator_->IsEndOfEmulation()) {
            ui->pushButton_2->setEnabled(false);
        }
    });
}

void MainWindow::SetAllDaysButton(emulator::Emulator *emulator) {
    emulator_ = emulator;
    QObject::connect(ui->pushButton_3,
                     &QPushButton::clicked,
                     [&](){
        emulator_->Init(
                    ui->spinBox->value(),
                    ui->spinBox_2->value(),
                    ui->spinBox_3->value(),
                    ui->spinBox_4->value(),
                    ui->spinBox_5->value());
        sleep(2);
        emulator_->AllDayRun();
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
    });
}

void MainWindow::UpdateMedicineTable(const std::unordered_map<medicine::MedicineName, medicine::Medicine>& medicines) {
    auto* table = ui->tableWidget;
    int row = 0;
    for(const auto &[_, medicine] : medicines) {
        QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(medicine.uniq_name));
        QTableWidgetItem* amount = new QTableWidgetItem(QString::number(medicine.amount));
        QTableWidgetItem* price = new QTableWidgetItem(QString::number(medicine.price));
        QTableWidgetItem* shelf_life = new QTableWidgetItem(QString::number(medicine.shelf_life));
        QTableWidgetItem* production_date = new QTableWidgetItem(QString::number(medicine.production_date));

        QTableWidgetItem* date_of_new_delivery = nullptr;
        if (medicine.date_of_new_delivery.has_value()) {
            date_of_new_delivery = new QTableWidgetItem(QString::number(*medicine.date_of_new_delivery));
        } else {
            date_of_new_delivery = new QTableWidgetItem("Есть в наличии");
        }

        table->setItem(row, 0, name);
        table->setItem(row, 1, amount);
        table->setItem(row, 2, price);
        table->setItem(row, 3, shelf_life);
        table->setItem(row, 4, production_date);
        table->setItem(row, 5, date_of_new_delivery);

        row++;
    }
    table->resizeColumnsToContents();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::UpdateOnSaleTable(const std::unordered_set<medicine::MedicineName>& medicines) {
    auto* table = ui->tableWidget_2;
    table->clear();
    int row = 0;
    for(const auto &medicine_name : medicines) {
        QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(medicine_name));
        table->setItem(row, 0, name);

        row++;
    }

    QStringList headers;
    headers << "Название";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::UpdateOutOfTable(const std::unordered_set<medicine::MedicineName>& medicines) {
    auto* table = ui->tableWidget_3;
    table->clear();
    int row = 0;
    for(const auto &medicine_name : medicines) {
        QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(medicine_name));
        table->setItem(row, 0, name);

        row++;
    }

    QStringList headers;
    headers << "Название";
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::UpdateCustomerTable(const std::vector<customer::Customer>& customers) {
    auto* table = ui->tableWidget_4;
    for(size_t row = 0; row < customers.size(); row++) {
        const auto& customer = customers[row];

        QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(customer.customer_name));
        QTableWidgetItem* phone = new QTableWidgetItem(QString::fromStdString(customer.phone_number));

        QTableWidgetItem* discount_card_number = new QTableWidgetItem("");
        QTableWidgetItem* discount_value = new QTableWidgetItem("");

        QTableWidgetItem* order_period = new QTableWidgetItem("");
        QTableWidgetItem* prev_order_date = new QTableWidgetItem("");
        QTableWidgetItem* medicines_widget = new QTableWidgetItem("");

        if (customer.discount_card.has_value()) {
            *discount_card_number = QTableWidgetItem(QString::number(customer.discount_card->discount_card_number));
            *discount_value = QTableWidgetItem(QString::number(customer.discount_card->discount_value));

            if (customer.discount_card->regular_customer_info.has_value()) {
                const auto& info = customer.discount_card->regular_customer_info;
                *order_period = QTableWidgetItem(QString::number(info->order_period));
                *prev_order_date = QTableWidgetItem(QString::number(info->prev_order_date));

                std::string medicines_list;
                for(const auto& medicine : info->medicines) {
                   medicines_list += "(" + medicine.first + ":" + std::to_string(medicine.second) + ")\n";
                }
                *medicines_widget = QTableWidgetItem(QString::fromStdString(medicines_list));
            }
        }

        table->setItem(row, 0, name);
        table->setItem(row, 1, phone);
        table->setItem(row, 2, prev_order_date);
        table->setItem(row, 3, discount_card_number);
        table->setItem(row, 4, discount_value);
        table->setItem(row, 5, order_period);
        table->setItem(row, 6, medicines_widget);
    }

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::UpdateStatistic(const statistics::Statistics &statistics, int cur_day) {
    const auto& cur_day_statistics = statistics.GetDayStatistics(cur_day);
    const auto& total_statistics = statistics.GetTotalStatistics();
    QString text =
            "СЕГОДНЯ:\nКоличество выполненных заказов: " + QString::number(cur_day_statistics.done_orders_count) +
            "\nВыручка: "  + QString::number(cur_day_statistics.revenue) +
            "\nНедополученная прибыль: "  + QString::number(cur_day_statistics.lost_profit) +
            "\nКоличество невыполненных заказов: "  +
                QString::number(cur_day_statistics.not_done_orders_count) +
            "\nКоличество курьеров для доставки заказов за вчера: "  +
                QString::number(cur_day_statistics.couriers_number) +
            "\n\nTOTAL:" +
            "\nКоличество выполненных заказов: " + QString::number(total_statistics.done_orders_count) +
            "\nКоличество невыполненных заказов: "  +
                QString::number(total_statistics.not_done_orders_count) +
            "\nВыручка: "  + QString::number(total_statistics.revenue) +
            "\nНедополученная прибыль: "  + QString::number(total_statistics.lost_profit) +
            "\nПотери за списание лекарств: "  + QString::number(total_statistics.expiration_losses_money) +
            "\nКолчиество списаных лекарств: "  + QString::number(total_statistics.expiration_losses_amount)
            ;
    ui->textBrowser->setText(text);

    *chart_series << QPointF(cur_day, cur_day_statistics.couriers_number);
    auto* chart = ui->graphicsView->chart();
    chart->axisX()->setRange(0, cur_day + 1);
    chart->axisY()->setRange(0, cur_day_statistics.couriers_number + 1);
}

void MainWindow::UpdateDay(int day) {
    static const QString kDayText = "День: ";
    ui->textBrowser_2->setText(kDayText + QString::number(day));
}

MainWindow::~MainWindow()
{
    delete ui;
}

