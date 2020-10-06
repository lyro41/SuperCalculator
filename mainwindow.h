#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "bignumber.h"
#include "checker.h"

#include <QMainWindow>
#include <string>
#include <stack>
#include <map>


QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  bool IsRightAssociative(std::string oper);
  int GetPriority(std::string oper);
  std::stack<std::string> StringToRPN(std::string input);
  void ResetTable(bool Locked);
  void PushVariable(std::string var, std::string value = "0");
  void BracketsCheck(std::string input);
  std::string CalculateRPN(std::stack<std::string> RPN);
  bool IsReal(std::string input);
  void Init(bool Reset);
  void CheckExpression(std::string input);
  void CheckRecursion(std::map<std::string, bool>, std::string expression);
  std::string VarConversion(std::string input);
  void ProcessVars(std::string input);

private slots:
  void on_input_field_textChanged();

  void on_var_table_cellChanged(int row, int column);

private:
  Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
