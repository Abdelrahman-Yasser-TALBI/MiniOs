#!/usr/bin/env python3
"""
Script de visualisation pour MiniOS
Génère des graphiques Gantt et des statistiques à partir des traces
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import re
import os
import sys
from collections import defaultdict

# Configuration des couleurs
COLORS = {
    'READY': '#FFD700',      # Or
    'RUNNING': '#32CD32',    # Vert
    'BLOCKED': '#FF6347',    # Rouge
    'TERMINATED': '#808080'  # Gris
}

def parse_trace_file(filename):
    """Parse le fichier de trace et extrait les événements"""
    events = []
    
    if not os.path.exists(filename):
        print(f"❌ Erreur: Fichier {filename} introuvable")
        return events
    
    with open(filename, 'r') as f:
        for line in f:
            # Ignorer les lignes de commentaire et d'en-tête
            if line.startswith('===') or line.startswith('Format') or \
               line.startswith('Total') or not line.strip():
                continue
            
            # Parser la ligne: Time | PID | Event | State | Remaining | Wait
            parts = [p.strip() for p in line.split('|')]
            if len(parts) >= 4:
                try:
                    time = int(parts[0])
                    pid = int(parts[1])
                    event = parts[2]
                    state = parts[3]
                    remaining = int(parts[4]) if len(parts) > 4 else 0
                    wait = int(parts[5]) if len(parts) > 5 else 0
                    
                    events.append({
                        'time': time,
                        'pid': pid,
                        'event': event,
                        'state': state,
                        'remaining': remaining,
                        'wait': wait
                    })
                except ValueError:
                    continue
    
    return sorted(events, key=lambda x: x['time'])

def build_gantt_data(events):
    """Construit les données pour le diagramme de Gantt"""
    gantt_data = defaultdict(list)  # pid -> [(start, end, state), ...]
    process_states = defaultdict(lambda: {'state': 'READY', 'start_time': 0})
    
    for event in events:
        pid = event['pid']
        time = event['time']
        state = event['state']
        
        # Si l'état change, fermer le segment précédent
        if pid in process_states:
            prev_state = process_states[pid]['state']
            prev_start = process_states[pid]['start_time']
            
            if prev_start < time:
                gantt_data[pid].append((prev_start, time, prev_state))
        
        # Mettre à jour l'état actuel
        process_states[pid] = {'state': state, 'start_time': time}
    
    # Fermer les segments restants
    max_time = max([e['time'] for e in events]) if events else 0
    for pid, info in process_states.items():
        if info['start_time'] < max_time:
            gantt_data[pid].append((info['start_time'], max_time, info['state']))
    
    return gantt_data

def plot_gantt_chart(gantt_data, output_file):
    """Génère le diagramme de Gantt"""
    if not gantt_data:
        print("⚠️  Aucune donnée pour le graphique Gantt")
        return
    
    fig, ax = plt.subplots(figsize=(14, max(6, len(gantt_data) * 0.8)))
    
    pids = sorted(gantt_data.keys())
    y_positions = {pid: i for i, pid in enumerate(pids)}
    
    max_time = 0
    for pid, segments in gantt_data.items():
        for start, end, state in segments:
            max_time = max(max_time, end)
            color = COLORS.get(state, '#CCCCCC')
            ax.barh(y_positions[pid], end - start, left=start, 
                   height=0.6, color=color, edgecolor='black', linewidth=0.5)
    
    # Configuration de l'axe Y
    ax.set_yticks(range(len(pids)))
    ax.set_yticklabels([f'PID {pid}' for pid in pids])
    ax.set_ylabel('Processus', fontsize=12, fontweight='bold')
    
    # Configuration de l'axe X
    ax.set_xlabel('Temps', fontsize=12, fontweight='bold')
    ax.set_xlim(0, max_time + 1)
    ax.grid(True, alpha=0.3, axis='x')
    
    # Légende
    patches = [mpatches.Patch(color=color, label=state) 
              for state, color in COLORS.items()]
    ax.legend(handles=patches, loc='upper right', fontsize=10)
    
    ax.set_title('📊 Diagramme de Gantt - MiniOS', fontsize=14, fontweight='bold', pad=20)
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"✅ Graphique Gantt sauvegardé: {output_file}")

def calculate_statistics(events):
    """Calcule les statistiques de performance"""
    stats = defaultdict(lambda: {
        'arrival': None,
        'start': None,
        'finish': None,
        'wait_times': [],
        'running_times': []
    })
    
    for event in events:
        pid = event['pid']
        time = event['time']
        
        if event['event'] == 'ARRIVAL':
            stats[pid]['arrival'] = time
        elif event['event'] == 'EXECUTE':
            if stats[pid]['start'] is None:
                stats[pid]['start'] = time
            stats[pid]['running_times'].append(time)
        elif event['event'] == 'TERMINATE':
            stats[pid]['finish'] = time
        elif event['state'] == 'READY':
            stats[pid]['wait_times'].append(time)
    
    return stats

def plot_statistics(stats, output_file):
    """Génère un graphique de statistiques"""
    if not stats:
        print("⚠️  Aucune statistique disponible")
        return
    
    pids = sorted([pid for pid in stats.keys() if stats[pid]['finish'] is not None])
    
    if not pids:
        print("⚠️  Aucun processus terminé pour les statistiques")
        return
    
    turnaround_times = []
    response_times = []
    wait_times = []
    
    for pid in pids:
        s = stats[pid]
        if s['arrival'] is not None and s['finish'] is not None:
            turnaround = s['finish'] - s['arrival']
            turnaround_times.append(turnaround)
            
            if s['start'] is not None:
                response = s['start'] - s['arrival']
                response_times.append(response)
            
            wait = len(s['wait_times'])
            wait_times.append(wait)
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('📈 Statistiques de Performance - MiniOS', 
                fontsize=16, fontweight='bold', y=0.995)
    
    # Graphique 1: Temps de retour par processus
    ax1 = axes[0, 0]
    if turnaround_times:
        ax1.bar(range(len(pids)), turnaround_times, color='#4CAF50', edgecolor='black')
        ax1.set_xticks(range(len(pids)))
        ax1.set_xticklabels([f'PID {pid}' for pid in pids], rotation=45)
        ax1.set_ylabel('Temps de retour', fontweight='bold')
        ax1.set_title('Temps de retour par processus', fontweight='bold')
        ax1.grid(True, alpha=0.3, axis='y')
        ax1.axhline(y=np.mean(turnaround_times), color='r', linestyle='--', 
                   label=f'Moyenne: {np.mean(turnaround_times):.2f}')
        ax1.legend()
    
    # Graphique 2: Temps de réponse par processus
    ax2 = axes[0, 1]
    if response_times:
        ax2.bar(range(len(response_times)), response_times, color='#2196F3', edgecolor='black')
        ax2.set_xticks(range(len(response_times)))
        ax2.set_xticklabels([f'PID {pids[i]}' for i in range(len(response_times))], rotation=45)
        ax2.set_ylabel('Temps de réponse', fontweight='bold')
        ax2.set_title('Temps de réponse par processus', fontweight='bold')
        ax2.grid(True, alpha=0.3, axis='y')
        ax2.axhline(y=np.mean(response_times), color='r', linestyle='--',
                   label=f'Moyenne: {np.mean(response_times):.2f}')
        ax2.legend()
    
    # Graphique 3: Temps d'attente par processus
    ax3 = axes[1, 0]
    if wait_times:
        ax3.bar(range(len(pids)), wait_times, color='#FF9800', edgecolor='black')
        ax3.set_xticks(range(len(pids)))
        ax3.set_xticklabels([f'PID {pid}' for pid in pids], rotation=45)
        ax3.set_ylabel('Temps d\'attente', fontweight='bold')
        ax3.set_title('Temps d\'attente par processus', fontweight='bold')
        ax3.grid(True, alpha=0.3, axis='y')
        ax3.axhline(y=np.mean(wait_times), color='r', linestyle='--',
                   label=f'Moyenne: {np.mean(wait_times):.2f}')
        ax3.legend()
    
    # Graphique 4: Résumé des moyennes
    ax4 = axes[1, 1]
    if turnaround_times and response_times and wait_times:
        categories = ['Temps de\nretour', 'Temps de\nréponse', 'Temps\nd\'attente']
        means = [np.mean(turnaround_times), np.mean(response_times), np.mean(wait_times)]
        colors = ['#4CAF50', '#2196F3', '#FF9800']
        
        bars = ax4.bar(categories, means, color=colors, edgecolor='black', width=0.6)
        ax4.set_ylabel('Temps moyen', fontweight='bold')
        ax4.set_title('Résumé des statistiques', fontweight='bold')
        ax4.grid(True, alpha=0.3, axis='y')
        
        # Ajouter les valeurs sur les barres
        for bar, mean in zip(bars, means):
            height = bar.get_height()
            ax4.text(bar.get_x() + bar.get_width()/2., height,
                    f'{mean:.2f}',
                    ha='center', va='bottom', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"✅ Graphique de statistiques sauvegardé: {output_file}")

def main():
    """Fonction principale"""
    # Changer vers le répertoire parent (minios)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    os.chdir(project_dir)
    
    trace_file = 'traces/minios_trace.txt'
    
    if not os.path.exists(trace_file):
        print(f"❌ Erreur: Fichier de trace introuvable: {trace_file}")
        print("💡 Exécutez d'abord MiniOS avec 'make run'")
        sys.exit(1)
    
    print("📊 Génération des graphiques de visualisation...\n")
    
    # Parser le fichier de trace
    events = parse_trace_file(trace_file)
    
    if not events:
        print("❌ Aucun événement trouvé dans le fichier de trace")
        sys.exit(1)
    
    print(f"✅ {len(events)} événements parsés\n")
    
    # Créer le dossier de sortie si nécessaire
    os.makedirs('traces', exist_ok=True)
    
    # Générer le diagramme de Gantt
    gantt_data = build_gantt_data(events)
    plot_gantt_chart(gantt_data, 'traces/gantt_chart.png')
    
    # Calculer et afficher les statistiques
    stats = calculate_statistics(events)
    plot_statistics(stats, 'traces/statistics.png')
    
    print("\n✅ Visualisation terminée!")
    print("📁 Fichiers générés:")
    print("   - traces/gantt_chart.png")
    print("   - traces/statistics.png\n")

if __name__ == '__main__':
    main()

