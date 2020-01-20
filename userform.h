#ifndef USERFORM_H
#define USERFORM_H

#include <QDate>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QStandardItem>
#include <QStandardItemModel>

#include "AuthController.h"
#include "Database.h"
#include "EventManager.h"
#include "User.h"
#include "Order.h"
#include "Car.h"

#include "careditform.h"
#include "carwidget.h"
#include "usereditform.h"
#include "addcarform.h"

namespace Ui {
class UserForm;
}

class UserForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserForm(Database *db, EventManager *em, User *user, QWidget *parent = nullptr);
    ~UserForm();

    void RenderCars();
    void RenderProfile();

    void CancelButtonRender();
    void AdminCarsRender();
    void AdminUsersRender();
    void AdminOrdersRender();
private slots:
    void on_ordersView_doubleClicked(const QModelIndex &index);

    void on_addCarButton_pressed();

    void on_changeProfileButton_pressed();

    void on_carsView_doubleClicked(const QModelIndex &index);

    void on_usersView_doubleClicked(const QModelIndex &index);

    void on_ordersView_2_doubleClicked(const QModelIndex &index);

    void on_changePasswordButton_pressed();

    void on_cancelButton_pressed();

private:
    Database *db;
    EventManager *em;
    User *user;
    Ui::UserForm *ui;
    size_t eventSubID;
};

#endif // USERFORM_H
