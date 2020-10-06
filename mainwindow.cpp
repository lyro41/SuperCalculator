#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->var_table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->var_table->horizontalHeader()->setSectionResizeMode (QHeaderView::Fixed);
  ui->var_table->setSortingEnabled(false);
  this->PushVariable("e", "2.718281828");
  this->PushVariable("pi", "3.141592653");
  ui->error_label->setStyleSheet("qproperty-alignment: AlignRight;");
}

MainWindow::~MainWindow() {
  delete ui;
}

int MainWindow::GetPriority(std::string oper) {
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

bool MainWindow::IsRightAssociative(std::string oper) {
  if (oper == "^") return true;
  else return false;
}

std::stack<std::string> MainWindow::StringToRPN(std::string input) {
  std::stack<std::string> output;
  std::stack<std::string> operations;
  for (size_t i = 0; i < input.size(); ++i) {

    if (input[i] == ' ') continue;

    std::string oper = std::string("", 0) + input[i];

    if (std::isdigit(input[i])) {
      for (; i + 1 < input.size() && (std::isdigit(input[i + 1]) ||
             input[i + 1] == 'e' || input[i + 1] == '.'); ++i) {
        oper += input[i + 1];
      }
      output.push(oper);
      continue;
    }

    switch(input[i]) {
      case '(':
      case '{':
      case '[':
        operations.push(oper);
        continue;

      case ')':
        while (operations.top() != "(") {
          output.push(operations.top());
          operations.pop();
        }
        operations.pop();
        if (!operations.empty() && GetPriority(operations.top()) == GetPriority("log")) {
          output.push(operations.top());
          operations.pop();
        }
        continue;

      case '}':
        while (operations.top() != "{") {
          output.push(operations.top());
          operations.pop();
        }
        operations.pop();
        if (!operations.empty() && GetPriority(operations.top()) == GetPriority("log")) {
          output.push(operations.top());
          operations.pop();
        }
        continue;

      case ']':
        while (operations.top() != "[") {
          output.push(operations.top());
          operations.pop();
        }
        operations.pop();
        if (!operations.empty() && GetPriority(operations.top()) == GetPriority("log")) {
          output.push(operations.top());
          operations.pop();
        }
        continue;

      case ',':
        while (operations.top() != "(" && operations.top() != "[" && operations.top() != "{") {
          output.push(operations.top());
          operations.pop();
        }
        continue;
    }

    if (std::isalpha(input[i])) {
      for (; i + 1 < input.size() && (std::isalpha(input[i + 1]) ||
                                      std::isdigit(input[i + 1])); ++i) {
        oper += input[i + 1];
      }

      if (GetPriority(oper) != GetPriority("log")) {
        this->PushVariable(oper);
        output.push(oper);
        continue;
      }

      if (i + 1 == input.size() || input[i + 1] != '(') {
        throw "error: used reserved name \"" + oper + "\" as variable";
      }
    }

    int now_priority = MainWindow::GetPriority(oper);
    if (now_priority == -1) throw "error: unknown operator " + oper;
    if (IsRightAssociative(oper)) {
      while (!operations.empty() && now_priority < GetPriority(operations.top())) {
        output.push(operations.top());
        operations.pop();
      }
    } else {
      while (!operations.empty() && now_priority <= GetPriority(operations.top())) {
        output.push(operations.top());
        operations.pop();
      }
    }
    operations.push(oper);

  }

  while (!operations.empty()) {
    output.push(operations.top());
    operations.pop();
  }

  while (!output.empty()) {
    operations.push(output.top());
    output.pop();
  }

  return operations;
}

void MainWindow::ResetTable(bool Locked) {
  std::string expression = ui->input_field->text().toStdString();
  for (int i = 0; i < ui->var_table->rowCount(); ++i) {
    std::string target = ui->var_table->item(i, 0)->text().toStdString();
    if (target == "e" || target == "pi") continue;
    size_t found = expression.find(target);
    if (found == std::string::npos || (found == 0 ? false : std::isalpha(expression[found])) ||
        (found + target.size() == expression.size() ? false :
         (std::isalpha(expression[found + target.size()]) ||
          std::isdigit(expression[found + target.size()])))) {
      if (Locked) {
        if (ui->var_table->item(i, 1)->text().toStdString() == "0") {
          ui->var_table->removeRow(i);
        }
        continue;
      }

      if (ui->var_table->findItems(QString::fromStdString(target),
                                   Qt::MatchContains |
                                   Qt::MatchCaseSensitive).size() == 1) {
        ui->var_table->removeRow(i);
        i = 0;
      }
    }
  }
}

void MainWindow::PushVariable(std::string var, std::string value) {
  if (ui->var_table->rowCount() != 0) {
    QTableWidgetItem *found = nullptr;
    for (int i = 0; i < ui->var_table->rowCount(); ++i) {
      if (ui->var_table->item(i, 0)->text().toStdString() == var) {
        found = ui->var_table->item(i, 1);
        break;
      }
    }

    if (found != nullptr) return;
  }
  QFont font("Comic Sans MS", 13);
  ui->var_table->setRowCount(ui->var_table->rowCount() + 1);

  QTableWidgetItem *item_var = new QTableWidgetItem(QString::fromStdString(var));
  QTableWidgetItem *item_val = new QTableWidgetItem(QString::fromStdString(value));

  item_var->setFont(font);
  item_val->setFont(font);
  item_var->setFlags(item_var->flags() ^ Qt::ItemIsEditable);

  ui->var_table->setItem(ui->var_table->rowCount() - 1, 0, item_var);
  ui->var_table->setItem(ui->var_table->rowCount() - 1, 1, item_val);
  ui->var_table->resizeColumnToContents(0);
  ui->var_table->resizeColumnToContents(1);

}

void MainWindow::BracketsCheck(std::string input) {
  std::stack<char> brackets;
  for (size_t i = 0; i < input.size(); ++i) {
    switch (input[i]) {
      case '(':
      case '{':
      case '[':
        brackets.push(input[i]);
        break;

      case ')':
        if (brackets.empty() || brackets.top() != '(') {
          throw std::string("error: wrong bracket sequence");
        }
        brackets.pop();
        break;

      case '}':
        if (brackets.empty() || brackets.top() != '{') {
          throw std::string("error: wrong bracket sequence");
        }
        brackets.pop();
        break;

      case ']':
        if (brackets.empty() || brackets.top() != '[') {
          throw std::string("error: wrong bracket sequence");
        }
        brackets.pop();
        break;
    }
  }

  if (!brackets.empty()) {
    throw std::string("error: wrong bracket sequence");
  }

  return;
}

std::string MainWindow::CalculateRPN(std::stack<std::string> RPN) {
  std::stack<std::string> stack;
  while (!RPN.empty()) {
    std::string now = RPN.top();
    RPN.pop();
    if (GetPriority(now) == -1) {
      if (std::isalpha(now[0])) {
        for (int i = 0; i < ui->var_table->rowCount(); ++i) {
          std::string expression = ui->var_table->item(i, 0)->text().toStdString();
          if (expression == now) {
            std::stack<std::string> now_stack;
          }
        }
      }
      stack.push(now);
      continue;
    }

    if (now == "-") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first) || this->IsReal(second)) {
        stack.push(std::to_string(std::atof(first.c_str()) - std::atof(second.c_str())));
        continue;
      }

      stack.push((BigNumber(1e9, first) - BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "+") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first) || this->IsReal(second)) {
        stack.push(std::to_string(std::atof(first.c_str()) + std::atof(second.c_str())));
        continue;
      }

      stack.push((BigNumber(1e9, first) + BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "*") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first) || this->IsReal(second)) {
        stack.push(std::to_string(std::atof(first.c_str()) * std::atof(second.c_str())));
        continue;
      }

      stack.push((BigNumber(1e9, first) * BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "/") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first) || this->IsReal(second)) {
        if (std::atof(second.c_str()) < kEps && std::atof(second.c_str()) > -kEps) {
          throw "error: division by zero";
        }
        stack.push(std::to_string(std::atof(first.c_str()) / std::atof(second.c_str())));
        continue;
      }

      stack.push((BigNumber(1e9, first) / BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "%") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first) || this->IsReal(second)) {
        throw "error: modular division by double";
      }

      stack.push((BigNumber(1e9, first) % BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "~") {
      std::string first = stack.top();
      stack.pop();

      if (this->IsReal(first)) {
        stack.push(std::to_string(-std::atof(first.c_str())));
        continue;
      }

      stack.push((-BigNumber(1e9, first)).ToString());
      continue;
    }

    if (now == "^") {
      std::string second = stack.top();
      stack.pop();
      std::string first = stack.top();
      stack.pop();

      BigNumber dummy;

      if (this->IsReal(first) || this->IsReal(second)) {
        stack.push(std::to_string(dummy.pow(std::atof(first.c_str()), std::atof(second.c_str()))));
        continue;
      }

      stack.push(dummy.pow(BigNumber(1e9, first), BigNumber(1e9, second)).ToString());
      continue;
    }

    if (now == "log") {
      double second = std::atof(stack.top().c_str());
      stack.pop();
      double first = std::atof(stack.top().c_str());
      stack.pop();

      if (first <= kEps || second <= kEps ||
          (first - kEps <= 1 && first + kEps >= 1)) {
        throw "error: wrong logarithm definition";
      }

      if (second - kEps <= 1 && second + kEps >= 1) {
        stack.push(std::to_string(0));
      } else {
        stack.push(std::to_string(std::log(second) / std::log(first)));
      }
      continue;
    }

    if (now == "sin") {
      double first = std::atof(stack.top().c_str());
      stack.pop();

      stack.push(std::to_string(std::sin(first)));
      continue;
    }

    if (now == "cos") {
      double first = std::atof(stack.top().c_str());
      stack.pop();

      stack.push(std::to_string(std::cos(first)));
      continue;
    }
  }

  if (stack.empty()) throw "";

  return stack.top();
}

