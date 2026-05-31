#define CATCH_CONFIG_MAIN  
#include "catch.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

void merge(vector<int>& array, int start, int mid, int end) {
    vector<int> left(array.begin() + start, array.begin() + mid + 1);
    vector<int> right(array.begin() + mid + 1, array.begin() + end + 1);

    int i = 0, j = 0, k = start;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            array[k++] = left[i++];
        }
        else {
            array[k++] = right[j++];
        }
    }

    while (i < left.size()) {
        array[k++] = left[i++];
    }

    while (j < right.size()) {
        array[k++] = right[j++];
    }
}

void mergeSort(vector<int>& array, int start, int end) {
    if (end - start + 1 <= 2) {
        if (end > start && array[end] < array[start]) {
            swap(array[start], array[end]);
        }
    }
    else {
        int mid = (start + end) / 2;
        mergeSort(array, start, mid);
        mergeSort(array, mid + 1, end);
        merge(array, start, mid, end);
    }
}

void customSort(vector<int>& array) {
    mergeSort(array, 0, array.size() - 1);
}

void measureSortingTime(const string& testName, void (*sortFunc)(vector<int>&), vector<int> arr) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(arr);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << testName << " | Время: " << elapsed.count() << " секунд" << endl;
}

vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(-1000, 1000);
    for (int i = 0; i < size; i++) {
        arr[i] = dis(gen);
    }
    return arr;
}

void performTimeAnalysis() {
    vector<int> sizes = { 10, 100, 1000, 10000, 100000 }; // Разные размеры массивов
    for (int size : sizes) {
        cout << "N = " << size << endl;
        vector<int> arr = generateRandomArray(size);

        {
            vector<int> arrCopy = arr;
            measureSortingTime("Сортировка слиянием", customSort, arrCopy);
        }

        {
            vector<int> arrCopy = arr;
            measureSortingTime("std::sort", [](vector<int>& a) { sort(a.begin(), a.end()); }, arrCopy);
        }

        cout << endl;
    }
}

// Основная функция для замера времени выполнения различных сортировок
int main() {
    performTimeAnalysis();
    // Вызываем тесты
    return Catch::Session().run();
}

// Тесты
TEST_CASE("Пустой массив") {
    vector<int> input = {};
    vector<int> expected = {};
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Массив с одним элементом") {
    vector<int> input = { 1 };
    vector<int> expected = { 1 };
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Отсортированный массив") {
    vector<int> input = { 1, 2, 3 };
    vector<int> expected = { 1, 2, 3 };
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Неотсортированный массив") {
    vector<int> input = { 4, -5, 1, 0, 3 };
    vector<int> expected = { -5, 0, 1, 3, 4 };
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Отсортированный массив с дубликатами") {
    vector<int> input = { -5, -5, 0, 2, 3, 3, 8 };
    vector<int> expected = { -5, -5, 0, 2, 3, 3, 8 };
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Неотсортированный массив с дубликатами") {
    vector<int> input = { 4, 2, 4, -1, 0, 3, -1 };
    vector<int> expected = { -1, -1, 0, 2, 3, 4, 4 };
    customSort(input);
    REQUIRE(input == expected);
}

TEST_CASE("Большой массив") {
    vector<int> input = { 4, -2, 5, 0, 2, 120, 11, 6, -3, -67, 9, -21, 11 };
    vector<int> expected = { -67, -21, -3, -2, 0, 2, 4, 5, 6, 9, 11, 11, 120 };
    customSort(input);
    REQUIRE(input == expected);
}
