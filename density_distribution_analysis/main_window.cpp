#include "main_window.h"

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

using namespace QtCharts;

MainWindow::MainWindow(double valueA, int startRange, int endRange, QWidget *parent)
    : QWidget(parent),
    _unit(valueA, startRange, endRange)
{
    auto group_graph = new QGroupBox("Графики: ", this);
    auto group_state = new QGroupBox("Характеристики: ", this);

    auto graph_layout = new QHBoxLayout(this);
    auto state_layout = new QVBoxLayout(this);

    auto pdf_series = new QLineSeries(this);
    auto cdf_series = new QLineSeries(this);

    double max_cdf_y = 0.0, max_pdf_y = 0.0;
    double min_x = -0.1, max_x = 1.1;
    for (double x = min_x; x <= max_x; x += 0.01)
    {
        auto pdf_y = _unit.pdf_with_const(x);
        if (pdf_y > max_pdf_y)
            max_pdf_y = pdf_y;
        pdf_series->append(x, pdf_y);

        auto cdf_y = _unit.distribution_function(x);
        if (cdf_y > max_cdf_y)
            max_cdf_y = cdf_y;
        cdf_series->append(x, cdf_y);
    }

    graph_layout->addWidget(createWidget(pdf_series, "График плотности распределения", "PDF", min_x, max_x, -0.1, max_pdf_y+0.2, Qt::blue));
    graph_layout->addWidget(createWidget(cdf_series, "График функции распределения", "CDF",  min_x, max_x, -0.1, max_cdf_y+0.2, Qt::green));
    state_layout->addWidget(createStatWidget());

    group_graph->setLayout(graph_layout);
    group_state->setLayout(state_layout);

    auto  wgt_layout = new QVBoxLayout(this);
    wgt_layout->addWidget(group_graph);
    wgt_layout->addWidget(group_state);

    setLayout(wgt_layout);

    resize(1200, 600);
    setMinimumHeight(600);
}

QChartView * MainWindow::createWidget(QLineSeries *series,
                                       const QString &name, const QString &graph_name,
                                       double min_x, double max_x,
                                       double min_y, double max_y,
                                       Qt::GlobalColor color)
{
    auto chart = new QChart;

    QPen pen(color, 2, Qt::SolidLine);
    series->setPen(pen);
    series->setName(graph_name);

    chart->addSeries(series);
    chart->setTitle(name);

    auto axis_x = new QValueAxis;
    axis_x->setTitleText("X");
    axis_x->setTickCount(max_x - min_x + 1);
    axis_x->setRange(min_x, max_x);

    chart->addAxis(axis_x, Qt::AlignBottom);
    series->attachAxis(axis_x);

    auto axis_y = new QValueAxis;
    axis_y->setTitleText("Y");
    axis_y->setTickCount((max_y - min_y) * 10 + 1);
    axis_y->setRange(min_y, max_y);
    chart->addAxis(axis_y, Qt::AlignLeft);
    series->attachAxis(axis_y);

    addMarkers(chart, axis_x, axis_y, _unit.mode_value(), "Мода", Qt::red);
    addMarkers(chart, axis_x, axis_y, _unit.expected_value(), "Мат. ожидание", Qt::magenta);
    addMarkers(chart, axis_x, axis_y, _unit.median(), "Медиана", Qt::yellow);

    auto chart_view = new ChartView(chart, this);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setRubberBand(QChartView::RectangleRubberBand);
    chart_view->setInteractive(true);

    return chart_view;
}

QWidget *MainWindow::createStatWidget()
{
    auto stat_widget = new QWidget(this);
    auto stat_layout = new QHBoxLayout(stat_widget);

    auto const_label = createCenteredLabel("Константа C: \n", _unit.const_value(), stat_widget);
    auto exp_value_label = createCenteredLabel("Математическое ожидание: \n", _unit.expected_value(), stat_widget);
    auto dispersion_label = createCenteredLabel("Дисперсия: \n", _unit.dispersion(), stat_widget);
    auto median_label = createCenteredLabel("Медиана: \n", _unit.median(), stat_widget);
    auto mode_label = createCenteredLabel("Мода: \n", _unit.mode_value(), stat_widget);
    auto std_dev_label = createCenteredLabel("Среднее квадратическое  \nотклонение: \n", _unit.standard_deviation(), stat_widget);


    stat_layout->addWidget(const_label);
    stat_layout->addWidget(exp_value_label);
    stat_layout->addWidget(dispersion_label);
    stat_layout->addWidget(median_label);
    stat_layout->addWidget(mode_label);
    stat_layout->addWidget(std_dev_label);

    stat_widget->setLayout(stat_layout);

    return stat_widget;
}

void MainWindow::addMarkers(QChart *chart, QValueAxis * x, QValueAxis * y,
                            double value, const QString &label,
                            Qt::GlobalColor color)
{
    auto scatterSeries = new QScatterSeries;
    scatterSeries->append(value, 0);
    scatterSeries->setMarkerSize(10);
    scatterSeries->setColor(color);
    scatterSeries->setName(label);
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);

    chart->addSeries(scatterSeries);

    scatterSeries->attachAxis(x);
    scatterSeries->attachAxis(y);
}

QLabel * MainWindow::createCenteredLabel(const QString &text, double value, QWidget *parent)
{
    auto label = new QLabel(text + QString::number(value), parent);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedWidth(200);
    return label;
}
