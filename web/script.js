/* ═══════════════════════════════════════════════════════════
   script.js — ECI Route Finder (Hyderabad Edition)
   Dijkstra's Algorithm · Interactive SVG Map · Analysis
   Dataset: 20 real Hyderabad locations, 30 roads
   ═══════════════════════════════════════════════════════════ */
'use strict';

/* ─────────────────────────────────────────────────────────
   1. DATASET — matches locations.csv and routes.csv exactly
   ───────────────────────────────────────────────────────── */
const LOCATIONS = [
  { id:'L01', name:'NIMS Hospital',                     type:'Hospital',      x:340, y:250 },
  { id:'L02', name:'Apollo Hospital Jubilee Hills',      type:'Hospital',      x:240, y:275 },
  { id:'L03', name:'Yashoda Hospital Secunderabad',      type:'Hospital',      x:450, y:100 },
  { id:'L04', name:'KIMS Hospital',                      type:'Hospital',      x:535, y:115 },
  { id:'L05', name:'Care Hospital Banjara Hills',        type:'Hospital',      x:200, y:315 },
  { id:'L06', name:'Osmania General Hospital',           type:'Hospital',      x:360, y:395 },
  { id:'L07', name:'Gandhi Hospital',                    type:'Hospital',      x:385, y:155 },
  { id:'L08', name:'Global Hospital Lakdi Ka Pool',      type:'Hospital',      x:565, y:245 },
  { id:'L09', name:'Rainbow Childrens Hospital',         type:'Hospital',      x:190, y:355 },
  { id:'L10', name:'Aware Global Hospital Dilsukhnagar', type:'Hospital',      x:510, y:400 },
  { id:'L11', name:'Hitech City',                        type:'IT Hub',        x:120, y:225 },
  { id:'L12', name:'Secunderabad Railway Station',       type:'Transport Hub', x:435, y: 90 },
  { id:'L13', name:'MGBS Bus Station',                   type:'Transport Hub', x:345, y:455 },
  { id:'L14', name:'Charminar',                          type:'Landmark',      x:400, y:465 },
  { id:'L15', name:'LB Nagar',                           type:'Area',          x:590, y:430 },
  { id:'L16', name:'Kukatpally',                         type:'Area',          x:135, y:145 },
  { id:'L17', name:'Miyapur',                            type:'Area',          x: 85, y: 90 },
  { id:'L18', name:'Uppal',                              type:'Area',          x:570, y:315 },
  { id:'L19', name:'Banjara Hills',                      type:'Area',          x:235, y:245 },
  { id:'L20', name:'Punjagutta',                         type:'Area',          x:315, y:205 },
];

const ROADS = [
  { id:'R01', from:'L20', to:'L01', distance:2000,  time:5  },
  { id:'R02', from:'L20', to:'L19', distance:3000,  time:8  },
  { id:'R03', from:'L20', to:'L12', distance:5000,  time:12 },
  { id:'R04', from:'L19', to:'L02', distance:3000,  time:7  },
  { id:'R05', from:'L19', to:'L05', distance:2000,  time:5  },
  { id:'R06', from:'L19', to:'L09', distance:2000,  time:5  },
  { id:'R07', from:'L02', to:'L11', distance:8000,  time:20 },
  { id:'R08', from:'L11', to:'L16', distance:5000,  time:12 },
  { id:'R09', from:'L11', to:'L17', distance:7000,  time:18 },
  { id:'R10', from:'L16', to:'L17', distance:4000,  time:10 },
  { id:'R11', from:'L16', to:'L20', distance:10000, time:25 },
  { id:'R12', from:'L12', to:'L03', distance:2000,  time:5  },
  { id:'R13', from:'L12', to:'L04', distance:3000,  time:7  },
  { id:'R14', from:'L12', to:'L07', distance:3000,  time:8  },
  { id:'R15', from:'L07', to:'L13', distance:4000,  time:10 },
  { id:'R16', from:'L13', to:'L06', distance:2000,  time:5  },
  { id:'R17', from:'L13', to:'L14', distance:3000,  time:8  },
  { id:'R18', from:'L14', to:'L10', distance:7000,  time:18 },
  { id:'R19', from:'L10', to:'L15', distance:4000,  time:10 },
  { id:'R20', from:'L15', to:'L18', distance:5000,  time:12 },
  { id:'R21', from:'L18', to:'L08', distance:6000,  time:15 },
  { id:'R22', from:'L08', to:'L12', distance:8000,  time:20 },
  { id:'R23', from:'L06', to:'L01', distance:6000,  time:15 },
  { id:'R24', from:'L05', to:'L01', distance:4000,  time:10 },
  { id:'R25', from:'L03', to:'L04', distance:2000,  time:5  },
  { id:'R26', from:'L18', to:'L13', distance:8000,  time:20 },
  { id:'R27', from:'L04', to:'L20', distance:4000,  time:10 },
  { id:'R28', from:'L11', to:'L19', distance:6000,  time:15 },
  { id:'R29', from:'L09', to:'L01', distance:5000,  time:12 },
  { id:'R30', from:'L14', to:'L06', distance:3000,  time:8  },
];

