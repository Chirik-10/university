#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "distribution.h"
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Вспомогательная функция для вычисления бета-функции
double beta_function(double a, double b) {
    return exp(lgamma(a) + lgamma(b) - lgamma(a + b));
}

// Плотность распределения Пирсона в точке x
double base_density(double x, base_distribution d) {
    if (d.lambda <= 0 || d.nu <= 0.5) {
        return 0.0;
    }

    // Стандартизация
    double z = (x - d.mu) / d.lambda;
    double nu = d.nu; // параметр формы nu

    double beta_val = beta_function(nu - 0.5, 0.5);
    double base_dens = pow(1.0 + z * z, -nu) / beta_val;

    // Масштабирование
    return base_dens / d.lambda;
}

// Вычисление характеристик распределения Пирсона
void base_characteristics(base_distribution d, double* M, double* D, double* gamma1, double* gamma2) {
    double nu = d.nu;

    // Математическое ожидание существует при nu > 1
    if (nu > 1.0) {
        *M = d.mu;
    }
    else {
        *M = INFINITY;
    }

    // Дисперсия существует при nu > 1.5: sigma^2 = 1/(2nu - 3)
    if (nu > 1.5) {
        *D = 1.0 / (2.0 * nu - 3.0) * d.lambda * d.lambda;
    }
    else {
        *D = INFINITY;
    }

    // Коэффициент асимметрии (симметричное распределение)
    *gamma1 = 0.0;

    // Коэффициент эксцесса существует при nu > 2.5: gamma2 = 6/(2nu - 5)
    if (nu > 2.5) {
        *gamma2 = 6.0 / (2.0 * nu - 5.0);
    }
    else {
        *gamma2 = INFINITY;
    }
}

// Генерация случайной величины распределения Пирсона методом обратного преобразования
double base_random(base_distribution d) {
    if (d.lambda <= 0 || d.nu <= 0.5) {
        return 0.0;
    }

    // Генерация равномерно распределенных случайных величин
    double r1, r2;
    do {
        r1 = (double)rand() / RAND_MAX;
        r2 = (double)rand() / RAND_MAX;
    } while (r1 == 0.0 || r2 == 0.0);

    // Преобразование Бокса-Мюллера для генерации распределения Пирсона
    double nu = d.nu;
    double exponent = -1.0 / (nu - 0.5);
    double x = sqrt(pow(r1, exponent) - 1.0) * cos(2.0 * M_PI * r2);

    // Сдвиг-масштабирование преобразование
    return d.mu + d.lambda * x;
}

// Плотность смеси распределений
double mixture_density(double x, mixture_distribution m) {
    return m.p * base_density(x, m.d1) + (1.0 - m.p) * base_density(x, m.d2);
}

