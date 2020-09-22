NAME = senselosc

SRC = main.cpp

SRCPRFX = $(addprefix src/, $(SRC))

OBJ = $(SRCPRFX:.cpp=.o)

OBJPRFX = build/

CC = gcc

CFLAGS = -c -std=c99 -Wall -Werror -O2

LDFLAGS = -lsensel -lpthread

all: cleanobj $(OBJ)
	mkdir -p $(OBJPRFX)/obj
	mv $(OBJ) $(OBJPRFX)/obj
	$(CC) $(addprefix $(OBJPRFX)/obj/, $(notdir $(OBJ))) -o $(addprefix $(OBJPRFX), $(NAME)) $(LDFLAGS)

clean: cleanobj
	rm -rf build/

cleanobj:
	rm -f src/*.o
