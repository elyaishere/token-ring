#pragma once
#include <string>

#define MESSAGE_SIZE 100

enum class DataType { kToken, kMessage };

class Data {
public:
    const DataType data_type;
    virtual ~Data() = default;
};

class Token : public Data {
public:
    Token();
};

class Message : public Data {
public:
    struct {
        int sender_id, reciever_id;
    } header;

    struct {
        unsigned total;
        unsigned id;
        unsigned size;
    } chunk;

    char data[MESSAGE_SIZE]{'\0'};
};
