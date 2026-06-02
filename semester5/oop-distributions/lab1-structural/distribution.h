#pragma once
#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

// Структура для основного распределения
struct base_distribution {
    double mu;      // параметр сдвига
    double lambda;  // параметр масштаба (>0)
    double nu;      // параметр формы (>0)
};
// Структура для смеси двух распределений
struct mixture_distribution {
    base_distribution d1;  // первый компонент
    base_distribution d2;  // второй компонент  
    double p;              // вес компонента (0 < p < 1)
};
// Структура для эмпирического распределения
struct empirical_distribution {
    double* data;     // массив с исходной выборкой
    int size;         // размер выборки
    double* bins;     // массив центров интервалов
    double* densities;// плотности в интервалах
    int bin_count;    // количество интервалов
};
// Вычисление плотности распределения в точке x
double base_density(double x, base_distribution d);
// Вычисление характеристик распределения
void base_characteristics(base_distribution d, double* M, double* D, double* gamma1, double* gamma2);
// Генерация случайной величины
double base_random(base_distribution d);
// Вычисление плотности смеси в точке x
double mixture_density(double x, mixture_distribution m);
// Вычисление характеристик смеси распределений
void mixture_characteristics(mixture_distribution m, double* M, double* D, double* gamma1, double* gamma2);
// Генерация случайной величины для смеси
double mixture_random(mixture_distribution m);
// Создание эмпирического распределения по выборке
empirical_distribution create_empirical(double* data, int size, int bin_count = -1);
// Вычисление плотности эмпирического распределения в точке x
double empirical_density(double x, empirical_distribution emp);
// Вычисление характеристик эмпирического распределения
void empirical_characteristics(empirical_distribution emp, double* M, double* D, double* gamma1, double* gamma2);
// Генерация случайной величины из эмпирического распределения
double empirical_random(empirical_distribution emp);
// Освобождение памяти эмпирического распределения
void free_empirical(empirical_distribution emp);
// Бета-функция B(a,b)
double beta_function(double a, double b);
// Функции для записи данных в файлы
void write_theoretical_density(double mu, double lambda, double nu,
    double min, double max, double step,
    const char* filename);
void write_empirical_density(double* centers, double* densities,
    int bins, const char* filename);
void write_sample(double* arr, int n, const char* filename);
void write_theoretical_mix_density(struct mixture_distribution m,
    double min, double max, double step,
    const char* filename);
void write_empirical_mix_density(double* centers, double* densities,
    int bins, const char* filename);
void write_mix_sample_data(double* arr, int n, const char* filename);
// Функции тестирования распределений
void run_all_tests();
void test_base_distribution();
void test_mixture_distributions();
#endif
