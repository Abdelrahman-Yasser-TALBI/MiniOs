/**
 * gui_main.c
 * Interface Graphique pour le Parking Intelligent avec SDL2
 * Style : Standard / Propre
 * Fonctionnalité : Copier-Coller depuis la liste activé
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "gestion_voitures.h"
#include "statistiques.h"

// Dimensions de la fenêtre
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Couleurs (Style Standard)
SDL_Color WHITE = {255, 255, 255, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color GREY = {50, 50, 50, 255};
SDL_Color GREEN = {0, 200, 0, 255};
SDL_Color RED = {200, 0, 0, 255};
SDL_Color BLUE = {0, 0, 200, 255};
SDL_Color LIGHT_GREY = {200, 200, 200, 255}; // Pour la sélection

// Variables globales SDL
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
TTF_Font* fontSmall = NULL;

// Variables d'état pour les champs de texte
char inputPlaque[20] = "";
char inputHeure[5] = "";
char inputMinute[5] = "";
char statusMessage[256] = "Bienvenue. Cliquez sur une voiture pour copier sa plaque.";

int activeInput = 0; // 0: Aucun, 1: Plaque, 2: Heure, 3: Minute

// Initialisation SDL
int initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() == -1) {
        printf("Erreur TTF: %s\n", TTF_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Gestion Parking SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 0;
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // CHARGEMENT DE LA POLICE
    font = TTF_OpenFont("arial.ttf", 24); 
    fontSmall = TTF_OpenFont("arial.ttf", 16);
    
    if (!font) {
        printf("Erreur chargement police (arial.ttf manquant ?): %s\n", TTF_GetError());
        return 0;
    }
    return 1;
}

void closeSDL() {
    TTF_CloseFont(font);
    TTF_CloseFont(fontSmall);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Fonction utilitaire pour dessiner du texte
void drawText(const char* text, int x, int y, SDL_Color color, TTF_Font* f) {
    if (!text || strlen(text) == 0) return;
    SDL_Surface* surface = TTF_RenderText_Solid(f, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Fonction utilitaire pour dessiner un bouton ou champ
void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, w, h};
    if (filled) SDL_RenderFillRect(renderer, &rect);
    else SDL_RenderDrawRect(renderer, &rect);
}

// Vérifie si un clic est dans une zone
bool isInside(int x, int y, SDL_Rect rect) {
    return (x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h);
}

void renderUI() {
    // Fond sombre standard
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Titre
    drawText("PARKING INTELLIGENT - SDL2", 350, 20, WHITE, font);

    // --- ZONE DE GAUCHE : FORMULAIRES ---
    int startY = 100;
    int col1 = 50;
    
    drawText("Nouvelle Entree / Sortie", col1, startY, WHITE, font);
    
    // Champ Plaque
    drawText("Plaque :", col1, startY + 50, WHITE, fontSmall);
    SDL_Rect rectPlaque = {col1 + 100, startY + 45, 150, 30};
    drawRect(rectPlaque.x, rectPlaque.y, rectPlaque.w, rectPlaque.h, (activeInput == 1) ? GREEN : WHITE, false);
    drawText(inputPlaque, rectPlaque.x + 5, rectPlaque.y + 5, WHITE, fontSmall);

    // Champ Heure
    drawText("Heure :", col1, startY + 100, WHITE, fontSmall);
    SDL_Rect rectHeure = {col1 + 100, startY + 95, 50, 30};
    drawRect(rectHeure.x, rectHeure.y, rectHeure.w, rectHeure.h, (activeInput == 2) ? GREEN : WHITE, false);
    drawText(inputHeure, rectHeure.x + 5, rectHeure.y + 5, WHITE, fontSmall);

    // Champ Minute
    drawText("Min :", col1 + 160, startY + 100, WHITE, fontSmall);
    SDL_Rect rectMin = {col1 + 200, startY + 95, 50, 30};
    drawRect(rectMin.x, rectMin.y, rectMin.w, rectMin.h, (activeInput == 3) ? GREEN : WHITE, false);
    drawText(inputMinute, rectMin.x + 5, rectMin.y + 5, WHITE, fontSmall);

    // BOUTONS ACTIONS
    // Bouton ENTRER
    SDL_Rect btnEntree = {col1, startY + 160, 120, 40};
    drawRect(btnEntree.x, btnEntree.y, btnEntree.w, btnEntree.h, GREEN, true);
    drawText("ENTRER", btnEntree.x + 25, btnEntree.y + 10, BLACK, fontSmall);

    // Bouton SORTIR
    SDL_Rect btnSortie = {col1 + 140, startY + 160, 120, 40};
    drawRect(btnSortie.x, btnSortie.y, btnSortie.w, btnSortie.h, RED, true);
    drawText("SORTIR", btnSortie.x + 30, btnSortie.y + 10, BLACK, fontSmall);

    // Bouton SAUVEGARDER
    SDL_Rect btnSave = {col1, startY + 220, 260, 40};
    drawRect(btnSave.x, btnSave.y, btnSave.w, btnSave.h, BLUE, true);
    drawText("SAUVEGARDER DONNEES", btnSave.x + 40, btnSave.y + 10, WHITE, fontSmall);

    // --- ZONE DE DROITE : LISTE DES VOITURES ---
    int listX = 400;
    drawText("Voitures Presentes (Cliquez pour copier)", listX, startY, WHITE, font);
    
    // Ligne de séparation
    drawRect(listX, startY + 40, 580, 2, WHITE, true);
    
    // Affichage de la liste
    int yPos = startY + 50;
    int count = 0;
    for (int i = 0; i < nbVoitures; i++) {
        if (parking[i].heureSortie == -1) {
            char buffer[100];
            sprintf(buffer, "%s  -  Entree: %02dh%02d", parking[i].plaque, parking[i].heureEntree, parking[i].minuteEntree);
            
            // Petit fond gris derrière chaque ligne pour la lisibilité
            SDL_Rect lineRect = {listX, yPos, 400, 22};
            // Si on survole ou clique, on pourrait changer la couleur, mais restons simple
            
            drawText(buffer, listX, yPos, WHITE, fontSmall);
            yPos += 25;
            count++;
            if (count > 20) break; // Limite d'affichage
        }
    }
    
    // --- ZONE BAS : STATS & STATUS ---
    drawRect(0, 650, SCREEN_WIDTH, 2, GREY, true);
    
    // Message status
    drawText("INFO:", 20, 670, GREEN, fontSmall);
    drawText(statusMessage, 70, 670, WHITE, fontSmall);
    
    // Stats rapides
    char statsBuf[200];
    sprintf(statsBuf, "Revenu Total: %.2f EUR  |  Voitures: %d", calculerRevenuTotal(), nbVoitures);
    drawText(statsBuf, 500, 670, WHITE, fontSmall);

    SDL_RenderPresent(renderer);
}

// Gestion des entrées clavier
void handleTextInput(SDL_Event e) {
    char* target = NULL;
    size_t maxLen = 0;
    
    if (activeInput == 1) { target = inputPlaque; maxLen = 9; }
    else if (activeInput == 2) { target = inputHeure; maxLen = 2; }
    else if (activeInput == 3) { target = inputMinute; maxLen = 2; }
    
    if (target) {
        if (e.type == SDL_TEXTINPUT) {
            if (strlen(target) < maxLen) {
                strcat(target, e.text.text);
            }
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
            size_t len = strlen(target);
            if (len > 0) target[len - 1] = '\0';
        }
    }
}

int main(int argc, char* args[]) {
    (void)argc;
    (void)args;

    // 1. Initialiser le système
    if (!initSDL()) return 1;
    
    // 2. Charger les données existantes
    chargerDonnees();

    SDL_Event e;
    bool quit = false;
    
    SDL_StartTextInput();

    while (!quit) {
        // --- GESTION ÉVÉNEMENTS ---
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                
                // Définition des zones de clic
                SDL_Rect rPlaque = {150, 145, 150, 30};
                SDL_Rect rHeure = {150, 195, 50, 30};
                SDL_Rect rMin = {250, 195, 50, 30};
                SDL_Rect btnEntree = {50, 260, 120, 40};
                SDL_Rect btnSortie = {190, 260, 120, 40};
                SDL_Rect btnSave = {50, 320, 260, 40};

                // Sélection des champs
                if (isInside(x, y, rPlaque)) activeInput = 1;
                else if (isInside(x, y, rHeure)) activeInput = 2;
                else if (isInside(x, y, rMin)) activeInput = 3;
                else activeInput = 0;

                // --- NOUVEAU : CLIC SUR LA LISTE POUR COPIER ---
                // La liste commence à x=400, y=150 (startY+50), hauteur ligne=25
                if (x > 400 && y > 150) {
                    int clickedRow = (y - 150) / 25;
                    
                    // On cherche quelle voiture correspond à cette ligne visuelle
                    int currentVis = 0;
                    for (int i = 0; i < nbVoitures; i++) {
                        if (parking[i].heureSortie == -1) {
                            if (currentVis == clickedRow) {
                                // Trouvé ! On copie la plaque
                                strcpy(inputPlaque, parking[i].plaque);
                                sprintf(statusMessage, "Plaque %s copiee !", inputPlaque);
                                activeInput = 1; // On met le focus sur le champ plaque
                                break;
                            }
                            currentVis++;
                        }
                    }
                }

                // Clic Bouton ENTRÉE
                if (isInside(x, y, btnEntree)) {
                    int h = atoi(inputHeure);
                    int m = atoi(inputMinute);
                    if (strlen(inputPlaque) > 0) {
                        if (ajouterVoiture(inputPlaque, h, m)) {
                            strcpy(statusMessage, "Succes : Voiture entree.");
                            // Reset champs
                            inputPlaque[0] = '\0'; inputHeure[0] = '\0'; inputMinute[0] = '\0';
                        } else {
                            strcpy(statusMessage, "Erreur : Parking complet ou voiture existe deja ou heure invalide.");
                        }
                    }
                }

                // Clic Bouton SORTIE
                if (isInside(x, y, btnSortie)) {
                    int h = atoi(inputHeure);
                    int m = atoi(inputMinute);
                    if (strlen(inputPlaque) > 0) {
                        double ancien_revenu = calculerRevenuTotal();
                        if (enregistrerSortie(inputPlaque, h, m)) {
                            double cout = calculerRevenuTotal() - ancien_revenu;
                            sprintf(statusMessage, "Sortie OK. Montant a payer : %.2f EUR", cout);
                            // Reset champs
                            inputPlaque[0] = '\0'; inputHeure[0] = '\0'; inputMinute[0] = '\0';
                        } else {
                            strcpy(statusMessage, "Erreur : Voiture introuvable ou heure invalide.");
                        }
                    }
                }
                
                // Clic Sauvegarde
                if (isInside(x, y, btnSave)) {
                    sauvegarderDonnees();
                    strcpy(statusMessage, "Donnees sauvegardees dans le fichier.");
                }
            }
            // Gestion clavier
            else if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT) {
                handleTextInput(e);
            }
        }

        // --- DESSIN ---
        renderUI();
        SDL_Delay(16); // ~60 FPS
    }

    // Sauvegarde auto à la fermeture
    sauvegarderDonnees();
    
    SDL_StopTextInput();
    closeSDL();
    return 0;
}