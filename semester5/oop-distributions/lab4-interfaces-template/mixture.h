#pragma once
#ifndef MIXTURE_H
#define MIXTURE_H

#include "interfaces.h"
#include <stdio.h>
#include <stdexcept>
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template<class Distribution1, class Distribution2>
class Mixture : public IDistribution, public IPersistent {
private:
    double p; // параметр смеси
    Distribution1 d1; // первый компонент смеси
    Distribution2 d2; // второй компонент смеси

public:
    // Конструкторы
    Mixture(const Distribution1& prim1 = Distribution1(), const Distribution2& prim2 = Distribution2(), double p0 = 0.5)
        : p((p0 > 0 && p0 < 1) ? p0 : throw std::runtime_error("Некорректный параметр смеси.")),
        d1(prim1), d2(prim2) {
    }

    Mixture(FILE* in) { load(in); } // из файла

    // Функции доступа к компонентам
    Distribution1& component1() { return d1; } // получение ссылки на первый компонент
    Distribution2& component2() { return d2; } // получение ссылки на второй компонент
    const Distribution1& component1() const { return d1; } // получение константной ссылки на первый компонент
    const Distribution2& component2() const { return d2; } // получение константной ссылки на второй компонент

    // Set-функции
    void setP(double newP); // установка параметра смеси

    // Get-функции
    double getP() const { return p; } // получение параметра смеси

    // Интерфейс IDistribution
    double density(double x) const override; // плотность в точке x
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const override;
    double random() const override; // генерация случайной величины

    // Интерфейс IPersistent
    void save(FILE* out) const override;
    void load(FILE* in) override;
};

// Set-функция
template<class Distribution1, class Distribution2>
void Mixture<Distribution1, Distribution2>::setP(double newP) {
    if (newP <= 0 || newP >= 1) {
        throw std::runtime_error("Некорректный параметр смеси.");
    }
    p = newP;
}

// Плотность смеси
template<class Distribution1, class Distribution2>
double Mixture<Distribution1, Distribution2>::density(double x) const {
    return p * d1.density(x) + (1.0 - p) * d2.density(x);
}

// Характеристики
template<class Distribution1, class Distribution2>
void Mixture<Distribution1, Distribution2>::characteristics(double* M, double* D, double* gamma1, double* gamma2) const {
    double M1, D1, gamma11, gamma21;
    double M2, D2, gamma12, gamma22;

    d1.characteristics(&M1, &D1, &gamma11, &gamma21);
    d2.characteristics(&M2, &D2, &gamma12, &gamma22);

    // Матожидание
    if (std::isinf(M1) || std::isinf(M2)) {
        *M = INFINITY;
    }
    else {
        *M = p * M1 + (1.0 - p) * M2;
    }

    //Дисперсия
    if (std::isinf(D1) || std::isinf(D2)) {
        *D = INFINITY;
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    *D = p * (D1 + M1 * M1) + (1.0 - p) * (D2 + M2 * M2) - (*M) * (*M);

    if (*D <= 0) {
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    if (std::isinf(gamma11) || std::isinf(gamma12) || std::isinf(gamma21) || std::isinf(gamma22)) {
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    // Третий момент
    double mu3 = p * (gamma11 * pow(D1, 1.5) + 3 * M1 * D1 + pow(M1, 3)) +
        (1.0 - p) * (gamma12 * pow(D2, 1.5) + 3 * M2 * D2 + pow(M2, 3)) -
        3 * (*M) * (*D + (*M) * (*M)) + 2 * pow(*M, 3);

    // Коэффициент асимметрии
    *gamma1 = mu3 / pow(*D, 1.5);

    // Четвертый момент
    double mu4 = p * ((gamma21 + 3) * D1 * D1 + 4 * gamma11 * M1 * pow(D1, 1.5) +
        6 * M1 * M1 * D1 + pow(M1, 4)) +
        (1.0 - p) * ((gamma22 + 3) * D2 * D2 + 4 * gamma12 * M2 * pow(D2, 1.5) +
            6 * M2 * M2 * D2 + pow(M2, 4)) -
        4 * (*M) * mu3 - 6 * (*M) * (*M) * (*D) - pow(*M, 4);

    // Коэффициент эксцесса
    *gamma2 = mu4 / (*D * *D) - 3;
}

// Генерация случайной величины
template<class Distribution1, class Distribution2>
double Mixture<Distribution1, Distribution2>::random() const {
    double u = (double)rand() / RAND_MAX;

    if (u < p) {
        return d1.random();
    }
    else {
        return d2.random();
    }
}

// Сохранение в файл
template<class Distribution1, class Distribution2>
void Mixture<Distribution1, Distribution2>::save(FILE* out) const {
    d1.save(out);
    d2.save(out);
    fprintf(out, "%lf\n", p);
}

// Загрузка из файла
template<class Distribution1, class Distribution2>
void Mixture<Distribution1, Distribution2>::load(FILE* in) {
    d1.load(in);
    d2.load(in);
    fscanf_s(in, "%lf", &p);

    if (p <= 0 || p >= 1) {
        throw std::runtime_error("Некорректный параметр смеси.");
    }
}
#endif
