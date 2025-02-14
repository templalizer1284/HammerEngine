CC = cc

EXE = hammer
INC = -I/opt/local/include
LIBS_PATH = -L/opt/local/lib

LINK = -lraylib

FLAGS = -Wall -Wextra -std=c99 -pedantic
DFLAGS = -O0 -g -fsanitize=address
RFLAGS = -O2

SRC = hammer.c window.c config.c processor.c model.c

debug:
	$(CC) $(INC) $(LIBS_PATH) $(LINK) $(FLAGS) $(DFLAGS) $(SRC) -o $(EXE)

release:
	$(CC) $(INC) $(LIBS_PATH) $(LINK) $(FLAGS) $(RFLAGS) $(SRC) -o $(EXE)
