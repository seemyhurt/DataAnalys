#include "calcunit.h"
#include "qglobal.h"
#include <algorithm>

calc_unit::calc_unit(double value_a, int lower_value, int top_value) :
    _value_a(value_a),
    _lower_value(lower_value),
    _top_value(top_value)
{
    calculate_const_value();
    calculate_expected_value();
    calculate_dispersion();
    calculate_median();
    calculate_mode();
}

double calc_unit::const_value() const
{
    return _const_value;
}

double calc_unit::expected_value() const
{
    return _expected_value;
}

double calc_unit::dispersion() const
{
    return _dispersion;
}

double calc_unit::median() const
{
    return _median;
}

double calc_unit::mode_value() const
{
    return _mode_value;
}

double calc_unit::standard_deviation() const
{
    return sqrt(_dispersion);
}

double calc_unit::probability_density_function(double x) const
{
    if (x >= _lower_value && x <= _top_value)
        return x + _value_a;
    else
        return 0.0;
}

double calc_unit::pdf_with_const(double x) const
{
    if (x >= _lower_value && x <= _top_value)
        return _const_value * (x + _value_a);
    else
        return 0.0;
}

double calc_unit::distribution_function(double x) const
{
    auto pdf = [this](double x) { return pdf_with_const(x); };

    if (x < _lower_value)
    {
        auto res = integrate(pdf, -std::numeric_limits<double>::infinity(), x, 1000);
        return std::isnan(res) ? 0 : res;
    }

    if (x >= _lower_value && x <= _top_value)
        return  primal_function(x);

    else
        return integrate(pdf, _lower_value, _top_value, 1000);
}

double calc_unit::exp_value_function(double x) const
{
    return pdf_with_const(x) * x;
}

double calc_unit::dispersion_function(double x) const
{
    return pdf_with_const(x) * pow(x - _expected_value, 2);
}

double calc_unit::primal_function(double x) const
{
    if (x >= _lower_value && x <= _top_value)
        return _const_value * (0.5 * pow(x, 2) + _value_a * x);
    else
        return 0.0;
}

double calc_unit::integrate(const std::function<double (double)> &func, double a, double b, int n) const
{
    double h = (b - a) / n;
    double result = func(a) + func(b);

    for (int i = 1; i < n; i += 2)
        result += 4 * func(a + i * h);


    for (int i = 2; i < n - 1; i += 2)
        result += 2 * func(a + i * h);

    auto integral = h / 3 * result;
    return std::fabs(integral) < std::numeric_limits<double>::epsilon() ? 0.0 : integral;
}

void calc_unit::calculate_const_value()
{
    auto func = [this](double x) { return probability_density_function(x); };
    _const_value = 1.0 / integrate(func, _lower_value, _top_value, 1000);
}

void calc_unit::calculate_dispersion()
{
    auto func = [this](double x) { return dispersion_function(x); };
    _dispersion = integrate(func, _lower_value, _top_value, 1000);
}

void calc_unit::calculate_expected_value()
{
    auto func = [this](double x) { return exp_value_function(x); };
    _expected_value = integrate(func, _lower_value, _top_value, 1000);
}

void calc_unit::calculate_median()
{
    double a = 0.5 * _const_value;
    double b = _const_value * _value_a;
    double c = -0.5;

    double discriminant = b * b - 4 * a * c;

    if (discriminant >= 0)
    {
        double root1 = (-b + std::sqrt(discriminant)) / (2.0 * a);
        double root2 = (-b - std::sqrt(discriminant)) / (2.0 * a);

        if (root1 >= _lower_value && root1 <= _top_value)
            _median = root1;
        else
            _median = root2;
    }
    else Q_ASSERT(true);
}

void calc_unit::calculate_mode()
{
    double max_density = 0.0;
    _mode_value = _lower_value;

    for (double x = _lower_value; x <= _top_value; x += 0.0001)
    {
        auto current_density = pdf_with_const(x);
        if (current_density > max_density)
        {
            max_density = current_density;
            _mode_value = x;
        }
    }
}
