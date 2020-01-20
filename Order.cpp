#include "Order.h"

Order::Order(Database *db) {
    this->absoluteDBPath = "../database/orders.db";
    this->db = db->Connect(this->absoluteDBPath);
}

Order::Order(Database *db, const Order::PacketFormat &packet) : Order(db) {
    this->carId = packet.carId;
    this->userId = packet.userId;
    this->orderDate = packet.orderDate;
    this->orderPassDate = packet.orderPassDate;
}

bool Order::Commit() {
    size_t days = QDate().fromString(this->getOrderDate().c_str(), "dd.MM.yyyy").daysTo(QDate().fromString(this->getOrderPassDate().c_str(), "dd.MM.yyyy"));

    Car car(this->db);
    car.deserialize(this->db->Connect("../database/cars.db")->Find(this->carId));

    User user(this->db);
    user.deserialize(this->db->Connect("../database/users.db")->Find(this->userId));

    size_t fullCost = days * car.getCostPerDay();
    if (fullCost > user.getBalance())
        return false;

    User nUser(this->db, {
                   user.getLogin(),
                   user.getPassword(),
                   user.getFirstname(),
                   user.getLastname(),
                   user.getBalance() - fullCost,
                   user.getRole()
               });
    user.Edit(&nUser);

    std::vector<Package::Structure> collection = this->db->Connect(absoluteDBPath)->Read();
    if (collection.size()) this->transactionId = collection[collection.size() - 1].transaction + 1;
    else this->transactionId = 1;

    Package::Structure packet = this->serialize();
    this->db->Write(packet);
    this->status = COMMITTED;
    return true;
}

void Order::Delete() {
    if (this->status == Event::DELETED)
        return;

    size_t days = QDate().currentDate().daysTo(QDate().fromString(this->orderPassDate.c_str(), "dd.MM.yyyy"));

    Car car(this->db);
    car.deserialize(this->db->Connect("../database/cars.db")->Find(this->carId));

    User user(this->db);
    user.deserialize(this->db->Connect("../database/users.db")->Find(this->userId));

    size_t cost = days * car.getCostPerDay();

    User nUser(this->db, {
                   user.getLogin(),
                   user.getPassword(),
                   user.getFirstname(),
                   user.getLastname(),
                   user.getBalance() + cost,
                   user.getRole()
               });
    user.Edit(&nUser);

    Package::Structure packet = this->serialize();
    this->db->Connect(absoluteDBPath)->Delete(packet);
    this->status = DELETED;
}

Package::Structure Order::serialize() const {
    std::vector<std::string> payload;

    payload.push_back(Package::ConvertSizet(this->carId));
    payload.push_back(Package::ConvertSizet(this->userId));
    payload.push_back(this->orderDate);
    payload.push_back(this->orderPassDate);

    return Package::Structure{this->transactionId, payload};
}

void Order::deserialize(const Package::Structure &packet) {
    this->transactionId = packet.transaction;
    this->carId = std::stoull(packet.payload[0].c_str());
    this->userId = std::stoull(packet.payload[1].c_str());
    this->orderDate = packet.payload[2];
    this->orderPassDate = packet.payload[3];
}
