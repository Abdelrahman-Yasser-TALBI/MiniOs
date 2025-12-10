/**
 * PARKING INTELLIGENT
 * * Programme de gestion d'un parking avec:
 * - Enregistrement des entrées/sorties
 * - Calcul automatique des montants (2€/heure)
 * - Statistiques
 * - Sauvegarde persistante
 * * Auteurs: TALBI Abdelrahman Yasser, LAMIA Tadam, ILYAS Laarif
 * Date: [Date]
 */

#include <stdio.h>
#include <stdlib.h> // Ajouté pour la fonction system()
#include "gestion_voitures.h"
#include "menu_logique.h"
#include "statistiques.h"

/**
 * Efface l'écran du terminal pour un affichage propre.
 * Utilise 'cls' pour Windows et 'clear' pour Linux/macOS.
 */
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    clearScreen(); // Efface l'écran au démarrage

    printf("\n");
    printf("========================================\n");
    printf("|   Bienvenue au Parking Intelligent !   |\n");
    printf("========================================\n");
    printf("\n");
    
    // Lancement du menu principal
    executerMenu();
    
    return 0;
}