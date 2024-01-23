#include "calcunit.h"

#include "calcunit.h"
#include <boost/math/distributions/students_t.hpp>
#include <boost/math/distributions/chi_squared.hpp>

#include <cmath>
#include <algorithm>
#include <QHash>
#include <QtMath>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QDebug>

inline double normilize(double value, int decimal)
{
    int scale = std::pow(10, decimal);
    return std::round(value * scale) / scale;
}

CalcUnit::CalcUnit(int variantNumber, double a) :
    _variantNumber(variantNumber),
    _a(a)
{
    //You need to create file with values
    QString dataFile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/random_data.txt";
    if(!readDataFromFile(dataFile))
        Q_ASSERT(false);
}

bool CalcUnit::readDataFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QTextStream in(&file);
    QVector<double> data;

    while (!in.atEnd())
    {
        auto line = in.readLine();
        bool ok;
        double value = line.toDouble(&ok);
        if (ok)
            data.append(value);
        else
        {
            qDebug() << "Failed to convert line to double:" << line;
            return false;
        }
    }

    _randomValues = data;
    file.close();
    return true;
}

double CalcUnit::calculateMode(const QVector<double> &data, int ranges)
{
    auto hist = createHistogramSet(data, ranges);
    int maxIndex = 0;
    for (int i = 0; i < hist.size(); i++)
    {
        if (hist[i] > hist[maxIndex])
            maxIndex = i;
    }

    auto min_max = std::minmax_element(data.begin(), data.end());
    double range = *min_max.second - *min_max.first;
    double delta = range / ranges;

    double start_x = *min_max.first + maxIndex * delta;
    double end_x = *min_max.first + (maxIndex + 1) * delta;

    return (start_x + end_x) / 2;
}

Statistics CalcUnit::calculateStatistics(const QVector<double> &data, int size)
{
    QVector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    double expectedValue = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    double mode = calculateMode(data, size);
    double median = (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0;

    double dispersion = 0.0;
    double skewness = 0.0;
    double kurtosis = 0.0;

    for (const auto &value : qAsConst(data))
    {
        double diff = value - expectedValue;
        dispersion += std::pow(diff, 2);
        skewness += std::pow(diff, 3);
        kurtosis += std::pow(diff, 4);
    }

    dispersion = dispersion / (data.size() - 1.0);
    skewness = skewness / data.size();
    kurtosis = kurtosis / data.size();

    double std_dev = std::sqrt(dispersion);
    double standardError = std_dev / std::sqrt(data.size());

    auto tValue = inverseStudent(_a, data.size() - 1) * standardError;

    return {expectedValue, dispersion, median, mode, std_dev,
            skewness, kurtosis, standardError,
            std::make_pair(expectedValue - tValue, expectedValue + tValue)};
}

double CalcUnit::calculateCriticalX(double probability, int degrees_of_freedom)
{
        boost::math::chi_squared mydist(degrees_of_freedom);
        return quantile(mydist, 1 - probability);
}

double CalcUnit::inverseStudent(double probability, int degrees_of_freedom)
{
    boost::math::students_t dist(degrees_of_freedom);
    return quantile(dist, 1 - probability);
}

QVector<int> CalcUnit::createHistogramSet(const QVector<double>& data, int ranges)
{
    auto min_max = std::minmax_element(data.begin(), data.end());

    double range = *min_max.second - *min_max.first;
    double delta = range / ranges;

    QVector<int> hist(ranges, 0);
    for (const auto &value : data)
    {
        for (int i = 0; i < ranges; ++i)
        {
            double start_x = *min_max.first + i * delta;
            double end_x = *min_max.first + (i + 1) * delta;

            if (value >= start_x && value <= end_x)
            {
                hist[i]++;
                break;
            }
        }
    }
    return hist;
}

HistInfo CalcUnit::getHistogramAnalysis(const QVector<double>& data, int ranges)
{
    HistInfo result;
    result.ranges.resize(ranges);
    result.values.resize(ranges);
    result.probabilitiesRanges.resize(ranges);
    result.probabilities.resize(ranges);
    result.muliplyProbabilities.resize(ranges);
    result.squaredMuliplyProbabilities.resize(ranges);
    result.results.resize(ranges);

    auto min_max = std::minmax_element(data.begin(), data.end());

    double range = *min_max.second - *min_max.first;
    double delta = range / ranges;

    double dispersion = 0.0;
    double expectedValue = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

    for (const auto &value : data)
    {
        dispersion += std::pow(value - expectedValue, 2);
        for (int i = 0; i < ranges; ++i)
        {
            double start_x = *min_max.first + i * delta;
            double end_x = *min_max.first + (i + 1) * delta;
            result.ranges[i] = std::make_pair(start_x, end_x);

            if (value >= start_x && value <= end_x)
            {
                result.values[i]++;
                break;
            }
        }
    }
    dispersion = dispersion / (data.size() - 1.0);
    for (int i = 0; i < ranges; i++)
    {
        const double &start_x = result.ranges[i].first;
        const double &end_x = result.ranges[i].second;

        double start_x_prob =  i == 0 ? 0 : normalDistributionFunction(start_x, expectedValue, dispersion);
        double end_x_prob = i == ranges - 1 ? 1 : normalDistributionFunction(end_x, expectedValue, dispersion);
        double range = end_x_prob - start_x_prob;

        result.probabilitiesRanges[i] = std::make_pair(start_x_prob, end_x_prob);
        result.probabilities[i] = range;

        double muliplyProb = normilize(range * data.size(), 4);
        result.muliplyProbabilities[i] = muliplyProb;

        double squaredMuliplyProb = normilize(pow(result.values[i] - muliplyProb, 2), 5);
        result.squaredMuliplyProbabilities[i] = squaredMuliplyProb;

        result.results[i] = normilize(squaredMuliplyProb / muliplyProb, 4);
    }
    result.x_crit = calculateCriticalX(_a, ranges - 2 - 1);
    return result;
}

QVector<double> CalcUnit::randomData()
{
    return _randomValues;
}

double CalcUnit::normalDistributionFunction(double x, double mean, double dispersion)
{
    boost::math::normal_distribution<> normalDist(mean, std::sqrt(dispersion));
    return normilize(boost::math::cdf(normalDist, x), 4);
}

