#include "bignumber.h"


BigNumber::BigNumber(uint64_t base, std::string value) : base_(base) {
  if (value[0] == '-') {
    value = value.substr(1, value.length() - 1);
    sign_ = sign_flag::NEGATIVE;
  } else sign_ = sign_flag::POSITIVE;

  while (value.length() > static_cast<uint64_t>(log10(base))) {
    value_.push_back(std::stoull(value.substr(value.length() - static_cast<uint64_t>(log10(base)),
                                              static_cast<uint64_t>(log10(base)))));
    value = value.substr(0, value.length() - static_cast<uint64_t>(log10(base)));
  }
  value_.push_back(std::stoull(value));

  return;
}

BigNumber::BigNumber(double value) : base_(1e9), sign_(sign_flag::POSITIVE) {
  value = floor(value);
  if (value < kEps && value > -kEps) {
    value_.push_back(0);
    return;
  }
  if (value < 0) {
    value *= -1;
    sign_ = sign_flag::NEGATIVE;
  }
  while (value > kEps) {
    value_.push_back(static_cast<uint64_t>(value - floor(value / base_) * base_));
    value = floor(value / base_);
  }
  return;
}

BigNumber::BigNumber(const BigNumber& other) : base_(other.base_), value_(other.value_),
                                               sign_(other.sign_) { return; }

std::string BigNumber::ToString() {
  std::string output = "";
  for (uint64_t i = 0; i < value_.size(); ++i) {
    output = std::to_string(value_[i]) + output;
    if (i != value_.size() - 1) {
      for (uint64_t j = std::to_string(this->value_[i]).size();
           j < static_cast<uint64_t>(log10(base_)); ++j) {
        output = "0" + output;
      }
    }
  }
  if (sign_ == sign_flag::NEGATIVE) output = "-" + output;
  return output;
}

double BigNumber::ToDouble() {
  double result = 0.0;
  for (uint64_t i = this->size() - 1; i > 0; --i) {
    result += (*this)[i];
    result *= this->base_;
  }
  result += (*this)[0];
  if (this->sign_ == BigNumber::sign_flag::NEGATIVE) result *= -1.0;
  return result;
}

BigNumber BigNumber::abs() {
  BigNumber result = BigNumber(*this);
  result.sign_ = sign_flag::POSITIVE;
  return result;
}

uint64_t BigNumber::size() {
  return this->value_.size();
}

void BigNumber::resize(uint64_t size, uint64_t value) {
  this->value_.resize(size, value);
  return;
}

void BigNumber::push_back(uint64_t value) {
  this->value_.push_back(value);
  return;
}

void BigNumber::pop_back() {
  this->value_.pop_back();
  return;
}

BigNumber BigNumber::pow(BigNumber base, BigNumber power) {
  BigNumber result(1e9, "1");
  BigNumber zero(1e9, "0");
  BigNumber one(1e9, "1");
  BigNumber two(1e9, "2");

  if (power == zero) {
    if (base == zero) throw "error: exponentiation result is NAN";
    return one;
  }

  if (power < zero) {
    if (base == zero) throw "error: division by zero";
    return zero;
  }

  while (power > zero) {
    if (power % two == one) {
      result = result * base;
    }
    base = base * base;
    power = power / two;
  }
  return result;
}

double BigNumber::pow(BigNumber base, double power) {
  BigNumber zero(1e9, "0");
  if (base < zero && !(power - floor(power) < kEps && power - floor(power) > -kEps)) {
    throw "error: wrong exponentiation definition";
  }
  if (base == zero && power < kEps) throw "error: exponentiation result is NAN";
  if (base < zero) return pow(base, static_cast<BigNumber>(power)).ToDouble();
  else return std::pow(base.ToDouble(), power);
}

double BigNumber::pow(double base, BigNumber power) {
  BigNumber zero(1e9, "0");
  BigNumber one(1e9, "1");
  BigNumber two(1e9, "2");

  if (power == zero) {
    if (base > -kEps && base < kEps) throw "error: exponentiation result is NAN";
    return one;
  }

  if (power < zero) {
    if (base < kEps && base > -kEps) throw "error: division by zero";
    base = 1.0 / base;
    power = -power;
  }

  double result = 1.0;
  while (power > zero) {
    if (power % two == one) result *= base;
    base *= base;
    power = power / two;
  }

  return result;
}

double BigNumber::pow(double base, double power) {
  if ((power - floor(power) < kEps && power - floor(power) > -kEps)) {
    return pow(base, BigNumber(power));
  }

  if (base < -kEps) throw "error: wrong exponentiation definition";

  if (base > -kEps && base < kEps && power < kEps) throw "error: exponentiation result is NAN";

  return std::pow(base, power);
}

