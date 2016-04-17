# Makefile for Ascii Art Project

SRC = ascii.cpp
BIN = ascii

IMGLIB = imageLib
ARCH := $(shell arch)

DBG = -g
CC = g++
CPPFLAGS = -O2 -W -Wall -I $(IMGLIB)
LDLIBS = -L$(IMGLIB) -lImg.$(ARCH)$(DBG) -lpng -lz
OBJ = $(SRC:.cpp=.o)

all: $(BIN)

ascii: ascii.o
	$(CC) -o ascii ascii.o $(LDLIBS)

clean: 
	rm -f $(BIN) $(OBJ)
