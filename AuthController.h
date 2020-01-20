#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <fstream>

#include "Controller.h"
#include "Database.h"
#include "User.h"

// Класс работы с аунтификацией
class AuthController :
        public Controller
{
    Database *db;
public:
    AuthController(Database *db);

    User * FindSession();                               // Поиск предыдущей сессии

    size_t UserExist(const User::PacketFormat &source); // Функция поиска пользователя по логину и паролю
    size_t LoginExist(const std::string &login);        // Функция поиска логина пользователя

    void Login(const User::PacketFormat &source);       // Функция авторизации
    void Register(const User::PacketFormat &source);    // Функция регистрации
};

#endif // AUTH_H
