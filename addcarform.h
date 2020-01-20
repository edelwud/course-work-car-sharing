#ifndef ADDCARFORM_H
#define ADDCARFORM_H

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
class AddCarForm;
}

// Класс окна добавление автомобиля
class AddCarForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddCarForm(Database *db, EventManager *em, QWidget *parent = nullptr);
    ~AddCarForm();

private slots:
    void on_fileInput_textChanged(const QString &arg1); // Событие изменение адреса фотографии
    void on_fileOpenButton_clicked(bool checked);       // Событие открытия окна выбора изображения
    void on_saveButton_clicked();                       // Событие нажатия на кнопку сохранения информации
    void on_pushButton_2_clicked(bool checked);         // Событие нажатия на кнопку закрыть

private:
    Ui::AddCarForm *ui;
    Database *db;     // Работа с файлом
    EventManager *em; // Менеджер событий
};

#endif // ADDCARFORM_H
