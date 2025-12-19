CXX = g++
CXXFLAGS = -Iinclude -std=c++17 -Wall

# Вихідні файли
SRC = src/main.cpp src/Hub.cpp
# Цільовий файл
TARGET = server

# Правило збірки
all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Очистка
clean:
	rm -f $(TARGET)