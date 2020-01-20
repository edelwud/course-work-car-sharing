#include "EventManager.h"

EventManager::EventManager() :
        Manager(5) {} // Глубина отмены событий = 5

EventManager::~EventManager() = default;

void EventManager::CommitEvent(Event* ev)
{
    ev->Commit();            // Добавляем в БД
    this->Push(ev->Clone()); // Добавляем копию события в начало списка
    this->AlertSubscribers(Event::Status::COMMITTED); // Оповещаем подпищиков события добавления в БД
}

void EventManager::DeleteEvent(Event* ev)
{
    ev->Delete();            // Удаление записи из БД
    this->Push(ev->Clone());
    this->AlertSubscribers(Event::Status::DELETED);
}

void EventManager::DeleteEventByTransaction(Event *ev) {
    Node *h = this->head;
    Node *t = this->tail;

    while (h != t->GetPrev()) {
        Node *node = h;

        // Удаляем указанные события из списка
        if (node->Get()->getTransactionId() == ev->getTransactionId()) {
            if (node == this->head && node == this->tail) {
                this->head->SetNext(nullptr);
                this->head->SetPrev(nullptr);
                this->tail = this->head;
                delete node;
            } else if (node == this->head) {
                this->head = node->GetPrev();
                this->head->SetNext(nullptr);
                delete node;
            } else if (node == this->tail) {
                this->tail = node->GetNext();
                this->tail->SetPrev(nullptr);
                delete node;
            } else {
                node->GetPrev()->SetNext(node->GetNext());
                node->GetNext()->SetPrev(node->GetPrev());
                delete node;
            }
            this->count--;
        }
        h = h->GetPrev();
    }
}

void EventManager::EditEvent(Event* dest, Event* source)
{
    this->Push(source->Clone()); // Добавляем изначальное значение
    source->Edit(dest);          // Изменяем
    this->count--;
    this->Push(source->Clone()); // Добавляем конечное значение

    this->AlertSubscribers(Event::Status::EDITED);
}

void EventManager::Cancel()
{
    Event *ev = this->Pop(); // Извлекаем последний добавленный в список элемент
    if (!ev) return;

    switch (ev->status) // Считываем его статус
    {
        case Event::Status::COMMITTED: // Удаляем если был добавлен
            ev->Delete();
            break;
        case Event::DELETED: // Восстанавливаем если был удален
            ev->Commit();
            break;
        case Event::Status::EDITED:
        {
            this->count++;
            Event* source = this->Pop(); // Извлекаем источник
            ev->Edit(source);            // Меняем текущее значение на исходное
            break;
        }
        case Event::SUPER:
        case Event::ADDED:
        case Event::CANCELLED:
            break;
    }

    this->AlertSubscribers(Event::Status::CANCELLED); // Оповещаем подпищиков
}

size_t EventManager::Subscribe(Event::Status status, const function<void(Event::Status)>& handler) {
    this->subscribers.push_back(Subscriber{ status, handler });
    return this->subscribers.size() - 1;
}

void EventManager::Unsubscribe(size_t index) {
    this->subscribers.erase(this->subscribers.begin() + index);
}

void EventManager::AlertSubscribers(Event::Status status) {
    for (Subscriber subscriber : this->subscribers) {
        // Оповещаем, если совпадают статусы, либо присутствует статус SUPER
        if (subscriber.status == Event::SUPER ||
            subscriber.status == status) subscriber.handler(status);
    }
}
