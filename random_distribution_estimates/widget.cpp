#include "widget.h"

#include "chartview.h"
#include "qboxlayout.h"
#include "qgroupbox.h"
#include "qvalueaxis.h"
#include <qbarset.h>
#include "qbarseries.h"
#include "qbarcategoryaxis.h"
#include "qlabel.h"
#include <QLineSeries>
#include <QTableWidget>
#include <QHeaderView>

using namespace QtCharts;

inline QString getDoubleString(double value)
{
    if (value < std::numeric_limits<double>::epsilon())
        value = 0;
    return QString::number(value, 'g', 8);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    auto wgt_layout = new QVBoxLayout(this);

    auto tabWidget = new QTabWidget(this);

    auto tab_rand = new QWidget(this);
    auto layout_rand = new QVBoxLayout(tab_rand);
    auto layout_gist = new QHBoxLayout(tab_rand);

    auto data = _unit.randomData();
    auto group_state = new QGroupBox("Характеристики: ", this);
    auto state_layout = new QVBoxLayout(this);
    state_layout->addWidget(createStatWidget(data, q2));
    group_state->setLayout(state_layout);

    layout_gist->addWidget(createWidget(data, q1));
    layout_gist->addWidget(createWidget(data, q2));
    layout_gist->addWidget(group_state);

    layout_rand->addLayout(layout_gist);
    tab_rand->setLayout(layout_rand);
    tabWidget->addTab(tab_rand, "Диаграмма распределения");

    tabWidget->addTab(createTableWidget(q2), "Таблица распределения");

    wgt_layout->addWidget(tabWidget);

    setLayout(wgt_layout);

    setMinimumHeight(800);
    setMinimumWidth(1600);
}

QWidget *Widget::createTable(int ranges)
{
    auto tableWidget = new QTableWidget(this);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setColumnCount(10);

    QFont font;
    font.setPointSize(12);
    tableWidget->horizontalHeader()->setFont(font);

    tableWidget->setHorizontalHeaderLabels(QStringList() << "Номер \nинтервала"
                                                         << "Нижняя \nграница \nxⱼ"
                                                         << "Верхняя \nграница \nxⱼ₊₁"
                                                         << "Количество \nпопаданий \nnⱼ"
                                                         << "Вероятность \nнижней \nграницы \nФ(xⱼ)"
                                                         << "Вероятность \nверхней \nграницы \nФ(xⱼ₊₁)"
                                                         << "Вероятность \nпопадания \nв интервал \npⱼ"
                                                         << "Вероятность \nумноженная \nна размер \nN∙pⱼ "
                                                         << "Отклонение \n (nₓ - N∙pⱼ)²"
                                                         << "Итоговая оценка \n((nₓ - N∙pⱼ)²) / (N∙pⱼ)");

    auto stats = _unit.getHistogramAnalysis(_unit.randomData(), ranges);
    for (int i = 0; i < ranges; i++)
    {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        addCenteredItem(QString::number(i), tableWidget, row, 0);
        addCenteredItem(getDoubleString(stats.ranges[i].first), tableWidget, row, 1);
        addCenteredItem(getDoubleString(stats.ranges[i].second), tableWidget, row, 2);
        addCenteredItem(QString::number(stats.values[i]), tableWidget, row, 3);
        addCenteredItem(getDoubleString(stats.probabilitiesRanges[i].first), tableWidget, row, 4);
        addCenteredItem(getDoubleString(stats.probabilitiesRanges[i].second), tableWidget, row, 5);
        addCenteredItem(getDoubleString(stats.probabilities[i]), tableWidget, row, 6);
        addCenteredItem(getDoubleString(stats.muliplyProbabilities[i]), tableWidget, row, 7);
        addCenteredItem(getDoubleString(stats.squaredMuliplyProbabilities[i]), tableWidget, row, 8);
        addCenteredItem(getDoubleString(stats.results[i]), tableWidget, row, 9);
    }

    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);

    auto totalMulProbabilities = std::accumulate(stats.muliplyProbabilities.cbegin(), stats.muliplyProbabilities.cend(), 0.0);
    auto totalValues = std::accumulate(stats.values.cbegin(), stats.values.cend(), 0.0);
    auto totalProbabilities = std::accumulate(stats.probabilities.cbegin(), stats.probabilities.cend(), 0.0);
    auto totalResult = std::accumulate(stats.results.cbegin(), stats.results.cend(), 0.0);

    addCenteredItem("Сумма", tableWidget, row, 0);
    addCenteredItem(getDoubleString(totalValues), tableWidget, row, 3);
    addCenteredItem(getDoubleString(totalProbabilities), tableWidget, row, 6);
    addCenteredItem(getDoubleString(totalMulProbabilities), tableWidget, row, 7);
    addCenteredItem(QString("x² = ") + getDoubleString(totalResult), tableWidget, row, 9);
    addCenteredItem(QString("Xкрит = ") + getDoubleString(stats.x_crit), tableWidget, row, 8);

    return tableWidget;
}

