#include "loginform.h"
#include "ui_loginform.h"

LoginForm::LoginForm(Database *db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm),
    db(db), auth(new AuthController(db))
{
    ui->setupUi(this);

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    // Создание валидатора пользовательского ввода
    QRegExp stringExpression("^[A-zА-яЁё]{15}");
    QRegExp passExpression("^[0-9A-zА-яЁё]{15}");
    this->validator = new QRegExpValidator(this);

    // Применение валидатора
    this->validator->setRegExp(stringExpression);
    this->ui->loginInput->setValidator(validator);

    this->validator->setRegExp(passExpression);
    this->ui->passInput->setValidator(validator);

    // Поиск сессии
    if (User *user = this->auth->FindSession()) {
        this->ui->loginInput->setText(user->getLogin().c_str());
        delete user;
    }

    // Сокрытие вводимых символов
    this->ui->passInput->setEchoMode(QLineEdit::Password);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::on_loginButton_clicked()
{
    std::string login = this->ui->loginInput->text().toUtf8().constData();
    std::string password = this->ui->passInput->text().toUtf8().constData();

    if (login.size() < 4 || password.size() < 4) {
        QMessageBox::warning(this, "Ошибка", "Проверьте правильность заполнения полей");
        return;
    }

    try {
        // Попытка авторизации пользователя
        this->auth->Login(User::PacketFormat{ login, password });
        this->close();

        User *user = new User(this->db);
        user->deserialize(this->db->Find(this->auth->UserExist(User::PacketFormat{login, password})));

        this->w = new MainWindow(this->db, user);
        w->show();
    } catch(Controller *exception) {
        QMessageBox::warning(this, "Ошибка", exception->Message().c_str());
    }
}

void LoginForm::on_registerButton_clicked()
{
    RegisterForm *registerForm = new RegisterForm(this->db);
    registerForm->show();
    this->close();
}
