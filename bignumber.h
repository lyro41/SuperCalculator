#ifndef BIGNUMBER_H
#define BIGNUMBER_H

#include <cstdint>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <iostream>


const double kEps = 1e-9;


class BigNumber {
 public:
  BigNumber(uint64_t base = 1e9, std::string value = "0");
  BigNumber(double value);
  BigNumber(const BigNumber& other);
  ~BigNumber() = default;

  std::string ToString();
  double ToDouble();
  BigNumber abs();
  uint64_t size();
  void resize(uint64_t size, uint64_t value = 0);
  void push_back(uint64_t value);
  void pop_back();

  BigNumber pow(BigNumber base, BigNumber power);
  double pow(double base, BigNumber power);
  double pow(BigNumber base, double power);
  double pow(double base, double power);

  void Swap(BigNumber &other);

  uint64_t& operator [](uint64_t index);

  bool operator ==(BigNumber other);
  bool operator <(BigNumber other);
  bool operator >(BigNumber other);
  bool operator !=(BigNumber other);
  bool operator <=(BigNumber other);
  bool operator >=(BigNumber other);

  friend BigNumber operator -(BigNumber operand);
  friend BigNumber operator <<(BigNumber operand, uint64_t shift);
  friend BigNumber operator >>(BigNumber operand, uint64_t shift);
  friend BigNumber operator +(BigNumber first, BigNumber second);
  friend BigNumber operator -(BigNumber first, BigNumber second);
  friend BigNumber operator *(BigNumber first, BigNumber second);
  friend BigNumber operator /(BigNumber first, BigNumber second);
  friend BigNumber operator %(BigNumber first, BigNumber second);

  friend double operator /(BigNumber first, double second);
  friend double operator /(double first, BigNumber second);
  friend double operator +(BigNumber first, double second);
  friend double operator +(double first, BigNumber second);
  friend double operator -(BigNumber first, double second);
  friend double operator -(double first, BigNumber second);
  friend double operator *(BigNumber first, double second);
  friend double operator *(double first, BigNumber second);

  friend double operator %(BigNumber, double);
  friend double operator %(double, BigNumber);

  BigNumber& operator =(BigNumber other);

  friend std::ostream& operator <<(std::ostream &outstream, BigNumber &data);

  operator double();

 private:
  enum class sign_flag { POSITIVE = 1, NEGATIVE = -1 };
  const size_t kMaxLength = 309;

  uint64_t base_;
  std::vector<uint64_t> value_;
  sign_flag sign_;
};


#endif // BIGNUMBER_H
