#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QTableWidget;
class CalcUnit;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void addDataToTable(QTableWidget * table, int coeff, bool isGauss);
    void addCenteredItem(const QString &text,
                         const QString &tooltip,
                         QTableWidget *widget,
                         int row, int column);

private:
    QSharedPointer<CalcUnit> _unit;
};
#endif // WIDGET_H
