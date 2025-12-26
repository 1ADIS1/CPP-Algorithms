#include "DynamicArray.h"

template<typename T>
class Array final {
public:
    class ConstIterator;
    class Iterator {
    public:
        Iterator(Array<T>* array = nullptr, int pos = 0, bool rev = false)
            : m_array(array), m_pos(pos), m_rev(rev) {
        }
        const T& get() const { return m_array->m_data[m_pos]; }
        void set(const T& value) { m_array->m_data[m_pos] = value; }
        void next() { if (m_rev) --m_pos; else ++m_pos; }
        bool hasNext() const {
            if (!m_array) return false;
            return m_rev ? (m_pos >= 0) : (m_pos < m_array->m_size);
        }
    private:
        Array<T>* m_array;
        int m_pos;
        bool m_rev;
    };

    class ConstIterator {
    public:
        ConstIterator(const Array<T>* array = nullptr, int pos = 0, bool rev = false)
            : m_array(array), m_pos(pos), m_rev(rev) {
        }
        const T& get() const { return m_array->m_data[m_pos]; }
        void next() { if (m_rev) --m_pos; else ++m_pos; }
        bool hasNext() const {
            if (!m_array) return false;
            return m_rev ? (m_pos >= 0) : (m_pos < m_array->m_size);
        }
    private:
        const Array<T>* m_array;
        int m_pos;
        bool m_rev;
    };

    Array() : m_size(0), m_capacity(kDefaultCapacity), m_data(nullptr) {
        allocate(m_capacity);
    }

    explicit Array(int capacity) : m_size(0), m_capacity(capacity), m_data(nullptr) {
        if (m_capacity <= 0) m_capacity = kDefaultCapacity;
        allocate(m_capacity);
    }

    ~Array() {
        clearElements();
        free(m_data);
        m_data = nullptr;
        m_capacity = 0;
    }

    // Copy constructor
    Array(const Array& other) : m_size(0), m_capacity(other.m_capacity), m_data(nullptr) {
        allocate(m_capacity);
        // copy-construct elements
        for (int i = 0; i < other.m_size; ++i) {
            new (&m_data[i]) T(other.m_data[i]);
            ++m_size;
        }
    }
        
    // Move constructor
    Array(Array&& other) noexcept : m_size(other.m_size), m_capacity(other.m_capacity), m_data(other.m_data) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    // Copy-and-swap assignment (single operator=)
    Array& operator=(Array other) {
        // 'other' is either copy-constructed or move-constructed depending on context
        swap(other);
        return *this;
    }

    // Insert at end
    int insert(const T& value) {
        return insert(m_size, value);
    }

    // Insert at index
    int insert(int index, const T& value) {
        // no bounds checks as specified
        if (m_size + 1 > m_capacity) {
            grow();
        }
        // shift elements to the right starting from last to index
        if (m_size > 0 && index < m_size) {
            for (int i = m_size - 1; i >= index; --i) {
                // move or copy element from i to i+1
                if constexpr (std::is_move_constructible<T>::value) {
                    new (&m_data[i + 1]) T(std::move(m_data[i]));
                }
                else {
                    new (&m_data[i + 1]) T(m_data[i]);
                }
                // destroy old instance
                m_data[i].~T();
            }
            // construct inserted value at index
            new (&m_data[index]) T(value);
        }
        else {
            // inserting at end
            new (&m_data[m_size]) T(value);
        }
        ++m_size;
        return index;
    }

    void remove(int index) {
        // destroy element at index
        m_data[index].~T();
        // shift left
        for (int i = index; i < m_size - 1; ++i) {
            if constexpr (std::is_move_constructible<T>::value) {
                new (&m_data[i]) T(std::move(m_data[i + 1]));
            }
            else {
                new (&m_data[i]) T(m_data[i + 1]);
            }
            m_data[i + 1].~T();
        }
        --m_size;
    }

    const T& operator[](int index) const {
        assert(index >= 0 && index < m_size);
        return m_data[index];
    }

    T& operator[](int index) {
        assert(index >= 0 && index < m_size);
        return m_data[index];
    }

    int size() const { return m_size; }

    Iterator iterator() { return Iterator(this, 0, false); }
    ConstIterator iterator() const { return ConstIterator(this, 0, false); }

    Iterator reverseIterator() { return Iterator(this, m_size - 1, true); }
    ConstIterator reverseIterator() const { return ConstIterator(this, m_size - 1, true); }

private:
    static constexpr int kDefaultCapacity = 8;
    int m_size;
    int m_capacity;
    T* m_data;

    void allocate(int capacity) {
        // allocate raw memory
        void* block = std::malloc(sizeof(T) * static_cast<size_t>(capacity));
        if (!block) throw std::bad_alloc();
        m_data = reinterpret_cast<T*>(block);
        m_capacity = capacity;
    }

    void grow() {
        int newCapacity = std::max(m_capacity + 1, static_cast<int>(std::ceil(m_capacity * 1.6)));
        if (newCapacity <= m_capacity) newCapacity = m_capacity + 1;
        // allocate new block
        void* block = std::malloc(sizeof(T) * static_cast<size_t>(newCapacity));
        if (!block) throw std::bad_alloc();
        T* newData = reinterpret_cast<T*>(block);
        // move or copy elements into new block
        for (int i = 0; i < m_size; ++i) {
            if constexpr (std::is_move_constructible<T>::value) {
                new (&newData[i]) T(std::move(m_data[i]));
            }
            else {
                new (&newData[i]) T(m_data[i]);
            }
            // destroy old
            m_data[i].~T();
        }
        // free old block
        std::free(m_data);
        m_data = newData;
        m_capacity = newCapacity;
    }

    void clearElements() {
        for (int i = 0; i < m_size; ++i) {
            m_data[i].~T();
        }
        m_size = 0;
    }
};

// ----------------- Google Test cases -----------------

TEST(ArrayBasic, InsertAndIndex) {
    Array<int> a;
    for (int i = 0; i < 10; ++i) a.insert(i + 1);
    ASSERT_EQ(a.size(), 10);
    for (int i = 0; i < a.size(); ++i) a[i] *= 2;
    for (int i = 0; i < 10; ++i) EXPECT_EQ(a[i], (i + 1) * 2);
}

TEST(ArrayIterator, ForwardAndReverse) {
    Array<int> a;
    for (int i = 0; i < 5; ++i) a.insert(i + 1);
    int expected = 1;
    for (auto it = a.iterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), expected);
        ++expected;
    }
    expected = 5;
    for (auto it = a.reverseIterator(); it.hasNext(); it.next()) {
        EXPECT_EQ(it.get(), expected);
        --expected;
    }
}

TEST(ArrayInsertRemove, AtIndex) {
    Array<int> a;
    a.insert(0, 1);
    a.insert(1, 3);
    a.insert(1, 2); // 1,2,3
    ASSERT_EQ(a.size(), 3);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 2);
    EXPECT_EQ(a[2], 3);
    a.remove(1); // 1,3
    ASSERT_EQ(a.size(), 2);
    EXPECT_EQ(a[0], 1);
    EXPECT_EQ(a[1], 3);
}

TEST(ArrayCopyMove, CopyAndMove) {
    Array<std::string> a;
    a.insert(std::string("one"));
    a.insert(std::string("two"));
    Array<std::string> b = a; // copy
    EXPECT_EQ(b.size(), a.size());
    EXPECT_EQ(b[0], "one");
    Array<std::string> c = std::move(a); // move
    EXPECT_EQ(c.size(), 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
