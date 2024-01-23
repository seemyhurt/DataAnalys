#include "widget.h"

#include <QChart>
#include <QPainter>
#include <QValueAxis>
#include <QLineSeries>
#include <QChartView>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScatterSeries>
#include <QGraphicsSimpleTextItem>
#include "QMessageBox"
#include "chartview.h"
#include "qlegendmarker.h"

using namespace QtCharts;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    double min_x = 0.0;
    double max_x = 6.0;

    double min_y = 0.0;
    double max_y = 5.0;

    auto group_graph = new QGroupBox("Графики: ", this);
    auto group_state = new QGroupBox("Характеристики: ", this);

    auto graph_layout = new QHBoxLayout(this);
    auto state_layout = new QVBoxLayout(this);

    auto linear_series = new QLineSeries(this);
    auto quadratic_series = new QLineSeries(this);
    auto cubic_series = new QLineSeries(this);

    QVector<SeriesInfo> info;
    for (double x = 1.0; x <= 5.0; x += 0.01)
    {
        linear_series->append(x, _unit.linear_function(x));
        quadratic_series->append(x, _unit.quadratic_function(x));
        cubic_series->append(x, _unit.cubic_function(x));
    }

    SeriesInfo linear_info {Qt::blue, "Линейная аппроксимация", linear_series};
    SeriesInfo quadratic_info {Qt::green, "Квадратичная аппроксимация", quadratic_series};
    SeriesInfo cubic_info {Qt::darkYellow, "Кубическая аппроксимация", cubic_series};
    info << linear_info << quadratic_info << cubic_info;

    graph_layout->addWidget(createWidget(info, "Метод наименьших квадратов", min_x, max_x, min_y, max_y));
    state_layout->addWidget(createStatWidget());

    group_graph->setLayout(graph_layout);
    group_state->setLayout(state_layout);

    auto  wgt_layout = new QVBoxLayout(this);
    wgt_layout->addWidget(group_graph);
    wgt_layout->addWidget(group_state);

    setLayout(wgt_layout);

    resize(1000, 1200);
    setMinimumHeight(1000);
}

QChartView * Widget::createWidget(const QVector<SeriesInfo> series,
                                 const QString &name,
                                 double min_x, double max_x,
                                 double min_y, double max_y)
{
    auto chart = new QChart;
    chart->setTitle(name);

    auto axis_x = new QValueAxis;
    axis_x->setTitleText("X");
    axis_x->setTickCount(max_x - min_x + 1);
    axis_x->setRange(min_x, max_x);
    chart->addAxis(axis_x, Qt::AlignBottom);

    auto axis_y = new QValueAxis;
    axis_y->setTitleText("Y");
    axis_y->setTickCount(max_y - min_y + 1);
    axis_y->setRange(min_y, max_y);
    chart->addAxis(axis_y, Qt::AlignLeft);


    auto yValues = _unit.yValues();
    for (int i = 0; i < yValues.size(); i++)
    {
        addMarkers(chart, axis_x, axis_y, i+1, yValues[i], Qt::red);
    }

    for (const auto &ser :qAsConst(series))
    {
        QPen pen(ser.color, 2, Qt::SolidLine);
        ser.series->setPen(pen);
        ser.series->setName(ser.name);
        chart->addSeries(ser.series);

        ser.series->attachAxis(axis_x);
        ser.series->attachAxis(axis_y);

    }

    auto chart_view = new ChartView(chart, this);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setRubberBand(QChartView::RectangleRubberBand);
    chart_view->setInteractive(true);

    return chart_view;
}

QWidget *Widget::createStatWidget()
{
    auto stat_widget = new QWidget(this);
    auto stat_layout = new QHBoxLayout(stat_widget);

    auto stats = _unit.getDispersion();
    stat_layout->addWidget(createCenteredLabel("Дисперсия \nлинейной \nаппроксимации\n", stats.linear_dispersion, stat_widget));
    stat_layout->addWidget(createCenteredLabel("СКО \nлинейной \nаппроксимации\n", std::sqrt(stats.linear_dispersion), stat_widget));
    stat_layout->addWidget(createCenteredLabel("Дисперсия \nквадратичной \nаппроксимации\n", stats.quadratic_dispersion, stat_widget));
    stat_layout->addWidget(createCenteredLabel("СКО \nквадратичной \nаппроксимации\n", std::sqrt(stats.quadratic_dispersion), stat_widget));
    stat_layout->addWidget(createCenteredLabel("Дисперсия \nкубической \nаппроксимации\n", stats.cubic_dispersion, stat_widget));
    stat_layout->addWidget(createCenteredLabel("СКО \nкубической \nаппроксимации\n", std::sqrt(stats.cubic_dispersion), stat_widget));
    stat_widget->setLayout(stat_layout);

    return stat_widget;
}

void Widget::addMarkers(QChart *chart, QValueAxis * x_axis, QValueAxis * y_axis,
                            double x, double y,
                            Qt::GlobalColor color)
{
    auto scatterSeries = new QScatterSeries;
    scatterSeries->append(x, y);
    scatterSeries->setMarkerSize(10);
    scatterSeries->setColor(color);
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    chart->addSeries(scatterSeries);

    scatterSeries->attachAxis(x_axis);
    scatterSeries->attachAxis(y_axis);

    auto markers = chart->legend()->markers(scatterSeries);
    for (const auto &marker : qAsConst(markers))
        marker->setVisible(false);
}

QLabel * Widget::createCenteredLabel(const QString &text, double value, QWidget *parent)
{
    auto label = new QLabel(text + QString::number(value), parent);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedWidth(200);

    QFont font = label->font();
    font.setPointSize(font.pointSize() + 4);
    label->setFont(font);

    return label;
}
