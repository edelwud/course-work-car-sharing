#include "Database.h"

Database::~Database() {
    // Закрытие файловых дескрипторов и освобождение памяти
    for (auto &connection : this->connections) {
        if (connection.second->is_open()) {
            connection.second->close();
        }
    }
    delete this->executor;
}

Database * Database::Connect(const std::string &filepath) {
    // Поиск существующего подключения
    if (this->Select(filepath)) {
        return this->Select(filepath);
    }
    // Создаем, если не удалось найти
    auto *connection = new std::fstream(filepath, std::ios::in | std::ios::app | std::ios::binary);
    this->connections.emplace(filepath, connection);
    this->path = filepath;
    this->executor = connection;
    return this;
}

Database * Database::Select(const std::string &filepath) {
    // Поиск открытого файлового потока
    if (this->connections.find(filepath) != this->connections.end()) {
        this->executor = this->connections.find(filepath)->second;
        this->path = filepath;
        return this;
    }
    return nullptr;
}

std::vector<Package::Structure> Database::Read() {
    std::vector<Package::Structure> collection;

    this->ResetStream();

    size_t length = 0,
            id = 0;

    while (!this->executor->eof()) {
        // Читаем идентификатор
        if (!this->executor->read(reinterpret_cast<char *>(&id), sizeof(size_t))) {
            break;
        }
        // Читаем длину пакета
        if (!this->executor->read(reinterpret_cast<char *>(&length), sizeof(size_t))) {
            break;
        }
        // Читаем пакет
        char* temp = new char[length + 1];
        if (!this->executor->read(temp, static_cast<std::streamsize>(length))) break;
        temp[length] = '\0';

        std::string buffer = temp;
        delete[] temp;

        // Добавляем в коллекцию
        collection.push_back(Package::Structure{ id, Package::Split(buffer) });
    }

    this->ResetStream();

    // Возвращаем коллекцию
    return collection;
}

void Database::Delete(Package::Structure &packet) {
    // Читаем с пакеты
    std::vector<Package::Structure> fmt = this->Read();

    // Закрываем текущий файловый поток
    this->executor->close();

    // Находим и удаляем заданный пакет
    for (size_t i = 0; i < fmt.size(); ++i) {
        if (fmt[i].transaction == packet.transaction)
            fmt.erase(fmt.begin() + i);
    }

    // Открываем файл с флагом "trunc"
    this->executor->open(path, std::ios::out | std::ios::trunc | std::ios::binary);
    // Записываем обновленную коллекцию
    for (Package::Structure &update : fmt) {
        this->Write(update);
    }
    this->executor->close();
    this->executor->open(path, std::ios::in | std::ios::app | std::ios::binary);
}

void Database::Replace(Package::Structure &before, Package::Structure &after) {
    std::vector<Package::Structure> fmt = this->Read();
    this->executor->close();

    // Заменяем один пакет на другой
    for (auto & i : fmt) {
        if (i.transaction == before.transaction) {
            i = after;
            break;
        }
    }

    this->executor->open(path, std::ios::out | std::ios::trunc | std::ios::binary);
    for (Package::Structure &packet : fmt) {
        this->Write(packet);
    }
    this->executor->close();
    this->executor->open(path, std::ios::in | std::ios::app | std::ios::binary);
}


void Database::Write(Package::Structure &packet) {
    // Конвертируем контейнер пакета в строку
    std::string collection = Package::Convert(packet.payload);
    // Запоминаем длину пакета
    size_t size = collection.size();

    this->executor->clear();
    // Записываем идентификатор
    this->executor->write(reinterpret_cast<char *>(&packet.transaction), sizeof(size_t));
    // Записываем длину пакета
    this->executor->write(reinterpret_cast<char *>(&size), sizeof(size_t));
    // Записываем пакет
    this->executor->write(reinterpret_cast<char *>(const_cast<char *>(collection.c_str())), size * sizeof(char));
    this->executor->clear();
    this->executor->flush();
}

Package::Structure Database::Find(size_t transaction) {
    std::vector<Package::Structure> fmt = this->Read();

    // Осуществляем поиск по идетификатору
    for (Package::Structure &package : fmt)
        if (package.transaction == transaction)
            return package;

    return Package::Structure{};
}

Package::Structure Database::Find(size_t index, const std::string &value) {
    std::vector<Package::Structure> fmt = this->Read();

    // Находим подходящий пакет сравнивая значения его столбца с заданным значением
    for (Package::Structure &package : fmt)
        if (package.payload[index] == value) {
            return package;
        }

    return Package::Structure{};
}

void Database::ResetStream() {
    this->executor->clear();
    this->executor->seekg(0);
}
