#pragma once
#ifndef EMPIRIC_H
#define EMPIRIC_H

#include "primary.h"
#include "mixture.h"
#include <stdio.h>
#include <stdexcept>
#include <algorithm>

class Empiric {
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
    Empiric(int n0, const Primary& prim, int k0 = -1); // из основного распределения 
    Empiric(int n0, const Mixture& mixt, int k0 = -1); // из смеси распределений
    Empiric(int n0, const Empiric& emp, int k0 = -1); // из структурно реализованного эмпирического распределения
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

    // Плотность, характеристики и генерация случайной величины
    double density(double x) const; // плотность в точке x 
    void characteristics(double* M, double* D, double* gamma1, double* gamma2) const;
    double random() const; // генерация случайной величины по эмпирическому распределению

    // Персистентность
    void save(FILE* out) const;
    void load(FILE* in);
};

#endif
