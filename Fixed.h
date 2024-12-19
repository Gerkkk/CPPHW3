#ifndef FIXED_H
#define FIXED_H

#include <iostream>
#include <cstdint>
#include <type_traits>
#include <limits>



namespace FixedDomain {
    template <size_t N>
    struct Store {
        using Type = void;
    };

    template <>
    struct Store<8> {
        using Type = int8_t;
    };

    template <>
    struct Store<16> {
        using Type = int16_t;
    };

    template <>
    struct Store<32> {
        using Type = int32_t;
    };

    template <>
    struct Store<64> {
        using Type = int64_t;
    };
}

template<int N, int K>
class Fixed {
public:
    // using StorageType = typename std::conditional<N != 64, typename std::conditional<N != 32, typename std::conditional<N != 16, typename std::conditional<N != 8, void, int8_t>::type, int16_t>::type, int32_t>::type, int64_t>::type;
    
    using StorageType = typename FixedDomain::Store<N>::Type;
    StorageType v;

    constexpr Fixed() : v(0) {}

    constexpr Fixed(int i) : v(static_cast<StorageType>(i) << K){}

    constexpr Fixed(float f) : v(static_cast<StorageType>(f * (1 << K))){}

    constexpr Fixed(double d) : v(static_cast<StorageType>(d * (1 << K))){}

    static constexpr Fixed from_raw(StorageType x) {
        Fixed ret;
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
    

    // Addition with another Fixed<N, K>
    template<int N2, int K2>
    Fixed<N, K> operator+(const Fixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return Fixed<N, K>::from_raw(v + other.v);
        } else if constexpr (K > K2) {
            return Fixed<N, K>::from_raw(v + (other.v << (K - K2)));
        } else {
            return Fixed<N, K>::from_raw((v << (K2 - K)) + other.v);
        }
    }

    // Multiplication with Fixed<N2, K2>
    template<int N2, int K2>
    Fixed<N, K> operator*(const Fixed<N2, K2>& other) const {
        // Use int64_t for intermediate calculation to avoid overflow
        int64_t result = static_cast<int64_t>(v) * other.v;
        if constexpr (K + K2 >= K) {
            result >>= K2;
        } else {
            result <<= (K - K2);
        }
        return Fixed<N, K>::from_raw(static_cast<StorageType>(result));
    }

    // Division with Fixed<N2, K2>
    template<int N2, int K2>
    Fixed<N, K> operator/(const Fixed<N2, K2>& other) const {
        // Use int64_t for intermediate calculation to avoid overflow
        int64_t result = (static_cast<int64_t>(v) << K2) / other.v;
        if constexpr (K2 > K) {
            result >>= (K2 - K);
        } else if constexpr (K > K2) {
            result <<= (K - K2);
        }
        return Fixed<N, K>::from_raw(static_cast<StorageType>(result));
    }

    // Subtraction with Fixed<N2, K2>
    template<int N2, int K2>
    Fixed<N, K> operator-(const Fixed<N2, K2>& other) const {
        if constexpr (K == K2) {
            return Fixed<N, K>::from_raw(v - other.v);
        } else if constexpr (K > K2) {
            return Fixed<N, K>::from_raw(v - (other.v << (K - K2)));
        } else {
            return Fixed<N, K>::from_raw((v << (K2 - K)) - other.v);
        }
    }


    // Addition with double
    Fixed<N, K> operator+(double d) const {
        return *this + Fixed<N, K>(d);
    }

    // Addition with float
    Fixed<N, K> operator+(float f) const {
        return *this + Fixed<N, K>(f);
    }

    // Addition with double
    Fixed<N, K> operator-(double d) const {
        return *this - Fixed<N, K>(d);
    }

    // Addition with float
    Fixed<N, K> operator-(float f) const {
        return *this -  Fixed<N, K>(f);
    }

    // Addition with double
    Fixed<N, K> operator*(double d) const {
        return *this * Fixed<N, K>(d);
    }

    // Addition with float
    Fixed<N, K> operator*(float f) const {
        return *this * Fixed<N, K>(f);
    }

    // Addition with double
    Fixed<N, K> operator/(double d) const {
        return *this / Fixed<N, K>(d);
    }

    // Addition with float
    Fixed<N, K> operator/(float f) const {
        return *this / Fixed<N, K>(f);
    }


    // Comparison operators
    bool operator<(const Fixed<N, K>& other) const { return v < other.v; }
    bool operator>(const Fixed<N, K>& other) const { return v > other.v; }
    bool operator<=(const Fixed<N, K>& other) const { return v <= other.v; }
    bool operator>=(const Fixed<N, K>& other) const { return v >= other.v; }
    bool operator==(const Fixed<N, K>& other) const { return v == other.v; }
    bool operator!=(const Fixed<N, K>& other) const { return v != other.v; }



    // Stream output
    friend std::ostream& operator<<(std::ostream& os, const Fixed<N, K>& f) {
        os << static_cast<double>(f);
        return os;
    }
};

// Non-member operator+ for double + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator+(double d, const Fixed<N, K>& f) {
    return Fixed<N, K>(d) + f;
}

// Non-member operator+ for float + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator+(float f, const Fixed<N, K>& fixed) {
    return Fixed<N, K>(f) + fixed;
}

// Non-member operator+ for double + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator-(double d, const Fixed<N, K>& f) {
    return Fixed<N, K>(d) - f;
}

// Non-member operator+ for float + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator-(float f, const Fixed<N, K>& fixed) {
    return Fixed<N, K>(f) - fixed;
}

// Non-member operator+ for double + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator*(double d, const Fixed<N, K>& f) {
    return Fixed<N, K>(d) * f;
}

// Non-member operator+ for float + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator*(float f, const Fixed<N, K>& fixed) {
    return Fixed<N, K>(f) * fixed;
}

// Non-member operator+ for double + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator/(double d, const Fixed<N, K>& f) {
    return Fixed<N, K>(d) / f;
}

// Non-member operator+ for float + Fixed<N, K>
template<int N, int K>
Fixed<N, K> operator/(float f, const Fixed<N, K>& fixed) {
    return Fixed<N, K>(f) / fixed;
}

template <int N, int M, int N1, int M1>
Fixed<N, M> &operator+=(Fixed<N, M> &a, Fixed<N1, M1> b) {
    return a = a + b;
}

template <int N, int M, int N1, int M1>
Fixed<N, M> &operator-=(Fixed<N, M> &a, Fixed<N1, M1> b) {
    return a = a - b;
}

template <int N, int M>
Fixed<N, M> &operator+=(Fixed<N, M> &a, double b) {
    return a = a + b;
}

template <int N, int M>
Fixed<N, M> &operator-=(Fixed<N, M> &a, double b) {
    return a = a - b;
}

template <int N, int M>
Fixed<N, M> &operator+=(Fixed<N, M> &a, float b) {
    return a = a + b;
}

template <int N, int M>
Fixed<N, M> &operator-=(Fixed<N, M> &a, float b) {
    return a = a - b;
}

template <int N, int M>
Fixed<N, M> &operator*=(Fixed<N, M> &a, double b) {
    return a = a * b;
}

#endif // FIXED_H