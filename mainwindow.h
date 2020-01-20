#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QDate>

#include "Database.h"
#include "EventManager.h"
#include "User.h"
#include "Car.h"

#include "carwidget.h"
#include "userform.h"
#include "loginform.h"


namespace Ui {
class MainWindow;
}

// Главное окно
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database *db, User *user, QWidget *parent = nullptr);
    ~MainWindow();

    void UpdateEventBox();              // Обновление окошка событий
    void UpdateFilter();                // Обновление окошка фильтров
    void RenderCars();                  // Заполнение списка автомобилей

private slots:
    void on_CarView_doubleClicked(const QModelIndex &index); // Событие двойного нажатие на автомобиль
    void on_logoutButton_clicked();                          // Событие нажатия на кнопку выхода
    void on_cancelEventButton_clicked();                     // Событие нажатия на кнопку отмены действия
    void on_markBox_currentTextChanged(const QString &arg1); // Событие смены марки автомобиля
    void on_filterButton_pressed();                          // Событие нажатия на кнопку фильтрации
    void on_profileButton_pressed();                         // Событие нажатия на кнопку профиля

private:
    Ui::MainWindow *ui;
    EventManager *em;       // Менеджер событий
    Database *db;           // Работа с базой данных
    User *user;             // Авторизованый пользователь
};

#endif // MAINWINDOW_H
