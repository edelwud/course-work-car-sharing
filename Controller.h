#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Database.h"

using namespace std;

// Класс контроллера
class Controller {
protected:
    string controllerName = "Controller";
    string message = "";
public:
    Controller(string message = "") {
        this->message = message; // Сообщение контроллера
    }

    // Получение сообщения контроллера
    const string Message() const {
        return this->controllerName + ": " + this->message;
    }

    // Установка сообщение контроллера
    void SetMessage(string message) {
        this->message = message;
    }
};

#endif // CONTROLLER_H
