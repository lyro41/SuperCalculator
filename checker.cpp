#include "checker.h"

void Checker::Check() {
  std::string func;
  while (id_ < input_.size()) {
    switch (now_) {
      case state::UNARY_EXPR:
        switch(input_[id_]) {
          case '-':
            output_.push_back('~');
            ++id_;
            break;

          case '+':
            ++id_;
            break;

          case ' ':
            ++id_;
            continue;

          case ']':
          case '}':
          case ')':
            throw std::string("error: symbol ") + std::to_string(id_ + 1);
        }
        now_ = state::EXPR;
        break;

      case state::EXPR:
        switch (input_[id_]) {
          case '[':
          case '{':
          case '(':
            output_.push_back(input_[id_]);
            ++id_;
            now_ = state::UNARY_EXPR;
            this->Check();
            if (output_.back() != ']' && output_.back() != '}' && output_.back() != ')') {
              throw std::string("error: symbol ") + std::to_string(id_);
            }
            now_ = state::OPERATOR_WAITING;
            break;

          case ' ':
            ++id_;
            break;

          case ']':
          case '}':
          case ')':
            if (GetPriority(std::string(1, output_.back())) != -1) {
              throw std::string("error: symbol ") + std::to_string(id_ + 1);
            }
            output_.push_back(input_[id_]);
            ++id_;
            return;

          default:
            now_ = state::OPERAND;
        }
        break;

      case state::OPERAND:
        if (std::isdigit(input_[id_])) {
          now_ = state::CONST;
          continue;
        }
        if (std::isalpha(input_[id_])) {
          if (id_ + 2 < input_.size()) {
            func = input_.substr(id_, 3);
            if (func == "log" || func == "sin" || func == "cos") {
              output_.push_back(input_[id_++]);
              output_.push_back(input_[id_++]);
              output_.push_back(input_[id_++]);
              now_ = state::FUNC;
              continue;
            }
          }
          output_.push_back(input_[id_]);
          ++id_;
          now_ = state::VAR;
          continue;
        }
        throw std::string("error: symbol ") + std::to_string(id_ + 1);

      case state::OPERATOR_WAITING:
        switch (input_[id_]) {
          case ' ':
            ++id_;
            break;

          case '-':
          case '+':
          case '*':
          case '/':
          case '%':
          case '^':
            output_.push_back(input_[id_]);
            ++id_;
            now_ = state::EXPR;
            break;

          case ']':
          case '}':
          case ')':
            output_.push_back(input_[id_]);
            ++id_;
            return;

          case ',':
            output_.push_back(input_[id_]);
            ++id_;
            return;

          default:
            throw std::string("error: symbol ") + std::to_string(id_ + 1);
        }
        break;

      case state::FUNC:
        while (id_ < input_.size()) {
          if (input_[id_] == ' ') {
            ++id_;
            continue;
          }
          if (input_[id_] == '(' || input_[id_] == '[' || input_[id_] == '{') {
            output_.push_back(input_[id_]);
            ++id_;
            now_ = state::UNARY_EXPR;
            this->Check();
            if (func == "log") {
              if (output_.back() != ',') throw std::string("error: symbol ") + std::to_string(id_);
              now_ = state::UNARY_EXPR;
              this->Check();
            }
            if (output_.back() != ']' && output_.back() != '}' && output_.back() != ')') {
              throw std::string("error: symbol ") + std::to_string(id_);
            }
            break;
          }
          throw std::string("error: symbol ") + std::to_string(id_ + 1);
        }
        now_ = state::OPERATOR_WAITING;
        break;

      case state::VAR:
        if (std::isalpha(input_[id_]) || std::isdigit(input_[id_])) {
          output_.push_back(input_[id_]);
          ++id_;
          continue;
        }
        now_ = state::OPERATOR_WAITING;
        break;

      case state::CONST:
        now_ = state::INT;
        this->Check();
        if (now_ == state::OPERATOR_WAITING) continue;
        if (output_.back() == '.') {
          now_ = state::INT;
          this->Check();
        }
        if (now_ == state::OPERATOR_WAITING) continue;
        if (output_.back() == 'e') {
          if (id_ != input_.size() && (input_[id_] == '+' || input_[id_] == '-')) {
            output_.push_back(input_[id_]);
            ++id_;
          }
          now_ = state::INT;
          this->Check();
          if (!std::isdigit(output_.back())) throw std::string("error: symbol ") + std::to_string(id_);
          if (now_ == state::OPERATOR_WAITING) continue;
        }
        now_ = state::OPERATOR_WAITING;
        break;

      case state::INT:
        while (id_ < input_.size()) {
          if (std::isdigit(input_[id_])) {
            output_.push_back(input_[id_]);
            ++id_;
            continue;
          }
          if (id_ == input_.size()) return;
          if (input_[id_] == '.' || input_[id_] == 'e') {
            output_.push_back(input_[id_]);
            ++id_;
            return;
          }
          now_ = state::OPERATOR_WAITING;
          return;
        }
        return;
    }
  }
  if (now_ == state::EXPR || now_ == state::UNARY_EXPR) {
    throw "error: wrong expression";
  }
  return;
}

std::string Checker::GetOutput() {
  return this->output_;
}

int Checker::GetPriority(std::string oper) {
  if (oper == "+" || oper == "-") {
    return 1;
  } else if (oper == "*" || oper == "/" || oper == "%") {
    return 2;
  } else if (oper == "~") {
    return 73;
  } else if (oper == "^") {
    return 89;
  } else if (oper == "log" || oper == "sin" || oper == "cos") {
    return 97;
  } else if (oper == "(" || oper == "{" || oper == "[") {
    return 0;
  } else return -1;
}
