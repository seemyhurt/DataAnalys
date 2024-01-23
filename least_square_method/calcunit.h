#ifndef CALCUNIT_H
#define CALCUNIT_H

#include <QVector>

struct DispersionResult
{
    double linear_dispersion = 0.0;
    double quadratic_dispersion = 0.0;
    double cubic_dispersion = 0.0;
};

struct LinearResult
{
    double a;
    double b;
};

struct QuadraticResult : public LinearResult
{
    double c;
};

struct CubicResult : public QuadraticResult
{
    double d;
};


static const QVector<double> y_values = {0.1, 1.3, 1.4, 2.1, 3.5};
static const QVector<double> x_values = {1.0, 2.0, 3.0, 4.0, 5.0};

class CalcUnit
{

public:
    CalcUnit(const QVector<double> xValues = x_values,
             const QVector<double> yValues = y_values);

    QVector<double> yValues() { return _yValues; }
    QVector<double> xValues() { return _xValues; }

    double linear_function(double x);
    double quadratic_function(double x);
    double cubic_function(double x);

    DispersionResult getDispersion();

private:
    LinearResult calculateLinearRegress();
    QuadraticResult calculateQuadraticRegress();
    CubicResult calculateCubicRegress();

    QVector<double> getPowVector(const QVector<double> &source, int squared);
    QVector<double> multiplyVectorsValues(const QVector<double> &first, const QVector<double> &second);
    QVector<double> calculatesystemOfLinearEquations(const QVector<QVector<double> > &matrix, const QVector<double> &vector);
    double determinant(const QVector<QVector<double>> &matrix);

private:
    QVector<double> _xValues;
    QVector<double> _yValues;

    LinearResult _linear_res;
    QuadraticResult _squared_res;
    CubicResult _cubic_res;

};

#endif // CALCUNIT_H
