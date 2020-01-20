#ifndef CAR_H
#define CAR_H

#include "Event.h"
#include "EventManager.h"
#include "Order.h"
#include "Database.h"

// Класс автомобилей
class Car :
        public Event {
    std::string mark, model, about, resource;
    std::string databasePath = ":/database/cars.db";
    size_t costPerDay;
public:
    // Структура входных данных
    struct PacketFormat {
        std::string mark, model, about, resource;
        size_t costPerDay;
    };

    explicit Car(Database *db);
    explicit Car(Database *db, PacketFormat packet);

    // Создание копии объекта
    Event * Clone() override {
        Event *clonned = new Car(*this);
        clonned->setTransactionId(this->transactionId);
        return clonned;
    }

    // Получение марки автомобиля
    const std::string & getMark() const {
        return this->mark;
    }

    // Получение модели автомобиля
    const std::string & getModel() const {
        return this->model;
    }

    // Получение информации об автомобиля
    const std::string & getAboutInfo() const {
        return this->about;
    }

    // Получение ресурсов автомобиля
    const std::string & getResource() const {
        return this->resource;
    }

    // Получение стоимость заказа автомобиля на день
    const size_t & getCostPerDay() const {
        return this->costPerDay;
    }

    // Получение имени события
    std::string getEventName() const override {
        return "Car";
    }

    // Переопределение функции удаления родительского класса
    void Delete() override;

    // Сериализация объекта
    Package::Structure serialize() const override;
    // Десериализация объекта
    void deserialize(const Package::Structure &packet) override;
};


#endif // CAR_H
