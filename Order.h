#ifndef CARSHARING_ORDER_H
#define CARSHARING_ORDER_H

#include <QDate>

#include "Event.h"
#include "Database.h"
#include "User.h"
#include "Car.h"

class Order : public Event {
    size_t carId, userId;
    std::string orderDate, orderPassDate;
    std::string databasePath = ":/database/orders.db";
public:
    struct PacketFormat {
        size_t carId, userId;
        std::string orderDate, orderPassDate;
    };

    explicit Order(Database *db);
    explicit Order(Database *db, const PacketFormat &packet);

    const size_t & getCarId() const {
        return this->carId;
    }

    const size_t & getUserId() const {
        return this->userId;
    }

    const std::string & getOrderDate() const {
        return this->orderDate;
    }

    const std::string & getOrderPassDate() const {
        return this->orderPassDate;
    }

    std::string getEventName() const override {
        return "Order";
    }

    bool Commit() override;
    void Delete() override;

    Event * Clone() override {
        Event *clonned = new Order(*this);
        clonned->setTransactionId(this->transactionId);
        return clonned;
    }

    Package::Structure serialize() const override;
    void deserialize(const Package::Structure &packet) override;
};

#endif //CARSHARING_ORDER_H
