#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "distribution.h"
#include <fstream>
using namespace std;

// Вывод таблицы сравнения теоретических и эмпирических характеристик
void print_comparison_theor_and_emp(double theoretical_fx, double empirical_fx,
    double M, double D, double gamma1, double gamma2,
    double emp_M, double emp_D, double emp_gamma1, double emp_gamma2) {
    printf("| Параметр                | Теоретический |  Эмпирический |       Разница |\n");
    printf("| Плотность распределения | %13.6f | %13.6f | %13.6f |\n",
        theoretical_fx, empirical_fx, fabs(theoretical_fx - empirical_fx));
    printf("| Математическое ожидание | %13.6f | %13.6f | %13.6f |\n",
        M, emp_M, fabs(M - emp_M));
    printf("| Дисперсия               | %13.6f | %13.6f | %13.6f |\n",
        D, emp_D, fabs(D - emp_D));
    printf("| Коэффициент асимметрии  | %13.6f | %13.6f | %13.6f |\n",
        gamma1, emp_gamma1, fabs(gamma1 - emp_gamma1));
    printf("| Коэффициент эксцесса    | %13.6f | %13.6f | %13.6f |\n",
        gamma2, emp_gamma2, fabs(gamma2 - emp_gamma2));
}

// Функция для демонстрации работы с основным распределением
void demo_base_distribution() {
    printf("\nДемонстрация основного распределения (структура): \n");
    base_distribution d;
    printf("Введите параметры основного распределения.\n");
    printf("Сдвиг mu: "); scanf_s("%lf", &d.mu);
    printf("Масштаб lambda (больше нуля): "); scanf_s("%lf", &d.lambda);
    printf("Форма nu (больше 0,5): "); scanf_s("%lf", &d.nu);
    if (d.lambda <= 0 || d.nu <= 0.5) {
        printf("Некорректный ввод.\n");
        return;
    }
    double x;
    printf("Введите точку x для вычисления плотности: ");
    scanf_s("%lf", &x);

    double M, D, gamma1, gamma2; // переменные для хранения числовых характеристик
    base_characteristics(d, &M, &D, &gamma1, &gamma2);
    double density = base_density(x, d);
    printf("\nРезультаты:\n");
    printf("Плотность: %.6f\n", density);
    printf("Математическое ожидание: %.6f\n", M);
    printf("Дисперсия: %.6f\n", D);
    printf("Коэффициент асимметрии: %.6f\n", gamma1);
    printf("Коэффициент эксцесса: %.6f\n", gamma2);
    // Генерация выборки
    int n;
    printf("\nВведите размер выборки для генерации: ");
    scanf_s("%d", &n);
    if (n <= 0) {
        printf("Некорректный ввод.\n");
        return;
    }
    double* sample = new double[n]; // выделение памяти под массив с выборкой
    for (int i = 0; i < n; i++) { // генерация выборки
        sample[i] = base_random(d);
    }
    // Создание эмпирического распределения
    empirical_distribution emp = create_empirical(sample, n);
    // Генерация новой выборки из эмпирического распределения
    double* new_sample = new double[n];
    for (int i = 0; i < n; i++) {
        new_sample[i] = empirical_random(emp);
    }
    // Сохраняем новую выборку
    write_sample(new_sample, n, "new_sample.txt");
    // Сравнение теоретических и эмпирических характеристик
    double emp_M, emp_D, emp_gamma1, emp_gamma2;
    empirical_characteristics(emp, &emp_M, &emp_D, &emp_gamma1, &emp_gamma2);
    double emp_density = empirical_density(x, emp);
    printf("\nСравнение теоретических и эмпирических характеристик:\n");
    print_comparison_theor_and_emp(density, emp_density, M, D, gamma1, gamma2, emp_M, emp_D, emp_gamma1, emp_gamma2);
    // Нахождение минимума и максимума в выборке для построения графиков
    double min_val = sample[0], max_val = sample[0];
    for (int i = 1; i < n; i++) {
        if (sample[i] < min_val) min_val = sample[i];
        if (sample[i] > max_val) max_val = sample[i];
    }
    // Запись теоретической и эмпирической плотностей и выборки в файл
    write_theoretical_density(d.mu, d.lambda, d.nu, min_val, max_val, 0.1, "theoretical_density.txt");
    write_empirical_density(emp.bins, emp.densities, emp.bin_count, "empirical_density.txt");
    write_sample(sample, n, "sample.txt");
    printf("\nДанные записаны в файлы.\n");
    delete[] sample; // освобождение памяти под выборку
    free_empirical(emp); // освобождение памяти эмпирического распределения
}

