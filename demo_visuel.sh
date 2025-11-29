#!/bin/bash

# Script de démonstration visuelle complète de MiniOS
# Génère un rapport HTML interactif avec visualisations

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║     🎬 DÉMONSTRATION VISUELLE COMPLÈTE - MiniOS             ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

cd "$(dirname "$0")"

# Vérifier que le projet est compilé
if [ ! -f "./bin/minios" ]; then
    echo "🔨 Compilation du projet..."
    make clean > /dev/null 2>&1
    make > /dev/null 2>&1
    echo "✅ Compilation terminée"
    echo ""
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📌 Simulation avec Round Robin"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "⚙️  Configuration:"
echo "   - Algorithme: Round Robin"
echo "   - Processus: 8"
echo "   - Quantum: 4"
echo "   - Temps: 80"
echo ""

# Exécuter la simulation
./bin/minios -a rr -n 8 -q 4 -t 80 2>/dev/null | grep -A 20 "Configuration:"

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "🎨 Génération du rapport HTML interactif..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Générer le rapport HTML
python3 scripts/generate_html_report.py

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ DÉMONSTRATION TERMINÉE !"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "📁 Fichiers générés:"
echo "   ✨ traces/minios_report.html  (Rapport HTML interactif)"
echo "   📊 traces/minios_report.json  (Données JSON)"
echo "   📝 traces/minios_trace.txt     (Trace brute)"
echo ""
echo "🌐 Pour visualiser:"
echo "   Ouvrez traces/minios_report.html dans votre navigateur"
echo ""
echo "   Sur macOS:"
echo "   open traces/minios_report.html"
echo ""
echo "   Sur Linux:"
echo "   xdg-open traces/minios_report.html"
echo ""
echo "   Ou simplement double-cliquez sur le fichier !"
echo ""

