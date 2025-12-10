#include <stdio.h>
#include <string.h>
#include "gestion_voitures.h"

// Variables globales
Voiture parking[MAX_VOITURES];
int nbVoitures = 0;

// Constante de facturation (2.10 EUR par heure / 60 minutes)
#define PRIX_PAR_MINUTE (2.1 / 60.0)

// Prototypes locaux (Pour résoudre l'erreur "undefined reference" lors de la compilation)
int verifierExistence(const char* plaque);
int rechercherVoiture(const char* plaque);

/**
 * Verifie si une voiture existe deja dans le parking (non sortie)
 * Retourne 1 si existe, 0 sinon
 */
int verifierExistence(const char* plaque) {
    for (int i = 0; i < nbVoitures; i++) {
        // La verification reste la meme (basée sur la plaque et l'état de sortie)
        if (strcmp(parking[i].plaque, plaque) == 0 && parking[i].heureSortie == -1) {
            return 1;
        }
    }
    return 0;
}

/**
 * Recherche une voiture par sa plaque
 * Retourne l'index si trouvee, -1 sinon
 */
int rechercherVoiture(const char* plaque) {
    for (int i = 0; i < nbVoitures; i++) {
        // La recherche reste la meme
        if (strcmp(parking[i].plaque, plaque) == 0 && parking[i].heureSortie == -1) {
            return i;
        }
    }
    return -1;
}

/**
 * Ajoute une nouvelle voiture dans le parking
 * Retourne 1 si succes, 0 si echec
 */
int ajouterVoiture(const char* plaque, int heureEntree, int minuteEntree) {
    // Verification de la capacite
    if (nbVoitures >= MAX_VOITURES) {
        printf("x Parking complet !\n");
        return 0;
    }
    
    // Verification si la voiture est deja presente
    if (verifierExistence(plaque)) {
        printf("x Cette voiture est dejq dans le parking !\n");
        return 0;
    }
    
    // Validation de l'heure et des minutes
    if (heureEntree < 0 || heureEntree > 23 || minuteEntree < 0 || minuteEntree > 59) {
        printf("❌ Heure ou minute invalide (Heure 0-23, Minute 0-59) !\n");
        return 0;
    }
    
    // Ajout de la voiture
    strcpy(parking[nbVoitures].plaque, plaque);
    parking[nbVoitures].heureEntree = heureEntree;
    parking[nbVoitures].minuteEntree = minuteEntree; // NOUVEAU
    parking[nbVoitures].heureSortie = -1;
    parking[nbVoitures].minuteSortie = -1; // NOUVEAU
    parking[nbVoitures].montant = 0.0;
    nbVoitures++;
    
    printf("a Voiture %s enregistree a %dh %dmin\n", plaque, heureEntree, minuteEntree);
    return 1;
}

/**
 * Enregistre la sortie d'une voiture et calcule le montant
 * Retourne 1 si succes, 0 si echec
 */
int enregistrerSortie(const char* plaque, int heureSortie, int minuteSortie) {
    int index = rechercherVoiture(plaque);
    
    if (index == -1) {
        printf("x Voiture non trouvee dans le parking !\n");
        return 0;
    }
    
    // Validation de l'heure et des minutes
    if (heureSortie < 0 || heureSortie > 23 || minuteSortie < 0 || minuteSortie > 59) {
        printf("❌ Heure ou minute invalide (Heure 0-23, Minute 0-59) !\n");
        return 0;
    }

    // --- CALCUL DE LA DURÉE EN MINUTES (Précision) ---
    
    // 1. Conversion des temps en minutes totales depuis minuit
    int temps_entree_min = (parking[index].heureEntree * 60) + parking[index].minuteEntree;
    int temps_sortie_min = (heureSortie * 60) + minuteSortie;
    
    // 2. Calcul de la durée initiale
    int duree_minutes = temps_sortie_min - temps_entree_min;

    // 3. Gestion du passage à minuit (+24h)
    // S'il y a un débordement (sortie avant l'entrée dans les 24h)
    if (duree_minutes < 0) {
        duree_minutes += (24 * 60); 
    }
    
    // --- CALCUL DU MONTANT (Facturation à la minute) ---
    
    parking[index].heureSortie = heureSortie;
    parking[index].minuteSortie = minuteSortie;
    parking[index].montant = (double)duree_minutes * PRIX_PAR_MINUTE; // Calcul précis en double
    
    // --- AFFICHAGE ---
    
    // Calcul de la durée pour l'affichage (heures et minutes)
    int duree_h = duree_minutes / 60;
    int duree_m = duree_minutes % 60;
    
    printf("\n===============================\n");
    printf("        TICKET DE SORTIE\n");
    printf("===============================\n");
    printf("Plaque      : %s\n", parking[index].plaque);
    printf("Entree      : %dh %dmin\n", parking[index].heureEntree, parking[index].minuteEntree);
    printf("Sortie      : %dh %dmin\n", parking[index].heureSortie, parking[index].minuteSortie);
    printf("Duree       : %d h %d min\n", duree_h, duree_m);
    printf("Montant     : %.2f e\n", parking[index].montant); // AFFICHAGE À VIRGULE (centimes)
    printf("===============================\n\n");
    
    return 1;
}

/**
 * Affiche toutes les voitures actuellement presentes
 */
void afficherVoituresPresentes() {
    int compteur = 0;
    
    printf("\n===================================\n");
    printf("|   VOITURES DANS LE PARKING      |\n");
    printf("===================================\n\n");
    
    for (int i = 0; i < nbVoitures; i++) {
        if (parking[i].heureSortie == -1) {
            printf("  Plaque: %-10s | Entree: %2dh %2dmin\n", 
                        parking[i].plaque, 
                        parking[i].heureEntree,
                        parking[i].minuteEntree); // AFFICHAGE MINUTES
            compteur++;
        }
    }
    
    if (compteur == 0) {
        printf("   Aucune voiture dans le parking.\n");
    }
    
    printf("\n   Total: %d voiture(s) presente(s)\n\n", compteur);
}