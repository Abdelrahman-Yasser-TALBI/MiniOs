#!/usr/bin/env python3
"""
Génère un rapport HTML avec visualisations SVG natives (sans Chart.js)
"""

import os
import sys
import json
from collections import defaultdict

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

def build_gantt_data(events):
    """Construit les données pour le diagramme de Gantt"""
    gantt_data = defaultdict(list)
    process_states = {}
    
    for event in events:
        pid = event['pid']
        time = event['time']
        state = event['state']
        
        if pid not in process_states:
            process_states[pid] = {'state': 'NEW', 'start_time': time}
        
        prev_state = process_states[pid]['state']
        prev_start = process_states[pid]['start_time']
        
        if prev_state != state or time > prev_start:
            if prev_start < time:
                gantt_data[pid].append({
                    'start': prev_start,
                    'end': time,
                    'state': prev_state
                })
        
        process_states[pid] = {'state': state, 'start_time': time}
    
    max_time = max([e['time'] for e in events]) if events else 0
    for pid, info in process_states.items():
        if info['start_time'] < max_time:
            gantt_data[pid].append({
                'start': info['start_time'],
                'end': max_time,
                'state': info['state']
            })
    
    return gantt_data, max_time

def calculate_statistics(events):
    """Calcule les statistiques de performance"""
    stats = defaultdict(lambda: {
        'arrival': None,
        'start': None,
        'finish': None,
        'state_history': [],  # Liste de (time, state)
        'total_wait': 0,
        'total_running': 0,
        'total_blocked': 0
    })
    
    # Construire l'historique des états
    for event in events:
        pid = event['pid']
        time = event['time']
        state = event['state']
        
        if event['event'] == 'ARRIVAL':
            stats[pid]['arrival'] = time
            stats[pid]['state_history'].append((time, state))
        elif event['event'] == 'EXECUTE':
            if stats[pid]['start'] is None:
                stats[pid]['start'] = time
            stats[pid]['state_history'].append((time, state))
        elif event['event'] == 'TERMINATE':
            stats[pid]['finish'] = time
            stats[pid]['state_history'].append((time, state))
        else:
            # Autres événements qui changent l'état
            if stats[pid]['state_history']:
                last_state = stats[pid]['state_history'][-1][1]
                if last_state != state:
                    stats[pid]['state_history'].append((time, state))
            else:
                stats[pid]['state_history'].append((time, state))
    
    # Calculer les temps réels passés dans chaque état
    result = {}
    for pid, s in stats.items():
        if s['arrival'] is not None and s['finish'] is not None:
            # Calculer le temps d'attente réel (temps en READY)
            wait_time = 0
            running_time = 0
            blocked_time = 0
            
            state_history = sorted(s['state_history'], key=lambda x: x[0])
            for i in range(len(state_history) - 1):
                current_time, current_state = state_history[i]
                next_time, _ = state_history[i + 1]
                duration = next_time - current_time
                
                if current_state == 'READY':
                    wait_time += duration
                elif current_state == 'RUNNING':
                    running_time += duration
                elif current_state == 'BLOCKED':
                    blocked_time += duration
            
            result[pid] = {
                'pid': pid,
                'arrival': s['arrival'],
                'start': s['start'] or s['arrival'],
                'finish': s['finish'],
                'turnaround': s['finish'] - s['arrival'],
                'response': (s['start'] - s['arrival']) if s['start'] else 0,
                'wait_time': wait_time,
                'running_time': running_time,
                'blocked_time': blocked_time
            }
    
    return result

def generate_svg_bar_chart(data, labels, title, color, max_value=None):
    """Génère un graphique en barres SVG"""
    if not data or len(data) == 0:
        return '<p style="text-align: center; padding: 20px; color: #666;">Aucune donnée disponible</p>'
    
    max_val = max_value or max(data) or 1
    width = 800
    height = 400
    bar_width = width / (len(data) * 1.5)
    bar_spacing = bar_width * 0.3
    
    svg = f'<svg width="{width}" height="{height}" style="background: #f8f9fa; border-radius: 10px;">'
    svg += f'<text x="{width/2}" y="30" text-anchor="middle" font-size="18" font-weight="bold" fill="#333">{title}</text>'
    
    # Axes
    svg += f'<line x1="60" y1="{height-40}" x2="{width-20}" y2="{height-40}" stroke="#333" stroke-width="2"/>'
    svg += f'<line x1="60" y1="50" x2="60" y2="{height-40}" stroke="#333" stroke-width="2"/>'
    
    # Bars
    for i, (value, label) in enumerate(zip(data, labels)):
        x = 70 + i * (bar_width + bar_spacing)
        bar_height = (value / max_val) * (height - 100)
        y = height - 40 - bar_height
        
        svg += f'<rect x="{x}" y="{y}" width="{bar_width}" height="{bar_height}" fill="{color}" stroke="#333" stroke-width="1" rx="3"/>'
        svg += f'<text x="{x + bar_width/2}" y="{y - 5}" text-anchor="middle" font-size="12" fill="#333" font-weight="bold">{value}</text>'
        svg += f'<text x="{x + bar_width/2}" y="{height-25}" text-anchor="middle" font-size="11" fill="#666">{label}</text>'
    
    # Y-axis labels
    for i in range(5):
        y_val = max_val * (1 - i/4)
        y_pos = 50 + (i/4) * (height - 100)
        svg += f'<text x="55" y="{y_pos + 4}" text-anchor="end" font-size="10" fill="#666">{y_val:.0f}</text>'
        svg += f'<line x1="58" y1="{y_pos}" x2="62" y2="{y_pos}" stroke="#999" stroke-width="1"/>'
    
    svg += '</svg>'
    return svg

