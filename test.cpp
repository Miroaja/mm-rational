#include "rational.hpp" // assuming your class is in this file
#include <cassert>
#include <iostream>
#include <print>
#include <sstream>

void test_construction() {
  mm::rational<int> a(3, 4);
  assert(a.numerator == 3);
  assert(a.denominator == 4);

  mm::rational<int> b(6, 8);
  assert(b.numerator == 3);
  assert(b.denominator == 4);
}

void test_arithmetic() {
  mm::rational<int> a(1, 2);
  mm::rational<int> b(1, 3);
  auto c = a + b;
  assert(c.numerator == 5);
  assert(c.denominator == 6);

  c = a - b;
  assert(c.numerator == 1);
  assert(c.denominator == 6);

  c = a * b;
  assert(c.numerator == 1);
  assert(c.denominator == 6);

  c = a / b;
  assert(c.numerator == 3);
  assert(c.denominator == 2);
}

void test_float_conversion() {
  mm::rational<int> a(7.0f / 11.0f);

  float f = a;
  assert(std::abs(f - 0.63636362552642822265625f) == 0);
}

void test_output() {
  mm::rational<int> a(3, 4);
  std::stringstream ss;
  ss << a;
  assert(ss.str() == "3/4");

  std::string s = std::format("{}", a);
  assert(s == "3/4");
}

int main() {
  test_construction();
  test_arithmetic();
  test_float_conversion();
  test_output();

  std::cout << "All tests passed!\n";
  return 0;
}