// Функция для демонстрации работы со смесью распределений
void demo_mixture_distribution() {
    printf("\nДемонстрация смеси распределений:\n");
    mixture_distribution mix; // переменная для смеси распределений
    printf("Введите параметры первой компоненты.\n");
    printf("Сдвиг mu1: "); scanf_s("%lf", &mix.d1.mu);
    printf("Масштаб lambda1 (больше нуля): "); scanf_s("%lf", &mix.d1.lambda);
    printf("Форма nu1 (больше 0,5): "); scanf_s("%lf", &mix.d1.nu);
    printf("Введите параметры второй компоненты:\n");
    printf("Сдвиг mu2: "); scanf_s("%lf", &mix.d2.mu);
    printf("Масштаб lambda2 (больше нуля): "); scanf_s("%lf", &mix.d2.lambda);
    printf("Форма nu2 (больше 0,5): "); scanf_s("%lf", &mix.d2.nu);
    printf("Введите параметр смеси p (0 < p < 1): ");
    scanf_s("%lf", &mix.p);
    if (mix.d1.lambda <= 0 || mix.d1.nu <= 0 ||
        mix.d2.lambda <= 0 || mix.d2.nu <= 0 ||
        mix.p <= 0 || mix.p >= 1) {
        printf("Некорректный ввод.\n");
        return;
    }
    double x;
    printf("Введите точку x для вычисления плотности: ");
    scanf_s("%lf", &x);
    // Вычисление характеристик смеси
    double M, D, gamma1, gamma2;
    mixture_characteristics(mix, &M, &D, &gamma1, &gamma2);
    double density = mixture_density(x, mix);
    printf("\nРезультаты для смеси:\n");
    printf("Плотность: %.6f\n", density);
    printf("Математическое ожидание: %.6f\n", M);
    printf("Дисперсия: %.6f\n", D);
    printf("Коэффициент асимметрии: %.6f\n", gamma1);
    printf("Коэффициент эксцесса: %.6f\n", gamma2);
    // Генерация выборки из смеси 
    int n;
    printf("\nВведите размер выборки для генерации: ");
    scanf_s("%d", &n);
    if (n <= 0) {
        printf("Некорректный ввод.\n");
        return;
    }
    double* sample = new double[n]; // выделение памяти
    for (int i = 0; i < n; i++) { // генерация выборки
        sample[i] = mixture_random(mix);
    }
    // Создание эмпирического распределения по выборке смеси
    empirical_distribution emp = create_empirical(sample, n);
    // Генерация новой выборки из эмпирического распределения смеси
    double* new_sample = new double[n];
    for (int i = 0; i < n; i++) {
        new_sample[i] = empirical_random(emp);
    }
    // Сохраняем новую выборку смеси
    write_mix_sample_data(new_sample, n, "new_mix_sample.txt");
    // Нахождение минимума и максимума для графика
    double min_val = sample[0], max_val = sample[0];
    for (int i = 1; i < n; i++) {
        if (sample[i] < min_val) min_val = sample[i];
        if (sample[i] > max_val) max_val = sample[i];
    }
    // Расширение границ для полного покрытия
    double range = max_val - min_val;
    min_val -= 0.1 * range;
    max_val += 0.1 * range;
    // Запись теоретической плотности смеси
    write_theoretical_mix_density(mix, min_val, max_val, 0.1, "theoretical_mix_density.txt");
    // Запись эмпирической плотности
    write_empirical_mix_density(emp.bins, emp.densities, emp.bin_count, "empirical_mix_density.txt");
    // Запись самой выборки
    write_mix_sample_data(sample, n, "mix_sample.txt");
    printf("\nДанные смеси записаны в файлы.\n");
    // Вычисление эмпирических характеристик
    double eM, eD, eGamma1, eGamma2;
    empirical_characteristics(emp, &eM, &eD, &eGamma1, &eGamma2);
    double eDensity = empirical_density(x, emp);
    // Сравнение теоретических и эмпирических характеристик
    printf("\nСравнение для смеси:\n");
    print_comparison_theor_and_emp(density, eDensity, M, D, gamma1, gamma2, eM, eD, eGamma1, eGamma2);
    // Очистка памяти
    delete[] sample;
    free_empirical(emp);
}

