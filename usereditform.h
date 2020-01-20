#ifndef USEREDITFORM_H
#define USEREDITFORM_H

#include <QWidget>
#include <QMessageBox>
#include <QValidator>

#include "Database.h"
#include "EventManager.h"
#include "User.h"
#include "Order.h"

namespace Ui {
class UserEditForm;
}

class UserEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit UserEditForm(Database *db, User *user, EventManager *em, QWidget *parent = nullptr);
    ~UserEditForm();

    void DisableDeleteButton();

private slots:
    void on_saveButton_pressed();

    void on_deleteButton_pressed();

private:
    Ui::UserEditForm *ui;
    Database *db;
    User *user;
    EventManager *em;
};

#endif // USEREDITFORM_H
