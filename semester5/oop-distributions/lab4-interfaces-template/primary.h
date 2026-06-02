#pragma once
#ifndef PRIMARY_H
#define PRIMARY_H

#include "interfaces.h"
#include <stdio.h>
#include <stdexcept>

class Primary : public IDistribution, public IPersistent {
private:
    double shift; // параметр сдвига
    double scale; // параметр масштаба
    double form; // параметр формы

    // Вспомогательные атрибуты
    double beta_function(double a, double b) const;

public:
    // Конструкторы
    Primary(double mu = 0.0, double lambda = 1.0, double nu = 1.0); // параметры по умолчанию
    Primary(FILE* in) { load(in); } // параметры из файла

    // Set-функции
    void setShift(double newShift); // установка параметра сдвига
    void setScale(double newScale); // установка параметра масштаба
    void setForm(double newForm); // установка параметра формы

    // Get-функции
    double getShift() const; // получение параметра сдвига
    double getScale() const; // получение параметра масштаба
    double getForm() const; // получение параметра формы

    // Интерфейс IDistribution
    double density(double x) const override; // вычисление плотности в точке x
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const override;
    double random() const override; // генерация случайной величины из распределения

    // Интерфейс IPersistent
    void save(FILE* out) const override;
    void load(FILE* in) override;
};
#endif
