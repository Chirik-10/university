#include <iostream>
#include <cmath>
#include <cstdio>
#include "distribution.h"

using namespace std;

// Структура для хранения табличных значений распределения
struct table_row {
    double nu; // параметр формы
    double sigma2; // дисперсия
    double gamma2; // эксцесс
    double f0; // плотность в x=0
};

// Табличные значения для стандартного распределения
const table_row table_vals[] = {
    {3.0, 1.0 / 3.0, 6.0, 0.849},
    {4.0, 0.2, 2.0, 1.019},
    {5.0, 0.143, 1.2, 1.164},
    {6.0, 1.0 / 9.0, 0.857, 1.293},
    {8.0, 0.077, 0.545, 1.520},
    {10.0, 0.059, 0.4, 1.716},
    {15.0, 0.037, 0.24, 2.130},
    {20.0, 0.027, 0.171, 2.475}
};

const int table_size = 8;

// 1. Тесты для основного распределения
void test_base_distribution() {
    // Сравнение с табличными значениями для разных ν
    printf("\nСравнение с табличными значениями\n");
    for (int i = 0; i < table_size; i++) {
        table_row row = table_vals[i];
        base_distribution d_table = { 0.0, 1.0, row.nu };
        double M_table, D_table, gamma1_table, gamma2_table;
        base_characteristics(d_table, &M_table, &D_table, &gamma1_table, &gamma2_table);
        double f0_table = base_density(0.0, d_table);
        printf("\nnu=%.1f:\n", row.nu);
        printf("  Дисперсия: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            D_table, row.sigma2, fabs(D_table - row.sigma2));
        printf("  Эксцесс: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            gamma2_table, row.gamma2, fabs(gamma2_table - row.gamma2));
        printf("  Плотность в 0: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            f0_table, row.f0, fabs(f0_table - row.f0));
    }

    printf("\n6.1. Тесты для основного распределения\n");
    printf("\n6.1.1. Стандартное распределение (mu=0, lambda=1, nu=1):\n");
    base_distribution d_std = { 0.0, 1.0, 1.0 };
    double M_std, D_std, gamma1_std, gamma2_std;
    base_characteristics(d_std, &M_std, &D_std, &gamma1_std, &gamma2_std);
    double f0_std = base_density(0.0, d_std);
    printf("  Плотность в x=0: %.6f\n", f0_std);
    printf("  Мат. ожидание: %.6f\n", M_std);
    printf("  Дисперсия: %.6f\n", D_std);
    printf("\n6.1.2. Масштабное преобразование (mu=0, lambda=2, nu=1):\n");
    base_distribution d_scale = { 0.0, 2.0, 1.0 };
    double M_scale, D_scale, gamma1_scale, gamma2_scale;
    base_characteristics(d_scale, &M_scale, &D_scale, &gamma1_scale, &gamma2_scale);
    double f0_scale = base_density(0.0, d_scale);
    printf("  Плотность в x=0: %.6f\n", f0_scale);
    printf("  Мат. ожидание: %.6f\n", M_scale);
    printf("  Дисперсия: %.6f\n", D_scale);
    printf("\n6.1.3. Сдвиг-масштабное преобразование (mu=10, lambda=2, nu=1):\n");
    base_distribution d_shift_scale = { 10.0, 2.0, 1.0 };
    double M_ss, D_ss, gamma1_ss, gamma2_ss;
    base_characteristics(d_shift_scale, &M_ss, &D_ss, &gamma1_ss, &gamma2_ss);
    double f10_ss = base_density(10.0, d_shift_scale);
    printf("  Плотность в x=10: %.6f\n", f10_ss);
    printf("  Мат. ожидание: %.6f\n", M_ss);
    printf("  Дисперсия: %.6f\n", D_ss);
}

// 2. Тесты для смеси распределений
void test_mixture_distributions() {
    printf("\n6.2. Тесты для смеси распределений\n");
    printf("\n6.2.1. Тривиальная смесь (mu1=mu2=10, lambda1=lambda2=2, nu1=nu2=1, p=0.5):\n");
    mixture_distribution mix1 = { {10.0, 2.0, 1.0}, {10.0, 2.0, 1.0}, 0.5 };
    double M1, D1, gamma11, gamma21;
    mixture_characteristics(mix1, &M1, &D1, &gamma11, &gamma21);
    printf("  Мат. ожидание: %.3f\n", M1);
    printf("  Дисперсия: %.3f\n", D1);
    printf("  Асимметрия: %.3f\n", gamma11);
    printf("  Эксцесс: %.3f\n", gamma21);
    printf("\n6.2.2. Смесь со сдвигом (mu1=0, mu2=2, lambda1=lambda2=1, nu1=nu2=1, p=0.75):\n");
    mixture_distribution mix2 = { {0.0, 1.0, 1.0}, {2.0, 1.0, 1.0}, 0.75 };
    double M2, D2, gamma12, gamma22;
    mixture_characteristics(mix2, &M2, &D2, &gamma12, &gamma22);
    printf("  Мат. ожидание: %.3f\n", M2);
    printf("  Дисперсия: %.3f\n", D2);
    printf("  Асимметрия: %.3f\n", gamma12);
    printf("\n6.2.3. Смесь с масштабом (mu1=mu2=0, lambda1=1, lambda2=3, nu1=nu2=1, p=0.5):\n");
    mixture_distribution mix3 = { {0.0, 1.0, 1.0}, {0.0, 3.0, 1.0}, 0.5 };
    double M3, D3, gamma13, gamma23;
    mixture_characteristics(mix3, &M3, &D3, &gamma13, &gamma23);
    printf("  Мат. ожидание: %.3f\n", M3);
    printf("  Дисперсия: %.3f\n", D3);
    printf("  Асимметрия: %.3f\n", gamma13);
    printf("  Эксцесс: %.3f\n", gamma23);
    printf("\n6.2.4. Смесь с разной формой (mu1=mu2=0, lambda1=lambda2=1, nu1=0.1, nu2=30, p=0.5):\n");
    mixture_distribution mix4 = { {0.0, 1.0, 0.1}, {0.0, 1.0, 30.0}, 0.5 };
    double M4, D4, gamma14, gamma24;
    mixture_characteristics(mix4, &M4, &D4, &gamma14, &gamma24);
    printf("  Мат. ожидание: %.3f\n", M4);
    printf("  Дисперсия: %.3f\n", D4);
    printf("  Асимметрия: %.3f\n", gamma14);
    printf("  Эксцесс: %.3f\n", gamma24);
}
// Функция для запуска всех тестов
void run_all_tests() {
    test_base_distribution();
    test_mixture_distributions();
}

void primary_class_test() {
    cout << "\n1.Сравнение с табличными значениями" << endl;

    for (int i = 0; i < table_size; i++) {
        table_row row = table_vals[i];
        Primary prim_table(0.0, 1.0, row.nu);
        double M_prim, D_prim, gamma1_prim, gamma2_prim;
        prim_table.characteristics(&M_prim, &D_prim, &gamma1_prim, &gamma2_prim);
        double f0_prim = prim_table.density(0.0);
        printf("\nnu=%.1f:\n", row.nu);
        printf("  Дисперсия: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            D_prim, row.sigma2, fabs(D_prim - row.sigma2));
        printf("  Эксцесс: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            gamma2_prim, row.gamma2, fabs(gamma2_prim - row.gamma2));
        printf("  Плотность в 0: вычислено=%.3f, табличное=%.3f, разница=%.3f\n",
            f0_prim, row.f0, fabs(f0_prim - row.f0));
    }

    try {
        cout << "\n2.Тестирование конструкторов" << endl;
        Primary prim1;
        Primary prim2(2.0);
        Primary prim3(3, 2.5, 0.6);

        cout << "  prim1 (без параметров): mu=" << prim1.getShift() << ", lambda=" << prim1.getScale() << ", nu=" << prim1.getForm() << endl;
        cout << "  prim2 (только nu): mu=" << prim2.getShift() << ", lambda=" << prim2.getScale() << ", nu=" << prim2.getForm() << endl;
        cout << "  prim3 (все параметры): mu=" << prim3.getShift() << ", lambda=" << prim3.getScale() << ", nu=" << prim3.getForm() << endl;

        cout << "\n3.Тестирование set-функций (изменяем параметры prim1)" << endl;
        prim1.setShift(10.0);
        prim1.setScale(2.0);
        prim1.setForm(3.0);
        cout << "  mu=" << prim1.getShift() << ", lambda=" << prim1.getScale() << ", nu=" << prim1.getForm() << endl;

        cout << "\n4.Тестирование вычисления плотности для prim1" << endl;
        double x = 1.0;
        double dens = prim1.density(x);
        cout << "  Плотность в точке x = " << x << ": " << dens << endl;

        cout << "\n5.Тестирование вычисления характеристик для prim1" << endl;
        double M, D, gamma1, gamma2;
        prim1.characteristics(&M, &D, &gamma1, &gamma2);
        cout << "  Мат. ожидание: " << M << endl;
        cout << "  Дисперсия: " << D << endl;
        cout << "  Асимметрия: " << gamma1 << endl;
        cout << "  Эксцесс: " << gamma2 << endl;

        cout << "\n6.Тестирование моделирования случайной величины" << endl;
        cout << "  Сгенерированные значения: ";
        for (int i = 0; i < 5; i++) {
            cout << prim1.random() << " ";
        }
        cout << endl;

        cout << "\n7.Тестирование персистентности (сохранение/загрузка атрибутов):" << endl;
        FILE* outFile;
        errno_t err = fopen_s(&outFile, "primary_test.txt", "w");
        if (err == 0 && outFile != NULL) {
            prim1.save(outFile);
            fclose(outFile);
            cout << "  Значения загружены в файл" << endl;
        }
        else {
            cout << "  Ошибка открытия файла для записи" << endl;
        }

        FILE* inFile;
        err = fopen_s(&inFile, "primary_test.txt", "r");
        if (err == 0 && inFile != NULL) {
            Primary prim4(inFile);
            fclose(inFile);
            cout << "  Значения загружены из файла: mu=" << prim4.getShift() << ", lambda=" << prim4.getScale() << ", nu=" << prim4.getForm() << endl;
        }
        else {
            cout << "  Ошибка открытия файла для чтения" << endl;
        }

        cout << "\n8.Тестирование исключительных ситуаций:" << endl;

        // Некорректные значения
        try {
            cout << "  Отрицательный параметр формы (nu = -1)" << endl;
            Primary prim5(0.0, 1.0, -1.0);
        }
        catch (const exception& e) {
            cout << "  Результат: " << e.what() << endl;
        }

        try {
            cout << "  Отрицательный параметр масштаба (lambda = -1)" << endl;
            Primary prim6(0.0, -1.0, 1.0);
        }
        catch (const exception& e) {
            cout << "  Результат: " << e.what() << endl;
        }

        // Граничные значения
        try {
            cout << "  Граничное значение формы (nu = 0,5)" << endl;
            Primary prim7(0.0, 1.0, 0.5);
        }
        catch (const exception& e) {
            cout << "  Результат: " << e.what() << endl;
        }

        try {
            cout << "  Граничное значение масштаба (lambda = 0)" << endl;
            Primary prim8(0.0, 0.0, 1.0);
        }
        catch (const exception& e) {
            cout << "  Результат: " << e.what() << endl;
        }

    }
    catch (const exception&) {
        cout << "Непредвиденная ошибка." << endl;
    }
}
