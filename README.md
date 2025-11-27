# 🖥️ MiniOS - Simulation d'un Système d'Exploitation

MiniOS est une simulation complète d'un système d'exploitation en espace utilisateur, implémentée en C. Le projet simule la gestion de processus, l'ordonnancement, la mémoire, les I/O et la synchronisation.

## ✨ Fonctionnalités

- **Gestion des processus** : Création, suspension, terminaison avec PCB complet
- **Ordonnancement configurable** : FCFS, Round Robin, Priority Scheduling
- **Gestion mémoire** : Allocation/désallocation dynamique simulée
- **I/O simulées** : Opérations bloquantes avec gestion de files d'attente
- **Synchronisation** : Mutex et sémaphores à jetons
- **Visualisation graphique** : Graphiques Gantt et statistiques automatiques

## 🚀 Compilation et Exécution

### Prérequis
- GCC avec support C11
- Python 3 avec matplotlib et numpy
- Make

### Installation des dépendances Python
```bash
pip3 install matplotlib numpy
```

### Compilation
```bash
make
```

### Exécution
```bash
make run
# ou directement
./bin/minios
```

### Visualisation

**Option 1 : Rapport HTML interactif (recommandé)** ⭐
```bash
make html-report
# Puis ouvrez traces/minios_report.html dans votre navigateur
```

**Option 2 : Visualisation terminal**
```bash
python3 scripts/visualize_terminal.py
```

**Option 3 : Graphiques Python (nécessite matplotlib)**
```bash
make visualize
```

## 📁 Structure du Projet

```
minios/
├── src/
│   ├── main.c              # Point d'entrée principal
│   ├── pcb.c/h             # Gestion des Process Control Blocks
│   ├── scheduler.c/h       # Ordonnanceurs (FCFS, RR, Priority)
│   ├── memory.c/h          # Gestion mémoire simulée
│   ├── io.c/h              # Gestion des I/O simulées
│   ├── sync.c/h            # Mutex et sémaphores
│   ├── queue.c/h           # Files d'attente
│   └── trace.c/h           # Système de traces
├── scripts/
│   └── visualize.py        # Script de visualisation graphique
├── traces/                 # Fichiers de traces générés
├── Makefile
└── README.md
```

## 🎮 Utilisation

### Exemple de configuration

Le système accepte plusieurs paramètres en ligne de commande :

```bash
./bin/minios [options]
```

Options disponibles :
- `-a ALGO` : Algorithme d'ordonnancement (fcfs, rr, priority)
- `-n NUM` : Nombre de processus à créer
- `-q QUANTUM` : Quantum pour Round Robin (défaut: 5)
- `-t TIME` : Temps total de simulation (défaut: 100)

### Exemple
```bash
./bin/minios -a rr -n 5 -q 10 -t 200
```

## 📊 Visualisation

Les traces sont automatiquement générées dans le dossier `traces/`. Le script de visualisation produit :

- **Graphique Gantt** : Chronologie d'exécution des processus
- **Statistiques** : Temps de réponse, temps de retour, utilisation CPU
- **Graphiques d'état** : Évolution des états (READY, RUNNING, BLOCKED)

## 🔧 Architecture

### États des processus
- `NEW` : Processus créé mais pas encore prêt
- `READY` : Prêt à être exécuté
- `RUNNING` : En cours d'exécution
- `BLOCKED` : Bloqué (I/O ou synchronisation)
- `TERMINATED` : Terminé

### Politiques d'ordonnancement

MiniOS implémente **3 algorithmes d'ordonnancement** configurables :

#### 1. FCFS (First Come First Served) - Premier Arrivé, Premier Servi

**Principe :**
- Les processus sont exécutés dans l'ordre de leur arrivée
- File d'attente FIFO (First In, First Out)
- Pas de préemption : un processus s'exécute jusqu'à la fin ou jusqu'à ce qu'il se bloque

**Caractéristiques :**
- ✅ Simple à implémenter
- ✅ Pas de changement de contexte inutile
- ❌ Peut causer l'effet "convoy" (processus courts bloqués par des processus longs)
- ❌ Temps de réponse élevé pour les processus interactifs

**Utilisation :**
```bash
./bin/minios -a fcfs -n 5 -t 100
```

#### 2. Round Robin (RR) - Tourniquet

**Principe :**
- Chaque processus reçoit un **quantum** de temps CPU
- Quand le quantum expire, le processus est préempté et remis en fin de file
- Partage équitable du temps CPU entre tous les processus

**Caractéristiques :**
- ✅ Équitable : tous les processus reçoivent du temps CPU
- ✅ Bon temps de réponse pour les processus interactifs
- ✅ Évite la famine (starvation)
- ❌ Plus de changements de contexte (overhead)
- ❌ Performance dépend du choix du quantum

**Paramètres :**
- `-q QUANTUM` : Durée du quantum (défaut: 5)

**Utilisation :**
```bash
./bin/minios -a rr -n 8 -q 5 -t 100
```

**Fonctionnement :**
1. Processus exécute pendant `quantum` unités de temps
2. Si le processus n'est pas terminé, il est préempté
3. Le processus est remis en fin de file READY
4. Le prochain processus de la file prend le CPU

#### 3. Priority Scheduling - Ordonnancement par Priorité

**Principe :**
- Chaque processus a une **priorité** (1 = plus haute, 5 = plus basse)
- Les processus avec la priorité la plus élevée sont exécutés en premier
- File d'attente triée par priorité décroissante

**Caractéristiques :**
- ✅ Permet de donner la priorité aux processus importants
- ✅ Flexible : peut être adapté aux besoins
- ❌ Risque de famine pour les processus de faible priorité
- ❌ Peut être injuste si mal configuré

**Utilisation :**
```bash
./bin/minios -a priority -n 10 -t 100
```

**Fonctionnement :**
1. Les processus sont triés par priorité (décroissante)
2. Le processus avec la priorité la plus élevée est exécuté
3. Si deux processus ont la même priorité, FCFS s'applique

### Comparaison des Algorithmes

| Algorithme | Temps Réponse | Équité | Overhead | Complexité |
|------------|---------------|--------|----------|------------|
| **FCFS** | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **Round Robin** | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ |
| **Priority** | ⭐⭐⭐ | ⭐ | ⭐⭐⭐ | ⭐⭐ |

### 🎯 Choix de l'Algorithme

**Utilisez FCFS si :**
- Les processus ont des durées similaires
- Vous voulez minimiser les changements de contexte
- La simplicité est importante

**Utilisez Round Robin si :**
- Vous avez des processus interactifs
- Vous voulez un partage équitable du CPU
- Vous avez besoin d'un bon temps de réponse

**Utilisez Priority si :**
- Certains processus sont plus importants que d'autres
- Vous avez des processus temps réel
- Vous voulez un contrôle fin sur l'ordonnancement

## 📝 Auteurs

Projet réalisé dans le cadre d'un cours de systèmes d'exploitation.

## 📄 Licence

Ce projet est fourni à des fins éducatives.

