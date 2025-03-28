EXE = hammer

LINK = -lraylib

FLAGS = -Wall -Wunused -Wextra -std=c99 -pedantic
DFLAGS = -O0 -g -fsanitize=address,undefined
RFLAGS = -O2

SRC = impl.c

debug:
	$(CC) -I$(INC) -L$(LIBS_PATH) $(LINK) $(FLAGS) $(DFLAGS) $(SRC) -o $(EXE)

release:
	$(CC) -I$(INC) -L$(LIBS_PATH) $(LINK) $(FLAGS) $(RFLAGS) $(SRC) -o $(EXE)
