#ifndef CARSHARING_PACKAGE_H
#define CARSHARING_PACKAGE_H

#include <vector>
#include <numeric>
#include <string>
#include <sstream>


class Package {
protected:
    size_t transactionId = 1;
    const char delimiter = '$';
public:
    struct Structure {
        size_t transaction;
        std::vector<std::string> payload;
    };

    size_t getTransactionId() const {
        return this->transactionId;
    }

    void setTransactionId(size_t transaction) {
        this->transactionId = transaction;
    }

    static std::string Convert(std::vector<std::string> packet) {
        return std::accumulate(std::next(packet.begin()), packet.end(), packet[0], [](std::string sum, std::string part) {
            return std::move(sum) + '$' + std::move(part);
        }) + '$';
    }

    static std::string ConvertSizet(size_t number) {
        std::stringstream sstream;
        sstream << number;
        return sstream.str();
    }

    static std::vector<std::string> Split(const std::string &packet) {
        std::vector<std::string> collection;

        collection.emplace_back("");

        for (size_t i = 0, position = 0; i < packet.size(); i++) {
            if (packet[i] == '$') {
                collection.emplace_back("");
                position++; continue;
            }
            collection[position] += packet[i];
        }

        return collection;
    }
    virtual Structure serialize() const = 0;
    virtual void deserialize(const Structure &) = 0;
};

#endif //CARSHARING_PACKAGE_H
