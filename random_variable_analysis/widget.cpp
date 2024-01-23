#include "widget.h"
#include "calcunit.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

inline QString getDoubleString(double value)
{
    if (value < std::numeric_limits<double>::epsilon())
        value = 0;
    return QString::number(value, 'g', 8);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    _unit(QSharedPointer<CalcUnit>::create())
{
    resize(1500, 800);

    auto tableWidget = new QTableWidget(this);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setColumnCount(11);

    auto headerFont = tableWidget->horizontalHeader()->font();
    headerFont.setPointSize(headerFont.pointSize() + 3);
    tableWidget->horizontalHeader()->setFont(headerFont);

    tableWidget->setHorizontalHeaderLabels(QStringList() << "Объем \nвыборки"
                                                        << "Оценка \nx̃₁"
                                                        << "Оценка \nx̃₂"
                                                        << "Оценка \nx̃₃"
                                                        << "Оценка \nx̃₄"
                                                        << "Относительная \nошибка δx₁"
                                                        << "Относительная \nошибка δx₂"
                                                        << "Относительная \nошибка δx₃"
                                                        << "Относительная \nошибка δx₄"
                                                        << "Дисперсия"
                                                        << "СКО");

    addDataToTable(tableWidget, 20, true);
    addDataToTable(tableWidget, 100, true);
    addDataToTable(tableWidget, 20, false);
    addDataToTable(tableWidget, 100, false);


    auto layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget);
    setLayout(layout);
}

Widget::~Widget()
{
}

void Widget::addDataToTable(QTableWidget *table, int coeff, bool isGauss)
{
    auto nameRow = table->rowCount();
    table->insertRow(table->rowCount());
    table->insertRow(table->rowCount());

    QString namePattern;
    isGauss ? namePattern = QString("Шум распределен по нормальному закону с параметрами: Mn = 0; σ = с/%1").arg(coeff)
            : namePattern = QString("Шум распределен по равномерному закону с параметрами: a=-c/%1, b=c/%1").arg(coeff);

    auto item = new QTableWidgetItem(namePattern);
    item->setTextAlignment(Qt::AlignCenter);

    QFont font = item->font();
    font.setPointSize(font.pointSize() + 7);
    item->setFont(font);

    table->setSpan(nameRow, 0, 2, table->columnCount());
    table->setItem(nameRow, 0, item);

    QVector<int> sampleSizes = {15, 30, 100, 1000};

    for (const auto& sampleSize : sampleSizes)
    {
        int row = table->rowCount();
        table->insertRow(row);

        QVector<double> data;
        isGauss ? data = _unit->gaussElements(sampleSize, coeff)
                : data = _unit->uniformElements(sampleSize, coeff);

        auto statistic = _unit->calculateStatistics(data);

        addCenteredItem(QString::number(sampleSize), "", table,  row, 0);
        addCenteredItem(getDoubleString(statistic.expectedValue), "Математическое ожидание", table,  row, 1);
        addCenteredItem(getDoubleString(statistic.halfSum), "Полусумма крайних членов", table,  row, 2);
        addCenteredItem(getDoubleString(statistic.median), "Медиана", table,  row, 3);
        addCenteredItem(getDoubleString(statistic.average), "Среднее арифметическое с отбросом крайних членов", table,  row, 4);
        addCenteredItem(getDoubleString(statistic.expectedValueDelta), "Дельта математического ожидания", table,  row, 5);
        addCenteredItem(getDoubleString(statistic.halfSumDelta), "Дельта полусуммы крайних членов", table,  row, 6);
        addCenteredItem(getDoubleString(statistic.medianDelta), "Дельта медианы", table,  row, 7);
        addCenteredItem(getDoubleString(statistic.averageDelta), "Дельта среднего арифметическое с отбросом крайних членов", table,  row, 8);
        addCenteredItem(getDoubleString(statistic.dispersion), "Дисперсия", table,  row, 9);
        addCenteredItem(getDoubleString(statistic.standardDeviation), "Среднее квадратическое отклонение", table,  row, 10);
    }
}

void Widget::addCenteredItem(const QString &text, const QString &tooltip, QTableWidget *widget, int row, int column)
{
    auto item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(tooltip);

    QFont font = item->font();
    font.setPointSize(font.pointSize() + 7);
    item->setFont(font);

    widget->setItem(row, column, item);
}

