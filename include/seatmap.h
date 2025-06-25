#pragma once
#include <string>


void displaySeatMap(const std::string& flightNumber);
bool bookSeat(const std::string& flightNumber,
              const std::string& seatNo,
              int userId);


bool cancelBooking(const std::string& flightNumber,
                   const std::string& seatNo,
                   int userId);
