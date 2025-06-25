CXX      := g++
CXXFLAGS := -std=c++17 -Iinclude -I/mingw64/include
LDFLAGS := -L/mingw64/lib -lmariadb -lssl -lcrypto -lcurl



SRCS := src/db.cpp \
        src/utils.cpp \
        src/auth.cpp \
        src/flight.cpp \
        src/search.cpp \
        src/seatmap.cpp \
        main.cpp

OBJS := $(SRCS:.cpp=.o)
TARGET := flight_app.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o *.o $(TARGET)