void BigNumber::Swap(BigNumber &other) {
  std::swap(this->base_, other.base_);
  std::swap(this->sign_, other.sign_);
  std::swap(this->value_, other.value_);
  return;
}

uint64_t& BigNumber::operator[](uint64_t index) {
  return this->value_[index];
}

bool BigNumber::operator ==(BigNumber other) {
  if (this->size() == other.size() && this->sign_ == other.sign_) {
    for (uint64_t i = 0; i < this->size(); ++i) {
      if ((*this)[i] != other[i]) return false;
    }
    return true;
  }
  return false;
}

bool BigNumber::operator <(BigNumber other) {
  if (this->sign_ != other.sign_) {
    if (this->sign_ == BigNumber::sign_flag::NEGATIVE) return true;
    return false;
  }

  if (this->sign_ == BigNumber::sign_flag::NEGATIVE && this->sign_ == other.sign_) {
    return -other < -(*this);
  }

  if (this->size() < other.size()) return true;
  if (this->size() > other.size()) return false;

  for (uint64_t i = this->size() - 1; i > 0; --i) {
    if ((*this)[i] > other[i]) return false;
    else if ((*this)[i] < other[i]) return true;
  }
  if ((*this)[0] < other[0]) return true;

  return false;
}

bool BigNumber::operator >(BigNumber other) {
  return !(*this < other || *this == other);
}

bool BigNumber::operator !=(BigNumber other) {
  return !(*this == other);
}

bool BigNumber::operator <=(BigNumber other) {
  return !(*this > other);
}

bool BigNumber::operator >=(BigNumber other) {
  return !(*this < other);
}

BigNumber operator -(BigNumber operand) {
  operand.sign_ = static_cast<BigNumber::sign_flag>(static_cast<int>(operand.sign_) * (-1));
  return operand;
}

BigNumber operator <<(BigNumber operand, uint64_t shift) {
  operand.resize(operand.size() + shift);
  for (uint64_t i = operand.size() - 1; i >= shift; --i) {
    operand[i] = operand[i - shift];
  }
  for (uint64_t i = 0; i < shift; ++i) {
    operand[i] = 0;
  }
  return operand;
}

BigNumber operator >>(BigNumber operand, uint64_t shift) {
  for (uint64_t i = 0; i < operand.size() - shift; ++i) {
    operand[i] = operand[i + shift];
  }
  operand.resize(operand.size() - shift);
  return operand;
}

BigNumber operator +(BigNumber first, BigNumber second) {
  BigNumber result(first.base_);

  if (first.abs() < second.abs()) first.Swap(second);
  result.sign_= first.sign_;

  if (first.sign_ == second.sign_) {
    for (uint64_t i = 0; i < second.size(); ++i) {
      uint64_t term = first[i] + second[i];
      result.push_back(term / result.base_);
      result[i] += term % result.base_;
    }
    for (uint64_t i = second.size(); i < first.size(); ++i) {
      result[i] += first[i];
      result.push_back(result[i] / result.base_);
      result[i] = result[i] % result.base_;
    }
  } else {
    result.pop_back();
    for (uint64_t i = 0; i < second.size(); ++i) {
      if (first[i] < second[i]) {
        uint64_t j = i + 1;
        for (; first[j] == 0; ++j) {}
        for (; j > i; --j) {
          first[j] -= 1;
          first[j - 1] += result.base_;
        }
      }
      result.push_back(first[i] - second[i]);
    }
    for (uint64_t i = second.size(); i < first.size(); ++i) {
      result.push_back(first[i]);
    }
  }

  for (uint64_t i = result.size() - 1; i > 0 && result[i] == 0; --i) {
    result.pop_back();
  }

  if (result[result.size() - 1] == 0) result.sign_ = BigNumber::sign_flag::POSITIVE;

  if (result.size() * std::log10(result.base_) >= result.kMaxLength) {
    throw "thrown exception: number is too big(length >= " +
          std::to_string(first.kMaxLength - static_cast<size_t>(std::log10(result.base_))) +
                         " numbers)";
  }

  return result;
}

BigNumber operator -(BigNumber first, BigNumber second) {
  return first + (-second);
}

