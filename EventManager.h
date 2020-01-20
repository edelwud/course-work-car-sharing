#ifndef REGULATOR_EVENTMANAGER_H
#define REGULATOR_EVENTMANAGER_H

#include <functional>

#include "Manager.h"
#include "Event.h"

using namespace std;

// Класс работы с событиями
class EventManager :
        public Manager<Event *>
{
private:
    // Структура подпищика на события
    struct Subscriber {
        Event::Status status;                   // Статус события
        function<void(Event::Status)> handler;  // Обработчик события
        bool operator==(const Subscriber&)
        {
            return this->status == Event::Status::ADDED;
        }
        bool operator!=(const Subscriber&)
        {
            return this->status != Event::Status::ADDED;
        }
    };
    vector<Subscriber> subscribers;       // Список подпищиков
    void AlertSubscribers(Event::Status); // Приватная функция оповещения подпищиков
public:
    explicit EventManager();
    virtual ~EventManager();

    // Функция подписки на определенное событие, возвращает идентификатор подпищика
    size_t Subscribe(Event::Status, const function<void(Event::Status)>& handler);
    // Функция отписки от события, принимает идентификатор подпищика
    void Unsubscribe(size_t index);

    void Cancel();                          // Отмена последнего события
    void DeleteEventByTransaction(Event *); // Удаление событий под идентификатору записи
    void DeleteEvent(Event *);              // Функция удаление события, переданного аргументом
    void EditEvent(Event *, Event *);       // Изменение события второго аргумента на событие первого аргумента
    void CommitEvent(Event *);              // Добавление события в БД
};


#endif //REGULATOR_EVENTMANAGER_H
