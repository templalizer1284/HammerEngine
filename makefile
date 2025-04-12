EXE = hammer

LINK = -lraylib

FLAGS = -Wall -Werror -Wunused -Wextra -std=c99 -pedantic
DFLAGS = -O0 -g -fsanitize=address,undefined
RFLAGS = -O2

STATIC_CHECK = cppcheck
STATIC_CHECK_FLAGS = --language=c --std=c99 --check-level=exhaustive --enable=all

HAMMER_SRC = hammer.h
SRC = impl.c

debug:
	$(STATIC_CHECK) $(STATIC_CHECK_FLAGS) $(HAMMER_SRC)
	$(CC) -I$(INC) -L$(LIBS_PATH) $(LINK) $(FLAGS) $(DFLAGS) $(SRC) -o $(EXE)

release:
	$(CC) -I$(INC) -L$(LIBS_PATH) $(LINK) $(FLAGS) $(RFLAGS) $(SRC) -o $(EXE)
