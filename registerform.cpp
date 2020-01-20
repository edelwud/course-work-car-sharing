#include "registerform.h"
#include "ui_registerform.h"

RegisterForm::RegisterForm(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegisterForm),
    db(db), auth(new AuthController(this->db))
{
    ui->setupUi(this);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    // Создание валидатора пользовательского ввода
    QRegExp stringExpression("^[A-zА-яЁё]{15}");
    QRegExp passExpression("^[0-9A-zА-яЁё]{15}");
    this->validator = new QRegExpValidator(passExpression, this);

    // Применение валидатора
    this->ui->loginInput->setValidator(validator);
    this->ui->passInput->setValidator(validator);
    this->ui->nameInput->setValidator(validator);
    this->ui->surnameInput->setValidator(validator);

    // Сокрытие символов пароля
    this->ui->passInput->setEchoMode(QLineEdit::Password);
}

RegisterForm::~RegisterForm()
{
    delete ui;
    delete auth;
}

void RegisterForm::on_loginButton_clicked()
{
    LoginForm *login = new LoginForm(this->db);
    login->show();
    this->close();
}

void RegisterForm::on_registerButton_clicked()
{
    std::string login = this->ui->loginInput->text().toUtf8().constData();
    std::string password = this->ui->passInput->text().toUtf8().constData();
    std::string firstname = this->ui->nameInput->text().toUtf8().constData();
    std::string lastname = this->ui->surnameInput->text().toUtf8().constData();

    if (login.size() < 4 || password.size() < 4 || firstname.size() < 4 || lastname.size() < 4) {
        QMessageBox::warning(this, "Ошибка", "Проверьте правильность заполнения полей");
        return;
    }

    try {
        // Попытка регистрации пользователя
        this->auth->Register(User::PacketFormat{ login, password, firstname, lastname, 200 });
        this->close();

        User *user = new User(this->db);
        user->deserialize(this->db->Find(this->auth->UserExist(User::PacketFormat{login, password})));

        // Переходим к главному окну
        this->w = new MainWindow(this->db, user);
        this->w->show();
    } catch(Controller *exception) {
        QMessageBox::warning(this, "Ошибка", exception->Message().c_str());
    }
}
