#ifndef CALCUNIT_H
#define CALCUNIT_H

#include <QVector>
#include <QHash>

struct Statistics
{
    double expectedValue = 0.0;       ///< Математическое ожидание
    double halfSum = 0.0;             ///< Полусумма крайних членов
    double median = 0.0;              ///< Медиана
    double average = 0.0;             ///< Среднее арифметическое с отбросом крайних членов

    double expectedValueDelta = 0.0;  ///< Дельта математического ожидания
    double halfSumDelta = 0.0;        ///< Дельта полусуммы крайних членов
    double medianDelta = 0.0;         ///< Дельта медианы
    double averageDelta = 0.0;        ///< Дельта среднего арифметическое с отбросом крайних членов


    double dispersion = 0.0;          ///< Дисперсия
    double standardDeviation = 0.0;   ///< Среднеквадратичное отклонение
};

class CalcUnit
{
public:
    CalcUnit(int variantNumber = 15);

    Statistics calculateStatistics(const QVector<double>& data);
    QVector<int> createHistogramSet(const QVector<double>& data, int size);

    QVector<double> uniformElements(int size, double coeff);
    QVector<double> gaussElements(int size, double coeff);

private:
    QVector<double> generateUniformRandomForm(double coef, int size);
    QVector<double> generateGaussRandomForm(double coef, int size);

    QVector<double> makeParametersSeries(const QVector<double>& noise);

    double calculateMode(const QVector<double>& data, int size);

    void writeDataToFile(const QString &fileName, const QVector<double> &data);
    QVector<double> readDataFromFile(const QString &fileName);

private:
    QHash<std::pair<int /*size*/, double /*coef*/>, QVector<double> /*data*/> _gaussSeries;
    QHash<std::pair<int /*size*/, double /*coef*/>, QVector<double> /*data*/> _uniformSeries;

    int _variantNumber;
};

#endif // CALCUNIT_H
