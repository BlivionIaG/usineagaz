CFLAGS = -g -Wall -Wextra -O3 -std=gnu11
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lGL -lGLU

SRC = $(wildcard src/*.c)
NOM = $(basename $(notdir $(SRC)))
OBJ = $(addprefix obj/, $(addsuffix .o, $(NOM)))

all: usine_a_gaz.bin

remake: clean usine_a_gaz.bin

usine_a_gaz.bin : $(OBJ)
	gcc obj/* $(LDFLAGS) -o $@

obj/%.o: src/%.c
	gcc -c $< -Iinclude -o $@ $(CFLAGS)

clean:
	rm obj/*.o *.out *.gch -f

clear:
	rm obj/*.o *.gch -f
