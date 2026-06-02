#include "empiric.h"
#include <cmath>
#include <cstdlib>

using namespace std;

Empiric::Empiric(const double* data_arr, int n0, int k0)
    : n(0), k(0), data(nullptr), bins(nullptr), densities(nullptr),
    min_val(0.0), max_val(0.0)
{
    if (n0 <= 1) throw runtime_error("Некорректный размер выборки.");
    if (data_arr == nullptr) throw runtime_error("Нулевой указатель на данные.");

    n = n0;
    k = (k0 > 1) ? k0 : 50;
    data = new double[n];
    bins = new double[k];
    densities = new double[k];

    copy(data_arr, data_arr + n, data);
    calculate_densities();
}

// Конструктор из распределения через интерфейс
Empiric::Empiric(int n0, const IDistribution& dist, int k0)
    : n(0), k(0), data(nullptr), bins(nullptr), densities(nullptr),
    min_val(0.0), max_val(0.0)
{
    if (n0 <= 1) throw runtime_error("Некорректный размер выборки.");

    n = n0;
    k = (k0 > 1) ? k0 : 50;
    data = new double[n];
    bins = new double[k];
    densities = new double[k];

    for (int i = 0; i < n; i++) {
        data[i] = dist.random(); // Позднее связывание
    }

    calculate_densities();
}

// Конструктор глубокого копирования
Empiric::Empiric(const Empiric& emp)
    : n(emp.n), k(emp.k), data(nullptr), bins(nullptr), densities(nullptr),
    min_val(emp.min_val), max_val(emp.max_val)
{
    data = new double[n];
    bins = new double[k];
    densities = new double[k];

    copy(emp.data, emp.data + n, data);
    copy(emp.bins, emp.bins + k, bins);
    copy(emp.densities, emp.densities + k, densities);
}

// Оператор присваивания
Empiric& Empiric::operator=(const Empiric& emp) {
    if (this == &emp) return *this;

    // Обработка массива данных
    if (n != emp.n) {
        delete[] data;
        n = emp.n;
        data = new double[n];
    }

    // Обработка массива интервалов и плотностей
    if (k != emp.k) {
        delete[] bins;
        delete[] densities;
        k = emp.k;
        bins = new double[k];
        densities = new double[k];
    }

    copy(emp.data, emp.data + n, data);
    copy(emp.bins, emp.bins + k, bins);
    copy(emp.densities, emp.densities + k, densities);

    min_val = emp.min_val;
    max_val = emp.max_val;

    return *this; // ссылка на объект
}

// Деструктор
Empiric::~Empiric() {
    delete[] data;
    delete[] bins;
    delete[] densities;
}

// Функция для вычисления плотностей
void Empiric::calculate_densities() {
    // Нахождение диапазона для гистограммы
    min_val = data[0];
    max_val = data[0];

    for (int i = 1; i < n; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }

    double range = max_val - min_val;

    // Ширина интервала
    double bin_width = range / k;

    // Нахождение количества точек в каждом интервале
    int* counts = new int[k]();

    for (int i = 0; i < n; i++) {
        int bin_index = (int)((data[i] - min_val) / bin_width);
        if (bin_index >= 0 && bin_index < k) {
            counts[bin_index]++;
        }
    }

    // Нахождение центров интервалов и их плотностей
    for (int i = 0; i < k; i++) {
        bins[i] = min_val + (i + 0.5) * bin_width;
        densities[i] = (double)counts[i] / (n * bin_width);
    }

    delete[] counts;
}

// Плотность в точке x
double Empiric::density(double x) const {
    double min_val_local = bins[0] - (bins[1] - bins[0]) / 2;
    double max_val_local = bins[k - 1] + (bins[k - 1] - bins[k - 2]) / 2;

    if (x < min_val_local || x > max_val_local) {
        return 0.0;
    }

    double bin_width = (max_val_local - min_val_local) / k; // ширина интервала
    int bin_index = (int)((x - min_val_local) / bin_width); // номер интервала с точкой

    if (bin_index >= 0 && bin_index < k) {
        return densities[bin_index];
    }

    return 0.0;
}

// Характеристики
void Empiric::characteristics(double* M, double* D, double* gamma1, double* gamma2) const {
    double sum = 0.0, sum_sq = 0.0; // сумма и сумма квадратов

    for (int i = 0; i < n; i++) {
        sum += data[i];
        sum_sq += data[i] * data[i];
    }

    *M = sum / n;
    *D = (sum_sq / n) - (*M) * (*M);

    // Третий и четвертый момент
    double sum_cube = 0.0;
    double sum_quad = 0.0;

    for (int i = 0; i < n; i++) {
        double dev = data[i] - *M;
        sum_cube += dev * dev * dev;
        sum_quad += dev * dev * dev * dev;
    }

    double mu3 = sum_cube / n;
    double mu4 = sum_quad / n;

    // Коэффициент асимметрии
    *gamma1 = mu3 / pow(*D, 1.5);

    // Коэффициент эксцесса
    *gamma2 = mu4 / (*D * *D) - 3;
}

// Генерация случайной величины
double Empiric::random() const {
    double total_density = 0.0;
    for (int i = 0; i < k; i++) {
        total_density += densities[i];
    }

    double u = (double)rand() / RAND_MAX * total_density;

    double cumulative = 0.0;
    int selected_bin = 0;

    for (int i = 0; i < k; i++) {
        cumulative += densities[i];
        if (u <= cumulative) {
            selected_bin = i;
            break;
        }
    }

    double bin_width = (bins[1] - bins[0]);
    double bin_min = bins[selected_bin] - bin_width / 2;
    double bin_max = bins[selected_bin] + bin_width / 2;

    return bin_min + (double)rand() / RAND_MAX * (bin_max - bin_min);
}

// Сохранение в файл
void Empiric::save(FILE* out) const {
    fprintf(out, "%d %d\n", n, k);
    for (int i = 0; i < n; i++) {
        fprintf(out, "%.6f\n", data[i]);
    }
}

// Загрузка из файла
void Empiric::load(FILE* in) {
    int n0, k0;
    fscanf_s(in, "%d %d", &n0, &k0);

    if (n0 <= 1) throw runtime_error("Некорректный размер выборки.");
    if (k0 <= 1) throw runtime_error("Некорректное количество интервалов.");

    if (n != n0) {
        delete[] data;
        n = n0;
        data = new double[n];
    }

    if (k != k0) {
        delete[] bins;
        delete[] densities;
        k = k0;
        bins = new double[k];
        densities = new double[k];
    }

    for (int i = 0; i < n; i++) {
        fscanf_s(in, "%lf", &data[i]);
    }

    calculate_densities();
}
