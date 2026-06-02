#pragma once
#ifndef MIXTURE_H
#define MIXTURE_H

#include "primary.h"
#include <stdio.h>
#include <stdexcept>

class Mixture {
private:
    double p; // параметр смеси
    Primary d1; // первый компонент смеси
    Primary d2; // второй компонент смеси

public:
    // Конструкторы
    Mixture(const Primary& prim1 = Primary(), const Primary& prim2 = Primary(), double p0 = 0.5); // параметры по умолчанию
    Mixture(FILE* in) { load(in); } // из файла

    // Функции доступа к компонентам
    Primary& component1() { return d1; } // получение ссылки на первый компонент
    Primary& component2() { return d2; } // получение ссылку на второй компонент
    const Primary& component1() const { return d1; } // получение константной ссылки на первый компонент 
    const Primary& component2() const { return d2; } // получение константной ссылки на второй компонент 

    // Set-функции
    void setP(double newP); // установка параметра смеси

    // Get-функции
    double getP() const { return p; } // получение параметра смеси
     
    // Плотность, характеристики и генерация случайной величины
    double density(double x) const; // плотность в точке x
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const;
    double random() const; // генерация случайной величины

    // Персистентность
    void save(FILE* out) const;
    void load(FILE* in);
};

#endif
