#include "calcunit.h"
#include <cmath>
#include "QDebug"

CalcUnit::CalcUnit(const QVector<double> xValues,
                   const QVector<double> yValues) :
    _xValues(xValues),
    _yValues(yValues)
{
    _linear_res = calculateLinearRegress();
    _squared_res = calculateQuadraticRegress();
    _cubic_res = calculateCubicRegress();
}

double CalcUnit::linear_function(double x)
{
    return _linear_res.a * x +
           _linear_res.b;
}

double CalcUnit::quadratic_function(double x)
{
    return _squared_res.a * pow(x, 2) +
           _squared_res.b * x +
           _squared_res.c;
}

double CalcUnit::cubic_function(double x)
{
    return _cubic_res.a * pow(x, 3) +
           _cubic_res.b * pow(x, 2) +
           _cubic_res.c * x +
           _cubic_res.d;
}

DispersionResult CalcUnit::getDispersion()
{
    DispersionResult result;
    for (int i = 0; i < _yValues.size(); i++)
    {
        result.linear_dispersion += pow(_yValues[i] - linear_function(_xValues[i]), 2);
        result.quadratic_dispersion += pow(_yValues[i] - quadratic_function(_xValues[i]), 2);
        result.cubic_dispersion += pow(_yValues[i] - cubic_function(_xValues[i]), 2);
    }

    result.linear_dispersion = result.linear_dispersion / (_yValues.size() - 1);
    result.quadratic_dispersion = result.quadratic_dispersion / (_yValues.size() - 1);
    result.cubic_dispersion = result.cubic_dispersion / (_yValues.size() - 1);
    return result;
}

LinearResult CalcUnit::calculateLinearRegress()
{
    double size = _xValues.size();
    auto sum_x = std::accumulate(_xValues.cbegin(), _xValues.cend(), 0.0);
    auto sum_y = std::accumulate(_yValues.cbegin(), _yValues.cend(), 0.0);

    auto multiplied_vector = multiplyVectorsValues(_xValues, _yValues);
    auto multiplied_sum = std::accumulate(multiplied_vector.cbegin(), multiplied_vector.cend(), 0.0);

    auto squared_x = getPowVector(_xValues, 2);
    auto squared_x_sum = std::accumulate(squared_x.cbegin(), squared_x.cend(), 0.0);

    auto denominator = std::pow(sum_x, 2) - size * squared_x_sum;
    auto coeff_a = (sum_x * sum_y - size * multiplied_sum) / denominator;
    auto coeff_b = (sum_x * multiplied_sum - squared_x_sum * sum_y) / denominator;

    return {coeff_a, coeff_b};
}

QuadraticResult CalcUnit::calculateQuadraticRegress()
{
    double size = _xValues.size();
    auto sum_x = std::accumulate(_xValues.cbegin(), _xValues.cend(), 0.0);
    auto sum_y = std::accumulate(_yValues.cbegin(), _yValues.cend(), 0.0);

    auto squared_x = getPowVector(_xValues, 2);
    auto squared_x_sum = std::accumulate(squared_x.cbegin(), squared_x.cend(), 0.0);

    auto cubed_x = getPowVector(_xValues, 3);
    auto cubed_x_sum = std::accumulate(cubed_x.cbegin(), cubed_x.cend(), 0.0);

    auto fourth_power_x = getPowVector(_xValues, 4);
    auto fourth_power_x_sum = std::accumulate(fourth_power_x.cbegin(), fourth_power_x.cend(), 0.0);

    auto multiplied_vector = multiplyVectorsValues(_xValues, _yValues);
    auto multiplied_sum = std::accumulate(multiplied_vector.cbegin(), multiplied_vector.cend(), 0.0);

    auto multiplied_squared_vector = multiplyVectorsValues(squared_x, _yValues);
    auto multiplied_squared_sum = std::accumulate(multiplied_squared_vector.cbegin(), multiplied_squared_vector.cend(), 0.0);

    QVector<QVector<double>> matrix = {
        {squared_x_sum, sum_x, size},
        {cubed_x_sum, squared_x_sum, sum_x},
        {fourth_power_x_sum, cubed_x_sum, squared_x_sum}
    };

    QVector<double> vector = {sum_y, multiplied_sum, multiplied_squared_sum};

    auto res = calculatesystemOfLinearEquations(matrix, vector);
    return {{res.at(0), res.at(1)}, res.at(2)};
}

