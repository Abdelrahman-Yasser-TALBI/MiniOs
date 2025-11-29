# Makefile pour MiniOS (Compatible Windows/MinGW)

SHELL = cmd.exe
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lpthread -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TRACEDIR = traces

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/minios.exe # Exécutable pour Windows

.PHONY: all clean directories run visualize html-report visualize-all help

# ----------------------------------------------------------------------
# Règle par défaut (Compilation complète)
# ----------------------------------------------------------------------
all: directories $(TARGET)

# Règle pour créer les répertoires. Utilise 'mkdir' de Windows.
directories:
	@echo "📂 Creation des repertoires..."
	-mkdir $(OBJDIR)
	-mkdir $(BINDIR)
	-mkdir $(TRACEDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ MiniOS compile avec succes!"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------
# Nettoyage (Compatible Windows/MinGW via SHELL = cmd.exe)
# ----------------------------------------------------------------------
clean:
	@echo "🧹 Nettoyage des fichiers generes..."
	-DEL /Q $(TRACEDIR)\*.txt $(TRACEDIR)\*.png
	-DEL /Q $(TARGET) $(OBJECTS)
	-RMDIR /S /Q $(OBJDIR)
	-RMDIR /S /Q $(BINDIR)
	-RMDIR /S /Q $(TRACEDIR)

# ----------------------------------------------------------------------
# Commandes Utilitaires
# ----------------------------------------------------------------------
# Lancement du programme
run: $(TARGET)
	./$(TARGET)

visualize:
	cd $(shell pwd) && python3 scripts/visualize.py

html-report:
	python3 scripts/generate_html_report_simple.py

visualize-all: html-report
	@echo "✅ Rapports generes:"
	@echo " 	- traces/minios_report.html (ouvrir dans le navigateur)"
	@echo " 	- traces/minios_report.json"

help:
	@echo "📋 Commandes disponibles:"
	@echo " 	make              - Compile le projet"
	@echo " 	make run          - Compile et execute MiniOS"
	@echo " 	make visualize    - Genere les graphiques de visualisation"
	@echo " 	make html-report  - Genere un rapport HTML interactif"
	@echo " 	make visualize-all - Genere tous les rapports"
	@echo " 	make clean        - Nettoie les fichiers generes"