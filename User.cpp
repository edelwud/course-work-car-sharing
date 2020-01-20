#include "User.h"

User::User(Database *db) {
    this->absoluteDBPath = "../database/users.db";
    this->db = db->Connect(this->absoluteDBPath);
}

User::User(Database *db, const User::PacketFormat &packet) : User(db) {
    this->firstname = packet.firstname;
    this->lastname = packet.lastname;
    this->balance = packet.balance;
    this->login = packet.login;
    this->password = packet.password;
    this->role = packet.role ? packet.role : SIMPLEUSER;
}

void User::Delete() {
    std::vector<Package::Structure> collection = this->db->Connect("../database/orders.db")->Read();
    for (Package::Structure fmt : collection) {
        Order order(this->db);
        order.deserialize(fmt);
        if (order.getUserId() == this->transactionId)
            order.Delete();
    }

    Package::Structure packet = this->serialize();
    this->db->Connect(absoluteDBPath)->Delete(packet);
    this->status = DELETED;
}

Package::Structure User::serialize() const {
    std::vector<std::string> payload;

    payload.push_back(this->login);
    payload.push_back(this->password);
    payload.push_back(this->firstname);
    payload.push_back(this->lastname);
    payload.push_back(Package::ConvertSizet(this->balance));
    payload.push_back((char *)(&this->role));

    return Package::Structure{this->transactionId, payload};
}

void User::deserialize(const Package::Structure &packet) {
    this->transactionId = packet.transaction;
    this->login = packet.payload[0];
    this->password = packet.payload[1];
    this->firstname = packet.payload[2];
    this->lastname = packet.payload[3];
    this->balance = std::stoull(packet.payload[4]);
    this->role = static_cast<User::Role>(packet.payload[5][0]);
}
