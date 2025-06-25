#pragma once

#include <mysql/mysql.h>


class DB {
public:
    // returns the singleton instance
    static DB& instance();

    // get raw MYSQL* connection
    MYSQL* get();

private:
    DB();                    // private ctor
    ~DB();                   // private dtor

    DB(const DB&) = delete;
    DB& operator=(const DB&) = delete;

    MYSQL* conn;
};
