#include "mainwindow.h"
#include "loginform.h"
#include "AuthController.h"
#include <QApplication>
#include <QDate>
#include <QDebug>

#include "User.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Database *db = new Database();

    LoginForm login(db);
    login.show();

    return a.exec();
}
