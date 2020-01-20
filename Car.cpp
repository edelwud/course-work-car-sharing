#include "Car.h"

Car::Car(Database *db) {
    // Путь к базе данных
    this->absoluteDBPath = "../database/cars.db";
    // Осуществляем подключение к базе
    this->db = db->Connect(this->absoluteDBPath);
}

Car::Car(Database *db, Car::PacketFormat packet) : Car(db) {
    this->mark = packet.mark;
    this->model = packet.model;
    this->about = packet.about;
    this->resource = packet.resource;
    this->costPerDay = packet.costPerDay;
}

void Car::Delete() {
    // Удаляем связанные с автомобилем заказы
    std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
    for (Package::Structure fmt : collection) {
        Order order(this->db);
        order.deserialize(fmt);
        if (order.getCarId() == this->transactionId)
            order.Delete();
    }

    // Удаляем сам автомобиль
    Package::Structure packet = this->serialize();
    this->db->Connect(absoluteDBPath)->Delete(packet);
    this->status = DELETED;
}

Package::Structure Car::serialize() const {
    // Формируем пакет
    std::vector<std::string> payload;

    payload.push_back(this->mark);
    payload.push_back(this->model);
    payload.push_back(this->about);
    payload.push_back(this->resource);
    payload.push_back(Package::ConvertSizet(this->costPerDay));

    return Package::Structure{this->transactionId, payload};
}

void Car::deserialize(const Package::Structure &packet) {
    // Преобразуем пакет к объекту
    this->transactionId = packet.transaction;
    this->mark = packet.payload[0];
    this->model = packet.payload[1];
    this->about = packet.payload[2];
    this->resource = packet.payload[3];
    this->costPerDay = std::stoull(packet.payload[4]);
}
