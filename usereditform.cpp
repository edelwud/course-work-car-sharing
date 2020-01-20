#include "usereditform.h"
#include "ui_usereditform.h"

UserEditForm::UserEditForm(Database *db, User *user, EventManager *em, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserEditForm),
    db(db),
    user(user),
    em(em)
{
    ui->setupUi(this);
    this->ui->passwordEdit->setEchoMode(QLineEdit::Password);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    // Создание валидатора пользовательского ввода
    QRegExpValidator *validator = new QRegExpValidator(QRegExp("^[0-9A-zА-яЁё]{15}"), this);
    QIntValidator *intValidator = new QIntValidator(1, 9999, this);

    // Применение валидатора
    this->ui->loginEdit->setValidator(validator);
    this->ui->passwordEdit->setValidator(validator);
    this->ui->nameEdit->setValidator(validator);
    this->ui->lastnameEdit->setValidator(validator);
    this->ui->balanceEdit->setValidator(intValidator);

    // Заполнение полей пользователя
    this->ui->loginEdit->setText(this->user->getLogin().c_str());
    this->ui->passwordEdit->setText(this->user->getPassword().c_str());
    this->ui->nameEdit->setText(this->user->getFirstname().c_str());
    this->ui->lastnameEdit->setText(this->user->getLastname().c_str());
    this->ui->balanceEdit->setText(Package::ConvertSizet(this->user->getBalance()).c_str());
    this->ui->comboBox->insertItem(1, "Администратор");
    this->ui->comboBox->insertItem(2, "Пользователь");
    this->ui->comboBox->setCurrentIndex(this->user->getRole() - 1);
}

UserEditForm::~UserEditForm()
{
    delete ui;
}

void UserEditForm::on_saveButton_pressed()
{
    if (this->ui->loginEdit->text().size() < 3 ||
        this->ui->passwordEdit->text().size() < 3 ||
        this->ui->nameEdit->text().size() < 3 ||
        this->ui->lastnameEdit->text().size() < 3)
    {
        QMessageBox::warning(this, "Ошибка", "Проверьте правильность заполнения полей");
        return;
    }

    if (this->user->getLogin() != this->ui->loginEdit->text().toUtf8().constData() ||
        this->user->getPassword() != this->ui->passwordEdit->text().toUtf8().constData() ||
        this->user->getFirstname() != this->ui->nameEdit->text().toUtf8().constData() ||
        this->user->getLastname() != this->ui->lastnameEdit->text().toUtf8().constData() ||
        Package::ConvertSizet(this->user->getBalance()).c_str() != this->ui->balanceEdit->text().toUtf8().constData() ||
        this->user->getRole() - 1 != this->ui->comboBox->currentIndex()) {

        int roleIndex = this->ui->comboBox->currentIndex() + 1;

        User::PacketFormat fmt {
            this->ui->loginEdit->text().toUtf8().constData(),
            this->ui->passwordEdit->text().toUtf8().constData(),
            this->ui->nameEdit->text().toUtf8().constData(),
            this->ui->lastnameEdit->text().toUtf8().constData(),
            std::stoull(this->ui->balanceEdit->text().toUtf8().constData()),
            (User::Role)roleIndex
        };

        User *user = new User(this->db, fmt);
        this->em->EditEvent(user, this->user);
        this->close();
    }
}

void UserEditForm::DisableDeleteButton() {
    this->ui->deleteButton->setDisabled(true);
}

void UserEditForm::on_deleteButton_pressed()
{
    // Удаляем связанные с пользователем заказы
    vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
    for (auto fmt : collection) {
        if (stoull(fmt.payload[0]) == this->user->getTransactionId()) {
            Order *order = new Order(this->db);
            order->deserialize(fmt);
            this->em->DeleteEventByTransaction(order);
            delete order;
        }
    }

    // Удаляем пользователя
    this->em->DeleteEvent(this->user);
    QMessageBox::information(this, "Успех", "Пользователь успешно удален");
    this->close();
}
