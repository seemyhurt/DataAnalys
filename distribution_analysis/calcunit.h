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
};

class CalcUnit
{
public:
    CalcUnit(int variantNumber = 15);

    Statistics calculateStatistics(const QVector<double>& data, int size);
    QVector<int> createHistogramSet(const QVector<double>& data, int size);

    QVector<double> uniformElements(int size);
    QVector<double> gaussElements(int size);

private:
    void generateUniformRandom(int size);
    void generateGaussRandom(int size);

    void generateUniformRandomForm(int size);
    void generateGaussRandomForm(int size);

    double calculateMode(const QVector<double>& data, int size);
    bool readDataFromFile(const QString &fileName, bool isGauss);
    void writeDataToFile(const QString &fileName, const QVector<double> &randomData);

private:
    QHash<std::pair<int /*size*/, bool /*gauss*/>, QVector<double> /*data*/> _randomValues;

    int _variantNumber;
};

#endif // CALCUNIT_H