bool MainWindow::IsReal(std::string input) {
  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == 'e' || input[i] == '.') return 1;
  }
  return 0;
}

void MainWindow::Init(bool Reset) {
  try {

    if (Reset) this->ResetTable(ui->lock_check->isChecked());
    ui->error_label->setText(QString(""));
    ui->RPN_field->setText(QString(""));
    ui->result_field->setText(QString(""));

    Checker DFA(ui->input_field->text().toStdString());

    DFA.Check();

    this->BracketsCheck(DFA.GetOutput());

    this->ProcessVars(DFA.GetOutput());

    for (int i = 0; i < ui->var_table->rowCount(); ++i) {
      this->ProcessVars(ui->var_table->item(i, 1)->text().toStdString());
    }

    std::map<std::string, bool> names;

    for (int i = 0; i < ui->var_table->rowCount(); ++i) {
      names.clear();
      this->CheckExpression(ui->var_table->item(i, 1)->text().toStdString());
      this->BracketsCheck(ui->var_table->item(i, 1)->text().toStdString());
      names.insert({ ui->var_table->item(i, 0)->text().toStdString(), 1 });
      this->CheckRecursion(names, ui->var_table->item(i, 1)->text().toStdString());
    }

    std::string converted = this->VarConversion(DFA.GetOutput());

    std::stack<std::string> RPN = this->StringToRPN(converted);

    std::string result = this->CalculateRPN(RPN);
    ui->result_field->setText(QString::fromStdString(result));

    std::string RPN_string = "";

    while (!RPN.empty()) {
      RPN_string += RPN.top() + ' ';
      RPN.pop();
    }

    ui->RPN_field->setText(QString::fromStdString(RPN_string));

  } catch (std::string exception) {
    ui->error_label->setText(QString::fromStdString(exception));
  } catch (const char* exception) {
    ui->error_label->setText(QString(exception));
  } catch (...) {
    ui->error_label->setText(QString("error: unknown exception"));
  }

  return;
}