/* ─────────────────────────────────────────────────────────
   2. HELPERS & ADJACENCY LIST
   ───────────────────────────────────────────────────────── */
const idToIndex = Object.fromEntries(LOCATIONS.map((l, i) => [l.id, i]));

const TYPE_COLOR = {
  'Hospital': '#ef4444', 'IT Hub': '#8b5cf6',
  'Transport Hub': '#f59e0b', 'Landmark': '#f97316', 'Area': '#3b82f6',
};
const TYPE_EMOJI = {
  'Hospital': '🏥', 'IT Hub': '💻', 'Transport Hub': '🚉',
  'Landmark': '⭐', 'Area': '📍',
};

function buildAdjList() {
  const adj = LOCATIONS.map(() => []);
  for (const r of ROADS) {
    const f = idToIndex[r.from], t = idToIndex[r.to];
    adj[f].push({ to: t, distance: r.distance, time: r.time, roadId: r.id });
    adj[t].push({ to: f, distance: r.distance, time: r.time, roadId: r.id });
  }
  return adj;
}
const ADJ = buildAdjList();

/* ─────────────────────────────────────────────────────────
   3. DIJKSTRA'S ALGORITHM (same O(V²) logic as dijkstra.c)
   ───────────────────────────────────────────────────────── */
function dijkstra(src, dest, mode) {
  const n = LOCATIONS.length;
  const dist = new Array(n).fill(Infinity);
  const vis  = new Array(n).fill(false);
  const par  = new Array(n).fill(-1);
  dist[src] = 0;

  for (let iter = 0; iter < n; iter++) {
    let u = -1;
    for (let i = 0; i < n; i++) {
      if (!vis[i] && (u === -1 || dist[i] < dist[u])) u = i;
    }
    if (u === -1 || dist[u] === Infinity) break;
    vis[u] = true;
    if (u === dest) break;

    for (const nb of ADJ[u]) {
      const cost = mode === 'distance' ? nb.distance : nb.time;
      if (!vis[nb.to] && dist[u] + cost < dist[nb.to]) {
        dist[nb.to] = dist[u] + cost;
        par[nb.to] = u;
      }
    }
  }
  if (dist[dest] === Infinity) return null;
  const path = [];
  let cur = dest;
  while (cur !== -1) { path.unshift(cur); cur = par[cur]; }
  return { path, cost: dist[dest], distArr: dist };
}

/* ─────────────────────────────────────────────────────────
   4. ROAD CONDITION (simulated — deterministic)
   ───────────────────────────────────────────────────────── */
function getRoadCondition(roadIndex) {
  const conds = ['Good', 'Fair', 'Poor'];
  return conds[(roadIndex * 7 + 3) % 3];
}
function condColor(c) {
  return c === 'Good' ? '#22c55e' : c === 'Fair' ? '#eab308' : '#ef4444';
}

/* ─────────────────────────────────────────────────────────
   5. EDGE HELPERS
   ───────────────────────────────────────────────────────── */
function edgeKey(a, b) { return `${Math.min(a,b)}-${Math.max(a,b)}`; }
function pathEdgeSet(path) {
  const s = new Set();
  for (let i = 0; i < path.length - 1; i++) s.add(edgeKey(path[i], path[i+1]));
  return s;
}

/* ─────────────────────────────────────────────────────────
   6. SVG MAP
   ───────────────────────────────────────────────────────── */
const edgeEls = {};
const nodeCircles = {};

function svgEl(tag, attrs) {
  const el = document.createElementNS('http://www.w3.org/2000/svg', tag);
  for (const [k, v] of Object.entries(attrs)) el.setAttribute(k, v);
  return el;
}

