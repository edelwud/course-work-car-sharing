#ifndef CARWIDGET_H
#define CARWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QMessageBox>

#include "EventManager.h"
#include "Database.h"
#include "User.h"
#include "Car.h"
#include "Order.h"

#include "careditform.h"


namespace Ui {
class CarWidget;
}

// Класс окна подробной информации об автомобиле
class CarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CarWidget(Database *db, Car *car, User *user, EventManager *em, User::Role rules = User::SIMPLEUSER, QWidget *parent = nullptr);
    void Render();  // Заполняем информацию об автомобиле
    ~CarWidget();

private slots:
    void on_closeButton_clicked();                          // Событие нажатия на кнопку закрыть
    void on_pushButton_clicked();                           // Событие нажатия на кнопку заказать
    void on_deleteCarButton_clicked();                      // Событие нажатия на кнопку удалить
    void on_editButton_clicked();                           // Событие нажатия на кнопку изменить
    void on_orderPassDate_dateChanged(const QDate &date);   // Событие изменения конечной даты заказа
    void on_orderDate_dateChanged(const QDate &date);       // Событие изменения начальной даты заказа

private:
    Ui::CarWidget *ui;
    Database *db;       // Работа с базой данных
    Car *car;           // Объект открытого автомобиля
    User *user;         // Авторизованный пользователь
    EventManager *em;   // Менеджер событий
    User::Role rules;   // Права пользователя
};

#endif // CARWIDGET_H
