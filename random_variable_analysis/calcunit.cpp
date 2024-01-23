#include "calcunit.h"

#include <cmath>
#include <random>
#include <algorithm>
#include <QHash>
#include <QtMath>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>
#include <QTextStream>
#include <QDebug>
#include <QDir>

double normilize(double value, int decimal)
{
    int scale = std::pow(10, decimal);
    return std::round(value * scale) / scale;
}

CalcUnit::CalcUnit(int variantNumber) :
    _variantNumber(variantNumber)
{
    QString dataFilePrefix = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString separator = "_";

    QVector<int> sizes = QVector<int>() << 15 << 30 << 100 << 1000;
    QVector<int> coeffs = QVector<int>() << 100 << 20;

    QString gaussSuffix = "/gauss";
    QString uniformSuffix = "/uniform";
    for (const auto & coef : qAsConst(coeffs))
    {
        for (const auto & size : qAsConst(sizes))
        {
            {
                auto gaussfileName = dataFilePrefix + gaussSuffix +
                                     separator + QString::number(coef) +
                                     separator + QString::number(size) + ".txt";

                auto gaussData = readDataFromFile(gaussfileName);
                if (gaussData.isEmpty())
                {
                    auto noise = generateGaussRandomForm(coef, size);
                    gaussData = makeParametersSeries(noise);
                    writeDataToFile(gaussfileName, gaussData);
                }
                _gaussSeries.insert({gaussData.size(), coef}, gaussData);


            }

            {
                auto uniformfileName = dataFilePrefix + uniformSuffix +
                                       separator +  QString::number(coef) +
                                       separator + QString::number(size) + ".txt";

                auto uniformData = readDataFromFile(uniformfileName);
                if (uniformData.isEmpty())
                {
                    auto noise = generateUniformRandomForm(coef, size);
                    uniformData = makeParametersSeries(noise);
                    writeDataToFile(uniformfileName, uniformData);
                }
                _uniformSeries.insert({uniformData.size(), coef}, uniformData);


            }
        }
    }
}

QVector<double> CalcUnit::readDataFromFile(const QString& filePath)
{
    QVector<double> data;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for reading:" << filePath;
        return data;
    }

    QTextStream in(&file);

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
            return QVector<double>();
        }
    }

    file.close();
    return data;
}

void CalcUnit::writeDataToFile(const QString& filePath, const QVector<double>& data)
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing:" << filePath << file.errorString();
        return;
    }

    QTextStream out(&file);
    for (const auto& value : qAsConst(data))
        out << QString::number(value) << "\n";

    file.close();
}

Statistics CalcUnit::calculateStatistics(const QVector<double> &data)
{
    Statistics result;

    int k = 3;
    if (data.size() < 10)
        k = 1;
    else if (data.size() <= 15)
        k = 2;

    if (data.isEmpty() || k >= data.size() / 2)
        return result;

    QVector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    result.expectedValue = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    result.halfSum = (sortedData.first() + sortedData.last()) / 2;

    result.median = sortedData.size() % 2 == 0
                    ? (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0
                    : sortedData[sortedData.size() / 2];

    result.average = std::accumulate(sortedData.begin() + k, sortedData.end() - k, 0.0) / (sortedData.size() - 2 * k);

    result.dispersion = 0.0;
    for (const auto & value : qAsConst(data))
        result.dispersion += std::pow(value - result.expectedValue, 2);

    result.dispersion = result.dispersion * (1.0 / (data.size() - 1.0));
    result.standardDeviation = std::sqrt(result.dispersion);

    result.expectedValue = normilize(result.expectedValue, 5);
    result.halfSum = normilize(result.halfSum, 5);
    result.median = normilize(result.median, 5);
    result.average = normilize(result.average, 5);
    result.dispersion = normilize(result.dispersion, 5);
    result.standardDeviation = normilize(result.standardDeviation, 5);;

    result.expectedValueDelta = std::abs(_variantNumber - result.expectedValue) * 100 / _variantNumber;
    result.halfSumDelta = std::abs(_variantNumber - result.halfSum) * 100 / _variantNumber;
    result.medianDelta = std::abs(_variantNumber - result.median) * 100 / _variantNumber;
    result.averageDelta = std::abs(_variantNumber - result.average) * 100 / _variantNumber;

    result.expectedValueDelta = normilize(result.expectedValueDelta, 5);
    result.halfSumDelta = normilize(result.halfSumDelta, 5);
    result.medianDelta = normilize(result.medianDelta, 5);
    result.averageDelta = normilize(result.averageDelta, 5);

    return result;
}

QVector<int> CalcUnit::createHistogramSet(const QVector<double>& data, int size)
{
    auto min_max = std::minmax_element(data.begin(), data.end());

    double range = min_max.second - min_max.first;
    double delta = range / size;

    QVector<int> hist(size, 0);
    for (const auto &value : data)
    {
        for (int i = 0; i < size; ++i)
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

QVector<double> CalcUnit::uniformElements(int size, double coeff)
{
    return _uniformSeries.value({size, coeff});
}

QVector<double> CalcUnit::gaussElements(int size, double coeff)
{
    return _gaussSeries.value({size, coeff});
}

QVector<double> CalcUnit::generateUniformRandomForm(double coef, int size)
{
    const double variableA = -_variantNumber / coef;
    const double variableB = _variantNumber / coef;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
    {
        double value = variableA + dis(gen) * (variableB - variableA);
        result[i] = normilize(value, 5);
    }

    return result;
}

QVector<double> CalcUnit::generateGaussRandomForm(double coef, int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    const double sko = _variantNumber / coef;
    const double Mo = 0.0;

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
    {
        double r1 = dis(gen);
        double r2 = dis(gen);

        double z = std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * M_PI * r2);
        double value = Mo + (z * (sko));
        result[i] = normilize(value, 5);
    }

    return result;
}

QVector<double> CalcUnit::makeParametersSeries(const QVector<double> &noise)
{
    QVector<double> result(noise.size(), 0.0);
    for (int i = 0; i < noise.size(); i++)
    {
        result[i] = _variantNumber + noise[i];
    }
    return result;
}
