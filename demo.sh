#!/bin/bash

# Script de démonstration MiniOS
# Affiche plusieurs exemples visuels d'utilisation

echo "╔══════════════════════════════════════════════════════════════╗"
echo "║     🎬 DÉMONSTRATION VISUELLE DE MiniOS                     ║"
echo "╚══════════════════════════════════════════════════════════════╝"
echo ""

cd "$(dirname "$0")"

# Exemple 1: Round Robin avec quantum court
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📌 EXEMPLE 1: Round Robin avec quantum court (q=3)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
./bin/minios -a rr -n 6 -q 3 -t 50 2>/dev/null | tail -20
echo ""
echo "📊 Visualisation:"
python3 scripts/visualize_terminal.py 2>/dev/null
echo ""
read -p "Appuyez sur Entrée pour continuer..." dummy
echo ""

# Exemple 2: FCFS
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📌 EXEMPLE 2: First Come First Served (FCFS)"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
./bin/minios -a fcfs -n 5 -t 50 2>/dev/null | tail -20
echo ""
echo "📊 Visualisation:"
python3 scripts/visualize_terminal.py 2>/dev/null
echo ""
read -p "Appuyez sur Entrée pour continuer..." dummy
echo ""

# Exemple 3: Priority
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📌 EXEMPLE 3: Ordonnancement par Priorité"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
./bin/minios -a priority -n 6 -t 50 2>/dev/null | tail -20
echo ""
echo "📊 Visualisation:"
python3 scripts/visualize_terminal.py 2>/dev/null
echo ""

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ Démonstration terminée!"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "💡 Pour tester vous-même:"
echo "   ./bin/minios -a [fcfs|rr|priority] -n [nombre] -q [quantum] -t [temps]"
echo "   python3 scripts/visualize_terminal.py"
echo ""

