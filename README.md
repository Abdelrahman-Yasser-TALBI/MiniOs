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

1. **FCFS (First Come First Served)** : Premier arrivé, premier servi
2. **Round Robin** : Partage du temps avec quantum fixe
3. **Priority** : Ordonnancement par priorité (plus haute priorité d'abord)

## 📝 Auteurs

Projet réalisé dans le cadre d'un cours de systèmes d'exploitation.

## 📄 Licence

Ce projet est fourni à des fins éducatives.

