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
    _randomValues.insert({100, false}, {});
    _randomValues.insert({1000, false}, {});
    _randomValues.insert({100, true}, {});
    _randomValues.insert({1000, true}, {});

    QString dataFilePrefix = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    for (auto it = _randomValues.begin(); it != _randomValues.end(); it++)
    {
        auto suffix = it.key().second ? "/gauss" : "/uniform";
        auto fileName = dataFilePrefix + suffix + QString::number(it.key().first) + ".txt";

        if (!readDataFromFile(fileName, it.key().second))
        {
            it.key().second ? generateGaussRandomForm(it.key().first) : generateUniformRandomForm(it.key().first);
            writeDataToFile(fileName, it.value());
        }
    }
}

bool CalcUnit::readDataFromFile(const QString& filePath, bool isGauss)
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

    file.close();

    _randomValues.insert({data.size(), isGauss}, data);
    return true;
}

void CalcUnit::writeDataToFile(const QString& filePath, const QVector<double>& randomData)
{
    QDir().mkpath(QFileInfo(filePath).absolutePath());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file for writing:" << filePath << file.errorString();
        return;
    }

    QTextStream out(&file);
    for (const auto& value : qAsConst(randomData))
        out << QString::number(value) << "\n";

    file.close();
}

std::pair<double, double> min_max_element(const QVector<double>& data)
{
    if (data.empty())
        return {0, 0};

    double min = data[0];
    double max = data[0];
    for (const auto& value : data)
    {
        if (value < min)
            min = value;
        if (value > max)
            max = value;
    }
    return {min, max};
}

double CalcUnit::calculateMode(const QVector<double> &data, int size)
{
    auto hist = createHistogramSet(data, size);
    int maxIndex = 0;
    for (int i = 0; i < hist.size(); i++)
    {
        if (hist[i] > hist[maxIndex])
            maxIndex = i;
    }

    auto min_max = min_max_element(data);
    double range = min_max.second - min_max.first;
    double delta = range / size;
    double start_x = min_max.first + maxIndex * delta;
    double end_x = min_max.first + (maxIndex + 1) * delta;

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
    for (const auto & value : qAsConst(data))
        dispersion += std::pow(value - expectedValue, 2);

    dispersion = dispersion * (1.0 / (data.size() - 1.0));
    double std_dev = std::sqrt(dispersion);

    return {expectedValue, dispersion, median, mode, std_dev};
}

QVector<int> CalcUnit::createHistogramSet(const QVector<double>& data, int size)
{
    auto min_max = min_max_element(data);

    double range = min_max.second - min_max.first;
    double delta = range / size;

    QVector<int> hist(size, 0);
    for (const auto &value : data)
    {
        for (int i = 0; i < size; ++i)
        {
            double start_x = min_max.first + i * delta;
            double end_x = min_max.first + (i + 1) * delta;

            if (value >= start_x && value <= end_x)
            {
                hist[i]++;
                break;
            }
        }
    }
    return hist;
}

QVector<double> CalcUnit::uniformElements(int size)
{
    return _randomValues.value({size, false});
}

QVector<double> CalcUnit::gaussElements(int size)
{
    return _randomValues.value({size, true});
}

void CalcUnit::generateUniformRandom(int size)
{
    const double variableA = -_variantNumber / 10.0;
    const double variableB = _variantNumber / 2.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(variableA, variableB);

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
        result[i] = normilize(dis(gen), 5);

    _randomValues.insert({size, false}, result);
}

void CalcUnit::generateGaussRandom(int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dis(_variantNumber, _variantNumber / 3);

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
        result[i] = normilize(dis(gen), 5);

    _randomValues.insert({size, true}, result);
}

void CalcUnit::generateUniformRandomForm(int size)
{
    const double variableA = -_variantNumber / 10.0;
    const double variableB = _variantNumber / 2.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
    {
        double value = variableA + dis(gen) * (variableB - variableA);
        result[i] = normilize(value, 5);
    }

    _randomValues.insert({size, false}, result);
}

void CalcUnit::generateGaussRandomForm(int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    QVector<double> result(size);
    for (int i = 0; i < size; i++)
    {
        double r1 = dis(gen);
        double r2 = dis(gen);

        double z = std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * M_PI * r2);
        double value = _variantNumber + (z * (_variantNumber / 3.0));
        result[i] = normilize(value, 5);
    }

    _randomValues.insert({size, true}, result);
}
