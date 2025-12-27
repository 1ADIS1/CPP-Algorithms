#include "DynamicArray.h"

template<typename T>
class Array final {
public:
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
    }

    // Copy constructor
    Array(const Array& other) : m_size(0), m_capacity(other.m_capacity), m_data(nullptr) {
        allocate(m_capacity);
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

    Array& operator=(Array other) {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
        return *this;
    }

    // Insert at end
    int insert(const T& value) {
        return insert(m_size, value);
    }

    int insert(int index, const T& value) {
        if (m_size + 1 > m_capacity) {
            grow();
        }

        // shift to right starting from last to index
        if (m_size > 0 && index < m_size) {
            for (int i = m_size - 1; i >= index; --i) {
                // move or copy element from i to i+1
                if constexpr (std::is_move_constructible<T>::value) {
                    new (&m_data[i + 1]) T(std::move(m_data[i]));
                }
                else {
                    new (&m_data[i + 1]) T(m_data[i]);
                }
                m_data[i].~T();
            }
            new (&m_data[index]) T(value);
        }
        else {
            new (&m_data[m_size]) T(value);
        }
        ++m_size;
        return index;
    }

    void remove(int index) {
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
        void* block = std::malloc(sizeof(T) * capacity);
        if (!block) throw std::bad_alloc();
        m_data = reinterpret_cast<T*>(block);
        m_capacity = capacity;
    }

    void grow() {
        int newCapacity = std::max(m_capacity + 1, static_cast<int>(std::ceil(m_capacity * 1.6)));
        if (newCapacity <= m_capacity) newCapacity = m_capacity + 1;

        // allocate new block
        void* block = std::malloc(sizeof(T) * newCapacity);
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
