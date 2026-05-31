#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <utility>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <random>

using namespace std;

// Структура для представления точки в 2D пространстве
struct Point {
    double x, y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<=(const Point& other) const {
        return (x < other.x) || (x == other.x && y <= other.y);
    }
};

// Функция для вычисления расстояния между двумя точками
double distance(const Point& p1, const Point& p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Полный перебор
pair<Point, Point> closest_pair_brute_force(const vector<Point>& points) {
    double min_dist = numeric_limits<double>::max();
    pair<Point, Point> closest_pair;

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            double dist = distance(points[i], points[j]);
            if (dist < min_dist) {
                min_dist = dist;
                closest_pair = { points[i], points[j] };
            }
        }
    }
    return closest_pair;
}

// Вспомогательная функция для нахождения ближайшей пары точек во временной полосе
pair<Point, Point> closest_pair_in_strip(const vector<Point>& strip, double d) {
    double min_dist = d;
    pair<Point, Point> closest_pair = strip.size() > 1 ? make_pair(strip[0], strip[1]) : pair<Point, Point>();

    for (size_t i = 0; i < strip.size(); ++i) {
        for (size_t j = i + 1; j < strip.size() && (strip[j].y - strip[i].y) < d; ++j) {
            double dist = distance(strip[i], strip[j]);
            if (dist < min_dist) {
                min_dist = dist;
                closest_pair = { strip[i], strip[j] };
            }
        }
    }
    return closest_pair;
}

// Основная функция "разделяй и властвуй"
pair<Point, Point> closest_pair_divide_and_conquer(vector<Point>& points_x, vector<Point>& points_y) {
    size_t n = points_x.size();

    if (n <= 3) {
        return closest_pair_brute_force(points_x);
    }

    size_t mid = n / 2;
    Point mid_point = points_x[mid];

    vector<Point> left(points_x.begin(), points_x.begin() + mid);
    vector<Point> right(points_x.begin() + mid, points_x.end());

    vector<Point> points_y_left, points_y_right;
    for (const auto& point : points_y) {
        if (point.x <= mid_point.x) {
            points_y_left.push_back(point);
        }
        else {
            points_y_right.push_back(point);
        }
    }

    auto closest_pair_left = closest_pair_divide_and_conquer(left, points_y_left);
    auto closest_pair_right = closest_pair_divide_and_conquer(right, points_y_right);

    double d_left = distance(closest_pair_left.first, closest_pair_left.second);
    double d_right = distance(closest_pair_right.first, closest_pair_right.second);
    double d = min(d_left, d_right);

    pair<Point, Point> closest_pair = d_left < d_right ? closest_pair_left : closest_pair_right;

    // Создание полосы точек
    vector<Point> strip;
    for (const auto& point : points_y) {
        if (abs(point.x - mid_point.x) < d) {
            strip.push_back(point);
        }
    }

    auto closest_pair_strip = closest_pair_in_strip(strip, d);
    double d_strip = distance(closest_pair_strip.first, closest_pair_strip.second);
    if (d_strip < distance(closest_pair.first, closest_pair.second)) {
        closest_pair = closest_pair_strip;
    }

    return closest_pair;
}

// Основная функция, которая подготавливает данные и запускает алгоритмы
pair<Point, Point> closest_pair(vector<Point>& points) {
    if (points.size() < 2) {
        throw runtime_error("Недостаточно точек для определения ближайшей пары.");
    }
    vector<Point> points_x = points;
    sort(points_x.begin(), points_x.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
        });
    vector<Point> points_y = points;
    sort(points_y.begin(), points_y.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
        });
    return closest_pair_divide_and_conquer(points_x, points_y);
}

// Определение помощи для форматирования вывода Catch2
namespace Catch {
    template<>
    struct StringMaker<Point> {
        static string convert(const Point& p) {
            ostringstream out;
            out << "Point(" << p.x << ", " << p.y << ")";
            return out.str();
        }
    };

    template<>
    struct StringMaker<pair<Point, Point>> {
        static string convert(const pair<Point, Point>& p) {
            ostringstream out;
            out << "{" << StringMaker<Point>::convert(p.first) << ", "
                << StringMaker<Point>::convert(p.second) << "}";
            return out.str();
        }
    };
}

pair<Point, Point> ordered(const pair<Point, Point>& p) {
    return p.first <= p.second ? p : make_pair(p.second, p.first);
}

// Юнит-тесты
TEST_CASE("No points") {
    std::vector<Point> test_points = {};
    CHECK_THROWS(closest_pair(test_points));
}