function drawMap() {
  const edgesG = document.getElementById('edgesG');
  const nodesG = document.getElementById('nodesG');
  if (!edgesG || !nodesG) return;

  // Edges
  for (const r of ROADS) {
    const f = LOCATIONS[idToIndex[r.from]];
    const t = LOCATIONS[idToIndex[r.to]];
    const line = svgEl('line', {
      x1: f.x, y1: f.y, x2: t.x, y2: t.y, class: 'map-edge',
    });
    edgesG.appendChild(line);
    edgeEls[edgeKey(idToIndex[r.from], idToIndex[r.to])] = line;

    const lbl = svgEl('text', {
      x: (f.x + t.x) / 2, y: (f.y + t.y) / 2 - 5,
      'text-anchor': 'middle', class: 'edge-lbl',
    });
    lbl.textContent = (r.distance / 1000).toFixed(1) + 'km';
    edgesG.appendChild(lbl);
  }

  // Nodes
  for (const loc of LOCATIONS) {
    const color = TYPE_COLOR[loc.type] || '#3b82f6';
    const g = svgEl('g', { class: 'map-node', 'data-id': loc.id });

    const ring = svgEl('circle', { cx: loc.x, cy: loc.y, r: 20, class: 'node-ring' });
    ring.style.stroke = color;

    const circle = svgEl('circle', { cx: loc.x, cy: loc.y, r: 14, class: 'node-circle' });
    circle.style.fill = color;
    circle.style.stroke = color;

    const icon = svgEl('text', {
      x: loc.x, y: loc.y + 4, 'text-anchor': 'middle', class: 'node-icon',
    });
    icon.textContent = TYPE_EMOJI[loc.type] || '📍';

    const idLbl = svgEl('text', {
      x: loc.x, y: loc.y + 29, 'text-anchor': 'middle', class: 'node-id',
    });
    idLbl.textContent = loc.id;

    const shortName = loc.name.length > 18 ? loc.name.slice(0, 17) + '…' : loc.name;
    const nameLbl = svgEl('text', {
      x: loc.x, y: loc.y + 40, 'text-anchor': 'middle', class: 'node-name',
    });
    nameLbl.textContent = shortName;

    g.append(ring, circle, icon, idLbl, nameLbl);
    nodesG.appendChild(g);
    nodeCircles[loc.id] = { ring, circle, g };

    g.addEventListener('click', () => onNodeClick(loc.id));
  }
}

/* ─────────────────────────────────────────────────────────
   7. NODE CLICK — alternates start / end
   ───────────────────────────────────────────────────────── */
let clickPhase = 'start';
function onNodeClick(locId) {
  const startSel = document.getElementById('startSel');
  const endSel = document.getElementById('endSel');
  if (clickPhase === 'start') {
    startSel.value = locId;
    clickPhase = 'end';
  } else {
    endSel.value = locId;
    clickPhase = 'start';
  }
  refreshNodeColors();
}

/* ─────────────────────────────────────────────────────────
   8. VISUAL STATE
   ───────────────────────────────────────────────────────── */
function resetEdgeStyles() {
  for (const line of Object.values(edgeEls)) {
    line.className.baseVal = 'map-edge';
    line.style.animationDelay = '';
  }
}

function highlightEdges(path, cssClass) {
  const edges = pathEdgeSet(path);
  for (const [key, line] of Object.entries(edgeEls))
    if (edges.has(key)) line.classList.add(cssClass);
}

function animateRoute(path) {
  const edges = pathEdgeSet(path);
  let delay = 0;
  for (let i = 0; i < path.length - 1; i++) {
    const key = edgeKey(path[i], path[i + 1]);
    const line = edgeEls[key];
    if (line) {
      line.classList.add('edge-path', 'edge-animated');
      line.style.animationDelay = delay + 's';
      delay += 0.15;
    }
  }
}

function refreshNodeColors(distPath = [], timePath = []) {
  const startId = document.getElementById('startSel')?.value;
  const endId = document.getElementById('endSel')?.value;

  for (const loc of LOCATIONS) {
    const n = nodeCircles[loc.id];
    if (!n) continue;
    const { ring, circle } = n;
    ring.classList.remove('ring-start', 'ring-end', 'ring-path');
    circle.classList.remove('circle-start', 'circle-end', 'circle-path');

    if (loc.id === startId) {
      ring.classList.add('ring-start');
      circle.classList.add('circle-start');
    } else if (loc.id === endId) {
      ring.classList.add('ring-end');
      circle.classList.add('circle-end');
    } else if (distPath.includes(idToIndex[loc.id]) || timePath.includes(idToIndex[loc.id])) {
      ring.classList.add('ring-path');
      circle.classList.add('circle-path');
    }
  }
}

