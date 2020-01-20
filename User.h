#ifndef CARSHARING_USER_H
#define CARSHARING_USER_H

#include <string>
#include "Event.h"
#include "Package.h"
#include "Database.h"
#include "Order.h"

class User :
        public Event
{
    std::string login, password, firstname, lastname;
    size_t balance = 100;
    std::string databasePath = ":/database/users.db";
public:
    enum Role { ADMIN = 1, SIMPLEUSER = 2 } role { SIMPLEUSER };

    struct PacketFormat {
        std::string login, password, firstname, lastname;
        size_t balance;
        Role role;
    };

    explicit User(Database *db);
    explicit User(Database *db, const PacketFormat &packet);

    const std::string & getLogin() const {
        return this->login;
    }

    const std::string & getPassword() const {
        return this->password;
    }

    const std::string & getFirstname() const {
        return this->firstname;
    }

    const std::string & getLastname() const {
        return this->lastname;
    }

    const size_t & getBalance() const {
        return this->balance;
    }

    const User::Role & getRole() const {
        return this->role;
    }

    std::string getEventName() const override {
        return "User";
    }

    void Delete() override;

    Event * Clone() override {
        Event *clonned = new User(*this);
        clonned->setTransactionId(this->transactionId);
        return clonned;
    }

    Package::Structure serialize() const override;
    void deserialize(const Package::Structure &packet) override;
};


#endif //CARSHARING_USER_H
