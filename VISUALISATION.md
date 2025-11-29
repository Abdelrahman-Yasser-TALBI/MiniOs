# 🎨 Guide de Visualisation - MiniOS

MiniOS offre plusieurs méthodes de visualisation pour analyser les résultats de simulation.

## 📊 Méthodes de Visualisation

### 1. Visualisation Terminal (Simple)

Affiche un diagramme de Gantt directement dans le terminal :

```bash
./bin/minios -a rr -n 5 -q 4 -t 50
python3 scripts/visualize_terminal.py
```

**Avantages :**
- Rapide et simple
- Pas de dépendances externes
- Idéal pour un aperçu rapide

### 2. Rapport HTML Interactif (Recommandé) ⭐

Génère un rapport HTML complet avec graphiques interactifs :

```bash
./bin/minios -a rr -n 8 -q 4 -t 70
make html-report
# ou directement
python3 scripts/generate_html_report.py
```

Puis ouvrez `traces/minios_report.html` dans votre navigateur.

**Fonctionnalités :**
- 📈 Diagramme de Gantt interactif
- 📊 Graphiques Chart.js (temps de retour, réponse, attente)
- 📋 Tableau détaillé des statistiques
- 📁 Export JSON des données
- 🎨 Interface moderne et responsive

**Avantages :**
- Visualisation professionnelle
- Graphiques interactifs
- Exportable et partageable
- Données JSON réutilisables

### 3. Graphiques Python (Si matplotlib installé)

Génère des graphiques PNG avec matplotlib :

```bash
pip3 install matplotlib numpy
make visualize
```

## 📁 Fichiers Générés

Après l'exécution, vous trouverez dans `traces/` :

- `minios_trace.txt` : Fichier de trace brut
- `minios_report.json` : Données structurées en JSON
- `minios_report.html` : Rapport HTML interactif
- `gantt_chart.png` : Graphique Gantt (si matplotlib disponible)
- `statistics.png` : Graphiques de statistiques (si matplotlib disponible)

## 🚀 Exemple Complet

```bash
# 1. Exécuter une simulation
./bin/minios -a priority -n 10 -q 5 -t 100

# 2. Générer le rapport HTML
make html-report

# 3. Ouvrir dans le navigateur
open traces/minios_report.html  # macOS
# ou
xdg-open traces/minios_report.html  # Linux
```

## 📊 Structure du JSON

Le fichier JSON contient :

```json
{
  "metadata": {
    "total_events": 150,
    "max_time": 100,
    "process_count": 10
  },
  "events": [...],
  "gantt": {
    "1": [{"start": 0, "end": 10, "state": "RUNNING"}, ...],
    ...
  },
  "statistics": {
    "1": {
      "pid": 1,
      "arrival": 0,
      "start": 0,
      "finish": 25,
      "turnaround": 25,
      "response": 0,
      "wait_time": 5
    },
    ...
  }
}
```

## 💡 Conseils

1. **Pour une meilleure visualisation** : Utilisez plus de processus (8-15) et un temps plus long (70-100)
2. **Comparer les algorithmes** : Exécutez plusieurs simulations et comparez les rapports HTML
3. **Partager les résultats** : Le fichier HTML est autonome et peut être partagé facilement

## 🔧 Personnalisation

Le script `generate_html_report.py` peut être modifié pour :
- Ajouter de nouveaux graphiques
- Changer les couleurs
- Ajouter des métriques personnalisées
- Exporter dans d'autres formats

