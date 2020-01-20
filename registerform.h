#ifndef REGISTERFORM_H
#define REGISTERFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QValidator>

#include "Database.h"
#include "Controller.h"
#include "AuthController.h"

#include "loginform.h"
#include "mainwindow.h"

namespace Ui {
class RegisterForm;
}

class RegisterForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterForm(Database *db, QWidget *parent = nullptr);
    ~RegisterForm();

private slots:
    void on_loginButton_clicked();      // Событие нажатия на кнопку авторизироваться
    void on_registerButton_clicked();   // Событие нажатия на кнопку зарегистрироваться

private:
    Ui::RegisterForm *ui;
    Database *db;                // Работа с базой данных
    AuthController *auth;        // Работа с аунтификацией
    QMainWindow *w;              // Главное окно
    QRegExpValidator *validator; // Валидация полей
};

#endif // REGISTERFORM_H
