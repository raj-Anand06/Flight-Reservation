#include "flight.h"
#include "db.h"
#include <mysql/mysql.h>

#include <stdexcept>

FlightInfo getFlightInfo(const std::string& flightNumber) {
    MYSQL* conn = DB::instance().get();
    
    std::string q =
      "SELECT Source, Destination, DepartureTime, ArrivalTime, "
      "Price, DurationMinutes, SeatsTotal "
      "FROM Airline WHERE FlightNumber = '" + flightNumber + "'";
    if (mysql_query(conn, q.c_str())) {
        throw std::runtime_error("MySQL error: " + std::string(mysql_error(conn)));
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        throw std::runtime_error("MySQL store_result error");
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        mysql_free_result(res);
        throw std::runtime_error("Flight not found: " + flightNumber);
    }
    FlightInfo f;
    f.flightNumber   = flightNumber;
    f.source         = row[0] ? row[0] : "";
    f.destination    = row[1] ? row[1] : "";
    f.departureTime  = row[2] ? row[2] : "";
    f.arrivalTime    = row[3] ? row[3] : "";
    f.price          = row[4] ? std::stod(row[4]) : 0.0;
    f.durationMins   = row[5] ? std::stoi(row[5]) : 0;
    f.seatsTotal     = row[6] ? std::stoi(row[6]) : 0;
    mysql_free_result(res);
    return f;
}
