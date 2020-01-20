#include "userform.h"
#include "ui_userform.h"

#include <QDebug>

UserForm::UserForm(Database *db, EventManager *em, User *user, QWidget *parent) :
    QWidget(parent),
    db(db),
    em(em),
    user(user),
    ui(new Ui::UserForm)
{
    ui->setupUi(this);
    this->ui->profileTitle->setText(user->getLogin().c_str());
    if (this->user->getRole() != User::ADMIN) {
        this->ui->addCarButton->hide();
        this->ui->admin->setEnabled(false);
    } else {
        this->AdminCarsRender();
        this->AdminUsersRender();
        this->AdminOrdersRender();
    }

    this->RenderCars();
    this->RenderProfile();
    this->CancelButtonRender();
    this->ui->balanceLabel->setText(("Баланс пользователя: " + Package::ConvertSizet(this->user->getBalance())).c_str());

    this->eventSubID = this->em->Subscribe(Event::SUPER, [&](Event::Status){
        this->RenderCars();
        this->RenderProfile();
        this->CancelButtonRender();
        this->ui->balanceLabel->setText(("Баланс пользователя: " + Package::ConvertSizet(this->user->getBalance())).c_str());
        if (this->user->getRole() == User::ADMIN) {
            this->AdminCarsRender();
            this->AdminUsersRender();
            this->AdminOrdersRender();
        }
    });
}

UserForm::~UserForm()
{
    delete ui;
    this->em->Unsubscribe(this->eventSubID);
}

void UserForm::CancelButtonRender() {
    if (!this->em->getCount()) {
        this->ui->cancelButton->hide();
        this->ui->addCarButton->setGeometry(773, 1, 167, 25);
        return;
    }

    this->ui->cancelButton->show();
    this->ui->addCarButton->setGeometry(687, 1, 167, 25);
}

