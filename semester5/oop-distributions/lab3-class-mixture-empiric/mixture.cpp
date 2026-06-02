#include "mixture.h"
#include <cmath>
#include <cstdlib>

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Конструктор с параметрами
Mixture::Mixture(const Primary& prim1, const Primary& prim2, double p0)
    : p((p0 > 0 && p0 < 1) ? p0 : throw runtime_error("Некорректный параметр смеси.")),
    d1(prim1), d2(prim2) {
}

// Set-функция
void Mixture::setP(double newP) {
    if (newP <= 0 || newP >= 1) {
        throw runtime_error("Некорректный параметр смеси.");
    }
    p = newP;
}

// Плотность смеси
double Mixture::density(double x) const {
    return p * d1.density(x) + (1.0 - p) * d2.density(x);
}

// Характеристики
void Mixture::characteristics(double* M, double* D, double* gamma1, double* gamma2) const {
    double M1, D1, gamma11, gamma21;
    double M2, D2, gamma12, gamma22;

    d1.characteristics(&M1, &D1, &gamma11, &gamma21);
    d2.characteristics(&M2, &D2, &gamma12, &gamma22);

    // Матожидание
    if (isinf(M1) || isinf(M2)) {
        *M = INFINITY;
    }
    else {
        *M = p * M1 + (1.0 - p) * M2;
    }

    //Дисперсия
    if (isinf(D1) || isinf(D2)) {
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

    if (isinf(gamma11) || isinf(gamma12) || isinf(gamma21) || isinf(gamma22)) {
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
double Mixture::random() const {
    double u = (double)rand() / RAND_MAX;

    if (u < p) {
        double r1, r2;
        do {
            r1 = (double)rand() / RAND_MAX;
            r2 = (double)rand() / RAND_MAX;
        } while (r1 == 0.0 || r2 == 0.0);

        double exponent = -1.0 / (d1.getForm() - 0.5);
        double x = sqrt(pow(r1, exponent) - 1.0) * cos(2.0 * M_PI * r2);
        return d1.getShift() + d1.getScale() * x;
    }
    else {
        double r1, r2;
        do {
            r1 = (double)rand() / RAND_MAX;
            r2 = (double)rand() / RAND_MAX;
        } while (r1 == 0.0 || r2 == 0.0);

        double exponent = -1.0 / (d2.getForm() - 0.5);
        double x = sqrt(pow(r1, exponent) - 1.0) * cos(2.0 * M_PI * r2);
        return d2.getShift() + d2.getScale() * x;
    }
}

// Сохранение в файл
void Mixture::save(FILE* out) const {
    d1.save(out);
    d2.save(out);
    fprintf(out, "%lf\n", p);
}

// Загрузка из файла
void Mixture::load(FILE* in) {
    d1.load(in);
    d2.load(in);
    fscanf_s(in, "%lf", &p);

    if (p <= 0 || p >= 1) {
        throw runtime_error("Некорректный параметр смеси.");
    }
}
