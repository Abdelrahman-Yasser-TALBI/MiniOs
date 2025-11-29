#!/usr/bin/env python3
"""
Visualisation terminal pour MiniOS - Affiche les résultats directement dans le terminal
"""

import os
import sys

def parse_trace_file(filename):
    """Parse le fichier de trace et extrait les événements"""
    events = []
    
    if not os.path.exists(filename):
        print(f"❌ Erreur: Fichier {filename} introuvable")
        return events
    
    with open(filename, 'r') as f:
        for line in f:
            if line.startswith('===') or line.startswith('Format') or \
               line.startswith('Total') or not line.strip():
                continue
            
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

def build_timeline(events):
    """Construit une timeline des états des processus"""
    timeline = {}
    max_time = max([e['time'] for e in events]) if events else 0
    
    for event in events:
        pid = event['pid']
        time = event['time']
        state = event['state']
        
        if pid not in timeline:
            timeline[pid] = [' '] * (max_time + 1)
        
        # Mapper les états à des caractères
        char_map = {
            'READY': '·',
            'RUNNING': '█',
            'BLOCKED': '░',
            'TERMINATED': '■',
            'NEW': '○'
        }
        
        char = char_map.get(state, '?')
        timeline[pid][time] = char
    
    # Remplir les espaces vides avec l'état précédent
    for pid in timeline:
        last_state = ' '
        for i in range(len(timeline[pid])):
            if timeline[pid][i] == ' ':
                timeline[pid][i] = last_state
            else:
                last_state = timeline[pid][i]
    
    return timeline, max_time

def print_gantt_terminal(timeline, max_time):
    """Affiche un diagramme de Gantt dans le terminal"""
    print("\n" + "="*80)
    print("📊 DIAGRAMME DE GANTT - Évolution des processus dans le temps")
    print("="*80)
    print("\nLégende:")
    print("  █ = RUNNING (en cours d'exécution)")
    print("  · = READY (prêt, en attente)")
    print("  ░ = BLOCKED (bloqué sur I/O ou synchronisation)")
    print("  ■ = TERMINATED (terminé)")
    print("  ○ = NEW (nouveau)")
    print("\n" + "-"*80)
    
    pids = sorted(timeline.keys())
    
    # En-tête avec numéros de temps
    header = "PID  "
    for t in range(min(50, max_time + 1)):  # Limiter à 50 pour la lisibilité
        if t % 5 == 0:
            header += str(t % 10)
        else:
            header += " "
    if max_time > 50:
        header += "..."
    print(header)
    print("-"*80)
    
    # Afficher chaque processus
    for pid in pids:
        row = f"{pid:3d}  "
        for t in range(min(50, max_time + 1)):
            row += timeline[pid][t]
        if max_time > 50:
            row += "..."
        print(row)
    
    print("-"*80)
    print(f"Temps: 0 à {max_time} (affichage des 50 premières unités)\n")

def print_statistics(events):
    """Affiche les statistiques de manière visuelle"""
    from collections import defaultdict
    
    stats = defaultdict(lambda: {
        'arrival': None,
        'start': None,
        'finish': None,
        'wait_count': 0,
        'running_count': 0,
        'blocked_count': 0
    })
    
    for event in events:
        pid = event['pid']
        time = event['time']
        
        if event['event'] == 'ARRIVAL':
            stats[pid]['arrival'] = time
        elif event['event'] == 'EXECUTE':
            if stats[pid]['start'] is None:
                stats[pid]['start'] = time
            if event['state'] == 'RUNNING':
                stats[pid]['running_count'] += 1
            elif event['state'] == 'BLOCKED':
                stats[pid]['blocked_count'] += 1
        elif event['event'] == 'TERMINATE':
            stats[pid]['finish'] = time
        elif event['state'] == 'READY':
            stats[pid]['wait_count'] += 1
    
    print("\n" + "="*80)
    print("📈 STATISTIQUES DÉTAILLÉES PAR PROCESSUS")
    print("="*80)
    print(f"{'PID':<5} {'Arrival':<8} {'Start':<8} {'Finish':<8} {'Turnaround':<12} {'Response':<10} {'Wait':<8}")
    print("-"*80)
    
    for pid in sorted(stats.keys()):
        s = stats[pid]
        if s['arrival'] is not None and s['finish'] is not None:
            turnaround = s['finish'] - s['arrival']
            response = (s['start'] - s['arrival']) if s['start'] is not None else 0
            wait = s['wait_count']
            
            print(f"{pid:<5} {s['arrival']:<8} {s['start'] or '-':<8} {s['finish']:<8} "
                  f"{turnaround:<12} {response:<10} {wait:<8}")
    
    print("="*80 + "\n")

def main():
    """Fonction principale"""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    os.chdir(project_dir)
    
    trace_file = 'traces/minios_trace.txt'
    
    if not os.path.exists(trace_file):
        print(f"❌ Erreur: Fichier de trace introuvable: {trace_file}")
        print("💡 Exécutez d'abord MiniOS avec './bin/minios'")
        sys.exit(1)
    
    print("\n🎨 GÉNÉRATION DE LA VISUALISATION TERMINAL...\n")
    
    events = parse_trace_file(trace_file)
    
    if not events:
        print("❌ Aucun événement trouvé dans le fichier de trace")
        sys.exit(1)
    
    print(f"✅ {len(events)} événements analysés\n")
    
    # Construire et afficher la timeline
    timeline, max_time = build_timeline(events)
    print_gantt_terminal(timeline, max_time)
    
    # Afficher les statistiques
    print_statistics(events)
    
    print("✨ Visualisation terminée!\n")

if __name__ == '__main__':
    main()

