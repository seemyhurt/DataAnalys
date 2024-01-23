#include "mainwidget.h"

#include "chartview.h"
#include "qboxlayout.h"
#include "qgroupbox.h"
#include "qvalueaxis.h"
#include <qbarset.h>
#include "qbarseries.h"
#include "qbarcategoryaxis.h"
#include "qlabel.h"
#include "QTabWidget"
#include <QLineSeries>

using namespace QtCharts;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    int l1 = 100;
    int l2 = 1000;

    int q1 = 5;
    int q2 = 7;

    auto wgt_layout = new QVBoxLayout(this);

    auto tabWidget = new QTabWidget(this);

    auto tab_rand = new QWidget(this);
    auto layout_rand = new QVBoxLayout(tab_rand);
    auto layout_hund = new QHBoxLayout(tab_rand);
    layout_hund->addWidget(createWidget(l1, false, q1));
    layout_hund->addWidget(createWidget(l1, false, q2));

    auto layout_thus = new QHBoxLayout(tab_rand);
    layout_thus->addWidget(createWidget(l2, false, q1));
    layout_thus->addWidget(createWidget(l2, false, q2));

    layout_rand->addLayout(layout_hund);
    layout_rand->addLayout(layout_thus);
    tab_rand->setLayout(layout_rand);
    tabWidget->addTab(tab_rand, "Равномерное распределение");

    auto tab_gauss = new QWidget();
    auto layout_gauss = new QVBoxLayout(tab_gauss);
    auto layout_gauss_hund = new QHBoxLayout(tab_rand);
    layout_gauss_hund->addWidget(createWidget(l1, true, q1));
    layout_gauss_hund->addWidget(createWidget(l1, true, q2));

    auto layout_gauss_thus = new QHBoxLayout(tab_rand);
    layout_gauss_thus->addWidget(createWidget(l2, true, q1));
    layout_gauss_thus->addWidget(createWidget(l2, true, q2));

    layout_gauss->addLayout(layout_gauss_hund);
    layout_gauss->addLayout(layout_gauss_thus);
    tab_gauss->setLayout(layout_gauss);
    tabWidget->addTab(tab_gauss, "Распределение гаусса");

    wgt_layout->addWidget(tabWidget);

    setLayout(wgt_layout);

    setMinimumHeight(800);
    setMinimumWidth(1350);
}

QWidget *MainWindow::createWidget(int dataSize, bool gauss, int ranges)
{
    QVector<double> data;
    if (gauss)
        data = _unit.gaussElements(dataSize);
    else
        data = _unit.uniformElements(dataSize);

    auto wgt = new QWidget(this);
    auto wgt_layout = new QHBoxLayout(this);

    auto group_graph = new QGroupBox("Диаграмма: ", this);
    auto group_state = new QGroupBox("Характеристики: ", this);

    auto graph_layout = new QHBoxLayout(this);
    auto state_layout = new QVBoxLayout(this);

    auto name = QString("Выборка: %1 на %2 диапазонов");
    graph_layout->addWidget(createView(name.arg(dataSize).arg(ranges), data, ranges));
    state_layout->addWidget(createStatWidget(data, ranges));

    group_graph->setLayout(graph_layout);
    group_state->setLayout(state_layout);

    wgt_layout->addWidget(group_graph);
    wgt_layout->addWidget(group_state);

    wgt->setLayout(wgt_layout);

    return wgt;
}

QChartView * MainWindow::createView(const QString & name, const QVector<double> &data, int histCount)
{
    auto set = new QBarSet("Гистограмма");

    auto  hist = _unit.createHistogramSet(data, histCount);

    for (int i = 0; i < histCount; ++i)
        *set << hist[i];

    auto series = new QBarSeries();
    series->append(set);
    series->setLabelsVisible(true);

    auto lineSeries = new QLineSeries();
    lineSeries->setPen(QPen(Qt::red));
    lineSeries->setName("Кривая распределения");
    for (int i = 0; i < histCount; ++i)
        lineSeries->append(i, hist[i]);

    auto chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(lineSeries);
    chart->setTitle(name);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    auto axisX = new QBarCategoryAxis();
    for (int i = 1; i <= histCount; ++i)
        axisX->append(QString::number(i));


    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    lineSeries->attachAxis(axisX);

    auto axisXData = new QValueAxis();
    auto minElement = *std::min_element(data.begin(), data.end());
    auto maxElement = *std::max_element(data.begin(), data.end());
    axisXData->setRange(minElement, maxElement);
    axisXData->setTickCount(histCount + 1);
    chart->addAxis(axisXData, Qt::AlignBottom);

    auto axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    lineSeries->attachAxis(axisY);

    auto chart_view = new ChartView(chart, this);
    chart_view->setRenderHint(QPainter::Antialiasing);
    chart_view->setRubberBand(QChartView::RectangleRubberBand);
    chart_view->setInteractive(true);

    return chart_view;
}

QWidget *MainWindow::createStatWidget(const QVector<double> &data, int size)
{
    auto stat_widget = new QWidget(this);
    auto stat_layout = new QVBoxLayout(stat_widget);

    auto stats = _unit.calculateStatistics(data, size);

    auto exp_value_label = createCenteredLabel("Математическое \nожидание: \n", stats.expectedValue, stat_widget);
    auto dispersion_label = createCenteredLabel("Дисперсия: \n", stats.dispersion, stat_widget);
    auto median_label = createCenteredLabel("Медиана: \n", stats.median, stat_widget);
    auto mode_label = createCenteredLabel("Мода: \n", stats.modeValue, stat_widget);
    auto std_dev_label = createCenteredLabel("Среднее \nквадратическое  \nотклонение: \n", stats.standardDeviation, stat_widget);

    stat_layout->addWidget(exp_value_label);
    stat_layout->addWidget(dispersion_label);
    stat_layout->addWidget(median_label);
    stat_layout->addWidget(mode_label);
    stat_layout->addWidget(std_dev_label);

    stat_widget->setLayout(stat_layout);

    return stat_widget;
}

QLabel * MainWindow::createCenteredLabel(const QString &text, double value, QWidget *parent)
{
    auto label = new QLabel(text + QString::number(value), parent);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