QWidget *Widget::createTableWidget(int ranges)
{
    QVector<double> data;
    data = _unit.randomData();
    QString namePattern = QString("Оценка выборки на %1 диапазонов");

    auto wgt = new QWidget(this);
    auto wgt_layout = new QVBoxLayout(this);

    auto group_q1 = new QGroupBox(namePattern.arg(q1), this);
    auto group_q2 = new QGroupBox(namePattern.arg(q2), this);

    auto q1_layout = new QHBoxLayout(this);
    auto q2_layout = new QVBoxLayout(this);

    q1_layout->addWidget(createTable(q1));
    q2_layout->addWidget(createTable(q2));

    group_q1->setLayout(q1_layout);
    group_q2->setLayout(q2_layout);

    wgt_layout->addWidget(group_q1);
    wgt_layout->addWidget(group_q2);

    wgt->setLayout(wgt_layout);

    return wgt;
}

QWidget *Widget::createWidget(const QVector<double> &data, int ranges)
{
    auto wgt = new QWidget(this);
    auto wgt_layout = new QHBoxLayout(this);

    auto group_graph = new QGroupBox("Диаграмма: ", this);
    auto graph_layout = new QHBoxLayout(this);

    auto name = QString("Выборка: %1 на %2 диапазонов");
    graph_layout->addWidget(createView(name.arg(data.size()).arg(ranges), data, ranges));

    group_graph->setLayout(graph_layout);
    wgt_layout->addWidget(group_graph);
    wgt->setLayout(wgt_layout);

    return wgt;
}

QChartView * Widget::createView(const QString & name, const QVector<double> &data, int histCount)
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
    auto min_max= std::minmax_element(data.begin(), data.end());
    axisXData->setRange(*min_max.first, *min_max.second);
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

QWidget *Widget::createStatWidget(const QVector<double> &data, int size)
{
    auto stat_widget = new QWidget(this);
    auto stat_layout = new QVBoxLayout(stat_widget);

    auto stats = _unit.calculateStatistics(data, size);

    addCenteredLabel("Математическое \nожидание: \n", stats.expectedValue, stat_widget, stat_layout);
    addCenteredLabel("Дисперсия: \n", stats.dispersion, stat_widget, stat_layout);
    addCenteredLabel("Медиана: \n", stats.median, stat_widget, stat_layout);
    addCenteredLabel("Мода: \n", stats.modeValue, stat_widget, stat_layout);
    addCenteredLabel("Среднее \nквадратическое  \nотклонение: \n", stats.standardDeviation, stat_widget, stat_layout);
    addCenteredLabel("Коэффициент \nассиметрии: \n", stats.skewness, stat_widget, stat_layout);
    addCenteredLabel("Эксцесс: \n", stats.kurtosis, stat_widget, stat_layout);
    addCenteredLabel("Стандартная \nошибка: \n", stats.standartError, stat_widget, stat_layout);

    auto intervalString = QString("Доверительный \nинтервал: \n [") +
                          QString::number(stats.confidenceInterval.first) + " : "
                          + QString::number(stats.confidenceInterval.second) + "]";

    auto interval_label = new QLabel(intervalString, stat_widget);
    interval_label->setAlignment(Qt::AlignCenter);

    QFont font = interval_label->font();
    font.setPointSize(font.pointSize() + 5);
    interval_label->setFont(font);

    stat_layout->addWidget(interval_label);
    stat_widget->setLayout(stat_layout);

    return stat_widget;
}

void Widget::addCenteredLabel(const QString &text, double value, QWidget *parent, QLayout * layout)
{
    auto label = new QLabel(text + QString::number(value), parent);
    label->setAlignment(Qt::AlignCenter);

    QFont font = label->font();
    font.setPointSize(font.pointSize() + 5);
    label->setFont(font);

    layout->addWidget(label);
}

void Widget::addCenteredItem(const QString &text, QTableWidget *widget, int row, int column)
{
    auto item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);

    QFont font = item->font();
    font.setPointSize(font.pointSize() + 4);
    item->setFont(font);

    widget->setItem(row, column, item);
}


