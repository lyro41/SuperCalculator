#ifndef CHECKER_H
#define CHECKER_H

#include <string>


class Checker {
 public:
  Checker(std::string input) : id_(0), input_(input), now_(state::UNARY_EXPR) {}
  ~Checker() = default;
  void Check();
  std::string GetOutput();
  int GetPriority(std::string operation);

 private:
  enum class state {
    UNARY_EXPR,
    EXPR,
    OPERATOR_WAITING,
    OPERAND,
    FUNC,
    VAR,
    CONST,
    INT
  };

  size_t id_;
  std::string input_;
  std::string output_;
  state now_;
};

#endif // CHECKER_H
