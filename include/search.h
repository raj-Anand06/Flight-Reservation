// search.h
#pragma once
#include <vector>
#include <string>

 struct Itinerary {

   std::vector<std::string> legs;   
   std::string stopover;            
   double totalPrice;
   int totalTimeMins;
 };

std::vector<Itinerary> findItinerariesDB(const std::string& src, const std::string& dst);
