#ifndef PARAMETERSINPUTDIALOG_H
#define PARAMETERSINPUTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QPushButton>

class ParametersDialog : public QDialog
{
public:
    ParametersDialog(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Введите параметры");

        startLineEdit = new QLineEdit(this);
        endLineEdit = new QLineEdit(this);
        valueLineEdit = new QLineEdit(this);

        auto double_validator = new QDoubleValidator(this);
        double_validator->setLocale(QLocale(QLocale::English));
        double_validator->setRange(-10.0, 10.0, 2);

        auto int_validator = new QIntValidator(this);
        int_validator->setLocale(QLocale(QLocale::English));
        int_validator->setRange(-10.0, 10.0);

        startLineEdit->setValidator(int_validator);
        endLineEdit->setValidator(int_validator);
        valueLineEdit->setValidator(double_validator);

        auto formLayout = new QFormLayout;
        formLayout->addRow("Начало диапазона:", startLineEdit);
        formLayout->addRow("Конец диапазона:", endLineEdit);
        formLayout->addRow("Параметр а:", valueLineEdit);

        auto okButton = new QPushButton("OK", this);
        auto cancelButton = new QPushButton("Отмена", this);

        connect(okButton, &QPushButton::clicked, this, &ParametersDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &ParametersDialog::reject);

        auto buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        auto mainLayout = new QVBoxLayout;
        mainLayout->addLayout(formLayout);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);
    }

    int getStartValue() const { return startLineEdit->text().toInt(); }
    int getEndValue() const { return endLineEdit->text().toInt(); }
    double getValueA() const { return valueLineEdit->text().toDouble(); }

private:
    QLineEdit *startLineEdit;
    QLineEdit *endLineEdit;
    QLineEdit *valueLineEdit;
};

#endif // PARAMETERSINPUTDIALOG_H
