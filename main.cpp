#include <bits/stdc++.h>
#include <algorithm>
#include <mysql/mysql.h>
#include "db.h"
#include <iomanip>

#include "auth.h"
#include "flight.h"
#include "search.h"
#include "seatmap.h"

int main() {
    int userId = 0;

    // Authentication loop
    while (true) {
        std::cout << "\n1) Signup\n2) Login\n3) Exit\n> ";
        int opt; std::cin >> opt;
        if (opt == 1) {
            std::string u, p;
            std::cout << "Username: "; std::cin >> u;
            std::cout << "Password: "; std::cin >> p;
            if (signup(u, p)) std::cout << "Signup successful!\n";
            else std::cout << "Signup failed (username taken?).\n";
        }
        else if (opt == 2) {
            std::string u, p;
            std::cout << "Username: "; std::cin >> u;
            std::cout << "Password: "; std::cin >> p;
            userId = login(u, p);
            if (userId > 0) { std::cout << "Login success.\n"; break; }
            std::cout << "Login failed.\n";
        }
        else return 0;
    }

    // Main menu
    while (true) {
        std::cout << "\n-- Main Menu --\n"
          << "1) Search by Flight#\n"
          << "2) Search by Route\n"
          << "3) Booking History\n"
          << "4) Logout\n"
          << "5) Cancel Ticket\n> ";

        int cmd; std::cin >> cmd;

        if (cmd == 1) {
            std::string fn;
            std::cout << "Enter Flight#: "; std::cin >> fn;
            try {
                auto info = getFlightInfo(fn);
                std::cout << "Flight: " << fn
                          << " | " << info.source << "→" << info.destination
                          << " | Price: " << info.price
                          << " | Duration: " << info.durationMins << "m\n";
                displaySeatMap(fn);
                std::cout << "Book a seat? (Y/N): ";
                char c; std::cin >> c;
                if (c=='Y'||c=='y') {
                    std::string seat;
                    std::cout << "Seat (e.g. A1): "; std::cin >> seat;
                    if (bookSeat(fn, seat, userId))
                        std::cout << "Booked " << seat << "!\n";
                    else
                        std::cout << "Seat unavailable.\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        }
        else if (cmd == 2) {
            std::string src, dst;
            std::cout << "Source: "; std::cin >> src;
            std::cout << "Destination: "; std::cin >> dst;
            auto itins = findItinerariesDB(src, dst);

            if (itins.empty()) {
                std::cout << "No routes found.\n";
                continue;
            }

            std::cout << "Sort by: 1) Price 2) Time > ";
            int sopt; std::cin >> sopt;
            if (sopt == 1) {
                std::sort(itins.begin(), itins.end(),
                        [](auto &a, auto &b){ return a.totalPrice < b.totalPrice; });
            } else {
                std::sort(itins.begin(), itins.end(),
                        [](auto &a, auto &b){ return a.totalTimeMins < b.totalTimeMins; });
            }

            // Aligned header
            std::cout << std::left
                    << std::setw(4)  << "No"
                    << std::setw(12) << "Flight1"
                    << std::setw(12) << "Flight2"
                    << std::setw(12) << "Stopover"
                    << std::setw(10) << "Price"
                    << std::setw(10) << "Duration"
                    << "\n";
            std::cout << std::string(60, '-') << "\n";

            // Display itineraries
            for (size_t i = 0; i < itins.size(); ++i) {
                auto &it = itins[i];
                std::cout << std::left
                        << std::setw(4)  << (i+1)
                        << std::setw(12) << it.legs[0]
                        << std::setw(12) << (it.legs.size() > 1 ? it.legs[1] : "-")
                        << std::setw(12) << (it.stopover.empty() ? "-" : it.stopover)
                        << std::setw(10) << it.totalPrice
                        << std::setw(10) << it.totalTimeMins << "m"
                        << "\n";
            }

            // Selection & booking
            std::cout << "Select itinerary (0 to cancel): ";
            int choice; std::cin >> choice;
            if (choice > 0 && choice <= (int)itins.size()) {
                auto &sel = itins[choice-1];
                displaySeatMap(sel.legs[0]);
                std::cout << "Book seat on " << sel.legs[0] << "? ";
                std::string seat; std::cin >> seat;
                if (bookSeat(sel.legs[0], seat, userId))
                    std::cout << "Booked!\n";
                else
                    std::cout << "Failed to book.\n";
            }
        }

        else if (cmd == 3) {
            // Display booking history
            MYSQL* conn = DB::instance().get();
            std::string q =
              "SELECT FlightNumber,SeatNo,booked_at FROM Bookings "
              "WHERE user_id = " + std::to_string(userId);
            mysql_query(conn, q.c_str());
            MYSQL_RES* res = mysql_store_result(conn);
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                std::cout << row[0] << " | " << row[1]
                          << " | " << row[2] << "\n";
            }
            mysql_free_result(res);
        }
        else if (cmd == 4) {
            std::cout << "Logging out...\n";
            break;
        }
         else if (cmd == 5) {
            // Cancel Ticket
            std::string fn, seat;
            std::cout << "Enter Flight#: "; std::cin >> fn;
            std::cout << "Enter Seat (e.g. A1): "; std::cin >> seat;
            if (cancelBooking(fn, seat, userId)) {
                std::cout << "Cancellation successful.\n";
            } else {
                std::cout << "Cancellation failed. Check flight/seat or booking history.\n";
            }
            // Show updated seat map
            displaySeatMap(fn);
        }
        else {
            std::cout << "Invalid command.\n";
            continue;
        }
    }

    return 0;
}