/* ─────────────────────────────────────────────────────────
   9. RESULT CARD BUILDER
   ───────────────────────────────────────────────────────── */
function buildCard(result, mode) {
  if (!result) {
    return `<div class="res-card err-card">
      <div class="res-title">❌ No Route Found</div>
      <p style="color:var(--muted);font-size:.84rem">These locations are not connected in the network.</p>
    </div>`;
  }
  const icon = mode === 'distance' ? '📏' : '⏱️';
  const title = mode === 'distance' ? 'Shortest Distance Route' : 'Fastest Time Route';
  const cls = mode === 'distance' ? 'res-card dist-card' : 'res-card time-card';
  const costLabel = mode === 'distance'
    ? `${(result.cost / 1000).toFixed(2)} km (${result.cost} m)`
    : `${result.cost} minutes`;

  const steps = result.path.map((idx, i) => {
    const dot = i === 0 ? '🟢' : i === result.path.length - 1 ? '🔴' : '🔵';
    const loc = LOCATIONS[idx];
    return `<li class="step-item">
      <span class="step-dot">${dot}</span>
      <span class="step-name">${loc.name}</span>
      <span class="step-type">${loc.type}</span>
    </li>`;
  }).join('');

  // Road conditions along the path
  let condHtml = '';
  for (let i = 0; i < result.path.length - 1; i++) {
    const a = result.path[i], b = result.path[i + 1];
    const road = ROADS.find(r =>
      (idToIndex[r.from] === a && idToIndex[r.to] === b) ||
      (idToIndex[r.from] === b && idToIndex[r.to] === a)
    );
    if (road) {
      const rIdx = ROADS.indexOf(road);
      const cond = getRoadCondition(rIdx);
      condHtml += `<span style="color:${condColor(cond)};font-weight:600">${road.id}:${cond}</span> `;
    }
  }

  return `<div class="${cls}">
    <div class="res-title">${icon} ${title}</div>
    <ul class="step-list">${steps}</ul>
    <div class="res-total">
      <span>Total ${mode === 'distance' ? 'Distance' : 'Time'}:</span>
      <strong>${costLabel}</strong>
    </div>
    <div class="res-stops">${result.path.length - 1} hop(s) · ${result.path.length} location(s)</div>
    <div class="res-condition">🛣️ Road conditions (simulated): ${condHtml}</div>
  </div>`;
}

/* ─────────────────────────────────────────────────────────
   10. ANALYSIS BUILDER
   ───────────────────────────────────────────────────────── */
