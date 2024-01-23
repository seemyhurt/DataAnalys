#ifndef CALCUNIT_H
#define CALCUNIT_H

#include <cmath>
#include <functional>

/**
 * @class CalcUnit
 * @brief Класс, представляющий расчетник характеристик случайной величины.
 * @details Класс рассчитывает характеристикики для функции плотности вида C(x+a) на заданном диапазоне.
 */
class calc_unit
{

public:
    /**
     * @brief Конструктор для класса CalcUnit.
     * @param value_a Значение 'a', используемое в функциях распределения.
     * @param lower_value Нижняя граница распределения.
     * @param top_value Верхняя граница распределения.
     */
    calc_unit(double value_a ,
             int lower_value,
             int top_value);

    /**
     * @brief Получить константное значение, используемое в функциях распределения.
     * @return Константное значение.
     */
    double const_value() const;

    /**
     * @brief Получить математическое ожидание распределения.
     * @return Математическое ожидание.
     */
    double expected_value() const;

    /**
     * @brief Получить дисперсию распределения.
     * @return Значение дисперсии.
     */
    double dispersion() const;

    /**
     * @brief Получить медиану распределения.
     * @return Значение медианы.
     */
    double median() const;

    /**
     * @brief Получить моду распределения.
     * @return Значение моды.
     */
    double mode_value() const;

    /**
     * @brief Получить среднеквадратичное отклонение
     * @return Значение среднеквадратичного отклонения.
     */
    double standard_deviation() const;

    /**
     * @brief Функция плотности вероятности с константным значением.
     * @param x Входное значение.
     * @return Значение функции плотности вероятности с константой.
     */
    double pdf_with_const(double x) const;

    /**
     * @brief Функция распределения вероятности.
     * @param x Входное значение.
     * @return Значение функции распределения вероятности.
     */
    double distribution_function(double x) const;

private:
    /**
     * @brief Функция плотности вероятности для распределения.
     * @param x Входное значение.
     * @return Значение функции плотности вероятности.
     */
    double probability_density_function(double x) const;

    /**
     * @brief Функция для нахождения математического ожидания.
     * @param x Входное значение.
     * @return Значение функции для расчета математического ожидания.
     */
    double exp_value_function(double x) const;

    /**
     * @brief Функция для нахождения дисперсии.
     * @param x Входное значение.
     * @return Значение функции для расчета дисперсии.
     */
    double dispersion_function(double x) const;


    /**
     * @brief Функция первообразной от функции плотности распределения.
     * @param x Входное значение.
     * @return Значение первообразной функции.
     */
    double primal_function(double x) const;

    /**
     * @brief Функция интеграции с использованием метода Симпсона.
     * @param func Функция для интегрирования.
     * @param a Нижняя граница интегрирования.
     * @param b Верхняя граница интегрирования.
     * @return Результат интегрирования.
     */
    double integrate(const std::function<double(double)> &func, double a, double b, int n) const;

    /**
     * @brief Вычислить константу C для функции плотности вероятности.
     */
    void calculate_const_value();

    /**
     * @brief Вычислить математическое ожидание.
     */
    void calculate_expected_value();

    /**
     * @brief Вычислить дисперсию.
     */
    void calculate_dispersion();

    /**
     * @brief Вычислить медиану.
     */
    void calculate_median();

    /**
     * @brief Вычислить моду.
     */
    void calculate_mode();

private:
    double _value_a;
    int _lower_value;
    int _top_value;

    double _const_value;
    double _expected_value;
    double _dispersion;
    double _median;
    double _mode_value;
};

#endif // CALCUNIT_H
