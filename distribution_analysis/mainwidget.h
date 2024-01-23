#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "calcunit.h"
#include <QWidget>

class QLabel;

namespace QtCharts { class QChartView; class QLineSeries; class QChart; class QValueAxis;}

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QWidget * createWidget(int dataSize, bool gauss, int ranges);

    QtCharts::QChartView * createView(const QString & name,
                                      const QVector<double> &data,
                                     int histCount);

    QWidget * createStatWidget(const QVector<double> &data, int size);
    QLabel *createCenteredLabel(const QString &text, double value, QWidget *parent);

private:
    CalcUnit _unit;
};

#endif // MAIN_WINDOW_H
