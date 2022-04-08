#ifndef CIRCLE_DEQUE_H
#define CIRCLE_DEQUE_H
#include "./02_ArrayList.h"

template <typename T>
class CircleDeque
{
    template <typename U>
    friend inline std::ostream &operator<<(std::ostream &os, const CircleDeque<U> &deque);

private:
    const size_t DEFAULT_CAPACITY = 8;
    size_t _capacity, _size, _front;
    std::shared_ptr<T> *_array;
    inline int true_index(int index) const;
    inline void ensure_capacity();

public:
    inline CircleDeque<T> &operator=(const CircleDeque<T> &deque);
    inline CircleDeque<T> &operator=(CircleDeque<T> &&deque);
    inline CircleDeque();
    CircleDeque(const CircleDeque<T> &deque) { *this = deque; }
    CircleDeque(CircleDeque<T> &&deque) { *this = std::move(deque); }
    inline ~CircleDeque();
    size_t size() const { return _size; }
    size_t capacity() const { return _capacity; }
    bool is_empty() const { return _size == 0; }
    std::shared_ptr<T> enqueue(std::shared_ptr<T> data);
    std::shared_ptr<T> dequeue();
    std::shared_ptr<T> enqueue_front(std::shared_ptr<T> data);
    std::shared_ptr<T> dequeue_rear();
    std::shared_ptr<T> front() const { return _array[_front]; }
    std::shared_ptr<T> rear() const { return _array[true_index(_size - 1)]; }
    void clear();
};

template <typename U>
std::ostream &operator<<(std::ostream &os, const CircleDeque<U> &deque)
{
    for (size_t i = 0; i < deque._capacity; ++i)
    {
        if (deque._array[i] != nullptr)
            os << *deque._array[i];
        else
            os << "nullptr\n";
    }
    return os;
}

template <typename T>
CircleDeque<T> &CircleDeque<T>::operator=(const CircleDeque<T> &deque)
{
    delete[] _array;
    _array = new std::shared_ptr<T>[deque._capacity];
    _capacity = deque._capacity;
    _size = deque._size;
    _front = deque._front;
    for (size_t i = 0; i < deque._capacity; ++i)
        _array[i] = deque._array[i];
    return *this;
}

template <typename T>
CircleDeque<T> &CircleDeque<T>::operator=(CircleDeque<T> &&deque)
{
    _array = deque._array;
    _capacity = deque._capacity;
    _size = deque._size;
    _front = deque._front;
    deque._size = 0;
    deque._front = 0;
    return *this;
}

template <typename T>
CircleDeque<T>::CircleDeque()
{
    _size = 0;
    _front = 0;
    _capacity = DEFAULT_CAPACITY;
    _array = new std::shared_ptr<T>[_capacity];
}

template <typename T>
CircleDeque<T>::~CircleDeque()
{
    clear();
    delete[] _array;
}

template <typename T>
std::shared_ptr<T> CircleDeque<T>::enqueue(std::shared_ptr<T> data)
{
    ensure_capacity();
    _array[true_index(_size)] = data;
    _size++;
    return data;
}

template <typename T>
std::shared_ptr<T> CircleDeque<T>::dequeue()
{
    std::shared_ptr<T> old = _array[_front];
    _array[_front] = nullptr;
    _front = true_index(1);
    _size--;
    return old;
}

template <typename T>
std::shared_ptr<T> CircleDeque<T>::enqueue_front(std::shared_ptr<T> data)
{
    ensure_capacity();
    _front = true_index(-1);
    _array[_front] = data;
    _size++;
    return data;
}

template <typename T>
std::shared_ptr<T> CircleDeque<T>::dequeue_rear()
{
    int rear = true_index(_size - 1);
    std::shared_ptr<T> old = _array[rear];
    _array[rear] = nullptr;
    _size--;
    return old;
}

template <typename T>
void CircleDeque<T>::clear()
{
    for (size_t i = 0; i < _capacity; ++i)
        _array[i] = nullptr;
    _front = 0;
    _size = 0;
}

template <typename T>
int CircleDeque<T>::true_index(int index) const
{
    index += _front;
    if (index < 0)
        return index + _capacity;
    return index % _capacity;
}

template <typename T>
void CircleDeque<T>::ensure_capacity()
{
    if (_size >= _capacity)
    {
        size_t new_cap = _capacity << 1;
        auto temp = new std::shared_ptr<T>[new_cap];
        for (size_t i = 0; i < _size; ++i)
            temp[i] = _array[true_index(i)];
        _array = temp;
        _front = 0;
        _capacity = new_cap;
    }
}

#endif /* CIRCLE_DEQUE_H */