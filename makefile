CXX= g++
CPPFLAGS= -std=c++2a -Wall



all: main
    $CXX $CPPFLAGS main.cpp -o main -I /opt/mvIMPACT_Acquire/ /opt/mvIMPACT_Acquire/lib/aarch64/libmvDeviceManager.so

.PHONEY: clean
clean:
    rm main
