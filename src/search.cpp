// search.cpp
#include "../include/search.h"
#include "../include/db.h"
#include <mariadb/mysql.h>

#include <iostream>

std::vector<Itinerary> findItinerariesDB(const std::string& src, const std::string& dst) {
    std::vector<Itinerary> result;
    MYSQL* conn = DB::instance().get();

    // 1. Direct flights
    std::string q1 = 
      "SELECT FlightNumber, Price, DurationMinutes "
      "FROM Airline WHERE Source = '" + src + "' AND Destination = '" + dst + "'";
    if (mysql_query(conn, q1.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            Itinerary it;
            it.legs.push_back(row[0]);
            it.stopover = "";                                       
            it.totalPrice = std::stod(row[1]);
            it.totalTimeMins = std::stoi(row[2]);
            result.push_back(it);
        }
        mysql_free_result(res);
    }

    // 2. One-stop flights
    std::string q2 =
        "SELECT A.FlightNumber, A.Destination, A.Price, A.DurationMinutes, "
        "       B.FlightNumber, B.Price, B.DurationMinutes "
        "FROM Airline A JOIN Airline B "
        "  ON A.Destination = B.Source "
        "WHERE A.Source = '" + src + "' AND B.Destination = '" + dst + "'";
    if (mysql_query(conn, q2.c_str()) == 0) {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res))) {
            Itinerary it;
            it.legs.push_back(row[0]);                              
            it.legs.push_back(row[4]);                             
            it.stopover = row[1];                                   
            it.totalPrice = std::stod(row[2]) + std::stod(row[5]);
            it.totalTimeMins = std::stoi(row[3]) + std::stoi(row[6]);
            result.push_back(it);
        }
        mysql_free_result(res);
    }

    return result;
}
