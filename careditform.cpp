#include "careditform.h"
#include "ui_careditform.h"

CarEditForm::CarEditForm(Database *db, Car *car, User *user, Order *order, EventManager *em, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarEditForm),
    db(db), car(car), user(user), em(em)
{
    ui->setupUi(this);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Widget| Qt::MSWindowsFixedSizeDialogHint);

    // Установка заголовка
    this->setWindowTitle(("Редактирование " + this->car->getMark() + " " + this->car->getModel()).c_str());

    // Создание валидаторов
    QRegExpValidator *stringValidator = new QRegExpValidator(QRegExp("^[0-9 A-zА-я]{50}"), this);
    QIntValidator *intValidator = new QIntValidator(1, 3000, this);

    // Применение валидаторов
    this->ui->markInput->setValidator(stringValidator);
    this->ui->modelInput->setValidator(stringValidator);
    this->ui->aboutInput->setValidator(stringValidator);
    this->ui->fileInput->setReadOnly(true);
    this->ui->costPerDay->setValidator(intValidator);

    // Загрузка информации
    this->Render();
}

CarEditForm::~CarEditForm()
{
    delete ui;
}

void CarEditForm::Render() {
    // Заполнение окна
    this->ui->markInput->setText(this->car->getMark().c_str());
    this->ui->modelInput->setText(this->car->getModel().c_str());
    this->ui->aboutInput->setText(this->car->getAboutInfo().c_str());
    this->ui->costPerDay->setText(Package::ConvertSizet(this->car->getCostPerDay()).c_str());
    this->ui->fileInput->setText(this->car->getResource().c_str());

    // Добавление изображения
    QPixmap image(this->car->getResource().c_str());
    image = image.scaled(230, 164);
    this->ui->resourceLabel->setPixmap(image);
}

void CarEditForm::on_pushButton_2_clicked()
{
    this->close();
}

void CarEditForm::on_fileOpenButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Откройте фотографию автомобиля"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    this->ui->fileInput->setText(filename);
}

void CarEditForm::on_fileInput_textChanged(const QString &filename)
{
    QPixmap image(filename);
    image = image.scaled(230, 164);
    this->ui->resourceLabel->setPixmap(image);
}

void CarEditForm::on_saveButton_clicked()
{
    if (this->ui->costPerDay->text().size() == 0)
        this->ui->costPerDay->setText("0");
    if (this->car->getMark() != this->ui->markInput->text().toUtf8().constData() ||
        this->car->getModel() != this->ui->modelInput->text().toUtf8().constData() ||
        this->car->getAboutInfo() != this->ui->aboutInput->text().toUtf8().constData() ||
        this->car->getResource() != this->ui->fileInput->text().toUtf8().constData() ||
        std::stoull(this->ui->costPerDay->text().toUtf8().constData()) != this->car->getCostPerDay()) {
        // Формирование нового объекта автомобиля
        Car::PacketFormat carFmt {
            this->ui->markInput->text().toUtf8().constData(),
            this->ui->modelInput->text().toUtf8().constData(),
            this->ui->aboutInput->text().toUtf8().constData(),
            this->ui->fileInput->text().toUtf8().constData(),
            std::stoull(this->ui->costPerDay->text().toUtf8().constData())
        };
        Car *carEdited = new Car(this->db, carFmt);
        // Редактирование старого объекта
        this->em->EditEvent(carEdited, this->car);
    }

    this->close();
}

void CarEditForm::on_deleteButton_pressed()
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

    // Удаляем автомобиль
    this->em->DeleteEvent(this->car);
    QMessageBox::information(this, "Успех", "Автомобиль успешно удален");
    this->close();
}
