#pragma once
#include <string>

struct FlightInfo {
    std::string flightNumber;
    std::string source;
    std::string destination;
    std::string departureTime;   
    std::string arrivalTime;     
    double      price;
    int         durationMins;
    int         seatsTotal;
};

// Fetching a single flight’s details from the DB
// Throws runtime_error on failure or if not found
FlightInfo getFlightInfo(const std::string& flightNumber);
