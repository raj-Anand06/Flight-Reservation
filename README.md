# Flight Seat Reservation System

A terminal-based C++ application that lets users sign up, log in, search flights, book/cancel seats and view booking history—powered by MySQL and classic DSA for route finding (direct & one-stop).

---

## 🚀 Features

- **User Authentication**  
  - Signup & login with SHA-256–hashed passwords  
  - Session scoped to terminal run  

- **Flight Lookup**  
  - **By Flight Number**: view route, price, duration, seat map  
  - **By Route**: find cheapest or fastest itineraries from _SRC_ → _DST_ with at most one stop  
    - Displays a neatly aligned table with columns: No, Flight 1, Flight 2, Stopover, Price, Duration  

- **Seat Map & Booking**  
  - Realistic **2-3-2** layout (rows A–G, seats 1–7)  
  - “O” = available, “X” = booked  
  - Book a seat (transaction-safe in MySQL)  
  - Cancel a booking (frees up the seat)  

- **Booking History**  
  - View all past bookings with flight, seat & timestamp  

---

## 📦 Tech Stack

- **Language**: C++17 (GCC via MSYS2 MinGW64)  
- **Database**: MariaDB/MySQL C API  
- **Hashing**: OpenSSL SHA-256  
- **Build**: `make` + `g++`  
- **Data Structures & Algorithms**:  
  - SQL self-join for 1-stop graph traversal  
  - In-memory sorting via `std::sort`  

---

## 🔧 Prerequisites

- **MSYS2 MinGW64** environment on Windows  
- **MariaDB/MySQL** server running (service name e.g. `MySQL92`)  
- Packages:  
  ```bash
  pacman -S --needed base-devel mingw-w64-x86_64-toolchain \
                 mingw-w64-x86_64-libmariadbclient \
                 mingw-w64-x86_64-openssl
