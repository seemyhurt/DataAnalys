#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "calcunit.h"

class QLabel;
class QTableWidget;

namespace QtCharts { class QChartView; class QLineSeries; class QChart; class QValueAxis;}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    QWidget * createWidget(const QVector<double> &data, int ranges);
    QWidget *createTable(int ranges);

    QtCharts::QChartView * createView(const QString & name,
                                     const QVector<double> &data,
                                     int histCount);

    QWidget * createStatWidget(const QVector<double> &data, int size);
    QWidget *createTableWidget(int ranges);
    void addCenteredLabel(const QString &text, double value, QWidget *parent, QLayout *layout);
    void addCenteredItem(const QString &text, QTableWidget *widget, int row, int column);

private:
    CalcUnit _unit;

    int q1 = 5;
    int q2 = 7;
};
#endif // WIDGET_H
