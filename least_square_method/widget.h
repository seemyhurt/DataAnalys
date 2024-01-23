#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "calcunit.h"

namespace QtCharts { class QChartView; class QLineSeries; class QChart; class QValueAxis;}
class QLabel;

struct SeriesInfo
{
    Qt::GlobalColor color;
    QString name;
    QtCharts::QLineSeries * series;
};

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

private:
    QtCharts::QChartView * createWidget(const QVector<SeriesInfo> series,
                                       const QString &name,
                                       double min_x, double max_x,
                                       double min_y, double max_y);

    void addMarkers(QtCharts::QChart *chart, QtCharts::QValueAxis *x_axis,
                    QtCharts::QValueAxis *y_axis,
                    double x, double y,
                    Qt::GlobalColor color);

    QWidget * createStatWidget();
    QLabel * createCenteredLabel(const QString &text, double value, QWidget *parent);

private:
    CalcUnit _unit;
};

#endif // WIDGET_H
