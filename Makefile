CC = gcc
RM = rm -f
# déclaration des options du compilateur
CFLAGS = -Wall -O3 -I. $(shell sdl2-config --cflags)
CPPFLAGS = 
LDFLAGS = -lm -lGL4Dummies $(shell sdl2-config --libs) -lSDL2_image
# définition des fichiers et dossiers
PROGNAME = som
VERSION = 1.0
distdir = $(PROGNAME)-$(VERSION)
HEADERS = 
SOURCES = param.c mjson.c func.c dataset.c som.c sfam.c algorithms.c main.c
OBJ = $(SOURCES:.c=.o)
all: $(PROGNAME)
$(PROGNAME): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(PROGNAME) 
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ 
clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~
