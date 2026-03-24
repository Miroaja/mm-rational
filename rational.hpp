#pragma once
#include <algorithm>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <format>
#include <limits>
#include <sys/types.h>
#include <type_traits>
#include <utility>

namespace mm {

template <std::integral T> class rational;
template <std::integral T> inline T gcd(const rational<T> &f);

template <std::integral T> class rational {
public:
  struct no_reduce_t {};
  inline constexpr static no_reduce_t no_reduce;

  T numerator;
  T denominator;

  inline rational(T num, T den) noexcept : numerator(num), denominator(den) {
    reduce();
  }
  inline rational(T num, T den, no_reduce_t) noexcept
      : numerator(num), denominator(den) {}
  inline rational(T x = (T)0) noexcept : numerator(x), denominator((T)1) {}
  template <std::floating_point F> inline rational(const F &val) {
    if (val == F(0)) {
      numerator = 0;
      denominator = 1;
      return;
    }

    constexpr auto digits = std::numeric_limits<F>::digits;

    using U = uint64_t;
    using S = int64_t;

    U bits;
    std::memcpy(&bits, &val, sizeof(bits));

    constexpr auto mantissa_bits = digits - 1;
    constexpr auto total_bits = sizeof(F) * 8;
    constexpr auto exponent_bits = total_bits - digits;
    constexpr auto exponent_bias = std::numeric_limits<F>::max_exponent - 1;

    U sign = bits >> (sizeof(U) * 8 - U(1));
    S exponent = (bits >> mantissa_bits) & ((U(1) << exponent_bits) - U(1));
    U mantissa = bits & ((U(1) << mantissa_bits) - U(1));

    exponent -= exponent_bias;

    if (exponent == 0 && mantissa == 0) {
      numerator = 0;
      denominator = 1;
      return;
    }

    U full_mantissa = mantissa | (U(1) << mantissa_bits);
    numerator = full_mantissa;
    denominator = U(1) << mantissa_bits;

    if (sign) {
      numerator = -numerator;
    }

    denominator = U(1) << (mantissa_bits);

    if (exponent < 0) {
      denominator *= U(1) << (-exponent);
    } else if (exponent > 0) {
      numerator *= U(1) << exponent;
    }
  }

  inline rational &reduce() {
    T d = gcd(*this);
    numerator /= d;
    denominator /= d;

    if (denominator < 0) {
      numerator = -numerator;
      denominator = -denominator;
    }
    return *this;
  }

  inline bool is_reduced() const { return gcd(numerator, denominator) == 1; }

  inline rational operator+(const rational &other) const {
    if (denominator == other.denominator) {
      return rational{numerator + other.numerator, denominator, no_reduce};
    } else {
      return rational{(numerator * other.denominator) +
                          (other.numerator * denominator),
                      denominator * other.denominator, no_reduce};
    }
  }

  inline rational operator-(const rational &other) const {
    if (denominator == other.denominator) {
      return rational{numerator - other.numerator, denominator, no_reduce};
    } else {
      return rational{(numerator * other.denominator) -
                          (other.numerator * denominator),
                      denominator * other.denominator, no_reduce};
    }
  }

  inline rational operator*(const rational &other) const {
    return rational{numerator * other.numerator,
                    denominator * other.denominator, no_reduce};
  }

  inline rational operator/(const rational &other) const {
    return rational{numerator * other.denominator,
                    other.numerator * denominator, no_reduce};
  }
  inline rational &operator+=(const rational &other) {
    return *this = this->operator+(other);
  }
  inline rational &operator-=(const rational &other) {
    return *this = this->operator-(other);
  }
  inline rational &operator*=(const rational &other) {
    return *this = this->operator*(other);
  }
  inline rational &operator/=(const rational &other) {
    return *this = this->operator/(other);
  }

  inline rational operator-() const { return {-numerator, denominator}; }
  inline rational operator+() const { return *this; }

  inline rational &operator++() { return *this = {++numerator, denominator}; }
  inline rational operator++(int) { return {numerator++, denominator}; }
  inline rational &operator--() { return *this = {--numerator, denominator}; }
  inline rational operator--(int) { return {numerator--, denominator}; }

  inline auto operator<=>(const rational &other) const {
    return numerator * other.denominator <=> denominator * other.numerator;
  }
  inline bool operator==(const rational &other) const {
    return numerator * other.denominator == other.numerator * denominator;
  }

  inline operator float() const {
    return static_cast<float>(numerator) / static_cast<float>(denominator);
  }

  inline operator double() const {
    return static_cast<double>(numerator) / static_cast<double>(denominator);
  }

  inline operator auto() const { return numerator / denominator; }
};

template <std::integral T> inline T gcd(const rational<T> &f) {
  using unsigned_t = std::make_unsigned_t<T>;
  unsigned_t u = f.numerator;
  unsigned_t v = f.denominator;
  if (u == 0) {
    return 1;
  }
  if constexpr (std::is_signed_v<T>) {
    if (f.numerator < 0) {
      u = -f.numerator;
    }
    if (f.denominator < 0) {
      v = -f.denominator;
    }
  }

  auto i = std::countr_zero(u);
  u >>= i;
  auto j = std::countr_zero(v);
  v >>= j;

  auto k = std::min(i, j);
  while (true) {
    if (u > v) {
      std::swap(u, v);
    }

    v -= u;

    if (v == 0) {
      return u << k;
    }

    v >>= std::countr_zero(v);
  }
}

template <std::integral T>
std::ostream &operator<<(std::ostream &os, const rational<T> &f) {
  return os << f.numerator << '/' << f.denominator;
}

} // namespace mm

namespace std {
template <std::integral T> struct formatter<mm::rational<T>> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const mm::rational<T> &f, FormatContext &ctx) const {
    return format_to(ctx.out(), "{}/{}", f.numerator, f.denominator);
  }
};
} // namespace std
