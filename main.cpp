#include <iostream>
#include <random>
#include <vector>

#include <unistd.h>

#include "emulator.h"
#include "mainwindow.h"

#include "customer.h"
#include "medicine.h"
#include "warehouse.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include <QWidget>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    MainWindow window(100, 100);

    window.setWindowTitle("Medicines delivery");
    window.show();

    emulator::Emulator emulator(&window);
    window.SetRunButton(&emulator);
    window.SetAllDaysButton(&emulator);

    return app.exec();
}
