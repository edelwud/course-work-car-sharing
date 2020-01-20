#ifndef DATABASE_H
#define DATABASE_H

#include <fstream>
#include <vector>
#include <map>

#include "Package.h"

// Класс осуществляющий работу с файлом базы данных
class Database {
private:
    std::string path;                                  // Путь к файлу для чтения
    std::fstream *executor;                            // Текущий исполнитель
    std::map<std::string, std::fstream *> connections; // Файловые соединения

    void ResetStream();                                // Сброс файлового потока
public:
    virtual ~Database();

    Database * Connect(const std::string &filepath);    // Подключиться к базе данных
    Database * Select(const std::string &filepath);     // Выбрать базу данных из подключенных

    void Write(Package::Structure &packet);             // Запись сериализованного объекта в базу данных
    std::vector<Package::Structure> Read();             // Чтение из БД

    void Replace(Package::Structure &dest, Package::Structure &source); // Замена одной записи на другую
    void Delete(Package::Structure &packet);    // Удаление записи из БД
    Package::Structure Find(size_t transaction); // Поиск записи по её идентификатору
    Package::Structure Find(size_t index, const std::string &value); // Поиск записи по значению
};

#endif // DATABASE_H