void mixture_characteristics(mixture_distribution m, double* M, double* D, double* gamma1, double* gamma2) {
    double M1, D1, gamma11, gamma21;
    double M2, D2, gamma12, gamma22;

    // Характеристики компонентов
    base_characteristics(m.d1, &M1, &D1, &gamma11, &gamma21);
    base_characteristics(m.d2, &M2, &D2, &gamma12, &gamma22);

    // Проверяем, существуют ли матожидания компонентов
    if (isinf(M1) || isinf(M2)) {
        *M = INFINITY;
    }
    else {
        *M = m.p * M1 + (1.0 - m.p) * M2;
    }

    // Проверяем, существуют ли дисперсии компонентов
    if (isinf(D1) || isinf(D2)) {
        *D = INFINITY;
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    *D = m.p * (D1 + M1 * M1) + (1.0 - m.p) * (D2 + M2 * M2) - (*M) * (*M);

    if (*D <= 0) {
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    // Проверяем, существуют ли коэффициенты асимметрии и эксцесса компонентов
    if (isinf(gamma21) || isinf(gamma22)) {
        *gamma1 = INFINITY;
        *gamma2 = INFINITY;
        return;
    }

    // Третий центральный момент для асимметрии
    double mu3 = m.p * (gamma11 * pow(D1, 1.5) + 3 * M1 * D1 + pow(M1, 3)) +
        (1.0 - m.p) * (gamma12 * pow(D2, 1.5) + 3 * M2 * D2 + pow(M2, 3)) -
        3 * (*M) * (*D + (*M) * (*M)) + 2 * pow(*M, 3);

    // Коэффициент асимметрии
    *gamma1 = mu3 / pow(*D, 1.5);

    // Четвертый центральный момент для эксцесса
    double mu4 = m.p * ((gamma21 + 3) * D1 * D1 + 4 * gamma11 * M1 * pow(D1, 1.5) +
        6 * M1 * M1 * D1 + pow(M1, 4)) +
        (1.0 - m.p) * ((gamma22 + 3) * D2 * D2 + 4 * gamma12 * M2 * pow(D2, 1.5) +
            6 * M2 * M2 * D2 + pow(M2, 4)) -
        4 * (*M) * mu3 - 6 * (*M) * (*M) * (*D) - pow(*M, 4);

    // Коэффициент эксцесса
    *gamma2 = mu4 / (*D * *D) - 3;
}

// Генерация случайной величины для смеси
double mixture_random(mixture_distribution m) {
    double u = (double)rand() / RAND_MAX;

    if (u < m.p) {
        return base_random(m.d1);
    }
    else {
        return base_random(m.d2);
    }
}

// Создание эмпирического распределения по выборке
empirical_distribution create_empirical(double* data, int size, int bin_count) {
    empirical_distribution emp;
    emp.data = data;
    emp.size = size;

    // Количество интервалов
    if (bin_count <= 0) {
        bin_count = 50;
    }
    emp.bin_count = bin_count;

    // Поиск минимума и максимума в данных
    double min_val = data[0];
    double max_val = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }

    // Расширение границ для обеспечения покрытия
    double range = max_val - min_val;
    min_val -= 0.1 * range;
    max_val += 0.1 * range;
    range = max_val - min_val;

    // Выделение памяти для центров интервалов и плотностей
    emp.bins = new double[bin_count];
    emp.densities = new double[bin_count];

    // Ширина интервала
    double bin_width = range / bin_count;
    // Подсчет частот
    int* counts = new int[bin_count]();

    // Распределение данных по интервалам
    for (int i = 0; i < size; i++) {
        int bin_index = (int)((data[i] - min_val) / bin_width);
        if (bin_index >= 0 && bin_index < bin_count) {
            counts[bin_index]++;
        }
    }

    // Вычисление центров интервалов и плотностей
    for (int i = 0; i < bin_count; i++) {
        emp.bins[i] = min_val + (i + 0.5) * bin_width;
        emp.densities[i] = (double)counts[i] / (size * bin_width);
    }

    delete[] counts;
    return emp;
}

// Плотность эмпирического распределения в точке x
double empirical_density(double x, empirical_distribution emp) {
    // Определение границ гистограммы
    double min_val = emp.bins[0] - (emp.bins[1] - emp.bins[0]) / 2;
    double max_val = emp.bins[emp.bin_count - 1] + (emp.bins[emp.bin_count - 1] - emp.bins[emp.bin_count - 2]) / 2;

    if (x < min_val || x > max_val) {
        return 0.0;
    }

    double bin_width = (max_val - min_val) / emp.bin_count; // ширина интервала

    int bin_index = (int)((x - min_val) / bin_width); // определение номера интервала

    if (bin_index >= 0 && bin_index < emp.bin_count) {
        return emp.densities[bin_index];
    }

    return 0.0;
}

// Вычисление характеристик эмпирического распределения
void empirical_characteristics(empirical_distribution emp, double* M, double* D, double* gamma1, double* gamma2) {
    double sum = 0.0;
    double sum_sq = 0.0;

    // Вычисление суммы и суммы квадратов
    for (int i = 0; i < emp.size; i++) {
        sum += emp.data[i];
        sum_sq += emp.data[i] * emp.data[i];
    }

    *M = sum / emp.size;
    *D = (sum_sq / emp.size) - (*M) * (*M);

    // Вычисление третьего и четвертого моментов
    double sum_cube = 0.0;
    double sum_quad = 0.0;

    for (int i = 0; i < emp.size; i++) {
        double dev = emp.data[i] - *M;
        sum_cube += dev * dev * dev;
        sum_quad += dev * dev * dev * dev;
    }

    double mu3 = sum_cube / emp.size;
    double mu4 = sum_quad / emp.size;

    *gamma1 = mu3 / pow(*D, 1.5);
    *gamma2 = mu4 / (*D * *D) - 3;
}

// Генерация случайной величины из эмпирического распределения
double empirical_random(empirical_distribution emp) {
    double total_density = 0.0;
    for (int i = 0; i < emp.bin_count; i++) {
        total_density += emp.densities[i];
    }

    double u = (double)rand() / RAND_MAX * total_density;
    double cumulative = 0.0;
    int selected_bin = 0;

    for (int i = 0; i < emp.bin_count; i++) {
        cumulative += emp.densities[i];
        if (u <= cumulative) {
            selected_bin = i;
            break;
        }
    }

    double bin_width = (emp.bins[1] - emp.bins[0]);
    double bin_min = emp.bins[selected_bin] - bin_width / 2;
    double bin_max = emp.bins[selected_bin] + bin_width / 2;

    return bin_min + (double)rand() / RAND_MAX * (bin_max - bin_min);
}

// Освобождение памяти эмпирического распределения
void free_empirical(empirical_distribution emp) {
    delete[] emp.bins;
    delete[] emp.densities;
}

// Функции для записи данных в файлы
void write_theoretical_density(double mu, double lambda, double nu,
    double min, double max, double step,
    const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    base_distribution d = { mu, lambda, nu };
    for (double x = min; x <= max; x += step) {
        fprintf(file, "%.6f\t%.6f\n", x, base_density(x, d));
    }

    fclose(file);
}

void write_empirical_density(double* centers, double* densities,
    int bins, const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    for (int b = 0; b < bins; b++) {
        fprintf(file, "%.6f\t%.6f\n", centers[b], densities[b]);
    }

    fclose(file);
}

void write_sample(double* arr, int n, const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%.6f\n", arr[i]);
    }

    fclose(file);
}

void write_theoretical_mix_density(mixture_distribution m,
    double min, double max, double step,
    const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    for (double x = min; x <= max; x += step) {
        fprintf(file, "%.6f\t%.6f\n", x, mixture_density(x, m));
    }

    fclose(file);
}

void write_empirical_mix_density(double* centers, double* densities,
    int bins, const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    for (int b = 0; b < bins; b++) {
        fprintf(file, "%.6f\t%.6f\n", centers[b], densities[b]);
    }

    fclose(file);
}

void write_mix_sample_data(double* arr, int n, const char* filename) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) {
        printf("Ошибка открытия файла %s\n", filename);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%.6f\n", arr[i]);
    }

    fclose(file);
}
