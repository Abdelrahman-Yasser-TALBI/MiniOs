#!/usr/bin/env python3
"""
Génère un rapport HTML interactif avec visualisations pour MiniOS
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
    
    # Fermer les segments restants
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
        'wait_times': [],
        'running_times': [],
        'blocked_times': [],
        'total_wait': 0,
        'total_running': 0,
        'total_blocked': 0
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
                stats[pid]['running_times'].append(time)
                stats[pid]['total_running'] += 1
            elif event['state'] == 'BLOCKED':
                stats[pid]['blocked_times'].append(time)
                stats[pid]['total_blocked'] += 1
        elif event['event'] == 'TERMINATE':
            stats[pid]['finish'] = time
        elif event['state'] == 'READY':
            stats[pid]['wait_times'].append(time)
            stats[pid]['total_wait'] += 1
    
    # Calculer les métriques finales
    result = {}
    for pid, s in stats.items():
        if s['arrival'] is not None and s['finish'] is not None:
            result[pid] = {
                'pid': pid,
                'arrival': s['arrival'],
                'start': s['start'] or s['arrival'],
                'finish': s['finish'],
                'turnaround': s['finish'] - s['arrival'],
                'response': (s['start'] - s['arrival']) if s['start'] else 0,
                'wait_time': s['total_wait'],
                'running_time': s['total_running'],
                'blocked_time': s['total_blocked']
            }
    
    return result

def generate_json_report(events, gantt_data, stats, max_time):
    """Génère un fichier JSON avec toutes les données"""
    report = {
        'metadata': {
            'total_events': len(events),
            'max_time': max_time,
            'process_count': len(set([e['pid'] for e in events]))
        },
        'events': events,
        'gantt': {str(pid): segments for pid, segments in gantt_data.items()},
        'statistics': stats
    }
    
    return report

def generate_html(report_data):
    """Génère la page HTML avec visualisations"""
    
    html_template = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MiniOS - Rapport de Simulation</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js"></script>
    <script>
        // Fallback si Chart.js ne charge pas
        window.addEventListener('error', function(e) {{
            if (e.message && e.message.includes('Chart')) {{
                console.warn('Chart.js non disponible, utilisation du fallback');
            }}
        }}, true);
    </script>
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
        
        .header p {{
            font-size: 1.2em;
            opacity: 0.9;
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
        
        .stat-card p {{
            font-size: 1.1em;
            opacity: 0.9;
        }}
        
        .chart-container {{
            position: relative;
            height: 400px;
            margin: 20px 0;
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
        }}
        
        .gantt-container {{
            overflow-x: auto;
            margin: 20px 0;
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
        }}
        
        .gantt-bar {{
            height: 40px;
            margin: 5px 0;
            position: relative;
            border-radius: 5px;
        }}
        
        .gantt-segment {{
            position: absolute;
            height: 100%;
            border-radius: 5px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
            font-size: 0.9em;
        }}
        
        .state-running {{ background: #32CD32; }}
        .state-ready {{ background: #FFD700; }}
        .state-blocked {{ background: #FF6347; }}
        .state-terminated {{ background: #808080; }}
        .state-new {{ background: #87CEEB; }}
        
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
            <!-- Statistiques globales -->
            <div class="section">
                <h2>📊 Statistiques Globales</h2>
                <div class="stats-grid">
                    <div class="stat-card">
                        <h3 id="total-processes">{report_data['metadata']['process_count']}</h3>
                        <p>Processus</p>
                    </div>
                    <div class="stat-card">
                        <h3 id="total-events">{report_data['metadata']['total_events']}</h3>
                        <p>Événements</p>
                    </div>
                    <div class="stat-card">
                        <h3 id="max-time">{report_data['metadata']['max_time']}</h3>
                        <p>Temps Total</p>
                    </div>
                    <div class="stat-card">
                        <h3 id="avg-turnaround">0</h3>
                        <p>Temps Retour Moyen</p>
                    </div>
                </div>
            </div>
            
            <!-- Diagramme de Gantt -->
            <div class="section">
                <h2>📈 Diagramme de Gantt</h2>
                <div class="legend">
                    <div class="legend-item">
                        <div class="legend-color state-running"></div>
                        <span>RUNNING</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color state-ready"></div>
                        <span>READY</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color state-blocked"></div>
                        <span>BLOCKED</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color state-terminated"></div>
                        <span>TERMINATED</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color state-new"></div>
                        <span>NEW</span>
                    </div>
                </div>
                <div class="gantt-container" id="gantt-container">
                    <p style="text-align: center; padding: 20px; color: #999;">Chargement du diagramme de Gantt...</p>
                </div>
            </div>
            
            <!-- Graphiques -->
            <div class="section">
                <h2>📉 Graphiques de Performance</h2>
                <div class="chart-container">
                    <canvas id="turnaroundChart"></canvas>
                    <p id="turnaround-loading" style="text-align: center; padding: 20px; color: #999;">Chargement...</p>
                </div>
                <div class="chart-container">
                    <canvas id="responseChart"></canvas>
                    <p id="response-loading" style="text-align: center; padding: 20px; color: #999;">Chargement...</p>
                </div>
                <div class="chart-container">
                    <canvas id="waitChart"></canvas>
                    <p id="wait-loading" style="text-align: center; padding: 20px; color: #999;">Chargement...</p>
                </div>
            </div>
            
            <!-- Tableau de statistiques -->
            <div class="section">
                <h2>📋 Statistiques par Processus</h2>
                <table id="stats-table">
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
                    <tbody id="stats-tbody">
                    </tbody>
                </table>
            </div>
        </div>
    </div>
    
    <script>
        const reportData = {json.dumps(report_data, indent=2, ensure_ascii=False)};
        
        // Vérifier que les données sont chargées
        console.log('Données chargées:', reportData);
        console.log('Statistiques:', reportData.statistics);
        
        // Convertir les statistiques en tableau si nécessaire
        let statsArray = [];
        if (Array.isArray(reportData.statistics)) {{
            statsArray = reportData.statistics;
        }} else {{
            statsArray = Object.values(reportData.statistics);
        }}
        
        console.log('Stats array:', statsArray);
        
        // Calculer la moyenne du temps de retour
        const avgTurnaround = statsArray.length > 0 
            ? (statsArray.reduce((sum, s) => sum + (s.turnaround || 0), 0) / statsArray.length).toFixed(2)
            : 0;
        
        const avgElement = document.getElementById('avg-turnaround');
        if (avgElement) {{
            avgElement.textContent = avgTurnaround;
        }}
        
        // Générer le diagramme de Gantt
        function generateGantt() {{
            console.log('🎨 Génération du diagramme de Gantt...');
            const container = document.getElementById('gantt-container');
            
            if (!container) {{
                console.error('❌ Conteneur Gantt introuvable!');
                return;
            }}
            
            if (!reportData.gantt || Object.keys(reportData.gantt).length === 0) {{
                console.error('❌ Aucune donnée Gantt disponible!');
                container.innerHTML = '<p style="text-align: center; padding: 20px; color: #666;">Aucune donnée disponible pour le diagramme de Gantt</p>';
                return;
            }}
            
            const maxTime = reportData.metadata.max_time || 100;
            const scale = 100 / maxTime; // Échelle pour l'affichage
            
            console.log('📊 Données Gantt:', Object.keys(reportData.gantt).length, 'processus');
            
            // Créer un wrapper pour le Gantt avec padding pour les labels
            const wrapper = document.createElement('div');
            wrapper.style.position = 'relative';
            wrapper.style.paddingLeft = '70px';
            wrapper.style.minHeight = '200px';
            
            Object.keys(reportData.gantt).sort((a, b) => parseInt(a) - parseInt(b)).forEach(pid => {{
                const segments = reportData.gantt[pid];
                if (!segments || segments.length === 0) return;
                
                const bar = document.createElement('div');
                bar.className = 'gantt-bar';
                bar.style.position = 'relative';
                bar.style.height = '35px';
                bar.style.marginBottom = '8px';
                bar.style.border = '1px solid #ddd';
                bar.style.borderRadius = '4px';
                bar.style.overflow = 'hidden';
                
                const label = document.createElement('div');
                label.textContent = `PID ${{pid}}`;
                label.style.position = 'absolute';
                label.style.left = '-70px';
                label.style.width = '60px';
                label.style.textAlign = 'right';
                label.style.fontWeight = 'bold';
                label.style.lineHeight = '35px';
                label.style.color = '#333';
                bar.appendChild(label);
                
                segments.forEach(segment => {{
                    if (!segment || !segment.state) return;
                    
                    const seg = document.createElement('div');
                    seg.className = `gantt-segment state-${{segment.state.toLowerCase()}}`;
                    const left = (segment.start || 0) * scale;
                    const width = ((segment.end || segment.start || 0) - (segment.start || 0)) * scale;
                    seg.style.left = `${{left}}%`;
                    seg.style.width = `${{Math.max(width, 0.5)}}%`;
                    seg.style.minWidth = '2px';
                    seg.title = `${{segment.state}}: t=${{segment.start}}-${{segment.end}}`;
                    seg.style.cursor = 'help';
                    bar.appendChild(seg);
                }});
                
                wrapper.appendChild(bar);
            }});
            
            // Vider et remplir le conteneur
            container.innerHTML = '';
            container.appendChild(wrapper);
            console.log('✅ Diagramme de Gantt généré avec', Object.keys(reportData.gantt).length, 'processus!');
        }}
        
        // Générer les graphiques
        function generateCharts() {{
            // Convertir les statistiques en tableau
            let stats = [];
            if (Array.isArray(reportData.statistics)) {{
                stats = reportData.statistics;
            }} else {{
                stats = Object.values(reportData.statistics);
            }}
            
            stats = stats.sort((a, b) => (a.pid || 0) - (b.pid || 0));
            console.log('Stats pour graphiques:', stats);
            
            if (stats.length === 0) {{
                console.error('Aucune statistique disponible');
                const container = document.getElementById('turnaroundChart');
                if (container && container.parentElement) {{
                    container.parentElement.innerHTML = '<p style="text-align: center; padding: 20px; color: #666;">Aucune donnée disponible pour les graphiques</p>';
                }}
                return;
            }}
            
            const pids = stats.map(s => `PID ${{s.pid || 'N/A'}}`);
            const turnarounds = stats.map(s => parseInt(s.turnaround) || 0);
            const responses = stats.map(s => parseInt(s.response) || 0);
            const waits = stats.map(s => parseInt(s.wait_time) || 0);
            
            console.log('Données graphiques:', {{ pids, turnarounds, responses, waits }});
            
            // Attendre que Chart.js soit chargé
            if (typeof Chart === 'undefined') {{
                console.error('Chart.js n\'est pas chargé');
                setTimeout(generateCharts, 100);
                return;
            }}
            
            // Graphique temps de retour
            const ctx1 = document.getElementById('turnaroundChart');
            const loading1 = document.getElementById('turnaround-loading');
            if (!ctx1) {{
                console.error('Canvas turnaroundChart introuvable');
                return;
            }}
            
            if (loading1) loading1.style.display = 'none';
            
            try {{
                const chart1 = new Chart(ctx1, {{
                type: 'bar',
                data: {{
                    labels: pids,
                    datasets: [{{
                        label: 'Temps de Retour',
                        data: turnarounds,
                        backgroundColor: 'rgba(102, 126, 234, 0.8)',
                        borderColor: 'rgba(102, 126, 234, 1)',
                        borderWidth: 2
                    }}]
                }},
                options: {{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {{
                        title: {{
                            display: true,
                            text: 'Temps de Retour par Processus',
                            font: {{ size: 16, weight: 'bold' }}
                        }},
                        legend: {{ display: false }}
                    }},
                    scales: {{
                        y: {{
                            beginAtZero: true,
                            title: {{
                                display: true,
                                text: 'Temps'
                            }}
                        }}
                    }}
                }}
                }});
                console.log('✅ Graphique turnaround créé');
            }} catch (e) {{
                console.error('Erreur création graphique turnaround:', e);
                if (loading1) loading1.textContent = 'Erreur lors du chargement du graphique';
            }}
            
            // Graphique temps de réponse
            const ctx2 = document.getElementById('responseChart');
            const loading2 = document.getElementById('response-loading');
            if (!ctx2) {{
                console.error('Canvas responseChart introuvable');
                return;
            }}
            
            if (loading2) loading2.style.display = 'none';
            
            try {{
                const chart2 = new Chart(ctx2, {{
                type: 'bar',
                data: {{
                    labels: pids,
                    datasets: [{{
                        label: 'Temps de Réponse',
                        data: responses,
                        backgroundColor: 'rgba(33, 150, 243, 0.8)',
                        borderColor: 'rgba(33, 150, 243, 1)',
                        borderWidth: 2
                    }}]
                }},
                options: {{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {{
                        title: {{
                            display: true,
                            text: 'Temps de Réponse par Processus',
                            font: {{ size: 16, weight: 'bold' }}
                        }},
                        legend: {{ display: false }}
                    }},
                    scales: {{
                        y: {{
                            beginAtZero: true,
                            title: {{
                                display: true,
                                text: 'Temps'
                            }}
                        }}
                    }}
                }}
                }});
                console.log('✅ Graphique response créé');
            }} catch (e) {{
                console.error('Erreur création graphique response:', e);
                if (loading2) loading2.textContent = 'Erreur lors du chargement du graphique';
            }}
            
            // Graphique temps d'attente
            const ctx3 = document.getElementById('waitChart');
            const loading3 = document.getElementById('wait-loading');
            if (!ctx3) {{
                console.error('Canvas waitChart introuvable');
                return;
            }}
            
            if (loading3) loading3.style.display = 'none';
            
            try {{
                const chart3 = new Chart(ctx3, {{
                type: 'bar',
                data: {{
                    labels: pids,
                    datasets: [{{
                        label: 'Temps d\'Attente',
                        data: waits,
                        backgroundColor: 'rgba(255, 152, 0, 0.8)',
                        borderColor: 'rgba(255, 152, 0, 1)',
                        borderWidth: 2
                    }}]
                }},
                options: {{
                    responsive: true,
                    maintainAspectRatio: false,
                    plugins: {{
                        title: {{
                            display: true,
                            text: 'Temps d\'Attente par Processus',
                            font: {{ size: 16, weight: 'bold' }}
                        }},
                        legend: {{ display: false }}
                    }},
                    scales: {{
                        y: {{
                            beginAtZero: true,
                            title: {{
                                display: true,
                                text: 'Temps'
                            }}
                        }}
                    }}
                }}
                }});
                console.log('✅ Graphique wait créé');
            }} catch (e) {{
                console.error('Erreur création graphique wait:', e);
                if (loading3) loading3.textContent = 'Erreur lors du chargement du graphique';
            }}
            
            console.log('✅ Tous les graphiques générés!');
        }}
        
        // Remplir le tableau
        function fillTable() {{
            const tbody = document.getElementById('stats-tbody');
            if (!tbody) {{
                console.error('Tableau stats-tbody introuvable');
                return;
            }}
            
            let stats = [];
            if (Array.isArray(reportData.statistics)) {{
                stats = reportData.statistics;
            }} else {{
                stats = Object.values(reportData.statistics);
            }}
            stats = stats.sort((a, b) => (a.pid || 0) - (b.pid || 0));
            
            stats.forEach(stat => {{
                const row = tbody.insertRow();
                row.insertCell(0).textContent = stat.pid;
                row.insertCell(1).textContent = stat.arrival;
                row.insertCell(2).textContent = stat.start;
                row.insertCell(3).textContent = stat.finish;
                row.insertCell(4).textContent = stat.turnaround;
                row.insertCell(5).textContent = stat.response;
                row.insertCell(6).textContent = stat.wait_time;
            }});
        }}
        
        // Fonction d'initialisation
        function initAll() {{
            console.log('🚀 Initialisation de MiniOS Report...');
            console.log('📦 Données chargées:', reportData);
            console.log('📊 Gantt keys:', Object.keys(reportData.gantt || {{}}));
            console.log('📈 Stats keys:', Object.keys(reportData.statistics || {{}}));
            
            // Générer immédiatement le Gantt et le tableau
            try {{
                generateGantt();
                fillTable();
                console.log('✅ Gantt et tableau générés');
            }} catch (e) {{
                console.error('❌ Erreur lors de la génération:', e);
            }}
            
            // Essayer de charger les graphiques
            function tryGenerateCharts() {{
                if (typeof Chart !== 'undefined') {{
                    console.log('✅ Chart.js chargé, génération des graphiques...');
                    try {{
                        generateCharts();
                        console.log('✅ Graphiques générés');
                    }} catch (e) {{
                        console.error('❌ Erreur lors de la génération des graphiques:', e);
                    }}
                }} else {{
                    console.log('⏳ Attente de Chart.js...');
                    setTimeout(tryGenerateCharts, 200);
                }}
            }}
            
            // Attendre que le DOM soit prêt
            if (document.readyState === 'loading') {{
                document.addEventListener('DOMContentLoaded', function() {{
                    console.log('📄 DOM chargé');
                    setTimeout(tryGenerateCharts, 100);
                }});
            }} else {{
                // DOM déjà chargé
                console.log('📄 DOM déjà chargé');
                setTimeout(tryGenerateCharts, 100);
            }}
        }}
        
        // Démarrer l'initialisation immédiatement
        if (document.readyState === 'loading') {{
            document.addEventListener('DOMContentLoaded', initAll);
        }} else {{
            // DOM déjà chargé, exécuter immédiatement
            setTimeout(initAll, 50);
        }}
    </script>
</body>
</html>
"""
    
    return html_template

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
    
    print("🎨 Génération du rapport HTML interactif...\n")
    
    # Parser les événements
    events = parse_trace_file(trace_file)
    
    if not events:
        print("❌ Aucun événement trouvé dans le fichier de trace")
        sys.exit(1)
    
    # Construire les données
    gantt_data, max_time = build_gantt_data(events)
    stats = calculate_statistics(events)
    
    # Générer le rapport JSON
    report_data = generate_json_report(events, gantt_data, stats, max_time)
    
    # Sauvegarder le JSON
    json_file = 'traces/minios_report.json'
    with open(json_file, 'w') as f:
        json.dump(report_data, f, indent=2)
    print(f"✅ Fichier JSON généré: {json_file}")
    
    # Générer et sauvegarder le HTML
    html_content = generate_html(report_data)
    html_file = 'traces/minios_report.html'
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html_content)
    print(f"✅ Rapport HTML généré: {html_file}")
    
    print(f"\n✨ Rapport complet généré!")
    print(f"📁 Ouvrez {html_file} dans votre navigateur pour voir la visualisation interactive\n")

if __name__ == '__main__':
    main()

