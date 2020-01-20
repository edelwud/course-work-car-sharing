#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Database *db, User* user, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(db), user(user)
{
    this->ui->setupUi(this);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    // Показываем логин авторизованного пользователя
    this->ui->loginLabel->setText(user->getLogin().c_str());

    // Создаем объект менеджера событий
    this->em = new EventManager();

    // Подписываемся на все события
    this->em->Subscribe(Event::SUPER, [this](Event::Status){
        this->RenderCars();
        this->UpdateEventBox();
    });

    // Заполняем главное окно информацией
    this->RenderCars();
    this->UpdateEventBox();
}

MainWindow::~MainWindow()
{
    delete this->user;
    delete this->db;
    delete this->ui;
}

void MainWindow::UpdateEventBox() {
    // Проверяем наличие последних действий
    if (!this->em->getCount()) {
        this->ui->eventBox->hide();
        this->ui->filterLayout->setGeometry(540, 90, 260, 130);
        return;
    }

    // Получаем последнее действие и оцениваем его
    Event * ev = this->em->First();
    std::string eventStatus;

    switch (ev->status) {
    case Event::ADDED:
        eventStatus = "добавлен";
        break;
    case Event::EDITED:
        eventStatus = "отредактирован";
        break;
    case Event::DELETED:
        eventStatus = "удален";
        break;
    case Event::COMMITTED:
        eventStatus = "занесен в базу";
        break;
    }

    // Показываем пользователю возможность отменить действие
    this->ui->eventLabel->setText((ev->getEventName() + " " + eventStatus).c_str());
    this->ui->eventBox->show();
    this->ui->filterLayout->setGeometry(540, 170, 260, 130);
}

void MainWindow::RenderCars() {
    // Загружаем информацию об автомобилях
    std::vector<Package::Structure> collection = this->db->Connect("../database/cars.db")->Read();

    // Создаем модель заполнения информации
    QStandardItemModel *model = new QStandardItemModel(this);

    // Количество колонок
    model->setColumnCount(4);

    // Шапка таблицы
    model->setHeaderData(0, Qt::Horizontal, QString("ID"));
    model->setHeaderData(1, Qt::Horizontal, QString("Марка"));
    model->setHeaderData(2, Qt::Horizontal, QString("Модель"));
    model->setHeaderData(3, Qt::Horizontal, QString("Заказан до"));

    // Собираем информацию о марках и моделях автомобилей
    std::vector<std::string> marks;
    std::vector<std::string> models;

    int row = 0;
    for (const auto &car : collection) {
        Car *carObject = new Car(db);
        carObject->deserialize(car);    // Десериализация

        QStandardItem *id = new QStandardItem(QString::number(carObject->getTransactionId()));
        model->setItem(row, 0, id);

        QStandardItem *mark = new QStandardItem(carObject->getMark().c_str());
        model->setItem(row, 1, mark);


        QStandardItem *carmodel = new QStandardItem(carObject->getModel().c_str());
        model->setItem(row, 2, carmodel);

        marks.push_back(carObject->getMark());
        models.push_back(carObject->getModel());

        // Проверяем статус автомобиля
        Package::Structure order = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(carObject->getTransactionId()));
        QStandardItem *aboutInfo;
        if (order.payload.empty()) {
            aboutInfo = new QStandardItem("Свободно");
        } else {
            aboutInfo = new QStandardItem(order.payload[3].c_str());
        }
        delete carObject;
        model->setItem(row++, 3, aboutInfo);
    }

    this->ui->CarView->setModel(model);
    this->ui->CarView->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->CarView->sortByColumn(3, Qt::AscendingOrder);     // Выполняем сортировку по статусу
    this->ui->CarView->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрещаем редактирование

    // Заполняем "фильтр-бокс"
    sort(marks.begin(), marks.end());
    marks.erase(unique( marks.begin(), marks.end()), marks.end());
    this->ui->markBox->clear();
    this->ui->markBox->addItem("Все автомобили");
    for (const auto &mark : marks) {
        this->ui->markBox->addItem(mark.c_str());
    }
}

void MainWindow::on_CarView_doubleClicked(const QModelIndex &index)
{
    // Получаем идентификатор автомобиля
    size_t transaction = index.siblingAtColumn(0).data().value<size_t>();

    Car *car = new Car(this->db);
    car->deserialize(this->db->Connect("../database/cars.db")->Find(transaction));

    // Открываем окно подробной информации об автомобиле
    CarWidget *carwidget = new CarWidget(this->db, car, this->user, this->em);
    carwidget->show();
    carwidget->Render();
    carwidget->focusWidget();
}

void MainWindow::on_logoutButton_clicked()
{
    this->close();
    // Открываем окно авторизации
    LoginForm *login = new LoginForm(this->db);
    login->show();
}

void MainWindow::on_cancelEventButton_clicked()
{
    // Отменяем последнее событие
    this->em->Cancel();
}

