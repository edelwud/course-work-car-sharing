#include "addcarform.h"
#include "ui_addcarform.h"

AddCarForm::AddCarForm(Database *db, EventManager *em, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddCarForm),
    db(db), em(em)
{
    ui->setupUi(this);

    // Установка заголовка окна
    this->setWindowTitle("Добавление автомобиля");

    // Установка фиксированного размера окна
    this->setFixedSize(this->window()->size());
    setWindowFlags(Qt::Widget| Qt::MSWindowsFixedSizeDialogHint);

    // Создание валидаторов
    QRegExpValidator *stringValidator = new QRegExpValidator(QRegExp("^[0-9 A-zА-я]{50}"), this);
    QIntValidator *intValidator = new QIntValidator(1, 3000, this);

    // Применение валидаторов
    this->ui->markInput->setValidator(stringValidator);
    this->ui->modelInput->setValidator(stringValidator);
    this->ui->aboutInput->setValidator(stringValidator);
    this->ui->fileInput->setReadOnly(true);
    this->ui->costPerDay->setValidator(intValidator);
}

AddCarForm::~AddCarForm()
{
    delete ui;
}

void AddCarForm::on_fileInput_textChanged(const QString &filename)
{
    QPixmap image(filename);
    image = image.scaled(230, 164);
    this->ui->resourceLabel->setPixmap(image);
}

void AddCarForm::on_fileOpenButton_clicked(bool checked)
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Откройте фотографию автомобиля"), "", tr("Image Files (*.png *.jpeg *.jpg *.bmp)"));
    this->ui->fileInput->setText(filename);
}

void AddCarForm::on_saveButton_clicked()
{
    std::string costPerDay = this->ui->costPerDay->text().toUtf8().constData();
    if (costPerDay.size() == 0)
        costPerDay = "0";
    Car::PacketFormat carFmt {
        this->ui->markInput->text().toUtf8().constData(),
        this->ui->modelInput->text().toUtf8().constData(),
        this->ui->aboutInput->text().toUtf8().constData(),
        this->ui->fileInput->text().toUtf8().constData(),
        std::stoull(costPerDay),
    };
    Car *carEdited = new Car(this->db, carFmt);
    this->em->CommitEvent(carEdited);
    this->close();
}

void AddCarForm::on_pushButton_2_clicked(bool checked)
{
    this->close();
}
