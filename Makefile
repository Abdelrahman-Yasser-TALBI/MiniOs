CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lpthread -lm

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TRACEDIR = traces

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/minios

.PHONY: all clean directories run visualize

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJDIR) $(BINDIR) $(TRACEDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ MiniOS compilé avec succès!"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(TRACEDIR)/*.txt $(TRACEDIR)/*.png
	@echo "🧹 Nettoyage terminé"

run: $(TARGET)
	./$(TARGET)

visualize:
	cd $(shell pwd) && python3 scripts/visualize.py

html-report:
	python3 scripts/generate_html_report_simple.py

visualize-all: html-report
	@echo "✅ Rapports générés:"
	@echo "   - traces/minios_report.html (ouvrir dans le navigateur)"
	@echo "   - traces/minios_report.json"

help:
	@echo "📋 Commandes disponibles:"
	@echo "  make              - Compile le projet"
	@echo "  make run          - Compile et exécute MiniOS"
	@echo "  make visualize    - Génère les graphiques de visualisation"
	@echo "  make html-report  - Génère un rapport HTML interactif"
	@echo "  make visualize-all - Génère tous les rapports"
	@echo "  make clean        - Nettoie les fichiers générés"

