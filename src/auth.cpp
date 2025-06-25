#include "auth.h"
#include "db.h"
#include "utils.h"
#include <mysql/mysql.h>

#include <iostream>

bool signup(const std::string& username, const std::string& password) {
    MYSQL* conn = DB::instance().get();
    std::string pwd_hash = sha256(password);

    std::string query =
        "INSERT INTO Users (username, password_hash) VALUES ('" +
        username + "', '" + pwd_hash + "')";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Signup error: " << mysql_error(conn) << "\n";
        return false;
    }
    return true;
}

int login(const std::string& username, const std::string& password) {
    MYSQL* conn = DB::instance().get();
    std::string query =
        "SELECT id, password_hash FROM Users WHERE username = '" +
        username + "'";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "Login query error: " << mysql_error(conn) << "\n";
        return -1;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        std::cerr << "Login store_result error: " << mysql_error(conn) << "\n";
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        return -1; 
    }

    int user_id = std::stoi(row[0]);
    std::string stored_hash = row[1] ? row[1] : "";
    mysql_free_result(res);

    if (sha256(password) == stored_hash) {
        return user_id;
    }
    return -1;
}
