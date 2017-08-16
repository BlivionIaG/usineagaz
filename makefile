CFLAGS = -g -Wall -Wextra -Os -std=gnu11
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -lSDL_gfx -lm -lGL -lGLU

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
	rm obj/*.o *.bin *.gch -f

clear:
	rm obj/*.o *.gch -f