function buildAnalysis(src, dest) {
  const dr = dijkstra(src, dest, 'distance');
  const tr = dijkstra(src, dest, 'time');
  const area = document.getElementById('analysisContent');
  if (!area) return;

  if (!dr && !tr) {
    area.innerHTML = `<div class="analysis-empty"><div class="empty-icon">❌</div><p>No routes found between these locations.</p></div>`;
    return;
  }

  // Compute total time for distance route and total distance for time route
  function routeMetrics(result) {
    if (!result) return { dist: 0, time: 0, conditions: [] };
    let totalDist = 0, totalTime = 0;
    const conditions = [];
    for (let i = 0; i < result.path.length - 1; i++) {
      const a = result.path[i], b = result.path[i + 1];
      const road = ROADS.find(r =>
        (idToIndex[r.from] === a && idToIndex[r.to] === b) ||
        (idToIndex[r.from] === b && idToIndex[r.to] === a)
      );
      if (road) {
        totalDist += road.distance;
        totalTime += road.time;
        conditions.push(getRoadCondition(ROADS.indexOf(road)));
      }
    }
    return { dist: totalDist, time: totalTime, conditions };
  }

  const dm = dr ? routeMetrics(dr) : null;
  const tm = tr ? routeMetrics(tr) : null;

  const maxDist = Math.max(dm?.dist || 0, tm?.dist || 0);
  const maxTime = Math.max(dm?.time || 0, tm?.time || 0);

  const sameRoute = dr && tr && JSON.stringify(dr.path) === JSON.stringify(tr.path);

  // Recommendation logic
  let recText = '';
  if (sameRoute) {
    recText = '✅ Both modes produce the <strong>same optimal route</strong> — this is the best path by every metric!';
  } else if (dr && tr) {
    const timeConditions = tm.conditions;
    const hasPoor = timeConditions.includes('Poor');
    if (hasPoor) {
      recText = `⚠️ The <strong>fastest route</strong> (${tm.time} min) has segments with <strong style="color:#ef4444">Poor</strong> road conditions. Consider the <strong>shortest distance route</strong> (${(dm.dist/1000).toFixed(1)} km, ${dm.time} min) for a safer path.`;
    } else {
      recText = `🚑 For emergencies, take the <strong>fastest route</strong> (${tm.time} min). The distance route is ${(dm.dist/1000).toFixed(1)} km but takes ${dm.time} min.`;
    }
  }

  area.innerHTML = `
    <div class="analysis-grid">
      <div class="analysis-card glass-card">
        <h4>📊 Distance Comparison</h4>
        <div class="bar-chart">
          <div class="bar-row">
            <span class="bar-label">Distance</span>
            <div class="bar-track"><div class="bar-fill fill-dist" style="width:${dm ? (dm.dist/maxDist*100) : 0}%">${dm ? (dm.dist/1000).toFixed(1)+'km' : ''}</div></div>
          </div>
          <div class="bar-row">
            <span class="bar-label">Time</span>
            <div class="bar-track"><div class="bar-fill fill-time" style="width:${tm ? (tm.dist/maxDist*100) : 0}%">${tm ? (tm.dist/1000).toFixed(1)+'km' : ''}</div></div>
          </div>
        </div>
        <p style="font-size:.72rem;color:var(--muted);margin-top:.5rem">Total distance of each route (km)</p>
      </div>
      <div class="analysis-card glass-card">
        <h4>⏱️ Travel Time Comparison</h4>
        <div class="bar-chart">
          <div class="bar-row">
            <span class="bar-label">Distance</span>
            <div class="bar-track"><div class="bar-fill fill-dist" style="width:${dm ? (dm.time/maxTime*100) : 0}%">${dm ? dm.time+'min' : ''}</div></div>
          </div>
          <div class="bar-row">
            <span class="bar-label">Time</span>
            <div class="bar-track"><div class="bar-fill fill-time" style="width:${tm ? (tm.time/maxTime*100) : 0}%">${tm ? tm.time+'min' : ''}</div></div>
          </div>
        </div>
        <p style="font-size:.72rem;color:var(--muted);margin-top:.5rem">Travel time of each route (minutes)</p>
      </div>
      <div class="analysis-card glass-card" style="grid-column:1/-1">
        <h4>🛣️ Route Comparison Table</h4>
        <div class="table-wrapper">
          <table class="data-table">
            <thead><tr><th>Metric</th><th>📏 Distance Route</th><th>⏱️ Time Route</th></tr></thead>
            <tbody>
              <tr><td>Total Distance</td><td>${dm ? (dm.dist/1000).toFixed(2)+' km' : 'N/A'}</td><td>${tm ? (tm.dist/1000).toFixed(2)+' km' : 'N/A'}</td></tr>
              <tr><td>Travel Time</td><td>${dm ? dm.time+' min' : 'N/A'}</td><td>${tm ? tm.time+' min' : 'N/A'}</td></tr>
              <tr><td>Hops</td><td>${dr ? dr.path.length-1 : 'N/A'}</td><td>${tr ? tr.path.length-1 : 'N/A'}</td></tr>
              <tr><td>Road Conditions</td><td>${dm ? dm.conditions.join(', ') : 'N/A'}</td><td>${tm ? tm.conditions.join(', ') : 'N/A'}</td></tr>
            </tbody>
          </table>
        </div>
      </div>
      <div class="recommendation-card glass-card" style="grid-column:1/-1">
        <h4>💡 Recommendation</h4>
        <p style="font-size:.88rem;line-height:1.7;color:var(--text)">${recText}</p>
        <p style="font-size:.7rem;color:var(--muted);margin-top:.5rem">⚠️ Road conditions are simulated for demonstration. Not real-time data.</p>
      </div>
    </div>`;

  // Animate bars after a tick
  requestAnimationFrame(() => {
    area.querySelectorAll('.bar-fill').forEach(b => {
      const w = b.style.width;
      b.style.width = '0%';
      requestAnimationFrame(() => { b.style.width = w; });
    });
  });
}