CubicResult CalcUnit::calculateCubicRegress()
{
    double size = _xValues.size();
    auto sum_x = std::accumulate(_xValues.cbegin(), _xValues.cend(), 0.0);
    auto sum_y = std::accumulate(_yValues.cbegin(), _yValues.cend(), 0.0);

    auto squared_x = getPowVector(_xValues, 2);
    auto squared_x_sum = std::accumulate(squared_x.cbegin(), squared_x.cend(), 0.0);

    auto cubed_x = getPowVector(_xValues, 3);
    auto cubed_x_sum = std::accumulate(cubed_x.cbegin(), cubed_x.cend(), 0.0);

    auto fourth_power_x = getPowVector(_xValues, 4);
    auto fourth_power_x_sum = std::accumulate(fourth_power_x.cbegin(), fourth_power_x.cend(), 0.0);

    auto fifth_power_x = getPowVector(_xValues, 5);
    auto fifth_power_x_sum = std::accumulate(fifth_power_x.cbegin(), fifth_power_x.cend(), 0.0);

    auto six_power_x = getPowVector(_xValues, 6);
    auto six_power_x_sum = std::accumulate(six_power_x.cbegin(), six_power_x.cend(), 0.0);

    auto multiplied_vector = multiplyVectorsValues(_xValues, _yValues);
    auto multiplied_sum = std::accumulate(multiplied_vector.cbegin(), multiplied_vector.cend(), 0.0);

    auto multiplied_squared_vector = multiplyVectorsValues(squared_x, _yValues);
    auto multiplied_squared_sum = std::accumulate(multiplied_squared_vector.cbegin(), multiplied_squared_vector.cend(), 0.0);

    auto multiplied_cubed_vector = multiplyVectorsValues(cubed_x, _yValues);
    auto multiplied_cubed_sum = std::accumulate(multiplied_cubed_vector.cbegin(), multiplied_cubed_vector.cend(), 0.0);


    QVector<QVector<double>> matrix = {
        {cubed_x_sum, squared_x_sum, sum_x, size},
        {fourth_power_x_sum, cubed_x_sum, squared_x_sum, sum_x},
        {fifth_power_x_sum, fourth_power_x_sum, cubed_x_sum, squared_x_sum},
        {six_power_x_sum, fifth_power_x_sum, fourth_power_x_sum, cubed_x_sum}
    };

    QVector<double> vector = {sum_y, multiplied_sum, multiplied_squared_sum, multiplied_cubed_sum};

    auto res = calculatesystemOfLinearEquations(matrix, vector);
    return {{{res.at(0), res.at(1)}, res.at(2)}, res.at(3)};
}

QVector<double> CalcUnit::getPowVector(const QVector<double> &source, int squared)
{
    QVector<double> result;
    result.reserve(source.size());
    for (const auto &value : qAsConst(source))
    {
        result << std::pow(value, squared);
    }
    return result;
}

QVector<double> CalcUnit::multiplyVectorsValues(const QVector<double> &first,
                                                const QVector<double> &second)
{
    if (first.size() != second.size())
        Q_ASSERT(false);

    QVector<double> result(first.size());
    for (int i = 0; i < first.size(); i++)
    {
        result[i] = first[i] * second[i];
    }
    return result;
}

QVector<double> CalcUnit::calculatesystemOfLinearEquations(const QVector<QVector<double>> &matrix,
                                                 const QVector<double> &vector)
{
    QVector<double> result;
    auto det = determinant(matrix);


    for (int i = 0; i < vector.size(); i++)
    {
        auto copy = matrix;
        for (int j = 0; j < matrix.size(); j++)
        {
            copy[j][i] = vector[j];
        }
        auto copy_det = determinant(copy);
        result << copy_det / det;
    }
    return result;
}

double CalcUnit::determinant(const QVector<QVector<double>> &matrix)
{
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();

    if (rows == 1)
        return matrix[0][0];

    double det = 0.0;

    for (size_t col = 0; col < cols; ++col)
    {
        QVector<QVector<double>> minor;
        for (size_t i = 1; i < rows; ++i)
        {
            QVector<double> row_minor;
            for (size_t j = 0; j < cols; ++j)
            {
                if (j != col)
                    row_minor.push_back(matrix[i][j]);
            }
            minor.push_back(row_minor);
        }

        double sign = (col % 2 == 0) ? 1.0 : -1.0;
        det += sign * matrix[0][col] * determinant(minor);
    }

    return det;
}