// Функция демонстрации класса с основным распределением
void demo_primary_class() {
    printf("\nДемонстрация основного распределения (класс): \n");

    try {
        double shift, scale, form;
        printf("Введите параметры основного распределения.\n");
        printf("Сдвиг mu: "); scanf_s("%lf", &shift);
        printf("Масштаб lambda (больше нуля): "); scanf_s("%lf", &scale);
        printf("Форма nu (больше 0,5): "); scanf_s("%lf", &form);

        Primary prim(shift, scale, form);

        double x;
        printf("Введите точку x для вычисления плотности: ");
        scanf_s("%lf", &x);

        double M, D, gamma1, gamma2;
        prim.characteristics(&M, &D, &gamma1, &gamma2);
        double density = prim.density(x);

        printf("\nРезультаты:\n");
        printf("Плотность: %.6f\n", density);
        printf("Математическое ожидание: %.6f\n", M);
        printf("Дисперсия: %.6f\n", D);
        printf("Коэффициент асимметрии: %.6f\n", gamma1);
        printf("Коэффициент эксцесса: %.6f\n", gamma2);

        // Генерация выборки
        int n;
        printf("\nВведите размер выборки для генерации: ");
        scanf_s("%d", &n);
        if (n <= 0) {
            printf("Некорректный ввод.\n");
            return;
        }

        double* sample = new double[n]; // выделение памяти под массив с выборкой
        for (int i = 0; i < n; i++) { // генерация выборки с помощью класса
            sample[i] = prim.random();
        }

        // Создание эмпирического распределения
        empirical_distribution emp = create_empirical(sample, n);

        // Генерация новой выборки из эмпирического распределения
        double* new_sample = new double[n];
        for (int i = 0; i < n; i++) {
            new_sample[i] = empirical_random(emp);
        }
        write_sample(new_sample, n, "new_sample_class.txt");
        
        // Сравнение теоретических и эмпирических характеристик
        double emp_M, emp_D, emp_gamma1, emp_gamma2;
        empirical_characteristics(emp, &emp_M, &emp_D, &emp_gamma1, &emp_gamma2);
        double emp_density = empirical_density(x, emp);

        printf("\nСравнение теоретических и эмпирических характеристик:\n");
        print_comparison_theor_and_emp(density, emp_density, M, D, gamma1, gamma2, emp_M, emp_D, emp_gamma1, emp_gamma2);

        // Нахождение минимума и максимума в выборке для построения графиков
        double min_val = sample[0], max_val = sample[0];
        for (int i = 1; i < n; i++) {
            if (sample[i] < min_val) min_val = sample[i];
            if (sample[i] > max_val) max_val = sample[i];
        }

        write_theoretical_density(shift, scale, form, min_val, max_val, 0.1, "theoretical_density_class.txt");
        write_empirical_density(emp.bins, emp.densities, emp.bin_count, "empirical_density_class.txt");
        write_sample(sample, n, "sample_class.txt");

        printf("\nДанные записаны в файлы.\n");

        // Очистка памяти
        delete[] sample;
        delete[] new_sample;
        free_empirical(emp);
    }
    catch (const exception&) {
        printf("Некорректный ввод.\n");
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    srand((unsigned)time(0)); // инициализация генератора случайных чисел
    int choice;
    do {
        printf("\nМЕНЮ\n");
        printf("1. Демонстрация основного распределения (структура)\n");
        printf("2. Демонстрация смеси распределений\n");
        printf("3. Демонстрация основного распределения (класс)\n");
        printf("4. Запуск тестов\n");
        printf("5. Запуск тестов для класса\n");
        printf("0. Выход\n");
        printf("Выберите пункт меню: ");
        if (scanf_s("%d", &choice) != 1) {
            printf("Некорректный ввод.\n");
            while (getchar() != '\n'); // Очистка буфера при ошибке
            continue;
        }
        switch (choice) {
        case 1:
            demo_base_distribution();
            break;
        case 2:
            demo_mixture_distribution();
            break;
        case 3:
            demo_primary_class();
            break;
        case 4:
            run_all_tests();
            break;
        case 5:
            primary_class_test();
            break;
        case 0:
            break;
        default:
            printf("Такого пункта нет.\n");
        }
    } while (choice != 0);
    return 0;
}
