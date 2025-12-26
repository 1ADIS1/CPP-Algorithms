// Benchmark.cpp
// Build (example):
//   g++ -O3 -std=c++17 Benchmark.cpp Quicksort.cpp -o benchmark
// Run:
//   ./benchmark
// Output:
//   Creates "benchmark_results.csv" in the current directory.
//
// NOTE: Quicksort.cpp must NOT define a main() when you compile this benchmark.
// If it contains the tests + main(), remove or comment them out or move algorithm
// definitions to a header.

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>
#include <limits>
#include <random>

#include "Quicksort.cpp"

#pragma once


int main(int argc, char** argv) {
    // Sizes to test (powers of two)
    std::vector<std::size_t> sizes = {
        128, 256, 512, 1024, 2048, 4096,
        8192, 16384, 32768, 65536, 131072
        // add more if you like (beware of insertion sort blow-up on large reversed arrays)
    };

    const int trials = 5; // runs per size, to average

    std::ofstream csv("benchmark_results.csv");
    if (!csv) {
        std::cerr << "Failed to open benchmark_results.csv for writing\n";
        return 1;
    }

    // Header:
    csv << "size,qs_avg_ns,qs_min_ns,qs_max_ns,is_avg_ns,is_min_ns,is_max_ns\n";

    // comparator
    auto comp = [](int a, int b) { return a < b; };

    for (std::size_t n : sizes) {
        std::vector<long long> qs_times;
        std::vector<long long> is_times;
        qs_times.reserve(trials);
        is_times.reserve(trials);

        for (int t = 0; t < trials; ++t) {
            // create reversed array: [n, n-1, ..., 1]
            std::vector<int> base(n);
            for (std::size_t i = 0; i < n; ++i) base[i] = static_cast<int>(n - i);

            // quicksort (without insertion-sort optimization)
            std::vector<int> qarr = base;
            auto t0 = std::chrono::steady_clock::now();
            qs::sort(qarr.data(), qarr.data() + qarr.size(), comp, /*use_insertion_sort=*/false);
            auto t1 = std::chrono::steady_clock::now();
            auto qs_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            if (!is_sorted_array(qarr.data(), qarr.data() + qarr.size(), comp)) {
                std::cerr << "Quicksort result is NOT sorted for n=" << n << " (trial " << t << ")\n";
            }
            qs_times.push_back(qs_ns);

            // insertion sort on reversed array (worst-case for insertion sort)
            std::vector<int> iarr = base;
            auto ti0 = std::chrono::steady_clock::now();
            qs::insertion_sort(iarr.data(), iarr.data() + iarr.size(), comp);
            auto ti1 = std::chrono::steady_clock::now();
            auto is_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(ti1 - ti0).count();
            if (!is_sorted_array(iarr.data(), iarr.data() + iarr.size(), comp)) {
                std::cerr << "Insertion sort result is NOT sorted for n=" << n << " (trial " << t << ")\n";
            }
            is_times.push_back(is_ns);

            // small pause between trials is usually unnecessary; we keep successive runs
        }

        auto avg = [](const std::vector<long long>& v) -> long double {
            if (v.empty()) return 0.0L;
            long double s = 0;
            for (auto x : v) s += static_cast<long double>(x);
            return s / v.size();
            };
        auto vmin = [](const std::vector<long long>& v) -> long long {
            if (v.empty()) return 0;
            return *std::min_element(v.begin(), v.end());
            };
        auto vmax = [](const std::vector<long long>& v) -> long long {
            if (v.empty()) return 0;
            return *std::max_element(v.begin(), v.end());
            };

        long double qs_avg = avg(qs_times);
        long long qs_min = vmin(qs_times);
        long long qs_max = vmax(qs_times);

        long double is_avg = avg(is_times);
        long long is_min = vmin(is_times);
        long long is_max = vmax(is_times);

        // write CSV row
        csv << n << ","
            << std::fixed << std::setprecision(0) << qs_avg << "," << qs_min << "," << qs_max << ","
            << std::fixed << std::setprecision(0) << is_avg << "," << is_min << "," << is_max << "\n";

        std::cout << "n=" << std::setw(7) << n
            << "  qs_avg(ms)=" << std::setw(9) << (qs_avg / 1e6)
            << "  is_avg(ms)=" << std::setw(9) << (is_avg / 1e6) << "\n";
    }

    csv.close();
    std::cout << "Wrote benchmark_results.csv\n";
    return 0;
}
