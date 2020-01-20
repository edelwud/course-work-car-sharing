#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QValidator>

#include "User.h"
#include "Database.h"
#include "EventManager.h"
#include "AuthController.h"

#include "registerform.h"
#include "mainwindow.h"

namespace Ui {
class LoginForm;
}

// Класс окна авторизации
class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(Database *db, QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void on_loginButton_clicked();      // Событие нажатия на кнопку авторизации
    void on_registerButton_clicked();   // Событие нажатия на кнопку зарегистрироваться

private:
    Ui::LoginForm *ui;
    Database *db;                // Работа с базой данных
    AuthController *auth;        // Работа с аунтификацией
    QMainWindow *w;              // Главное окно
    QRegExpValidator *validator; // Валидация полей
};

#endif // LOGINFORM_H
