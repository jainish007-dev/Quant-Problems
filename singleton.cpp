#include <type_traits>
#include <cstdint>

class Singleton {
public:
    // Using noexcept and inline for maximum optimization.
    // The instance address is constant known at link time.
    [[gnu::always_inline]] 
    static inline Singleton* GetInstance() noexcept {
        return &instance;
    }

    // Delete copy/move
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    // Trivial default constructor allows constant initialization
    Singleton() = default;
    
    // Eagerly initialized static instance in .bss segment (zero-cost startup)
    static Singleton instance;
};

// Definition
Singleton Singleton::instance;