void MainWindow::on_input_field_textChanged() {
  this->Init(true);
  return;
}

void MainWindow::on_var_table_cellChanged(int, int column) {
  ui->var_table->resizeColumnToContents(0);
  ui->var_table->resizeColumnToContents(1);
  if (column != 0) this->Init(false);
  return;
}

void MainWindow::CheckExpression(std::string input) {
  Checker checker(input);
  checker.Check();
  return;
}

void MainWindow::CheckRecursion(std::map<std::string, bool> names, std::string expression) {
  for (size_t i = 0; i < expression.size(); ++i) {
    if (std::isalpha(expression[i])) {
      std::string oper(1, expression[i]);
      for (; i + 1 < expression.size() && (std::isalpha(expression[i + 1]) ||
                                           std::isdigit(expression[i + 1])); ++i) {
        oper += expression[i + 1];
      }

      if (GetPriority(oper) != GetPriority("log")) {
        QTableWidgetItem* found = ui->var_table->findItems(QString::fromStdString(oper),
                                                           Qt::MatchFixedString |
                                                           Qt::MatchCaseSensitive).first();
        if (names.find(found->text().toStdString()) != names.end()) {
          throw std::string("error: self definition of variable ") +
                found->text().toStdString();
        }
        names.insert({ found->text().toStdString(), 1 });
        CheckRecursion(names, ui->var_table->item(found->row(), 1)->text().toStdString());
        continue;
      }
    }
  }
}

std::string MainWindow::VarConversion(std::string input) {
  for (size_t i = 0; i < input.size(); ++i) {
    if (std::isalpha(input[i])) {
      std::string var(1, input[i++]);
      for (; i < input.size() && (std::isalpha(input[i]) || std::isdigit(input[i])); ++i) {
        var.push_back(input[i]);
      }
      if (GetPriority(var) != -1) continue;

      QTableWidgetItem* found = ui->var_table->findItems(QString::fromStdString(var),
                                                         Qt::MatchFixedString |
                                                         Qt::MatchCaseSensitive).first();
      std::string conversion = ui->var_table->item(found->row(), 1)->text().toStdString();
      i -= var.size();
      input = input.substr(0, i) + "(" + conversion + ")" + input.substr(i + var.size());
    }
  }
  return input;
}

void MainWindow::ProcessVars(std::string input) {
  std::string oper;
  for (size_t i = 0; i < input.size(); ++i) {
    if (std::isalpha(input[i])) {
      oper = input[i];
      for (; i + 1 < input.size() && (std::isalpha(input[i + 1]) ||
                                      std::isdigit(input[i + 1])); ++i) {
        oper += input[i + 1];
      }

      if (GetPriority(oper) == -1) {
        this->PushVariable(oper);
        continue;
      }

      if (i + 1 == input.size() || input[i + 1] != '(') {
        throw "error: used reserved name \"" + oper + "\" as variable";
      }
    }
  }
}