/* ─────────────────────────────────────────────────────────
   11. ROUTES FOUND TRACKER
   ───────────────────────────────────────────────────────── */
let routesFound = 0;
function incrementRoutes() {
  routesFound++;
  const el = document.getElementById('routesFoundCounter');
  if (el) el.textContent = routesFound;
}

/* ─────────────────────────────────────────────────────────
   12. BUTTON EVENT HANDLERS
   ───────────────────────────────────────────────────────── */
function initButtons() {
  // Find Route
  document.getElementById('findBtn')?.addEventListener('click', () => {
    const src  = idToIndex[document.getElementById('startSel').value];
    const dest = idToIndex[document.getElementById('endSel').value];
    const mode = document.querySelector('input[name="mode"]:checked')?.value || 'distance';

    if (src === undefined || dest === undefined) { showErr('Please select both locations.'); return; }
    if (src === dest) { showErr('Start and destination cannot be the same location.'); return; }

    const result = dijkstra(src, dest, mode);
    resetEdgeStyles();
    if (result) {
      animateRoute(result.path);
      incrementRoutes();
    }
    refreshNodeColors(result ? result.path : [], []);
    document.getElementById('resultsArea').innerHTML = buildCard(result, mode);
  });

  // Compare Both
  document.getElementById('compareBtn')?.addEventListener('click', () => {
    const src  = idToIndex[document.getElementById('startSel').value];
    const dest = idToIndex[document.getElementById('endSel').value];

    if (src === undefined || dest === undefined) { showErr('Please select both locations.'); return; }
    if (src === dest) { showErr('Start and destination cannot be the same location.'); return; }

    const dr = dijkstra(src, dest, 'distance');
    const tr = dijkstra(src, dest, 'time');

    resetEdgeStyles();
    if (dr) highlightEdges(dr.path, 'edge-dist');
    if (tr) highlightEdges(tr.path, 'edge-time');
    refreshNodeColors(dr?.path ?? [], tr?.path ?? []);

    const same = dr && tr && JSON.stringify(dr.path) === JSON.stringify(tr.path);
    let recommend = '';
    if (dr && tr) {
      recommend = same
        ? `<div class="recommend">✅ Both modes give the <strong>same route</strong> — this is the optimal path!</div>`
        : `<div class="recommend">💡 Take the <strong>Time route</strong> to arrive in ${tr.cost} min, or the <strong>Distance route</strong> for the physically shorter ${(dr.cost/1000).toFixed(2)} km path.</div>`;
    }

    const legend = `<p class="compare-legend">
      <span class="leg-dist">━━</span> Distance route &nbsp;
      <span class="leg-time">╌╌</span> Time route
    </p>`;

    document.getElementById('resultsArea').innerHTML =
      legend + buildCard(dr, 'distance') + buildCard(tr, 'time') + recommend;

    incrementRoutes();
    buildAnalysis(src, dest);
  });

  // Reset
  document.getElementById('resetBtn')?.addEventListener('click', () => {
    resetEdgeStyles();
    for (const loc of LOCATIONS) {
      const n = nodeCircles[loc.id];
      if (!n) continue;
      n.ring.classList.remove('ring-start', 'ring-end', 'ring-path');
      n.circle.classList.remove('circle-start', 'circle-end', 'circle-path');
    }
    document.getElementById('resultsArea').innerHTML = `
      <div class="results-empty">
        <div class="empty-icon">🗺️</div>
        <p>Select start & end locations, then click <strong>Find Best Route</strong> to see results.</p>
      </div>`;
    const analysisContent = document.getElementById('analysisContent');
    if (analysisContent) {
      analysisContent.innerHTML = `<div class="analysis-empty">
        <div class="empty-icon">📊</div>
        <p>Run a route comparison from the Route Finder to see detailed analysis here.</p>
        <a href="#finder" class="btn btn-outline">Go to Route Finder</a>
      </div>`;
    }
  });

  // Mode card toggle
  document.querySelectorAll('.mode-card').forEach(card => {
    card.addEventListener('click', () => {
      const radio = card.querySelector('input[type="radio"]');
      if (radio) {
        radio.checked = true;
        document.querySelectorAll('.mode-card').forEach(c => c.classList.remove('active'));
        card.classList.add('active');
      }
    });
  });

  // Dropdown change → refresh node highlights
  document.getElementById('startSel')?.addEventListener('change', () => refreshNodeColors());
  document.getElementById('endSel')?.addEventListener('change', () => refreshNodeColors());

  // Quick Finder
  document.getElementById('quickFindBtn')?.addEventListener('click', () => {
    const qs = document.getElementById('quickStart')?.value;
    const qe = document.getElementById('quickEnd')?.value;
    if (!qs || !qe) return;
    const startSel = document.getElementById('startSel');
    const endSel = document.getElementById('endSel');
    if (startSel) startSel.value = qs;
    if (endSel) endSel.value = qe;
    document.getElementById('finder')?.scrollIntoView({ behavior: 'smooth' });
    setTimeout(() => document.getElementById('findBtn')?.click(), 600);
  });
}

