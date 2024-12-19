#ifndef FAST_FIXED_H
#define FAST_FIXED_H

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <limits>

#include <compare>

#include "Fixed.h"

template<int N, int K>
class FastFixed {
public:
    using StorageType = typename std::conditional<N <= 64, typename std::conditional<N <= 32, typename std::conditional<N <= 16, typename std::conditional<N <= 8, int_fast8_t, int_fast16_t>::type, int_fast32_t>::type, int_fast64_t>::type, void>::type;
  
    StorageType v;

    constexpr FastFixed() : v(0) {}

    constexpr FastFixed(int i) : v(static_cast<StorageType>(i) << K){}

    constexpr FastFixed(float f) : v(static_cast<StorageType>(f * (1 << K))){}

    constexpr FastFixed(double d) : v(static_cast<StorageType>(d * (1 << K))){}

    static constexpr FastFixed from_raw(StorageType x) {
        FastFixed ret;
        ret.v = x;
        return ret;
    }

    static void size() {
        std::cout << sizeof(v) << std::endl;
    }

    // Conversion to float
    explicit operator float() const {
        return static_cast<float>(v) / (1 << K);
    }

    // Conversion to double
    explicit operator double() const {
        return static_cast<double>(v) / (1 << K);
    }
    

    // Addition with another FastFixed<N, K>
    template<int N2, int K2>
    FastFixed<N, K> operator+(const FastFixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return FastFixed<N, K>::from_raw(v + other.v);
        } else if constexpr (K > K2) {
            return FastFixed<N, K>::from_raw(v + (other.v << (K - K2)));
        } else {
            return FastFixed<N, K>::from_raw((v << (K2 - K)) + other.v);
        }
    }

    // Multiplication with FastFixed<N2, K2>
    template<int N2, int K2>
    FastFixed<N, K> operator*(const FastFixed<N2, K2>& other) const {
        // Use int64_t for intermediate calculation to avoid overflow
        int64_t result = static_cast<int64_t>(v) * other.v;
        if constexpr (K + K2 >= K) {
            result >>= K2;
        } else {
            result <<= (K - K2);
        }
        return FastFixed<N, K>::from_raw(static_cast<StorageType>(result));
    }

    // Division with FastFixed<N2, K2>
    template<int N2, int K2>
    FastFixed<N, K> operator/(const FastFixed<N2, K2>& other) const {
        // Use int64_t for intermediate calculation to avoid overflow
        int64_t result = (static_cast<int64_t>(v) << K2) / other.v;
        if constexpr (K2 > K) {
            result >>= (K2 - K);
        } else if constexpr (K > K2) {
            result <<= (K - K2);
        }
        return FastFixed<N, K>::from_raw(static_cast<StorageType>(result));
    }

    // Subtraction with FastFixed<N2, K2>
    template<int N2, int K2>
    FastFixed<N, K> operator-(const FastFixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return FastFixed<N, K>::from_raw(v - other.v);
        } else if constexpr (K > K2) {
            return FastFixed<N, K>::from_raw(v - (other.v << (K - K2)));
        } else {
            return FastFixed<N, K>::from_raw((v << (K2 - K)) - other.v);
        }
    }


    // Addition with double
    FastFixed<N, K> operator+(double d) const {
        return *this + FastFixed<N, K>(d);
    }

    // Addition with float
    FastFixed<N, K> operator+(float f) const {
        return *this + FastFixed<N, K>(f);
    }

    // Addition with double
    FastFixed<N, K> operator-(double d) const {
        return *this - FastFixed<N, K>(d);
    }

    // Addition with float
    FastFixed<N, K> operator-(float f) const {
        return *this -  FastFixed<N, K>(f);
    }

    // Addition with double
    FastFixed<N, K> operator*(double d) const {
        return *this * FastFixed<N, K>(d);
    }

    // Addition with float
    FastFixed<N, K> operator*(float f) const {
        return *this * FastFixed<N, K>(f);
    }

    // Addition with double
    FastFixed<N, K> operator/(double d) const {
        return *this / FastFixed<N, K>(d);
    }

    // Addition with float
    FastFixed<N, K> operator/(float f) const {
        return *this / FastFixed<N, K>(f);
    }



    template<int N2, int K2>
    FastFixed<N, K> operator+(const Fixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return FastFixed<N, K>::from_raw(v + other.v);
        } else if constexpr (K > K2) {
            return FastFixed<N, K>::from_raw(v + (other.v << (K - K2)));
        } else {
            return FastFixed<N, K>::from_raw((v << (K2 - K)) + other.v);
        }
    }

    template<int N2, int K2>
    FastFixed<N, K> operator-(const Fixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return FastFixed<N, K>::from_raw(v - other.v);
        } else if constexpr (K > K2) {
            return FastFixed<N, K>::from_raw(v - (other.v << (K - K2)));
        } else {
            return FastFixed<N, K>::from_raw((v << (K2 - K)) - other.v);
        }
    }

    template<int N2, int K2>
    FastFixed<N, K> operator*(const Fixed<N2, K2>& other) const {
        int64_t result = static_cast<int64_t>(v) * other.v;
        if constexpr (K + K2 >= K) {
            result >>= K2;
        } else {
            result <<= (K - K2);
        }
        return FastFixed<N, K>::from_raw(static_cast<StorageType>(result));
    }

    template<int N2, int K2>
    FastFixed<N, K> operator/(const Fixed<N2, K2>& other) const {
        int64_t result = (static_cast<int64_t>(v) << K2) / other.v;
        if constexpr (K2 > K) {
            result >>= (K2 - K);
        } else if constexpr (K > K2) {
            result <<= (K - K2);
        }
        return FastFixed<N, K>::from_raw(static_cast<StorageType>(result));
    }


    // Comparison operators
    bool operator<(const FastFixed<N, K>& other) const { return v < other.v; }
    bool operator>(const FastFixed<N, K>& other) const { return v > other.v; }
    bool operator<=(const FastFixed<N, K>& other) const { return v <= other.v; }
    bool operator>=(const FastFixed<N, K>& other) const { return v >= other.v; }
    bool operator==(const FastFixed<N, K>& other) const { return v == other.v; }
    bool operator!=(const FastFixed<N, K>& other) const { return v != other.v; }

    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const FastFixed<N, K>& f) {
        os << static_cast<double>(f);
        return os;
    }
};

