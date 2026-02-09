Question for the code !

Implement a singleton.
In software engineering, oftentimes loggers, settings managers, database connections, and other objects are implemented as singletons. A singleton is a design pattern that restricts the user from creating more than one instance of said object. This means that it cannot be copied or moved.

Requirements
Implement a class called Singleton where only one instance of it can be returned through the GetInstance method.

The Singleton should not be movable or copyable.

Test Cases
copy:- Your object cannot be copied.
move:- Your object cannot be moved.
create:- Can only create one instance of the object.

There is one hidden test case.
How do you ensure thread-safety on instance creation?

# High-Performance C++ Singleton

This repository contains an optimized C++ Singleton implementation designed for ultra-low latency applications (e.g., High Frequency Trading).

## Implementation Details

The implementation uses **Eager Initialization** rather than the standard "Lazy" Meyers Singleton.

### Why Eager Initialization?

In standard C++11 Singletons (Meyers Singleton), the instance is created on the first call to `GetInstance()`:

```cpp
// Standard (Lazy)
static Singleton& GetInstance() {
    static Singleton instance; // Thread-safe init check happens here!
    return instance;
}
```

While thread-safe, the compiler inserts a hidden atomic check (guard variable) to ensure the constructor runs only once. This adds a small overhead of ~5-10 nanoseconds (or more with contention) on *every single call*.

For latency-critical systems where `GetInstance()` might be called millions of times per second in a hot loop, this overhead is unacceptable.

### Our Solution (Zero-Cost)

We define the instance as a `static` member variable that is initialized at program startup (before `main()`):

```cpp
// Optimized (Eager)
static inline Singleton* GetInstance() noexcept {
    return &instance; // Pure pointer load (1 instruction)
}
```

This compiles down to a simple memory load instruction, effectively **0 runtime overhead**.

## Code Structure

```cpp
class Singleton {
public:
    // [[gnu::always_inline]] forces the compiler to inline this function
    // resulting in zero function call overhead.
    [[gnu::always_inline]] 
    static inline Singleton* GetInstance() noexcept {
        return &instance;
    }

    // Delete copy/move semantics to enforce Singleton property
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    Singleton() = default;
    static Singleton instance;
};

// Initialized before main()
Singleton Singleton::instance;
```

---

## Common Interview Questions

### 1. Is this Thread-Safe?
**Answer**: Yes.
- **Reads**: `GetInstance()` returns a pointer to a memory location that is guaranteed to be initialized before `main()` starts. Concurrent reads of a constant address are thread-safe.
- **Writes**: If the Singleton has mutable state, you must still use `std::mutex` or `std::atomic` for that state. The *access* to the instance itself is thread-safe.

### 2. What is the "Static Initialization Order Fiasco"?
**Answer**: If this Singleton depends on *another* global static object in its constructor, we might crash because the order of initialization between different translation units (cpp files) is undefined.
- **Fix**: Since this Singleton implementation uses a `default` (trivial) constructor, it has no dependencies, making it safe from this specific issue.

### 3. Why delete the Copy/Move constructors?
**Answer**: A Singleton must be unique. If we allowed copying (`Singleton s2 = *Singleton::GetInstance()`), we would have two instances of the class, breaking the design pattern.

### 4. What is the difference between `alignas(64)` and standard alignment?
**Answer**: `alignas(64)` aligns the object to a CPU cache line (typically 64 bytes). This prevents "False Sharing" in multi-threaded environments, where writing to an adjacent variable in memory might invalidate the cache line containing our Singleton, causing performance degradation.

### 5. Why use `noexcept`?
**Answer**: It allows the compiler to optimize the function by removing exception handling logic (unwinding tables), reducing binary size and runtime overhead.