/* ─────────────────────────────────────────────────────────
   13. TABS
   ───────────────────────────────────────────────────────── */
function initTabs() {
  document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
      btn.classList.add('active');
      document.getElementById(btn.dataset.tab)?.classList.add('active');
    });
  });
}

/* ─────────────────────────────────────────────────────────
   14. NAVBAR — scroll-spy + mobile toggle
   ───────────────────────────────────────────────────────── */
function initNavbar() {
  const navbar = document.getElementById('navbar');
  const sections = ['landing', 'dashboard', 'finder', 'analysis', 'algorithm', 'about'];

  window.addEventListener('scroll', () => {
    navbar?.classList.toggle('scrolled', window.scrollY > 50);
    for (const id of sections) {
      const el = document.getElementById(id);
      if (!el) continue;
      const rect = el.getBoundingClientRect();
      if (rect.top <= 80 && rect.bottom > 80) {
        document.querySelectorAll('.nav-link').forEach(a => a.classList.remove('active'));
        document.querySelector(`.nav-link[href="#${id}"]`)?.classList.add('active');
      }
    }
  });

  document.getElementById('navToggle')?.addEventListener('click', () => {
    document.getElementById('navLinks')?.classList.toggle('open');
  });

  // Close mobile nav on link click
  document.querySelectorAll('.nav-link').forEach(link => {
    link.addEventListener('click', () => {
      document.getElementById('navLinks')?.classList.remove('open');
    });
  });
}

/* ─────────────────────────────────────────────────────────
   15. LOADING SCREEN
   ───────────────────────────────────────────────────────── */
function initLoader() {
  setTimeout(() => {
    const loader = document.getElementById('loader');
    if (loader) {
      loader.classList.add('hidden');
      setTimeout(() => { loader.style.display = 'none'; }, 600);
    }
  }, 2200);
}

/* ─────────────────────────────────────────────────────────
   16. HERO CANVAS — animated network particles
   ───────────────────────────────────────────────────────── */
function initHeroCanvas() {
  const canvas = document.getElementById('heroCanvas');
  if (!canvas) return;
  const ctx = canvas.getContext('2d');
  let w, h;
  const particles = [];
  const PARTICLE_COUNT = 45;

  function resize() {
    w = canvas.width = canvas.offsetWidth;
    h = canvas.height = canvas.offsetHeight;
  }
  resize();
  window.addEventListener('resize', resize);

  for (let i = 0; i < PARTICLE_COUNT; i++) {
    particles.push({
      x: Math.random() * w,
      y: Math.random() * h,
      vx: (Math.random() - 0.5) * 0.6,
      vy: (Math.random() - 0.5) * 0.6,
      r: 1.5 + Math.random() * 2,
      opacity: 0.3 + Math.random() * 0.5,
    });
  }

  function draw() {
    ctx.clearRect(0, 0, w, h);
    // Lines
    for (let i = 0; i < particles.length; i++) {
      for (let j = i + 1; j < particles.length; j++) {
        const dx = particles[i].x - particles[j].x;
        const dy = particles[i].y - particles[j].y;
        const d = Math.sqrt(dx * dx + dy * dy);
        if (d < 150) {
          ctx.beginPath();
          ctx.moveTo(particles[i].x, particles[i].y);
          ctx.lineTo(particles[j].x, particles[j].y);
          ctx.strokeStyle = `rgba(59,130,246,${0.12 * (1 - d / 150)})`;
          ctx.lineWidth = 0.8;
          ctx.stroke();
        }
      }
    }
    // Dots
    for (const p of particles) {
      ctx.beginPath();
      ctx.arc(p.x, p.y, p.r, 0, Math.PI * 2);
      ctx.fillStyle = `rgba(59,130,246,${p.opacity})`;
      ctx.fill();

      p.x += p.vx;
      p.y += p.vy;
      if (p.x < 0) p.x = w;
      if (p.x > w) p.x = 0;
      if (p.y < 0) p.y = h;
      if (p.y > h) p.y = 0;
    }
    requestAnimationFrame(draw);
  }
  draw();
}

