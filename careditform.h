#ifndef CAREDITFORM_H
#define CAREDITFORM_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>

#include "Database.h"
#include "EventManager.h"
#include "Car.h"
#include "User.h"
#include "Order.h"

namespace Ui {
class CarEditForm;
}

// Класс окна редактирования автомобиля
class CarEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CarEditForm(Database *db, Car *car, User *user, Order *order, EventManager *em, QWidget *parent = nullptr);
    ~CarEditForm();

    void Render(); // Заполнение окна информацией

private slots:
    void on_pushButton_2_clicked();                     // Событие нажатия на кнопку закрыть
    void on_fileOpenButton_clicked();                   // Событие нажатия на кнопку открыть фото
    void on_fileInput_textChanged(const QString &arg1); // Событие изменения адреса изображения
    void on_saveButton_clicked();                       // Событие нажатия на кнопку сохранить
    void on_deleteButton_pressed();                     // Событие нажатия на кнопку удалить

private:
    Ui::CarEditForm *ui;
    Database *db;     // Работа с базой данных
    Car *car;         // Текущий автомобиль
    User *user;       // Авторизованный пользователь
    EventManager *em; // Менеджер событий
};

#endif // CAREDITFORM_H
