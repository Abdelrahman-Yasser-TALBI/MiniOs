# 🚗 Projet Ece Praking Intelligent

Application en C pour la gestion complète d'un parking avec statistiques et sauvegarde persistante.

## 📋 Fonctionnalités

- ✅ Enregistrement des entrées de véhicules
- ✅ Enregistrement des sorties avec calcul automatique du prix (2€/heure)
- ✅ Affichage des véhicules présents
- ✅ Historique complet des tickets
- ✅ Statistiques détaillées (revenus, durée moyenne, etc.)
- ✅ Sauvegarde automatique dans fichier texte

## 🏗️ Architecture du Projet

```
parking-intelligent/
│
├── main.c                   # Point d'entrée du programme
├── gestion_voitures.c/h     # Module 1 - Gestion des données
├── menu_logique.c/h         # Module 2 - Menu et logique
├── statistiques.c/h         # Module 3 - Stats et fichiers
├── Makefile                 # Compilation automatique
└── parking.txt              # Fichier de données (généré)
```

## 🚀 Installation et Compilation

### Prérequis
- Compilateur GCC
- Make (optionnel mais recommandé)

### Compilation avec Make
```bash
make
```

### Compilation manuelle
```bash
gcc -Wall -Wextra -std=c99 -o parking main.c gestion_voitures.c menu_logique.c statistiques.c
```

### Lancement
```bash
./parking
```

## 📖 Utilisation

### Menu Principal

```
╔════════════════════════════════════════╗
║     PARKING INTELLIGENT v1.0           ║
╚════════════════════════════════════════╝

  1. 🚗 Enregistrer une entrée
  2. 🚙 Enregistrer une sortie
  3. 📋 Voir les voitures présentes
  4. 📜 Afficher l'historique complet
  5. 📊 Afficher les statistiques
  6. 💾 Sauvegarder et quitter
```

### Exemples d'utilisation

#### Enregistrer une entrée
```
Plaque d'immatriculation : AB123CD
Heure d'entrée (0-23) : 14
✅ Voiture AB123CD enregistrée à 14h
```

#### Enregistrer une sortie
```
Plaque d'immatriculation : AB123CD
Heure de sortie (0-23) : 17

═══════════════════════════════
       TICKET DE SORTIE
═══════════════════════════════
Plaque      : AB123CD
Entrée      : 14h
Sortie      : 17h
Durée       : 3 heure(s)
Montant     : 6.00 €
═══════════════════════════════
```

## 📊 Structure de Données

### Structure Voiture
```c
typedef struct {
    char plaque[10];      // Plaque d'immatriculation
    int heureEntree;      // Heure d'entrée (0-23)
    int heureSortie;      // Heure de sortie (-1 si présent)
    float montant;        // Montant à payer
} Voiture;
```

### Format du fichier parking.txt
```
# PARKING INTELLIGENT - HISTORIQUE
# Format: PLAQUE HEURE_ENTREE HEURE_SORTIE MONTANT
4
AB123CD 14 17 6.00
XY789ZE 9 -1 0.00
CD456GH 10 15 10.00
EF789IJ 16 20 8.00
```

## 👥 Répartition du Travail

### TALBI Abdelrahman Yasser - Gestion des données (`gestion_voitures.c/h`)
- ✅ Structure de données `Voiture`
- ✅ Ajout de véhicules dans le tableau
- ✅ Recherche par plaque d'immatriculation
- ✅ Vérification d'existence
- ✅ Enregistrement des sorties
- ✅ Calcul des montants
- ✅ Affichage des véhicules présents

### TAMDA Lamiae - Menu et logique (`menu_logique.c/h`)
- ✅ Menu interactif avec interface utilisateur
- ✅ Gestion des entrées utilisateur
- ✅ Validation des données
- ✅ Traitement des entrées/sorties
- ✅ Affichage de l'historique
- ✅ Boucle principale du programme

### LAARIF Ilyas - Statistiques et fichiers (`statistiques.c/h`)
- ✅ Sauvegarde dans fichier texte
- ✅ Chargement au démarrage
- ✅ Calcul du revenu total
- ✅ Calcul de la durée moyenne
- ✅ Comptage des véhicules
- ✅ Affichage des statistiques

## 🧪 Tests Recommandés

1. **Test d'entrée basique**
   - Ajouter une voiture
   - Vérifier l'affichage

2. **Test de sortie**
   - Enregistrer une sortie
   - Vérifier le calcul du montant

3. **Test de persistance**
   - Ajouter des données
   - Quitter et relancer
   - Vérifier que les données sont conservées

4. **Test de cas limites**
   - Parking complet (100 véhicules)
   - Plaque déjà présente
   - Heures invalides
   - Passage minuit (ex: entrée 22h, sortie 2h)

5. **Test des statistiques**
   - Vérifier les calculs de revenu
   - Vérifier la durée moyenne

## 📐 Algorithmes Importants

### Calcul de la durée avec gestion du passage minuit
```c
int duree = heureSortie - heureEntree;
if (duree < 0) {
    duree += 24;  // Correction pour passage minuit
}
```

### Recherche d'une voiture
```c
for (int i = 0; i < nbVoitures; i++) {
    if (strcmp(parking[i].plaque, plaque) == 0 
        && parking[i].heureSortie == -1) {
        return i;  // Trouvée
    }
}
return -1;  // Non trouvée
```

### Calcul du revenu total
```c
float total = 0.0;
for (int i = 0; i < nbVoitures; i++) {
    if (parking[i].heureSortie != -1) {
        total += parking[i].montant;
    }
}
return total;
```

## 🔧 Améliorations Possibles

- [ ] Ajout de types de véhicules (voiture, moto, camion)
- [ ] Tarifs différenciés selon le type
- [ ] Gestion de plusieurs niveaux de parking
- [ ] Système de réservation
- [ ] Interface graphique (GTK+)
- [ ] Base de données SQLite
- [ ] Mode abonnement mensuel
- [ ] Export des statistiques en CSV

## 📝 Rapport de Projet

### Objectif
Créer un système de gestion de parking intelligent avec sauvegarde persistante.

### Technologies utilisées
- Langage C (C99)
- Fichiers texte pour la persistance
- Compilation avec GCC/Make

### Difficultés rencontrées
1. **Gestion du passage minuit** : Résolu avec modulo 24
2. **Validation des entrées** : Ajout de contrôles stricts
3. **Format du fichier** : Utilisation d'un format simple mais robuste

### Résultats
- ✅ Toutes les fonctionnalités implémentées
- ✅ Programme stable et testé
- ✅ Code modulaire et maintenable
- ✅ Documentation complète

## 📄 Licence

Projet académique - Libre d'utilisation pour l'apprentissage

## 👨‍💻 Auteurs

- **Étudiant 1** : TALBI Abdelrahman Yasser - Gestion des données
- **Étudiant 2** : TAMDA Lamiae - Menu et logique  
- **Étudiant 3** : LAARIF Ilyas - Statistiques et fichiers

---

**Date de réalisation** :  25/11/2025
**Cours** : Programmation e/n C  /
**Institution** : [ECE Campus de Lyon]
