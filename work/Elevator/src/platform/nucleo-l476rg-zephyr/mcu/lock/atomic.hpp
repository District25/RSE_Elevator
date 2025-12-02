#ifndef LOCK_ATOMIC_H
#define LOCK_ATOMIC_H

#include <cstdint>
#include <zephyr/sys/atomic.h>

/**
 * @brief Atomic increment and decrement of a variable.
 * 
 * Incrementation and decrementation of the value is Thread and ISR safe.
 */
template <typename T = uint32_t> class Atomic
{
public:
    Atomic<T>() :
        atomic(0)
	{}

    T operator++() { return (T)atomic_inc(&atomic); }
    T operator--() { return (T)atomic_dec(&atomic); }

    T get() const { return (T)atomic_get(&atomic); }
    operator T() const { return get(); }

private:
    atomic_t atomic;
};

#endif // LOCK_ATOMIC_H
