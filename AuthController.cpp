#include "AuthController.h"

AuthController::AuthController(Database *db) : db(db)
{
    this->controllerName = "AuthController";
}

User * AuthController::FindSession() {
    // Осуществляем поиск сессии в файле last.session
    std::fstream sessionStream("../sessions/last.session", std::ios::in | std::ios::binary);
    size_t transaction;
    // Считываем идентификатор
    if (sessionStream.read(reinterpret_cast<char *>(&transaction), sizeof(size_t))) {
        // Если находим возвращаем пользователя системы
        User *user = new User(db);
        user->deserialize(this->db->Connect("../database/users.db")->Find(transaction));
        sessionStream.close();
        return user;
    }
    sessionStream.close();
    return nullptr;
}

size_t AuthController::LoginExist(const std::string &login) {
    Package::Structure user = this->db->Connect("../database/users.db")->Find(0, login);
    if (!user.payload.empty())
        return user.transaction;

    return 0;
}

size_t AuthController::UserExist(const User::PacketFormat &source) {
    std::vector<Package::Structure> users = this->db->Connect("../database/users.db")->Read();
    for (Package::Structure user : users) {
        if (user.payload[0] == source.login && user.payload[1] == source.password) {
            return user.transaction;
        }
    }
    return 0;
}

void AuthController::Login(const User::PacketFormat &source) {
    size_t transaction = this->UserExist(source);
    if (transaction != 0) {
        // При совпадении пары логин-пароль, осуществляем запись идентификатора в файл сессий
        std::fstream sessionStream("../sessions/last.session", std::ios::out | std::ios::trunc | std::ios::binary);
        sessionStream.write(reinterpret_cast<char *>(&transaction), sizeof(size_t));
        sessionStream.close();
        return;
    }
    // Иначе вызываем ошибку
    this->SetMessage("Пользователь не найден в базе");
    throw this;
}

void AuthController::Register(const User::PacketFormat &source) {
    if (this->LoginExist(source.login) != 0) {
        this->SetMessage("Пользователь уже присутствует в базе");
        throw this;
    }
    User *newUser = new User(this->db, source);
    if (newUser->Commit()) {
        // При успешном сохранении, записываем идетификатор в файл сессий
        std::fstream sessionStream("../sessions/last.session", std::ios::out | std::ios::trunc | std::ios::binary);
        size_t transaction = newUser->getTransactionId();
        sessionStream.write(reinterpret_cast<char *>(&transaction), sizeof(size_t));
        sessionStream.close();
        return;
    }
    this->SetMessage("Произошла внутренняя ошибка");
    throw this;
}
