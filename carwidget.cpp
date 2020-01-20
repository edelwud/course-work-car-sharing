#include "carwidget.h"
#include "ui_carwidget.h"

CarWidget::CarWidget(Database *db, Car *car, User *user, EventManager *em, User::Role rules, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarWidget),
    db(db), car(car), user(user), em(em), rules(rules)
{
    ui->setupUi(this);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Widget| Qt::MSWindowsFixedSizeDialogHint);

    // Проверяем права пользователя
    if (user->role == User::SIMPLEUSER && rules != User::ADMIN) {
        // Если авторизованный пользователь == простой пользователь, скрываем возможность редактирования и удаления
        this->ui->header->hide();
    }
}

void CarWidget::Render() {
    // Изменяем заголовок окна
    this->setWindowTitle(("Аренда автомобиля " + this->car->getMark() + " " + this->car->getModel()).c_str());

    this->ui->markLabel->setWordWrap(true);
    this->ui->modelLabel->setWordWrap(true);
    this->ui->aboutLabel->setWordWrap(true);

    // Заполняем окно информацией
    this->ui->markLabel->setText((this->car->getMark() + " " + this->car->getModel()).c_str());
    this->ui->markLabel_2->setText(this->car->getMark().c_str());
    this->ui->modelLabel->setText(this->car->getModel().c_str());
    this->ui->aboutLabel->setText(this->car->getAboutInfo().c_str());
    this->ui->costLabel->setText(Package::ConvertSizet(this->car->getCostPerDay()).c_str());

    // Загружаем изображение
    QPixmap image(this->car->getResource().c_str());
    image = image.scaled(400, 300);
    this->ui->resourceLabel->setPixmap(image);
    this->ui->pushButton->setText("Арендовать");

    // Оцениваем статус автомобиля
    Package::Structure order = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(this->car->getTransactionId()));
    if (!order.payload.empty()) {
        // Автомобиль заказан
        Order *o = new Order(this->db);
        o->deserialize(order);
        this->ui->orderDate->setDate(QDate().fromString(o->getOrderDate().c_str(), "dd.MM.yyyy"));
        this->ui->orderPassDate->setDate(QDate().fromString(o->getOrderPassDate().c_str(), "dd.MM.yyyy"));
        this->ui->pushButton->setDisabled(true);
        if (order.payload[1] == Package::ConvertSizet(this->user->getTransactionId()) || rules == User::ADMIN) {
            // Владелец заказа или администратор может его отменить
            this->ui->pushButton->setText("Отменить заказ");
            this->ui->pushButton->setDisabled(false);
        }
        this->ui->fullCostLabel->setText("X");
        this->ui->orderDate->setDisabled(true);
        this->ui->orderPassDate->setDisabled(true);
        delete o;
    } else {
        // Устанавливаем дату
        this->ui->orderDate->setDate(QDate().currentDate());
        this->ui->orderPassDate->setDate(QDate().currentDate().addDays(1));
        this->ui->orderDate->setDisabled(false);
        this->ui->orderPassDate->setDisabled(false);
        this->ui->fullCostLabel->setText(Package::ConvertSizet(this->car->getCostPerDay()).c_str());
    }
}

CarWidget::~CarWidget()
{
    delete this->car;
    delete ui;
}

void CarWidget::on_closeButton_clicked()
{
    this->close();
}

void CarWidget::on_pushButton_clicked()
{
    // Проверяем статус автомобиля
    if (this->ui->pushButton->text() == "Отменить заказ") {
        // Загружаем список заказов
        std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
        for (Package::Structure &statement : collection) {
            Order o(this->db);
            o.deserialize(statement);
            // Находим тот, что относится к открытому автомобилю
            if (o.getCarId() == this->car->getTransactionId() && o.getUserId() == this->user->getTransactionId()) {
                // Удаляем заказ
                this->em->DeleteEvent(&o);
                this->Render();
            }
        }
        return;
    }

    QDate orderDate = this->ui->orderDate->date();
    QDate orderPassDate = this->ui->orderPassDate->date();

    if (orderDate >= orderPassDate) {
        QMessageBox alertBox;
        alertBox.setWindowTitle("Ошибка");
        alertBox.setText("Начальная дата не может быть больше или равна конечной");
        alertBox.exec();
        return;
    }

    if (orderDate < QDate().currentDate()) {
        QMessageBox alertBox;
        alertBox.setWindowTitle("Ошибка");
        alertBox.setText("Начальная дата не может быть меньше текущей");
        alertBox.exec();
        this->ui->orderDate->setDate(QDate().currentDate());
        return;
    }

    // Формируем измененный заказ
    Order *car = new Order(this->db,
                           Order::PacketFormat{
                               this->car->getTransactionId(),
                               this->user->getTransactionId(),
                               orderDate.toString("dd.MM.yyyy").toUtf8().constData(),
                               orderPassDate.toString("dd.MM.yyyy").toUtf8().constData(),
                           });

    // Добавляем его в базу данных
    this->em->CommitEvent(car);
    // Заново отрисовываем страницу
    this->Render();
}

void CarWidget::on_deleteCarButton_clicked()
{
    // Удаляем связанные с автомобилем заказы
    vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
    for (auto fmt : collection) {
        if (stoull(fmt.payload[0]) == this->car->getTransactionId()) {
            Order *order = new Order(this->db);
            order->deserialize(fmt);
            this->em->DeleteEventByTransaction(order);
            delete order;
        }
    }

    // Удаляем автомобиль из базы данных
    this->em->DeleteEvent(this->car);
    QMessageBox::information(this, "Успех", "Автомобиль успешно удален");
    this->close();
}

void CarWidget::on_editButton_clicked()
{
    Order *order = new Order(this->db);
    Package::Structure orderData = this->db->Connect("../database/orders.db")->Find(0, Package::ConvertSizet(this->car->getTransactionId()));
    if (orderData.payload.empty()) {
        delete order;
        order = nullptr;
    } else {
        order->deserialize(orderData);
    }
    this->close();
    CarEditForm *carEdit = new CarEditForm(this->db, this->car, this->user, order, this->em);
    carEdit->show();
}

void CarWidget::on_orderPassDate_dateChanged(const QDate &date)
{
    if (date <= this->ui->orderDate->date()) {
        this->ui->pushButton->setDisabled(true);
        return;
    }
    this->ui->pushButton->setDisabled(false);

    size_t fullCost = this->ui->orderDate->date().daysTo(date) * this->car->getCostPerDay();
    this->ui->fullCostLabel->setText(Package::ConvertSizet(fullCost).c_str());
    if (fullCost > this->user->getBalance()) this->ui->pushButton->setDisabled(true);
}

void CarWidget::on_orderDate_dateChanged(const QDate &date)
{
    if (date < QDate().currentDate()) {
        this->ui->pushButton->setDisabled(true);
        return;
    }
    this->ui->pushButton->setDisabled(false);

    size_t fullCost = date.daysTo(this->ui->orderPassDate->date()) * this->car->getCostPerDay();
    this->ui->fullCostLabel->setText(Package::ConvertSizet(fullCost).c_str());
    if (fullCost > this->user->getBalance()) this->ui->pushButton->setDisabled(true);
}
