#pragma once
#include <string>

// Returns true on successful signup, false on error (e.g., username taken)
bool signup(const std::string& username, const std::string& password);

// Returns user ID on successful login, -1 on failure (e.g., wrong credentials)
int login(const std::string& username, const std::string& password);
