#include <stdio.h>
#include "statistiques.h"
#include "gestion_voitures.h"
#include <math.h> // Utile pour les calculs de float/double

/**
 * Compte le nombre de voitures qui sont sorties
 */
int compterVoituresSorties() {
    int compteur = 0;
    for (int i = 0; i < nbVoitures; i++) {
        // La verification reste la meme (basée sur la plaque et l'état de sortie)
        if (parking[i].heureSortie != -1) {
            compteur++;
        }
    }
    return compteur;
}

/**
 * Calcule le revenu total du parking
 * MODIFIÉ : Type de retour float -> double
 */
double calculerRevenuTotal() {
    double total = 0.0;
    for (int i = 0; i < nbVoitures; i++) {
        if (parking[i].heureSortie != -1) {
            total += parking[i].montant;
        }
    }
    return total;
}

/**
 * Calcule la duree moyenne de stationnement
 * MODIFIÉ : Type de retour float -> double
 */
double calculerDureeMoyenne() {
    int totalDureeMinutes = 0; // Calculer la durée totale en minutes
    int nbSorties = 0;
    
    for (int i = 0; i < nbVoitures; i++) {
        if (parking[i].heureSortie != -1) {
            // Conversion en minutes pour le calcul précis de la durée
            int temps_entree_min = (parking[i].heureEntree * 60) + parking[i].minuteEntree;
            int temps_sortie_min = (parking[i].heureSortie * 60) + parking[i].minuteSortie;
            
            int duree_minutes = temps_sortie_min - temps_entree_min;
            if (duree_minutes < 0) {
                duree_minutes += (24 * 60); 
            }

            totalDureeMinutes += duree_minutes;
            nbSorties++;
        }
    }
    
    // Retourne la moyenne en heures
    return nbSorties > 0 ? (double)totalDureeMinutes / (nbSorties * 60.0) : 0.0;
}

/**
 * Affiche les statistiques du parking
 */
void afficherStatistiques() {
    int nbSorties = compterVoituresSorties();
    int nbPresentes = nbVoitures - nbSorties;
    
    // MODIFIÉ : Utilisation de double pour les variables de revenu et durée
    double revenu = calculerRevenuTotal();
    double dureeMoyenne = calculerDureeMoyenne();
    
    printf("\n=======================================\n");
    printf("|          STATISTIQUES               |\n");
    printf("=======================================\n\n");
    
    printf("M Nombre total de voitures accueillies : %d\n", nbVoitures);
    printf("v Voitures actuellement presentes      : %d\n", nbPresentes);
    printf("a Voitures sorties                     : %d\n", nbSorties);
    printf("e Revenu total genere                  : %.2f e\n", revenu);
    
    if (nbSorties > 0) {
        printf("t Duree moyenne de stationnement       : %.2f heure(s)\n", dureeMoyenne);
    } else {
        printf("t Duree moyenne de stationnement       : N/A\n");
    }
    
    printf("\n");
}

/**
 * Sauvegarde les donnees dans un fichier texte
 */
void sauvegarderDonnees() {
    FILE* fichier = fopen(FICHIER_DONNEES, "w");
    
    if (fichier == NULL) {
        printf("x Erreur lors de l'ouverture du fichier de sauvegarde !\n");
        return;
    }
    
    // En-tete du fichier (MODIFIÉ pour inclure les minutes)
    fprintf(fichier, "# PARKING INTELLIGENT - HISTORIQUE\n");
    fprintf(fichier, "# Format: PLAQUE HEURE_ENTREE MIN_ENTREE HEURE_SORTIE MIN_SORTIE MONTANT\n");
    fprintf(fichier, "# HEURE_SORTIE = -1 si voiture encore presente\n");
    fprintf(fichier, "#\n");
    fprintf(fichier, "%d\n", nbVoitures);
    
    // Sauvegarde de chaque voiture (MODIFIÉ pour inclure les minutes)
    for (int i = 0; i < nbVoitures; i++) {
        fprintf(fichier, "%s %d %d %d %d %.2f\n",
                parking[i].plaque,
                parking[i].heureEntree,
                parking[i].minuteEntree,
                parking[i].heureSortie,
                parking[i].minuteSortie,
                parking[i].montant); // Écrit le double
    }
    
    fclose(fichier);
    printf("S Donnees sauvegardees dans %s\n", FICHIER_DONNEES);
}

/**
 * Charge les donnees depuis le fichier texte
 */
void chargerDonnees() {
    FILE* fichier = fopen(FICHIER_DONNEES, "r");
    
    if (fichier == NULL) {
        printf("i Aucun fichier de donnees trouve. Demarrage avec un parking vide.\n");
        return;
    }
    
    char ligne[100];
    
    // Ignorer les lignes de commentaire
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (ligne[0] != '#') {
            break;
        }
    }
    
    // Lire le nombre de voitures
    sscanf(ligne, "%d", &nbVoitures);
    
    // Lire chaque voiture (MODIFIÉ : Ajout des minutes, et %lf pour lire un double)
    for (int i = 0; i < nbVoitures; i++) {
        fscanf(fichier, "%s %d %d %d %d %lf", 
                parking[i].plaque,
                &parking[i].heureEntree,
                &parking[i].minuteEntree, 
                &parking[i].heureSortie,
                &parking[i].minuteSortie, 
                &parking[i].montant); // %lf pour lire le double
    }
    
    fclose(fichier);
    printf("a Donnees chargees : %d enregistrement(s)\n", nbVoitures);
}