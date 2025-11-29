# 📚 Exemples d'utilisation de MiniOS

Ce document présente différents exemples d'utilisation de MiniOS avec différentes configurations.

## Exemple 1 : Ordonnancement FCFS (First Come First Served)

```bash
./bin/minios -a fcfs -n 5 -t 100
```

Cet exemple crée 5 processus et utilise l'ordonnancement FCFS. Les processus sont exécutés dans l'ordre de leur arrivée.

## Exemple 2 : Round Robin avec quantum personnalisé

```bash
./bin/minios -a rr -n 8 -q 10 -t 200
```

Cet exemple utilise Round Robin avec un quantum de 10 unités de temps. Chaque processus reçoit 10 unités avant d'être préempté.

## Exemple 3 : Ordonnancement par priorité

```bash
./bin/minios -a priority -n 10 -t 150
```

Cet exemple utilise l'ordonnancement par priorité. Les processus avec la priorité la plus élevée sont exécutés en premier.

## Exemple 4 : Simulation longue avec beaucoup de processus

```bash
./bin/minios -a rr -n 15 -q 5 -t 500
```

Cet exemple simule un système avec 15 processus sur une durée de 500 unités de temps.

## Visualisation des résultats

Après chaque exécution, générez les graphiques :

```bash
make visualize
```

Cela créera :
- `traces/gantt_chart.png` : Diagramme de Gantt montrant l'évolution des états
- `traces/statistics.png` : Graphiques de statistiques de performance

## Comparaison des algorithmes

Pour comparer les performances des différents algorithmes :

```bash
# Test FCFS
./bin/minios -a fcfs -n 5 -t 100 > results_fcfs.txt

# Test Round Robin
./bin/minios -a rr -n 5 -q 5 -t 100 > results_rr.txt

# Test Priority
./bin/minios -a priority -n 5 -t 100 > results_priority.txt
```

Ensuite, comparez les statistiques affichées dans chaque fichier de résultats.

