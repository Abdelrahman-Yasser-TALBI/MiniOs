# ----------------------------------------------------------------------
# Makefile pour MSYS2 / UCRT64 (Windows)
# ----------------------------------------------------------------------

# Le compilateur
CC = gcc

# ----------------------------------------------------------------------
# Configuration automatique via pkg-config (Standard MSYS2)
# ----------------------------------------------------------------------
# On demande à pkg-config de nous donner les bons flags pour SDL2 et SDL2_ttf
# Cela va chercher automatiquement dans C:/msys64/ucrt64/include/...
CFLAGS_SDL = $(shell pkg-config --cflags sdl2 SDL2_ttf)
LIBS_SDL   = $(shell pkg-config --libs sdl2 SDL2_ttf)

# Options de compilation générales
# -I. : cherche les .h dans le dossier courant
# -Dmain=SDL_main : nécessaire pour SDL2 sur Windows (MinGW)
CFLAGS = -Wall -Wextra -std=c99 -I. $(CFLAGS_SDL) -Dmain=SDL_main

# Options de l'éditeur de liens (Libraries)
# -lm : pour les maths (utilisé dans statistiques.c)
LIBS = $(LIBS_SDL) -lm

# Nom de l'exécutable final
TARGET = parking_gui.exe

# Les fichiers sources (On utilise gui_main.c au lieu de main.c)
SOURCES = gui_main.c gestion_voitures.c statistiques.c
OBJECTS = $(SOURCES:.c=.o)

# ----------------------------------------------------------------------
# Règles de construction
# ----------------------------------------------------------------------

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Liaison de l'executable..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo "[SUCCES] Compilation terminee ! Lance avec : make run"

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------
# Commandes utilitaires
# ----------------------------------------------------------------------

run: all
	@echo "Lancement du programme..."
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

info:
	@echo "Configuration detectee :"
	@echo "CFLAGS_SDL : $(CFLAGS_SDL)"
	@echo "LIBS_SDL   : $(LIBS_SDL)"