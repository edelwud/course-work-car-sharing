#ifndef REGULATOR_MANAGER_H
#define REGULATOR_MANAGER_H

#include <iterator>
#include <initializer_list>

#include "Event.h"

// Итератор шаблонного контейнера Manager
template <class ValueType>
class ManagerIterator;

// Шаблонный контейнер
template <class T>
class Manager
{
protected:
    // Узел списка
    class Node {
        T value;
        Node *next = nullptr;
        Node *prev = nullptr;
    public:
        explicit Node(T value) {
            this->value = value;
        }
        T Get() const {             // Получение значения
            return this->value;
        }
        void SetValue(T value) {    // Установка значения
            this->value = value;
        }
        void SetNext(Node *node) {  // Установка последующего элемента
            this->next = node;
        }
        void SetPrev(Node *node) {  // Установка предыдущего элемента
            this->prev = node;
        }
        Node * GetNext() const {    // Получение следующего узла
            return this->next;
        }
        Node * GetPrev() const {    // Получение предыдущего узла
            return this->prev;
        }
    };
    int MAX_NODE_COUNT = 0;         // Максимальное количество сохраняемых узлов; если 0, то не ограничено
    int count = 0;                  // Текущее количество узлов
    Node *head = nullptr;           // Головной узел
    Node *tail = nullptr;           // Хвостовой узел
    Node *pseudo = new Node(T());   // Псевдоузел
public:
    Manager(int limit = 0) {
        MAX_NODE_COUNT = limit;
    }

    // Инициализация с помощью списка инициализации
    // Example: Manager manager({ 1, 2, 3 });
    Manager(std::initializer_list<T> list) {
        for (T &element : list) {
            this->Push(element);
        }
    }

    // Конструктор копирования
    Manager(const Manager* obj) {
        this->head = obj->head;
        this->tail = obj->tail;
        this->MAX_NODE_COUNT = obj->MAX_NODE_COUNT;
    }

    // Создание копии контейнера
    Manager * Clone() {
        return new Manager(*this);
    }

    // Освобождение занятой памяти
    virtual ~Manager() {
        while (this->head != this->tail) {
            Node *node = this->head;
            this->head = this->head->GetPrev();
            this->head->SetNext(nullptr);
            delete node;
        }
    }

    // Получение итератора на первый узел
    ManagerIterator<T> begin() {
        return ManagerIterator<T>(this->head);
    }

    // Получение итератора на нулевой итератор
    ManagerIterator<T> end() {
        return ManagerIterator<T>(new Node(T()));
    }

    // Получение значения головного узла
    T First() const {
        if (this->count == 0) {
            return T();
        }
        return this->head->Get();
    }

    // Получение текущего количества узлов
    int getCount() const {
        return this->count;
    }

    // Получени значения хвостового узла
    T Last() const {
        return this->tail->Get();
    }

    // Добавление узла в контейнер
    void Push(T element) {
        if (this->MAX_NODE_COUNT && this->count == this->MAX_NODE_COUNT) this->PopLast();
        if (this->head == this->tail && this->head == nullptr) {
            this->head = new Node(element);
            this->tail = this->head;
            this->tail->SetPrev(this->pseudo);
        } else if(this->head == this->tail && this->head != nullptr) {
            Node *node = new Node(element);
            this->head->SetNext(node);
            node->SetPrev(this->head);
            this->head = node;
            this->tail->SetNext(this->head);
        } else {
            Node *node = new Node(element);
            this->head->SetNext(node);
            node->SetPrev(this->head);
            this->head = node;
        }
        this->count++;
    }

    // Извлечение последнего добавленного узла
    T Pop() {
        if (this->count == 0)
            return nullptr;

        Node *node = this->head;

        this->count--;

        this->head = this->head->GetPrev();
        if (this->head) this->head->SetNext(nullptr);
        else this->tail = nullptr;

        return node->Get();
    }

    // Извлечение первого добавленного узла
    T PopLast() {
        if (this->count == 0)
            return T();

        Node *node = this->tail;

        this->count--;
        this->tail = this->tail->GetNext();
        this->tail->SetPrev(nullptr);

        return node->Get();
    }
};

// Итератор контейнера Manager
template<typename ValueType>
class ManagerIterator: public std::iterator<std::input_iterator_tag, ValueType>
{
    friend class Manager<ValueType>; // Устанавливаем дружественным классом класс контейнера
private:
    explicit ManagerIterator(typename Manager<ValueType>::Node *p);
public:
    ManagerIterator(const ManagerIterator &it);

    bool operator!=(ManagerIterator const& other) const; // Операции сравнения
    bool operator==(ManagerIterator const& other) const; //
    ValueType operator*() const;                         // Операция разадресации
    ManagerIterator& operator++();                       // Операция префиксной инкриментации
    ManagerIterator<ValueType> operator++(int);          // Операция постфиксной инкриментации
private:
    typename Manager<ValueType>::Node *node;             // Текущий узел
};

template<typename ValueType>
ManagerIterator<ValueType>::ManagerIterator(typename Manager<ValueType>::Node *p) :
        node(p) {}

template<typename ValueType>
ManagerIterator<ValueType>::ManagerIterator(const ManagerIterator& it) :
        node(it.node) {}

template<typename ValueType>
bool ManagerIterator<ValueType>::operator!=(ManagerIterator const& other) const
{
    if (!node || !other.node) return false;
    return node->Get() != other.node->Get();
}

template<typename ValueType>
bool ManagerIterator<ValueType>::operator==(ManagerIterator const& other) const
{
    return node->Get() == other.node->Get();
}

template<typename ValueType>
ValueType ManagerIterator<ValueType>::operator*() const
{
    ValueType value = node->Get();
    return value;
}

template<typename ValueType>
ManagerIterator<ValueType> &ManagerIterator<ValueType>::operator++()
{
    node = node->GetPrev();
    return *this;
}

template<typename ValueType>
ManagerIterator<ValueType> ManagerIterator<ValueType>::operator++(int)
{
    ManagerIterator<ValueType> copy = *this;
    node = node->GetPrev();
    return copy;
}

#endif //REGULATOR_MANAGER_H