BigNumber operator *(BigNumber first, BigNumber second) {
  if (first.size() == 1 || second.size() == 1) {
    BigNumber zero(1e9, "0");
    if (first.abs() == zero || second.abs() == zero) return zero;
    BigNumber result(first.base_);
    result.sign_ = static_cast<BigNumber::sign_flag>(static_cast<int>(first.sign_) *
                                                     static_cast<int>(second.sign_));
    result.resize(3, 0);
    if (first.size() < second.size()) first.Swap(second);
    for (uint64_t i = 0; i < first.size(); ++i) {
      uint64_t term = first[i] * second[0];
      result[i] += term % result.base_;
      result[i + 1] += term / result.base_;
      if (result[i] >= result.base_) {
        result[i + 1] += result[i] / result.base_;
        result[i] %= result.base_;
      }
    }

    for (uint64_t i = result.size() - 1; i > 0 && result[i] == 0; --i) {
      result.pop_back();
    }
    if (result.size() == 1 && result[0] == 0) result.sign_ = BigNumber::sign_flag::POSITIVE;

    return result;
  }
  for (uint64_t i = first.size(); i < second.size(); ++i) first.push_back(0);
  for (uint64_t i = second.size(); i < first.size(); ++i) second.push_back(0);

  uint64_t power = first.size() / 2;

  BigNumber first_first = first;
  first_first.resize(power);
  BigNumber first_second = first >> power;

  BigNumber second_first = second;
  second_first.resize(power);
  BigNumber second_second = second >> power;

  BigNumber ff_sf = first_first * second_first;
  BigNumber fs_ss = first_second * second_second;

  BigNumber result = ff_sf + (((first_first + first_second) *
                             (second_first + second_second) - ff_sf - fs_ss) << power) +
                     (fs_ss << (power + power));

  for (uint64_t i = result.size() - 1; i > 0 && result[i] == 0; --i) {
    result.pop_back();
  }
  if (result.size() == 1 && result[0] == 0) result.sign_ = BigNumber::sign_flag::POSITIVE;

  return result;
}

BigNumber operator /(BigNumber first, BigNumber second) {
  if (second == BigNumber(1e9, "0")) throw "error: division by zero";
  if (first < second) return BigNumber(1e9, "0");
  if (second.size() > 1) throw "error: division by big number";

  BigNumber result(first);
  uint64_t carry = 0;
  for (int64_t i = static_cast<int64_t>(first.size()) - 1; i >= 0; --i) {
    uint64_t term = first[static_cast<uint64_t>(i)] + carry * result.base_;
    result[static_cast<uint64_t>(i)] = term / second[0];
    carry = term % second[0];
  }
  while (result.size() > 1 && result[result.size() - 1] == 0) result.pop_back();

  if (first.sign_ != second.sign_) result.sign_ = BigNumber::sign_flag::NEGATIVE;

  return result;
}

double operator /(BigNumber first, double second) {
  if (second >= -kEps && second <= kEps) throw "error: division by zero";
  return first.ToDouble() / second;
}

double operator /(double first, BigNumber second) {
  if (second == BigNumber(1e9, "0")) throw "error: division by zero";
  return first / second.ToDouble();
}

BigNumber operator %(BigNumber first, BigNumber second) {
  if (second == BigNumber(1e9, "0")) throw "error: division by zero";
  if (first < second) return first;
  if (second.size() > 1) throw "error: division by big number";

  BigNumber carry = BigNumber(first.base_, "0");
  for (int64_t i = static_cast<int64_t>(first.size()) - 1; i >= 0; --i) {
    uint64_t term = first[static_cast<uint64_t>(i)] + carry[0] * first.base_;
    carry[0] = term % second[0];
  }

  if (first.sign_ != second.sign_ && carry != BigNumber(1e9, "0")) {
    carry = BigNumber(1e9, "1000000000") - carry;
  }

  return carry;
}

double operator %(BigNumber, double) {
  throw "error: modular division by double";
}

double operator %(double, BigNumber) {
  throw "error: modular division of double";
}

std::ostream& operator <<(std::ostream &outstream, BigNumber &data) {
  outstream << data.ToString();
  return outstream;
}

double operator +(BigNumber first, double second) {
  return first.ToDouble() + second;
}

double operator +(double first, BigNumber second) {
  return first + second.ToDouble();
}

double operator -(BigNumber first, double second) {
  return first.ToDouble() - second;
}

double operator -(double first, BigNumber second) {
  return first - second.ToDouble();
}

double operator *(BigNumber first, double second) {
  return first.ToDouble() * second;
}

double operator *(double first, BigNumber second) {
  return first * second.ToDouble();
}

BigNumber::operator double() {
  return this->ToDouble();
}

BigNumber& BigNumber::operator =(BigNumber other) {
  this->Swap(other);
  return *this;
}
