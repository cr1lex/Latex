/**
 * sorting.cpp — реализация алгоритмов сортировки на C++17.
 *
 * Содержит:
 *   bubbleSort — пузырьковая сортировка
 *   mergeSort  — сортировка слиянием
 *   quickSort  — быстрая сортировка
 *   benchmark  — замер производительности
 */

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iomanip>
#include <random>
#include <vector>

// ---------------------------------------------------------------------------
// Пузырьковая сортировка
// ---------------------------------------------------------------------------

template <typename It>
void bubbleSort(It first, It last) {
    for (auto i = first; i != last; ++i) {
        bool swapped = false;
        for (auto j = first; j != std::prev(last, i - first + 1); ++j) {
            if (*std::next(j) < *j) {
                std::iter_swap(j, std::next(j));
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// ---------------------------------------------------------------------------
// Сортировка слиянием
// ---------------------------------------------------------------------------

template <typename It>
void doMerge(It first, It mid, It last,
             std::vector<typename std::iterator_traits<It>::value_type>& buf) {
    buf.clear();
    auto l = first, r = mid;
    while (l != mid && r != last)
        buf.push_back(*l < *r ? *l++ : *r++);
    buf.insert(buf.end(), l, mid);
    buf.insert(buf.end(), r, last);
    std::copy(buf.begin(), buf.end(), first);
}

template <typename It>
void mergeSort(It first, It last,
               std::vector<typename std::iterator_traits<It>::value_type>& buf) {
    if (std::distance(first, last) <= 1) return;
    auto mid = std::next(first, std::distance(first, last) / 2);
    mergeSort(first, mid, buf);
    mergeSort(mid, last, buf);
    doMerge(first, mid, last, buf);
}

// Публичная перегрузка без буфера (буфер создаётся внутри)
template <typename It>
void mergeSort(It first, It last) {
    std::vector<typename std::iterator_traits<It>::value_type> buf;
    buf.reserve(std::distance(first, last));
    mergeSort(first, last, buf);
}

// ---------------------------------------------------------------------------
// Быстрая сортировка
// ---------------------------------------------------------------------------

template <typename It>
It medianOfThree(It first, It last) {
    auto mid  = std::next(first, std::distance(first, last) / 2);
    auto back = std::prev(last);
    if (*mid  < *first) std::iter_swap(first, mid);
    if (*back < *first) std::iter_swap(first, back);
    if (*mid  < *back)  std::iter_swap(mid,   back);
    return back; // опорный элемент помещён на последнюю позицию
}

template <typename It>
void quickSort(It first, It last) {
    if (std::distance(first, last) <= 1) return;
    auto pivot = medianOfThree(first, last);
    auto p = std::partition(first, std::prev(last),
                            [&pivot](const auto& x) { return x < *pivot; });
    std::iter_swap(p, pivot);
    quickSort(first, p);
    quickSort(std::next(p), last);
}

// ---------------------------------------------------------------------------
// Замер производительности
// ---------------------------------------------------------------------------

template <typename SortFn>
double benchmark(SortFn sortFn, int n, int repeats = 5) {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, 10 * n);
    double total = 0.0;
    for (int r = 0; r < repeats; ++r) {
        std::vector<int> data(n);
        std::generate(data.begin(), data.end(), [&] { return dist(rng); });
        auto t0 = std::chrono::high_resolution_clock::now();
        sortFn(data.begin(), data.end());
        auto t1 = std::chrono::high_resolution_clock::now();
        total += std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
    return total / repeats;
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    const std::vector<int> sizes = {500, 1000, 2000, 5000, 10000};

    struct Algo {
        std::string name;
        std::function<void(std::vector<int>::iterator, std::vector<int>::iterator)> fn;
    };

    std::vector<Algo> algos = {
        {"bubbleSort", [](auto b, auto e) { bubbleSort(b, e); }},
        {"mergeSort",  [](auto b, auto e) { mergeSort(b, e);  }},
        {"quickSort",  [](auto b, auto e) { quickSort(b, e);  }},
    };

    std::cout << std::setw(8) << "n";
    for (const auto& a : algos)
        std::cout << std::setw(14) << a.name;
    std::cout << "\n" << std::string(8 + 14 * algos.size(), '-') << "\n";

    for (int n : sizes) {
        std::cout << std::setw(8) << n;
        for (const auto& a : algos) {
            double ms = benchmark(a.fn, n);
            std::cout << std::setw(14) << std::fixed << std::setprecision(2) << ms;
        }
        std::cout << "\n";
    }

    return 0;
}
