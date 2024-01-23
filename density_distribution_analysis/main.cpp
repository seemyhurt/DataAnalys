#include "main_window.h"
#include "parametersinputdialog.h"
#include <QApplication>
#include <QDebug>

#include <iostream>
#include <cmath>
#include <functional>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int rangeStart, rangeEnd;
    double valueA;
    ParametersDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        rangeStart = dialog.getStartValue();
        rangeEnd = dialog.getEndValue();
        valueA = dialog.getValueA();
    }
    else
        return 0;

    MainWindow w(valueA, rangeStart, rangeEnd);
    w.show();

    return app.exec();
}
