CC = cc

EXE = hammer
INC = -I/opt/local/include
LIBS_PATH = -L/opt/local/lib

LINK = -lraylib

FLAGS = -Wall -Wextra -std=c99 -pedantic -fsanitize=address
DFLAGS = -O0 -g
RFLAGS = -O2

SRC = hammer.c window.c config.c util.c processor.c model.c
OBJ = $(SRC:.c=.o)

all: $(EXE)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LINK) $(LIBS_PATH)

%.o: %.c
	$(CC) -c $< -o $@ $(INC)

clean:
	rm -f $(OBJS) $(TARGET)
