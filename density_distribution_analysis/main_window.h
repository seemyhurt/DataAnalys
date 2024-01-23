#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "calcunit.h"
#include <QWidget>

namespace QtCharts { class QChartView; class QLineSeries; class QChart; class QValueAxis;}
class QLabel;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(double valueA, int startRange, int endRange, QWidget *parent = nullptr);

private:
    QtCharts::QChartView * createWidget(QtCharts::QLineSeries *series,
                                        const QString &name, const QString &graph_name,
                                        double min_x, double max_x,
                                        double min_y, double max_y,
                                        Qt::GlobalColor color);

    void addMarkers(QtCharts::QChart *chart, QtCharts::QValueAxis *x,
                    QtCharts::QValueAxis *y,
                    double value, const QString &label,
                    Qt::GlobalColor color);

    QWidget * createStatWidget();
    QLabel * createCenteredLabel(const QString &text, double value, QWidget *parent);

private:
    calc_unit _unit;
};

#endif // MAIN_WINDOW_H
