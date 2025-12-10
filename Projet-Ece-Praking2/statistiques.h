#ifndef STATISTIQUES_H
#define STATISTIQUES_H

#define FICHIER_DONNEES "parking.txt"

// Prototypes des fonctions - Étudiant 3
void afficherStatistiques();

// MODIFIÉ : Types de retour passés à double pour la précision
double calculerRevenuTotal(); 
double calculerDureeMoyenne();

int compterVoituresSorties();
void sauvegarderDonnees();
void chargerDonnees();


#endif // STATISTIQUES_H