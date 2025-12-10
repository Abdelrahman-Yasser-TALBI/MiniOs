#ifndef GESTION_VOITURES_H
#define GESTION_VOITURES_H

#define MAX_VOITURES 100
#define TAILLE_PLAQUE 10

// Structure principale
typedef struct {
    char plaque[TAILLE_PLAQUE];
    int heureEntree;
    int minuteEntree;    // NOUVEAU
    int heureSortie;
    int minuteSortie;    // NOUVEAU
    double montant;      // MODIFIÉ : float -> double
} Voiture;

// Variables globales partagees
extern Voiture parking[MAX_VOITURES];
extern int nbVoitures;

// Prototypes des fonctions - Etudiant 1
int ajouterVoiture(const char* plaque, int heureEntree, int minuteEntree); // MODIFIÉ
int rechercherVoiture(const char* plaque);
int verifierExistence(const char* plaque);
int enregistrerSortie(const char* plaque, int heureSortie, int minuteSortie); // MODIFIÉ
void afficherVoituresPresentes();

#endif