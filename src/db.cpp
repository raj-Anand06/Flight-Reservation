#include "db.h"
#include <iostream>
#include <cstdlib>

static constexpr const char* HOST   = "localhost";
static constexpr const char* USER   = "root";
static constexpr const char* PASS   = "Anand101raj";
static constexpr const char* DBNAME = "flight_seat_reservation";

static constexpr unsigned      PORT   = 3306;

DB::DB() {
    conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "mysql_init() failed\n";
        std::exit(EXIT_FAILURE);
    }
    if (!mysql_real_connect(conn, HOST, USER, PASS, DBNAME, PORT, nullptr, 0)) {
        std::cerr << "mysql_real_connect() failed: "
                  << mysql_error(conn) << "\n";
        mysql_close(conn);
        std::exit(EXIT_FAILURE);
    }
}

DB::~DB() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
    }
}

DB& DB::instance() {
    static DB instance;
    return instance;
}

MYSQL* DB::get() {
    return conn;
}