def generate_html_simple(report_data):
    """Génère la page HTML avec graphiques SVG natifs"""
    
    # Préparer les données pour les graphiques
    stats = list(report_data['statistics'].values())
    stats.sort(key=lambda x: x['pid'])
    
    pids = [f"PID {s['pid']}" for s in stats]
    turnarounds = [s['turnaround'] for s in stats]
    responses = [s['response'] for s in stats]
    waits = [s['wait_time'] for s in stats]
    
    max_turnaround = max(turnarounds) if turnarounds else 1
    max_response = max(responses) if responses else 1
    max_wait = max(waits) if waits else 1
    
    # Générer le Gantt SVG avec dimensions fixes
    num_processes = len(report_data['gantt'])
    gantt_height = num_processes * 50 + 80
    gantt_width = 1200
    
    gantt_svg = f'<svg width="{gantt_width}" height="{gantt_height}" style="background: #f8f9fa; border-radius: 10px; padding: 20px; overflow: visible;">'
    
    max_time = report_data['metadata']['max_time']
    usable_width = gantt_width - 120  # Réserver de l'espace pour les labels
    width_scale = usable_width / max_time if max_time > 0 else 1
    
    # Ligne de temps
    gantt_svg += f'<line x1="100" y1="30" x2="{gantt_width - 20}" y2="30" stroke="#333" stroke-width="2"/>'
    
    # Graduations de temps
    for t in range(0, max_time + 1, max(1, max_time // 10)):
        x = 100 + t * width_scale
        gantt_svg += f'<line x1="{x}" y1="25" x2="{x}" y2="35" stroke="#666" stroke-width="1"/>'
        gantt_svg += f'<text x="{x}" y="20" text-anchor="middle" font-size="10" fill="#666">{t}</text>'
    
    y_pos = 50
    for pid in sorted(report_data['gantt'].keys(), key=int):
        segments = report_data['gantt'][pid]
        gantt_svg += f'<text x="10" y="{y_pos + 20}" font-size="14" font-weight="bold" fill="#333">PID {pid}</text>'
        
        x_start = 100
        for seg in segments:
            x = x_start + seg['start'] * width_scale
            w = (seg['end'] - seg['start']) * width_scale
            
            # S'assurer que la largeur est au moins 2px pour être visible
            if w < 2:
                w = 2
            
            colors = {
                'RUNNING': '#32CD32',
                'READY': '#FFD700',
                'BLOCKED': '#FF6347',
                'TERMINATED': '#808080',
                'NEW': '#87CEEB'
            }
            color = colors.get(seg['state'], '#CCCCCC')
            
            gantt_svg += f'<rect x="{x}" y="{y_pos}" width="{w}" height="35" fill="{color}" stroke="#333" stroke-width="1" rx="3"/>'
            
            # Ajouter un tooltip avec les informations
            gantt_svg += f'<title>{seg["state"]}: t={seg["start"]}-{seg["end"]} (durée: {seg["end"] - seg["start"]})</title>'
            
            # Texte seulement si la barre est assez large
            if w > 20:
                gantt_svg += f'<text x="{x + w/2}" y="{y_pos + 22}" text-anchor="middle" font-size="11" fill="#000" font-weight="bold">{seg["state"][:3]}</text>'
        
        y_pos += 50
    
    gantt_svg += '</svg>'
    
    html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MiniOS - Rapport de Simulation</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}
        
        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }}
        
        .container {{
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }}
        
        .header {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 40px;
            text-align: center;
        }}
        
        .header h1 {{
            font-size: 2.5em;
            margin-bottom: 10px;
        }}
        
        .content {{
            padding: 40px;
        }}
        
        .section {{
            margin-bottom: 50px;
        }}
        
        .section h2 {{
            color: #667eea;
            font-size: 1.8em;
            margin-bottom: 20px;
            padding-bottom: 10px;
            border-bottom: 3px solid #667eea;
        }}
        
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }}
        
        .stat-card {{
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 25px;
            border-radius: 15px;
            text-align: center;
            box-shadow: 0 5px 15px rgba(0,0,0,0.2);
        }}
        
        .stat-card h3 {{
            font-size: 2.5em;
            margin-bottom: 10px;
        }}
        
        .chart-wrapper {{
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            margin: 20px 0;
            overflow-x: auto;
        }}
        
        .gantt-wrapper {{
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            margin: 20px 0;
            overflow-x: auto;
            overflow-y: visible;
        }}
        
        table {{
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: white;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            border-radius: 10px;
            overflow: hidden;
        }}
        
        th {{
            background: #667eea;
            color: white;
            padding: 15px;
            text-align: left;
            font-weight: 600;
        }}
        
        td {{
            padding: 12px 15px;
            border-bottom: 1px solid #eee;
        }}
        
        tr:hover {{
            background: #f5f5f5;
        }}
        
        .legend {{
            display: flex;
            flex-wrap: wrap;
            gap: 15px;
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 10px;
        }}
        
        .legend-item {{
            display: flex;
            align-items: center;
            gap: 8px;
        }}
        
        .legend-color {{
            width: 20px;
            height: 20px;
            border-radius: 3px;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🖥️ MiniOS - Rapport de Simulation</h1>
            <p>Système d'exploitation simulé - Analyse détaillée</p>
        </div>
        
        <div class="content">
            <div class="section">
                <h2>📊 Statistiques Globales</h2>
                <div class="stats-grid">
                    <div class="stat-card">
                        <h3>{report_data['metadata']['process_count']}</h3>
                        <p>Processus</p>
                    </div>
                    <div class="stat-card">
                        <h3>{report_data['metadata']['total_events']}</h3>
                        <p>Événements</p>
                    </div>
                    <div class="stat-card">
                        <h3>{report_data['metadata']['max_time']}</h3>
                        <p>Temps Total</p>
                    </div>
                    <div class="stat-card">
                        <h3>{sum(turnarounds) / len(turnarounds) if turnarounds else 0:.2f}</h3>
                        <p>Temps Retour Moyen</p>
                    </div>
                </div>
            </div>
            
            <div class="section">
                <h2>📈 Diagramme de Gantt</h2>
                <div class="legend">
                    <div class="legend-item">
                        <div class="legend-color" style="background: #32CD32;"></div>
                        <span>RUNNING</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color" style="background: #FFD700;"></div>
                        <span>READY</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color" style="background: #FF6347;"></div>
                        <span>BLOCKED</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color" style="background: #808080;"></div>
                        <span>TERMINATED</span>
                    </div>
                </div>
                <div class="gantt-wrapper">
                    {gantt_svg}
                </div>
            </div>
            
            <div class="section">
                <h2>📉 Graphiques de Performance</h2>
                <div class="chart-wrapper">
                    {generate_svg_bar_chart(turnarounds, pids, 'Temps de Retour par Processus', '#667eea', max_turnaround)}
                </div>
                <div class="chart-wrapper">
                    {generate_svg_bar_chart(responses, pids, 'Temps de Réponse par Processus', '#2196F3', max_response)}
                </div>
                <div class="chart-wrapper">
                    {generate_svg_bar_chart(waits, pids, 'Temps d\'Attente par Processus', '#FF9800', max_wait)}
                </div>
            </div>
            
            <div class="section">
                <h2>📋 Statistiques par Processus</h2>
                <table>
                    <thead>
                        <tr>
                            <th>PID</th>
                            <th>Arrivée</th>
                            <th>Début</th>
                            <th>Fin</th>
                            <th>Temps Retour</th>
                            <th>Temps Réponse</th>
                            <th>Temps Attente</th>
                        </tr>
                    </thead>
                    <tbody>
"""
    
    for stat in stats:
        html += f"""
                        <tr>
                            <td>{stat['pid']}</td>
                            <td>{stat['arrival']}</td>
                            <td>{stat['start']}</td>
                            <td>{stat['finish']}</td>
                            <td>{stat['turnaround']}</td>
                            <td>{stat['response']}</td>
                            <td>{stat['wait_time']}</td>
                        </tr>
"""
    
    html += """
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</body>
</html>
"""
    
    return html

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
    
    print("🎨 Génération du rapport HTML avec graphiques SVG natifs...\n")
    
    events = parse_trace_file(trace_file)
    
    if not events:
        print("❌ Aucun événement trouvé dans le fichier de trace")
        sys.exit(1)
    
    gantt_data, max_time = build_gantt_data(events)
    stats = calculate_statistics(events)
    
    report_data = {
        'metadata': {
            'total_events': len(events),
            'max_time': max_time,
            'process_count': len(set([e['pid'] for e in events]))
        },
        'gantt': gantt_data,
        'statistics': stats
    }
    
    html_content = generate_html_simple(report_data)
    html_file = 'traces/minios_report.html'
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html_content)
    print(f"✅ Rapport HTML généré: {html_file}")
    print(f"\n✨ Rapport complet avec graphiques SVG natifs!")
    print(f"📁 Ouvrez {html_file} dans votre navigateur\n")

if __name__ == '__main__':
    main()

