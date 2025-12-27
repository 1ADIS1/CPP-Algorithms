#include "Quicksort.h"

namespace qs {

    template<typename T, typename Compare>
    void insertion_sort(T* first, T* last, Compare comp) {
        for (T* it = first + 1; it != last; ++it) {
            T tmp = std::move(*it);
            T* j = it;
            // shift element to the right by move
            while (j > first && comp(tmp, *(j - 1))) {
                *j = std::move(*(j - 1));
                --j;
            }
            *j = std::move(tmp);
        }
    }

    /// Find median pivot point of *a, *b, *c according to comp.
    template<typename T, typename Compare>
    T* get_median_of_three(T* a, T* b, T* c, Compare comp) {
        if (comp(*a, *b)) {
            // a < b
            if (comp(*b, *c)) {
                // a < b < c
                return b;
            }
            else {
                // b >= c
                // return middle of a and c
                return comp(*a, *c) ? c : a;
            }
        }
        else {
            // a >= b
            if (comp(*a, *c)) {
                // b <= a < c
                return a;
            }
            else {
                // if a >= b and a >= c
                // then median is either b or c
                return comp(*b, *c) ? c : b;
            }
        }
    }

    /// Hoare's partitioning but with setting pivot at last index.
    template<typename T, typename Compare>
    T* partition(T* first, T* last, Compare comp) {
        T* mid = first + (last - first) / 2;
        T* pivot = get_median_of_three(first, mid, last - 1, comp);
        // move pivot to last-1
        std::swap(*pivot, *(last - 1));

        T* left = first;
        // because last-1 is pivot
        T* right = last - 2;

        while (true) {
            // move right
            while (left <= right && comp(*left, *(last - 1))) ++left;
            // move left
            while (right >= left && comp(*(last - 1), *right)) --right;
            if (left >= right) break;
            std::swap(*left, *right);
            ++left;
            --right;
        }
        // move pivot into its final position
        std::swap(*left, *(last - 1));
        return left;
    }

    template<typename T, typename Compare>
    void sort(T* first, T* last, Compare comp, bool use_insertion_sort) {
        // value between 5 to 15 is likely to work well. 
        // https://algs4.cs.princeton.edu/23quicksort/
        const std::size_t insertion_threshold = 10;

        // iteration + recursion on smaller partition
        while (last - first > insertion_threshold) {
            T* pivot = partition(first, last, comp);

            // determine lengths
            int64_t left_size = pivot - first;
            int64_t right_size = last - (pivot + 1);

            // recurse on small half
            if (left_size < right_size) {
                if (left_size > 0) sort(first, pivot, comp, use_insertion_sort);
                // continue with right half
                first = pivot + 1;
            }
            else {
                if (right_size > 0) sort(pivot + 1, last, comp, use_insertion_sort);
                // continue with left half
                last = pivot;
            }
        }

        if (!use_insertion_sort) {
            std::sort(first, last, comp);
        }
        else {
            // for small partitions use insertion sort
            insertion_sort(first, last, comp);
        }
    }

}

template<typename T, typename Compare>
bool is_sorted_array(T* first, T* last, Compare comp) {
    if (first == last) return true;
    for (T* it = first + 1; it != last; ++it) {
        if (comp(*it, *(it - 1))) return false;
    }
    return true;
}

//TEST(QuickSortTest, EmptyArray) {
//    int a[1];
//    qs::sort(a, a, [](int a, int b) { return a < b; }, true);
//    SUCCEED();
//}
//
//TEST(QuickSortTest, SingleElement) {
//    int a[1] = { 42 };
//    qs::sort(a, a + 1, [](int a, int b) { return a < b; }, true);
//    EXPECT_EQ(a[0], 42);
//}
//
//TEST(QuickSortTest, AlreadySorted) {
//    int a[] = { 1,2,3,4,5,6,7,8,9,10 };
//    qs::sort(a, a + 10, [](int a, int b) { return a < b; }, true);
//    EXPECT_TRUE(is_sorted_array(a, a + 10, [](int a, int b) { return a < b; }));
//}
//
//TEST(QuickSortTest, ReverseSorted) {
//    int a[] = { 10,9,8,7,6,5,4,3,2,1 };
//    qs::sort(a, a + 10, [](int a, int b) { return a < b; }, true);
//    EXPECT_TRUE(is_sorted_array(a, a + 10, [](int a, int b) { return a < b; }));
//}
//
//TEST(QuickSortTest, Duplicates) {
//    int a[] = { 5,1,3,5,5,2,2,1,9,5,5,5 };
//    qs::sort(a, a + 12, [](int a, int b) { return a < b; }, true);
//    EXPECT_TRUE(is_sorted_array(a, a + 12, [](int a, int b) { return a < b; }));
//}
//
//TEST(QuickSortTest, RandomLarge) {
//    const int N = 1000;
//    std::vector<int> v(N);
//    std::mt19937_64 rng(12345);
//    for (int i = 0; i < N; ++i) v[i] = std::uniform_int_distribution<int>(-10000, 10000)(rng);
//    qs::sort(v.data(), v.data() + v.size(), [](int a, int b) { return a < b; }, true);
//    EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
//}
//
//TEST(QuickSortTest, VariousLengths) {
//    for (int n = 0; n <= 100; ++n) {
//        std::vector<int> v(n);
//        for (int i = 0; i < n; ++i) v[i] = n - i; // reverse
//        qs::sort(v.data(), v.data() + v.size(), [](int a, int b) { return a < b; }, true);
//        EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
//    }
//}
//
//class Player {
//public:
//    int health;
//    int armor;
//    Player(int a = 0, int h = 0) : health(h), armor(a) {}
//};
//
//
//TEST(QuickSortTest, PlayerType) {
//    Player arr[] = { Player(5,50), Player(1,100), Player(3,75), Player(3,75), Player(2,60) };
//    qs::sort(arr, arr + 5, [](const Player& a, const Player& b) {
//        if (a.health != b.health) return a.health < b.health;
//        return a.armor < b.armor;
//        }, true);
//    for (int i = 1; i < 5; ++i) {
//        EXPECT_LE(arr[i - 1].health, arr[i].health);
//        if (arr[i - 1].health == arr[i].health) EXPECT_LE(arr[i - 1].armor, arr[i].armor);
//    }
//}
//
//TEST(QuickSortTest, RandomPlayers) {
//    const int N = 1000;
//    std::vector<Player> v;
//    v.reserve(N);
//    std::mt19937_64 rng(54321);
//    for (int i = 0; i < N; ++i) v.emplace_back(std::uniform_int_distribution<int>(0, 100)(rng), std::uniform_int_distribution<int>(0, 1000)(rng));
//    qs::sort(v.data(), v.data() + v.size(), [](const Player& a, const Player& b) {
//        if (a.health != b.health) return a.health < b.health;
//        return a.armor < b.armor;
//        }, true);
//    // verify sorted
//    for (size_t i = 1; i < v.size(); ++i) {
//        EXPECT_FALSE((v[i].health < v[i - 1].health) || (v[i].health == v[i - 1].health && v[i].armor < v[i - 1].armor));
//    }
//}
//
//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}
