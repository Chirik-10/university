#include "primary.h"
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Конструктор с параметрами
Primary::Primary(double mu, double lambda, double nu) {
    if (lambda <= 0) throw runtime_error("Некорректное значение масштаба.");
    if (nu <= 0.5) throw runtime_error("Некорректное значение формы.");

    shift = mu;
    scale = lambda;
    form = nu;
}

// Бета-функция
double Primary::beta_function(double a, double b) const {
    return exp(lgamma(a) + lgamma(b) - lgamma(a + b));
}

// Set-функции
void Primary::setShift(double newShift) {
    shift = newShift;
}

void Primary::setScale(double newScale) {
    if (newScale <= 0) throw runtime_error("Некорректное значение масштаба.");
    scale = newScale;
}

void Primary::setForm(double newForm) {
    if (newForm <= 0.5) throw runtime_error("Некорректное значение формы.");
    form = newForm;
}

// Get-функции
double Primary::getShift() const { return shift; }
double Primary::getScale() const { return scale; }
double Primary::getForm() const { return form; }

// Плотность распределения
double Primary::density(double x) const {
    double z = (x - shift) / scale;
    double base_dens = pow(1.0 + z * z, -form) / beta_function(form - 0.5, 0.5);
    return base_dens / scale;
}

// Характеристики распределения
void Primary::characteristics(double* M, double* D, double* gamma1, double* gamma2) const {
    if (form > 1.0) {
        *M = shift;
    }
    else {
        *M = INFINITY;
    }

    if (form > 1.5) {
        *D = 1.0 / (2.0 * form - 3.0) * scale * scale;
    }
    else {
        *D = INFINITY;
    }

    *gamma1 = 0.0;

    if (form > 2.5) {
        *gamma2 = 6.0 / (2.0 * form - 5.0);
    }
    else {
        *gamma2 = INFINITY;
    }
}

// Моделирование случайной величины
double Primary::random() const {
    double r1, r2;
    do {
        r1 = (double)rand() / RAND_MAX;
        r2 = (double)rand() / RAND_MAX;
    } while (r1 == 0.0 || r2 == 0.0);

    double exponent = -1.0 / (form - 0.5);
    double x = sqrt(pow(r1, exponent) - 1.0) * cos(2.0 * M_PI * r2);

    return shift + scale * x;
}

// Сохранение в файл
void Primary::save(FILE* out) const {
    fprintf(out, "%lf %lf %lf\n", shift, scale, form);
}

// Загрузка из файла
void Primary::load(FILE* in) {
    double mu, lambda, nu;
    fscanf_s(in, "%lf %lf %lf", &mu, &lambda, &nu);

    if (lambda <= 0) throw runtime_error("Некорректное значение масштаба.");
    if (nu <= 0.5) throw runtime_error("Некорректное значение формы.");

    shift = mu;
    scale = lambda;
    form = nu;
}