void UserForm::AdminCarsRender()
{
    std::vector<Package::Structure> collection = this->db->Connect("../database/cars.db")->Read();

    QStandardItemModel *model = new QStandardItemModel(this);

    model->setRowCount(static_cast<int>(collection.size()));
    model->setColumnCount(3);

    model->setHeaderData(0, Qt::Horizontal, QString("ID"));
    model->setHeaderData(1, Qt::Horizontal, QString("Марка"));
    model->setHeaderData(2, Qt::Horizontal, QString("Модель"));

    int row = 0;
    for (const auto &car : collection) {
        Car *carObject = new Car(db);
        carObject->deserialize(car);

        QStandardItem *id = new QStandardItem(QString::number(carObject->getTransactionId()));
        model->setItem(row, 0, id);

        QStandardItem *mark = new QStandardItem(carObject->getMark().c_str());
        model->setItem(row, 1, mark);

        QStandardItem *carmodel = new QStandardItem(carObject->getModel().c_str());
        model->setItem(row++, 2, carmodel);
    }

    this->ui->carsView->setModel(model);
    this->ui->carsView->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->carsView->sortByColumn(0, Qt::AscendingOrder);
    this->ui->carsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void UserForm::AdminUsersRender()
{
    std::vector<Package::Structure> collection = this->db->Connect("../database/users.db")->Read();

    QStandardItemModel *model = new QStandardItemModel(this);

    model->setRowCount(static_cast<int>(collection.size()));
    model->setColumnCount(3);

    model->setHeaderData(0, Qt::Horizontal, QString("ID"));
    model->setHeaderData(1, Qt::Horizontal, QString("Имя"));
    model->setHeaderData(2, Qt::Horizontal, QString("Фамилия"));

    int row = 0;
    for (const auto &user : collection) {
        User *userObject = new User(db);
        userObject->deserialize(user);

        QStandardItem *id = new QStandardItem(QString::number(userObject->getTransactionId()));
        model->setItem(row, 0, id);

        QStandardItem *firstname = new QStandardItem(userObject->getFirstname().c_str());
        model->setItem(row, 1, firstname);

        QStandardItem *lastname = new QStandardItem(userObject->getLastname().c_str());
        model->setItem(row++, 2, lastname);
    }

    this->ui->usersView->setModel(model);
    this->ui->usersView->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->usersView->sortByColumn(0, Qt::AscendingOrder);
    this->ui->usersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UserForm::AdminOrdersRender()
{
    std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();

    QStandardItemModel *model = new QStandardItemModel(this);

    model->setRowCount(static_cast<int>(collection.size()));
    model->setColumnCount(3);

    model->setHeaderData(0, Qt::Horizontal, QString("ID"));
    model->setHeaderData(1, Qt::Horizontal, QString("Логин"));
    model->setHeaderData(2, Qt::Horizontal, QString("Автомобиль"));

    int row = 0;
    for (const auto &order : collection) {
        Order *orderObject = new Order(db);
        orderObject->deserialize(order);

        QStandardItem *id = new QStandardItem(QString::number(orderObject->getTransactionId()));
        model->setItem(row, 0, id);

        QStandardItem *login = new QStandardItem(this->db->Connect("../database/users.db")->Find(orderObject->getUserId()).payload[0].c_str());
        model->setItem(row, 1, login);

        QStandardItem *carmodel = new QStandardItem(this->db->Connect("../database/cars.db")->Find(orderObject->getCarId()).payload[0].c_str());
        model->setItem(row++, 2, carmodel);
    }

    this->ui->ordersView_2->setModel(model);
    this->ui->ordersView_2->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->ordersView_2->sortByColumn(0, Qt::AscendingOrder);
    this->ui->ordersView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UserForm::RenderProfile() {
    Package::Structure fmt = this->db->Connect("../database/users.db")->Find(this->user->getTransactionId());
    this->user->deserialize(fmt);
    this->ui->nameEdit->setText(this->user->getFirstname().c_str());
    this->ui->surnameEdit->setText(this->user->getLastname().c_str());

    this->ui->oldPassword->setEchoMode(QLineEdit::Password);
    this->ui->newPassword->setEchoMode(QLineEdit::Password);
    this->ui->confNewPassword->setEchoMode(QLineEdit::Password);
}

void UserForm::RenderCars() {
    std::vector<Package::Structure> collection = this->db->Connect("../database/cars.db")->Read();

    QStandardItemModel *model = new QStandardItemModel(this);

    model->setRowCount(static_cast<int>(collection.size()));
    model->setColumnCount(4);

    model->setHeaderData(0, Qt::Horizontal, QString("Индификатор"));
    model->setHeaderData(1, Qt::Horizontal, QString("Марка"));
    model->setHeaderData(2, Qt::Horizontal, QString("Модель"));
    model->setHeaderData(3, Qt::Horizontal, QString("Заказан до"));

    int row = 0;

    std::vector<Package::Structure> orders = this->db->Connect("../database/orders.db")->Read();

    for (const auto &car : collection) {
        Car *carObject = new Car(db);
        carObject->deserialize(car);

        for (Package::Structure data : orders) {
            if (std::stoull(data.payload[0].c_str()) == carObject->getTransactionId() &&
                    std::stoull(data.payload[1].c_str()) == this->user->getTransactionId()) {

                QStandardItem *id = new QStandardItem(QString::number(carObject->getTransactionId()));
                model->setItem(row, 0, id);

                QStandardItem *mark = new QStandardItem(carObject->getMark().c_str());
                model->setItem(row, 1, mark);


                QStandardItem *carmodel = new QStandardItem(carObject->getModel().c_str());
                model->setItem(row, 2, carmodel);

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
        }
    }

    this->ui->ordersView->setModel(model);
    this->ui->ordersView->setStyleSheet("QTreeView::item { height: 32px;}");
    this->ui->ordersView->sortByColumn(3, Qt::AscendingOrder);
    this->ui->ordersView->setColumnWidth(0, 230);
    this->ui->ordersView->setColumnWidth(1, 230);
    this->ui->ordersView->setColumnWidth(2, 230);
    this->ui->ordersView->setColumnWidth(3, 230);
    this->ui->ordersView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void UserForm::on_ordersView_doubleClicked(const QModelIndex &index)
{
    int ret = QMessageBox::question(this, "Отменить заказ", "Хотите отменить заказ?", QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        size_t transaction = index.siblingAtColumn(0).data().value<size_t>();

        std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
        for (Package::Structure fmt : collection) {
            if (std::stoull(fmt.payload[0]) == transaction) {
                Order result(this->db);
                result.deserialize(fmt);
                this->em->DeleteEvent(&result);
            }
        }
    }
}

void UserForm::on_addCarButton_pressed()
{
    AddCarForm *acf = new AddCarForm(this->db, this->em);
    acf->show();
}

void UserForm::on_changeProfileButton_pressed()
{
    if (this->user->getFirstname() == this->ui->nameEdit->text().toUtf8().constData() &&
            this->user->getLastname() == this->ui->surnameEdit->text().toUtf8().constData()) {
        return;
    }
    User result(this->db,
                {
                    this->user->getLogin(),
                    this->user->getPassword(),
                    this->ui->nameEdit->text().toUtf8().constData(),
                    this->ui->surnameEdit->text().toUtf8().constData(),
                    this->user->getBalance(),
                    this->user->getRole()
                });
    this->em->EditEvent(&result, this->user);
    QMessageBox::information(this, "Успех", "Информация успешно изменена");
}

void UserForm::on_carsView_doubleClicked(const QModelIndex &index)
{
    size_t transaction = index.siblingAtColumn(0).data().value<size_t>();
    Package::Structure fmt = this->db->Connect("../database/cars.db")->Find(transaction);
    Car *car = new Car(this->db);
    car->deserialize(fmt);

    Order *order = nullptr;
    std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
    for (Package::Structure payload : collection) {
        if (std::stoull(payload.payload[0]) == transaction) {
            order = new Order(this->db);
            order->deserialize(payload);
        }
    }

    CarEditForm *cef = new CarEditForm(this->db, car, this->user, order, this->em);
    cef->show();
}

void UserForm::on_usersView_doubleClicked(const QModelIndex &index)
{
    size_t transaction = index.siblingAtColumn(0).data().value<size_t>();
    Package::Structure fmt = this->db->Connect("../database/users.db")->Find(transaction);
    User *user = new User(this->db);
    user->deserialize(fmt);

    UserEditForm *uef = new UserEditForm(this->db, user, this->em);
    if (user->getLogin() == this->user->getLogin()) uef->DisableDeleteButton();
    uef->show();
}

void UserForm::on_ordersView_2_doubleClicked(const QModelIndex &index)
{
    size_t transaction = index.siblingAtColumn(0).data().value<size_t>();
    Package::Structure fmt = this->db->Connect("../database/orders.db")->Find(transaction);
    Order *order = new Order(this->db);
    order->deserialize(fmt);

    Car *car = new Car(this->db);
    car->deserialize(this->db->Connect("../database/cars.db")->Find(order->getCarId()));

    User *user = new User(this->db);
    user->deserialize(this->db->Connect("../database/users.db")->Find(order->getUserId()));

    CarWidget *cw = new CarWidget(this->db, car, user, this->em, User::ADMIN);
    cw->Render();
    cw->show();
}

void UserForm::on_changePasswordButton_pressed()
{
    if (this->ui->oldPassword->text().size() < 4 ||
        this->ui->newPassword->text().size() < 4 ||
        this->ui->confNewPassword->text().size() < 4)
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте правильность заполнения полей");
        return;
    }

    if (this->ui->confNewPassword->text() != this->ui->newPassword->text()) {
        QMessageBox::warning(this, "Ошибка", "Новый пароль не совпадает с подтверждением пароля");
        return;
    }

    if (this->ui->oldPassword->text().toUtf8().constData() != this->user->getPassword()) {
        QMessageBox::warning(this, "Ошибка", "Старый пароль введен не верно");
        return;
    }

    if (this->ui->oldPassword->text() == this->ui->newPassword->text()) {
        QMessageBox::warning(this, "Ошибка", "Новый пароль не должен совпадать со старым");
        return;
    }

    User::PacketFormat pfmt {
        this->user->getLogin(),
        this->ui->newPassword->text().toUtf8().constData(),
        this->user->getFirstname(),
        this->user->getLastname(),
        this->user->getBalance(),
        this->user->getRole()
    };

    User user(this->db, pfmt);
    this->em->EditEvent(&user, this->user);

    QMessageBox::information(this, "Успех", "Пароль сменен успешно");

    this->ui->oldPassword->setText("");
    this->ui->newPassword->setText("");
    this->ui->confNewPassword->setText("");
}

void UserForm::on_cancelButton_pressed()
{
    this->em->Cancel();
}