/* ─────────────────────────────────────────────────────────
   17. ANIMATED COUNTERS
   ───────────────────────────────────────────────────────── */
function initCounters() {
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) {
        entry.target.querySelectorAll('.counter').forEach(counter => {
          const target = parseInt(counter.getAttribute('data-target'));
          if (!target || counter.dataset.counted) return;
          counter.dataset.counted = 'true';
          let current = 0;
          const step = Math.max(1, Math.floor(target / 35));
          const interval = setInterval(() => {
            current += step;
            if (current >= target) { current = target; clearInterval(interval); }
            counter.textContent = current;
          }, 45);
        });
        observer.unobserve(entry.target);
      }
    });
  }, { threshold: 0.3 });

  document.querySelectorAll('.dashboard-stats').forEach(el => observer.observe(el));
}

/* ─────────────────────────────────────────────────────────
   18. SCROLL ANIMATIONS
   ───────────────────────────────────────────────────────── */
function initScrollAnimations() {
  const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if (entry.isIntersecting) entry.target.classList.add('visible');
    });
  }, { threshold: 0.08, rootMargin: '0px 0px -40px 0px' });

  document.querySelectorAll('.animate-on-scroll').forEach(el => observer.observe(el));
}

/* ─────────────────────────────────────────────────────────
   19. POPULATE UI
   ───────────────────────────────────────────────────────── */
function populateUI() {
  const selects = ['startSel', 'endSel', 'quickStart', 'quickEnd'];
  for (const selId of selects) {
    const sel = document.getElementById(selId);
    if (!sel) continue;
    // Keep the first option if it exists
    for (const loc of LOCATIONS) {
      const emoji = TYPE_EMOJI[loc.type] || '📍';
      sel.appendChild(new Option(`${emoji} ${loc.id} — ${loc.name}`, loc.id));
    }
  }

  // Defaults
  const startSel = document.getElementById('startSel');
  const endSel = document.getElementById('endSel');
  if (startSel) startSel.value = 'L01';
  if (endSel) endSel.value = 'L08';

  // Location table
  const locBody = document.getElementById('locTbody');
  if (locBody) {
    for (const loc of LOCATIONS) {
      const tr = document.createElement('tr');
      tr.innerHTML = `<td>${loc.id}</td><td>${TYPE_EMOJI[loc.type] || ''} ${loc.name}</td><td>${loc.type}</td>`;
      locBody.appendChild(tr);
    }
  }

  // Road table
  const roadBody = document.getElementById('roadTbody');
  if (roadBody) {
    for (const r of ROADS) {
      const f = LOCATIONS[idToIndex[r.from]];
      const t = LOCATIONS[idToIndex[r.to]];
      const tr = document.createElement('tr');
      tr.innerHTML = `<td>${r.id}</td>
        <td>${r.from} · ${f.name}</td>
        <td>${r.to} · ${t.name}</td>
        <td>${(r.distance / 1000).toFixed(1)} km</td>
        <td>${r.time} min</td>`;
      roadBody.appendChild(tr);
    }
  }

  refreshNodeColors();
}

/* ─────────────────────────────────────────────────────────
   20. ERROR DISPLAY
   ───────────────────────────────────────────────────────── */
function showErr(msg) {
  document.getElementById('resultsArea').innerHTML =
    `<div class="results-empty" style="color:var(--red)">
       <div class="empty-icon">⚠️</div><p>${msg}</p>
     </div>`;
}

/* ─────────────────────────────────────────────────────────
   21. INITIALIZATION
   ───────────────────────────────────────────────────────── */
document.addEventListener('DOMContentLoaded', () => {
  initLoader();
  initHeroCanvas();
  drawMap();
  populateUI();
  initButtons();
  initTabs();
  initNavbar();
  initCounters();
  initScrollAnimations();
});