TEST_CASE("Single point") {
    std::vector<Point> test_points = { Point{1, 1} };
    CHECK_THROWS(closest_pair(test_points));
}

TEST_CASE("Two points") {
    std::vector<Point> test_points = { Point{2, 3}, Point{3, 4} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 2, 3 }, Point{ 3, 4 }));
}

TEST_CASE("Three points") {
    std::vector<Point> test_points = { Point{2, 3}, Point{1, 9}, Point{6, 2} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 2, 3 }, Point{ 6, 2 }));
}

TEST_CASE("Duplicate points") {
    std::vector<Point> test_points = { Point{2, 3}, Point{2, 3}, Point{3, 4} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 2, 3 }, Point{ 2, 3 }));
}

TEST_CASE("Same x coordinate") {
    std::vector<Point> test_points = { Point{2, 9}, Point{2, 4}, Point{2, 1}, Point{2, -8} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 2, 1 }, Point{ 2, 4 }));
}

TEST_CASE("Many points") {
    std::vector<Point> test_points = { Point{2, 3}, Point{0, 4}, Point{11, 9}, Point{2, 8},
                                       Point{4, 4}, Point{3, 6}, Point{6, 5}, Point{1, 9} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 1, 9 }, Point{ 2, 8 }));
}

TEST_CASE("Negative points") {
    std::vector<Point> test_points = { Point{-5, 6}, Point{1, 2}, Point{4, -2}, Point{-9, 0},
                                       Point{-1, -2}, Point{0, 7}, Point{2, -1}, Point{-3, 1} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ 2, -1 }, Point{ 4, -2 }));
}

TEST_CASE("Closest points are from stripe") {
    std::vector<Point> test_points = { Point{-1, 20}, Point{-1.5, 10}, Point{-2, -10}, Point{-2.7, -20},
                                       Point{-10, 20}, Point{-10.5, 10}, Point{-11.7, -10}, Point{-12.2, -20},
                                       Point{1, 21}, Point{1.5, 11}, Point{2, -9}, Point{2.7, -19},
                                       Point{10, 21}, Point{10.5, 11}, Point{11.7, -9}, Point{12.2, -19} };
    CHECK(ordered(closest_pair(test_points)) == make_pair(Point{ -1, 20 }, Point{ 1, 21 }));
}

// Функция для генерации случайных точек
vector<Point> generate_random_points(size_t N, double min_val = -1000, double max_val = 1000) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(min_val, max_val);

    vector<Point> points(N);
    for (auto& point : points) {
        point.x = dis(gen);
        point.y = dis(gen);
    }
    return points;
}

// Функция для замера времени работы алгоритмов
void measure_time(size_t N) {
    auto points = generate_random_points(N);
    // Замер времени для алгоритма полного перебора
    auto start_brute_force = chrono::high_resolution_clock::now();
    auto result_brute_force = closest_pair_brute_force(points);
    auto end_brute_force = chrono::high_resolution_clock::now();

    // Вычисление времени в секундах
    chrono::duration<double> brute_force_time = end_brute_force - start_brute_force;

    // Подготовка данных для алгоритма "разделяй и властвуй"
    vector<Point> points_x = points;
    sort(points_x.begin(), points_x.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
        });
    vector<Point> points_y = points;
    sort(points_y.begin(), points_y.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
        });

    // Замер времени для алгоритма "разделяй и властвуй"
    auto start_divide_conquer = chrono::high_resolution_clock::now();
    auto result_divide_conquer = closest_pair_divide_and_conquer(points_x, points_y);
    auto end_divide_conquer = chrono::high_resolution_clock::now();

    // Вычисление времени в секундах
    chrono::duration<double> divide_conquer_time = end_divide_conquer - start_divide_conquer;

    // Вывод результатов
    cout << "N = " << setw(6) << N << ": "
        << "Полный перебор: " << setw(10) << brute_force_time.count() << " с | "
        << "Разделяй и властвуй: " << setw(10) << divide_conquer_time.count() << " с\n";
}

int main(int argc, char* argv[]) {
    // Запуск тестов
    int result = Catch::Session().run(argc, argv);

    // Пропуск к замерам времени после выполнения юнит-тестов
    cout << "\nЗамеры времени:\n";
    vector<size_t> sizes = { 10, 100, 500, 1000, 5000, 10000 };
    for (size_t size : sizes) {
        measure_time(size);
    }

    return result;
}