// Non-member operator+ for double + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator+(double d, const FastFixed<N, K>& f) {
    return FastFixed<N, K>(d) + f;
}

// Non-member operator+ for float + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator+(float f, const FastFixed<N, K>& fixed) {
    return FastFixed<N, K>(f) + fixed;
}

// Non-member operator+ for double + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator-(double d, const FastFixed<N, K>& f) {
    return FastFixed<N, K>(d) - f;
}

// Non-member operator+ for float + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator-(float f, const FastFixed<N, K>& fixed) {
    return FastFixed<N, K>(f) - fixed;
}

// Non-member operator+ for double + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator*(double d, const FastFixed<N, K>& f) {
    return FastFixed<N, K>(d) * f;
}

// Non-member operator+ for float + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator*(float f, const FastFixed<N, K>& fixed) {
    return FastFixed<N, K>(f) * fixed;
}

// Non-member operator+ for double + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator/(double d, const FastFixed<N, K>& f) {
    return FastFixed<N, K>(d) / f;
}

// Non-member operator+ for float + FastFixed<N, K>
template<int N, int K>
FastFixed<N, K> operator/(float f, const FastFixed<N, K>& fixed) {
    return FastFixed<N, K>(f) / fixed;
}



template<int N, int K, int N2, int K2>
Fixed<N, K> operator+(const Fixed<N, K>& lhs, const FastFixed<N2, K2>& rhs) {
    if constexpr (K == K2) {
        return Fixed<N, K>::from_raw(lhs.v + rhs.v);
    } else if constexpr (K > K2) {
        return Fixed<N, K>::from_raw(lhs.v + (rhs.v << (K - K2)));
    } else {
        return Fixed<N, K>::from_raw((lhs.v << (K2 - K)) + rhs.v);
    }
}

template<int N, int K, int N2, int K2>
Fixed<N, K> operator-(const Fixed<N, K>& lhs, const FastFixed<N2, K2>& rhs) {
    if constexpr (K == K2) {
        return Fixed<N, K>::from_raw(lhs.v - rhs.v);
    } else if constexpr (K > K2) {
        return Fixed<N, K>::from_raw(lhs.v - (rhs.v << (K - K2)));
    } else {
        return Fixed<N, K>::from_raw((lhs.v << (K2 - K)) - rhs.v);
    }
}


template<int N, int K, int N2, int K2>
Fixed<N, K> operator*(const Fixed<N, K>& lhs, const FastFixed<N2, K2>& rhs) {
    int64_t result = static_cast<int64_t>(lhs.v) * rhs.v;
    if constexpr (K + K2 >= K) {
        result >>= K2;
    } else {
        result <<= (K - K2);
    }
    return Fixed<N, K>::from_raw(static_cast<typename Fixed<N, K>::StorageType>(result));
}


template<int N, int K, int N2, int K2>
Fixed<N, K> operator/(const Fixed<N, K>& lhs, const FastFixed<N2, K2>& rhs) {
    int64_t result = (static_cast<int64_t>(lhs.v) << K2) / rhs.v;
    if constexpr (K2 > K) {
        result >>= (K2 - K);
    } else if constexpr (K > K2) {
        result <<= (K - K2);
    }
    return Fixed<N, K>::from_raw(static_cast<typename Fixed<N, K>::StorageType>(result));
}


template <int N, int M, int N1, int M1>
FastFixed<N, M> &operator+=(FastFixed<N, M> &a, Fixed<N1, M1> b) {
    return a = a + b;
}

template <int N, int M, int N1, int M1>
FastFixed<N, M> &operator-=(FastFixed<N, M> &a, Fixed<N1, M1> b) {
    return a = a - b;
}

template <int N, int M, int N1, int M1>
FastFixed<N, M> &operator+=(FastFixed<N, M> &a, FastFixed<N1, M1> b) {
    return a = a + b;
}

template <int N, int M, int N1, int M1>
FastFixed<N, M> &operator-=(FastFixed<N, M> &a, FastFixed<N1, M1> b) {
    return a = a - b;
}

template <int N, int M>
FastFixed<N, M> &operator+=(FastFixed<N, M> &a, double b) {
    return a = a + b;
}

template <int N, int M>
FastFixed<N, M> &operator-=(FastFixed<N, M> &a, double b) {
    return a = a - b;
}

template <int N, int M>
FastFixed<N, M> &operator+=(FastFixed<N, M> &a, float b) {
    return a = a + b;
}

template <int N, int M>
FastFixed<N, M> &operator-=(FastFixed<N, M> &a, float b) {
    return a = a - b;
}

template <int N, int M, int N1, int M1>
Fixed<N, M> &operator+=(Fixed<N, M> &a, FastFixed<N1, M1> b) {
    return a = a + b;
}

template <int N, int M, int N1, int M1>
Fixed<N, M> &operator-=(Fixed<N, M> &a, FastFixed<N1, M1> b) {
    return a = a - b;
}

template <int N, int M>
FastFixed<N, M> &operator*=(FastFixed<N, M> &a, double b) {
    return a = a * b;
}


#endif // FAST_FIXED_H