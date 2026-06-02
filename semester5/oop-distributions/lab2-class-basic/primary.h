#pragma once
#ifndef PRIMARY_H
#define PRIMARY_H
#include <stdio.h>

class Primary {
private:
    double shift; 
    double scale;
    double form;

    // Вспомогательные атрибуты
    double normalization_const;

    void const_update();
    double beta_function(double a, double b) const;

public:
    // Конструкторы
    Primary(double mu = 0.0, double lambda = 1.0, double nu = 1.0);
    Primary(FILE* in);

    // Set-функции
    void setShift(double newShift);
    void setScale(double newScale);
    void setForm(double newForm);

    // Get-функции
    double getShift() const;
    double getScale() const;
    double getForm() const;

    // Плотность, характеристики и генерация случайной величины
    double density(double x) const;
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const;
    double random() const;

    // Персистентность
    void save(FILE* out) const;
    void load(FILE* in);
};
#endif