void MainWindow::on_markBox_currentTextChanged(const QString &markLabel)
{
    std::vector<Package::Structure> collection = this->db->Connect("../database/cars.db")->Read();
    std::vector<std::string> models;

    for (const auto &carLabel : collection) {
        Car *car = new Car(this->db);
        car->deserialize(carLabel);
        if (QString(car->getMark().c_str()) == markLabel) {
            models.push_back(car->getModel());
        }
        delete car;
    }

    // Формируем "фильтр-бокс"
    this->ui->modelBox->setEnabled(true);
    sort(models.begin(), models.end());
    models.erase(unique(models.begin(), models.end()), models.end());
    this->ui->modelBox->clear();
    this->ui->modelBox->addItem("Все автомобили");
    for (const auto &model : models) {
        this->ui->modelBox->addItem(model.c_str());
    }
}

void MainWindow::on_filterButton_pressed()
{
    std::string carMark = this->ui->markBox->currentText().toUtf8().constData();
    std::string carModel = this->ui->modelBox->currentText().toUtf8().constData();

    this->ui->CarView->reset();

    QStandardItemModel *model = new QStandardItemModel(this);

    model->setColumnCount(4);

    model->setHeaderData(0, Qt::Horizontal, QString("Индификатор"));
    model->setHeaderData(1, Qt::Horizontal, QString("Марка"));
    model->setHeaderData(2, Qt::Horizontal, QString("Модель"));
    model->setHeaderData(3, Qt::Horizontal, QString("Состояние"));

    int row = 0;

    // Формируем список автомобилей
    std::vector<Package::Structure> collection = this->db->Connect("../database/cars.db")->Read();
    for (Package::Structure pkg : collection) {
        Car carObject(this->db);
        carObject.deserialize(pkg);
        if (carMark == "Все автомобили") {
            if (carModel == "Все автомобили") {
                QStandardItem *id = new QStandardItem(QString::number(carObject.getTransactionId()));
                model->setItem(row, 0, id);

                QStandardItem *mark = new QStandardItem(carObject.getMark().c_str());
                model->setItem(row, 1, mark);


                QStandardItem *carmodel = new QStandardItem(carObject.getModel().c_str());
                model->setItem(row, 2, carmodel);

                Package::Structure order = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(carObject.getTransactionId()));
                QStandardItem *aboutInfo;
                if (order.payload.empty()) {
                    aboutInfo = new QStandardItem("Свободно");
                } else {
                    qint64 daysTo = QDate().fromString(order.payload[2].c_str(), "dd.MM.yyyy")
                                            .daysTo(QDate()
                                                    .fromString(order.payload[3].c_str(), "dd.MM.yyyy")
                                            );
                    aboutInfo = new QStandardItem((std::to_string(daysTo) + " суток").c_str());
                }
                model->setItem(row++, 3, aboutInfo);
            }
        } else {
            if (carModel == "Все автомобили") {
                if (carMark == carObject.getMark()) {
                    QStandardItem *id = new QStandardItem(QString::number(carObject.getTransactionId()));
                    model->setItem(row, 0, id);

                    QStandardItem *mark = new QStandardItem(carObject.getMark().c_str());
                    model->setItem(row, 1, mark);


                    QStandardItem *carmodel = new QStandardItem(carObject.getModel().c_str());
                    model->setItem(row, 2, carmodel);

                    Package::Structure order = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(carObject.getTransactionId()));
                    QStandardItem *aboutInfo;
                    if (order.payload.empty()) {
                        aboutInfo = new QStandardItem("Свободно");
                    } else {
                        qint64 daysTo = QDate().fromString(order.payload[2].c_str(), "dd.MM.yyyy")
                                                .daysTo(QDate()
                                                        .fromString(order.payload[3].c_str(), "dd.MM.yyyy")
                                                );
                        aboutInfo = new QStandardItem((std::to_string(daysTo) + " суток").c_str());
                    }
                    model->setItem(row++, 3, aboutInfo);
                }
            } else if (carObject.getMark() == carMark && carObject.getModel() == carModel) {
                QStandardItem *id = new QStandardItem(QString::number(carObject.getTransactionId()));
                model->setItem(row, 0, id);

                QStandardItem *mark = new QStandardItem(carObject.getMark().c_str());
                model->setItem(row, 1, mark);


                QStandardItem *carmodel = new QStandardItem(carObject.getModel().c_str());
                model->setItem(row, 2, carmodel);

                Package::Structure order = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(carObject.getTransactionId()));
                QStandardItem *aboutInfo;
                if (order.payload.empty()) {
                    aboutInfo = new QStandardItem("Свободно");
                } else {
                    qint64 daysTo = QDate().fromString(order.payload[2].c_str(), "dd.MM.yyyy")
                                            .daysTo(QDate()
                                                    .fromString(order.payload[3].c_str(), "dd.MM.yyyy")
                                            );
                    aboutInfo = new QStandardItem((std::to_string(daysTo) + " суток").c_str());
                }
                model->setItem(row++, 3, aboutInfo);
            }
        }

    }
    this->ui->CarView->setModel(model);
    this->ui->CarView->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->CarView->sortByColumn(3, Qt::AscendingOrder);
    this->ui->CarView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_profileButton_pressed()
{
    // Открываем профиль пользователя
    UserForm *uf = new UserForm(this->db, this->em, this->user);
    uf->show();
}
