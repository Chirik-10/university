#pragma once
#ifndef EMPIRIC_H
#define EMPIRIC_H

#include "interfaces.h"
#include <stdio.h>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <cstdlib>

class Empiric : public IDistribution, public IPersistent {
private:
    int n; // размер выборки
    int k; // количество интервалов
    double* data; // указатель на массив с исходными данными
    double* bins; // указатель на массив центров интервалов
    double* densities; // указатель на массив значений плотности в каждом интервале
    double min_val; // минимальное значение выборки
    double max_val; // максимальное значение выборки

    void calculate_densities(); // расчет плотностей по данным

public:
    // Конструкторы
    Empiric(const double* data_arr, int n0, int k0 = -1); // из готовой выборки
    Empiric(int n0, const IDistribution& dist, int k0 = -1); // из распределения через интерфейс
    Empiric(FILE* in) { load(in); } // из файла

    // Конструктор копирования
    Empiric(const Empiric& emp); // глубокая копия объекта

    // Оператор присваивания
    Empiric& operator=(const Empiric& emp); // присваивание с глубоким копированием

    // Деструктор
    ~Empiric(); // освобождение динамической памяти

    // Get-функции
    int getSize() const { return n; } // получение размера выборки
    int getBinCount() const { return k; } // получение количества интервалов
    const double* getData() const { return data; } // получение указателя на константный массив данных
    const double* getBins() const { return bins; } // получение указателя на константные центры интервалов
    const double* getDensities() const { return densities; } // получение указателя на константные значения плотностей
    double* getData() { return data; } // получение указателя на массив данных
    double* getBins() { return bins; } // получение указателя на центры интервалов
    double* getDensities() { return densities; } // получение указателя на значения плотностей
    double getMin() const { return min_val; } // получение минимального значения в выборке
    double getMax() const { return max_val; } // получение максимального значения в выборке

    // Интерфейс IDistribution
    double density(double x) const override; // плотность в точке x
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const override;
    double random() const override; // генерация случайной величины по эмпирическому распределению

    // Интерфейс IPersistent
    void save(FILE* out) const override;
    void load(FILE* in) override;
};
#endif
