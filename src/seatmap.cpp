#include "../include/seatmap.h"
#include "../include/db.h"
#include <mysql/mysql.h>
#include <iostream>
#include <map>

/// Display a realistic 2-3-2 layout:  
///  A B   C D E   F G
void displaySeatMap(const std::string& flightNumber) {
    MYSQL* conn = DB::instance().get();
    std::string q =
      "SELECT SeatNo, isBooked FROM Seats "
      "WHERE FlightNumber = '" + flightNumber + "'";
    if (mysql_query(conn, q.c_str())) {
        std::cerr << "SeatMap query error: " << mysql_error(conn) << "\n";
        return;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    std::map<char, std::map<int, bool>> booked;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::string seat = row[0];
        bool isB = (row[1][0] == '1');
        char r = seat[0];
        int  c = std::stoi(seat.substr(1));
        booked[r][c] = isB;
    }
    mysql_free_result(res);

    // Layout: rows A–G, 7 columns, with aisle after 2 and 5
    const std::string ROWS = "ABCDEFG";
    const int COLS = 7;

    // Header
    std::cout << "    1 2   3 4 5   6 7\n";
    std::cout << "   -------------------\n";
    for (char r : ROWS) {
        std::cout << r << " |";
        for (int c = 1; c <= COLS; ++c) {
            bool isB = booked[r][c];
            std::cout << (isB ? "X" : "O");
            // spacing: after col 2 and 5, add extra gap
            if (c == 2 || c == 5) std::cout << " | ";
            else std::cout << " ";
        }
        std::cout << "\n";
    }
}

bool bookSeat(const std::string& flightNumber,
              const std::string& seatNo,
              int userId) {
    MYSQL* conn = DB::instance().get();

    // 1) Start a transaction
    if (mysql_query(conn, "START TRANSACTION")) {
        std::cerr << "Transaction start failed: " << mysql_error(conn) << "\n";
        return false;
    }

    // 2) Lock and check if already booked
    std::string q1 =
      "SELECT isBooked FROM Seats "
      "WHERE FlightNumber = '" + flightNumber +
      "' AND SeatNo = '" + seatNo + "' FOR UPDATE";
    if (mysql_query(conn, q1.c_str())) {
        std::cerr << "Lock query failed: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    MYSQL_RES* res1 = mysql_store_result(conn);
    if (!res1) {
        std::cerr << "Store result failed: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    MYSQL_ROW row1 = mysql_fetch_row(res1);
    bool isBooked = (row1 && row1[0] && row1[0][0] == '1');
    mysql_free_result(res1);

    if (isBooked) {
        // Seat already taken
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // 3) Mark the seat as booked
    std::string q2 =
      "UPDATE Seats SET isBooked = 1 "
      "WHERE FlightNumber = '" + flightNumber +
      "' AND SeatNo = '" + seatNo + "'";
    if (mysql_query(conn, q2.c_str())) {
        std::cerr << "Update seat failed: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // 4) Insert into booking history
    std::string q3 =
      "INSERT INTO Bookings (user_id, FlightNumber, SeatNo) VALUES ("
      + std::to_string(userId) + ", '"
      + flightNumber + "', '"
      + seatNo + "')";
    if (mysql_query(conn, q3.c_str())) {
        std::cerr << "Insert booking failed: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return false;
    }

    // 5) Commit transaction
    if (mysql_query(conn, "COMMIT")) {
        std::cerr << "Commit failed: " << mysql_error(conn) << "\n";
        return false;
    }

    return true;
}



/// Cancel a booked seat and remove history
bool cancelBooking(const std::string& flightNumber,
                   const std::string& seatNo,
                   int userId) {
    MYSQL* conn = DB::instance().get();
    
    std::string del = 
      "DELETE FROM Bookings WHERE user_id=" + std::to_string(userId) +
      " AND FlightNumber='" + flightNumber +
      "' AND SeatNo='" + seatNo + "'";
    if (mysql_query(conn, del.c_str())) {
        std::cerr << "Cancel delete error: " << mysql_error(conn) << "\n";
        return false;
    }
    
    std::string upd = 
      "UPDATE Seats SET isBooked=0 "
      "WHERE FlightNumber='" + flightNumber +
      "' AND SeatNo='" + seatNo + "'";
    if (mysql_query(conn, upd.c_str())) {
        std::cerr << "Cancel update error: " << mysql_error(conn) << "\n";
        return false;
    }
    return true;
}
