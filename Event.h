#ifndef REGULATOR_EVENT_H
#define REGULATOR_EVENT_H

#include <string>
#include "Package.h"
#include "Database.h"

// Класс событий
class Event :
        public Package {
protected:
    std::string absoluteDBPath; // Путь к файлу базе данных
    Database *db;               // Работа с БД
public:
    // Статус события
    enum Status { ADDED, EDITED, DELETED, COMMITTED, CANCELLED, SUPER } status{ ADDED };

    virtual ~Event() {}

    // Редактирование записи в БД с изменением статуса события
    virtual void Edit(Event *after) {
        after->setTransactionId(this->transactionId);
        Package::Structure packet = this->serialize();
        Package::Structure serialized = after->serialize();

        this->deserialize(after->serialize());
        this->db->Connect(absoluteDBPath)->Replace(packet, serialized);

        this->status = EDITED;
    }

    // Удаление записи в БД с изменением статуса события
    virtual void Delete() {
        Package::Structure packet = this->serialize();
        this->db->Connect(absoluteDBPath)->Delete(packet);
        this->status = DELETED;
    }

    // Сохранение записи в БД с изменением статуса события
    virtual bool Commit() {
        std::vector<Package::Structure> collection = this->db->Connect(absoluteDBPath)->Read();
        // Читаем последний элемент, если он существует находим его идетификатор и добавляем единицу
        if (collection.size()) this->transactionId = collection[collection.size() - 1].transaction + 1;
        // Если он не существует, присваеваем идетификатору единицу
        else this->transactionId = 1;

        Package::Structure packet = this->serialize();
        this->db->Write(packet);
        this->status = COMMITTED;
        return true;
    }

    // Имя события
    virtual std::string getEventName() const = 0;

    // Создание копии события
    virtual Event * Clone() = 0;
};

#endif //REGULATOR_EVENT_H
