#ifndef CALCUNIT_H
#define CALCUNIT_H

#include <QVector>
#include <QHash>

struct Statistics
{
    double expectedValue;       ///< Математическое ожидание
    double dispersion;          ///< Дисперсия
    double median;              ///< Медиана
    double modeValue;           ///< Мода
    double standardDeviation;   ///< Среднеквадратичное отклонение
    double skewness;            ///< Коэффициент ассиметрии
    double kurtosis;            ///< Эксцесс
    double standartError;       ///< Стандартная ошибка

    std::pair<double, double> confidenceInterval; ///< Доверительный интервал
};

struct HistInfo
{
    QVector<std::pair<double, double>> ranges;              // Диапазоны значений
    QVector<int> values;                                    // Количество элементов в диапазоне

    QVector<std::pair<double, double>> probabilitiesRanges; // Диапазон вероятностей в интервале
    QVector<double> probabilities;                          // Вероятность в интервале

    QVector<double> muliplyProbabilities;                   // Вероятности, умноженные на размер выборки
    QVector<double> squaredMuliplyProbabilities;            // Отклонения значений от выборки

    QVector<double> results;                                // Итоговый Коэффициент x0
    double x_crit;                                          // Коэффициент x0 критический
};

class CalcUnit
{
public:
    CalcUnit(int variantNumber = 10, double a = 0.025);

    Statistics calculateStatistics(const QVector<double>& data, int ranges);
    QVector<int> createHistogramSet(const QVector<double>& data, int ranges);
    HistInfo getHistogramAnalysis(const QVector<double> &data, int ranges);

    QVector<double> randomData();

private:
    double calculateMode(const QVector<double>& data, int ranges);
    bool readDataFromFile(const QString &fileName);
    double inverseStudent(double alpha, int degreesOfFreedom);
    double normalDistributionFunction(double x, double mean, double std_dev);
    double calculateCriticalX(double probability, int degrees_of_freedom);

private:
    QVector<double> _randomValues;
    int _variantNumber;
    double _a;
};


#endif // CALCUNIT_H
