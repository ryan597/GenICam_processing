CXX= g++
CPPFLAGS= -std=c++2a -Wall

INCLUDE= -I/opt/mvIMPACT_Acquire/ #-I/usr/local/include/opencv4/
LINKER_FLAGS= -L/opt/mvIMPACT_Acquire/lib/arm64/libmvDeviceManager.so

all: main
	${CXX} ${CPPFLAGS} ${INCLUDE} $(LINKER_FLAGS) main.cpp -o main


.PHONEY: clean
clean:
	rm main
