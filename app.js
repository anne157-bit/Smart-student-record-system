// ============================================================
//  SMART STUDENT RECORD SYSTEM — JavaScript Port
//  All DSA structures implemented from scratch (no library)
// ============================================================

// ============================================================
//  THEME TOGGLE
// ============================================================
function toggleTheme() {
  const html  = document.documentElement;
  const isDark = html.getAttribute('data-theme') === 'dark';
  const next   = isDark ? 'light' : 'dark';
  html.setAttribute('data-theme', next);
  localStorage.setItem('ssrs-theme', next);
  updateThemeUI(next);
  // Re-render BST SVG (colors change)
  if (document.getElementById('bst-svg')) renderBSTViz();
}

function updateThemeUI(theme) {
  const label = document.getElementById('theme-label');
  const thumb = document.getElementById('theme-thumb');
  if (label) label.textContent = theme === 'dark' ? 'Dark' : 'Light';
  if (thumb) thumb.textContent = theme === 'dark' ? '🌙' : '☀️';
}

function initTheme() {
  const saved = localStorage.getItem('ssrs-theme') || 'light';
  document.documentElement.setAttribute('data-theme', saved);
  updateThemeUI(saved);
}

initTheme();

// ── 1. DOUBLY LINKED LIST ──────────────────────────────────
class DLLNode {
  constructor(student) {
    this.data = { ...student };
    this.prev = null;
    this.next = null;
  }
}

class DoublyLinkedList {
  constructor() { this.head = null; this.tail = null; this.size = 0; }

  insertBack(student) {
    const node = new DLLNode(student);
    if (!this.tail) { this.head = this.tail = node; }
    else { node.prev = this.tail; this.tail.next = node; this.tail = node; }
    this.size++;
  }

  findByRoll(roll) {
    let cur = this.head;
    while (cur) { if (cur.data.rollNo === roll) return cur; cur = cur.next; }
    return null;
  }

  deleteByRoll(roll) {
    const node = this.findByRoll(roll);
    if (!node) return false;
    if (node.prev) node.prev.next = node.next; else this.head = node.next;
    if (node.next) node.next.prev = node.prev; else this.tail = node.prev;
    this.size--;
    return true;
  }

  toArrayForward() {
    const arr = []; let cur = this.head;
    while (cur) { arr.push({ ...cur.data }); cur = cur.next; }
    return arr;
  }

  toArrayBackward() {
    const arr = []; let cur = this.tail;
    while (cur) { arr.push({ ...cur.data }); cur = cur.prev; }
    return arr;
  }
}

// ── 2. BINARY SEARCH TREE ─────────────────────────────────
class BSTNode {
  constructor(student) { this.data = { ...student }; this.left = null; this.right = null; }
}

class BST {
  constructor() { this.root = null; }

  insert(student) { this.root = this._insert(this.root, student); }
  _insert(node, s) {
    if (!node) return new BSTNode(s);
    if (s.rollNo < node.data.rollNo) node.left = this._insert(node.left, s);
    else if (s.rollNo > node.data.rollNo) node.right = this._insert(node.right, s);
    return node;
  }

  search(roll) { return this._search(this.root, roll); }
  _search(node, roll) {
    if (!node) return null;
    if (node.data.rollNo === roll) return node;
    return roll < node.data.rollNo ? this._search(node.left, roll) : this._search(node.right, roll);
  }

  inOrder() {
    const arr = [];
    const rec = (node) => { if (!node) return; rec(node.left); arr.push({ ...node.data }); rec(node.right); };
    rec(this.root);
    return arr;
  }

  deleteNode(roll) { this.root = this._delete(this.root, roll); }
  _delete(node, roll) {
    if (!node) return null;
    if (roll < node.data.rollNo) { node.left = this._delete(node.left, roll); }
    else if (roll > node.data.rollNo) { node.right = this._delete(node.right, roll); }
    else {
      if (!node.left) return node.right;
      if (!node.right) return node.left;
      let succ = node.right;
      while (succ.left) succ = succ.left;
      node.data = { ...succ.data };
      node.right = this._delete(node.right, succ.data.rollNo);
    }
    return node;
  }

  // Returns structure for visualization
  getStructure() {
    const nodes = [], edges = [];
    const rec = (node, parentId, side) => {
      if (!node) return;
      const id = node.data.rollNo;
      nodes.push({ id, roll: node.data.rollNo, name: node.data.name, cgpa: node.data.cgpa });
      if (parentId !== null) edges.push({ from: parentId, to: id, side });
      rec(node.left, id, 'left');
      rec(node.right, id, 'right');
    };
    rec(this.root, null, null);
    return { nodes, edges, root: this.root ? this.root.data.rollNo : null };
  }
}

// ── 3. HASH TABLE (djb2 + Separate Chaining) ──────────────
const HASH_SIZE = 101;

class HashTable {
  constructor() { this.table = new Array(HASH_SIZE).fill(null); }

  _hash(key) {
    let h = 5381;
    for (let i = 0; i < key.length; i++) h = ((h << 5) + h) + key.charCodeAt(i);
    return Math.abs(h) % HASH_SIZE;
  }
  _lower(s) { return s.toLowerCase(); }

  insert(student) {
    const k = this._lower(student.email);
    const idx = this._hash(k);
    let cur = this.table[idx];
    while (cur) {
      if (cur.key === k) { cur.data = { ...student }; return; }
      cur = cur.next;
    }
    this.table[idx] = { key: k, data: { ...student }, next: this.table[idx] };
  }

  search(email) {
    const k = this._lower(email);
    const idx = this._hash(k);
    let cur = this.table[idx];
    while (cur) { if (cur.key === k) return cur.data; cur = cur.next; }
    return null;
  }

  remove(email) {
    const k = this._lower(email);
    const idx = this._hash(k);
    let cur = this.table[idx], prev = null;
    while (cur) {
      if (cur.key === k) {
        if (prev) prev.next = cur.next; else this.table[idx] = cur.next;
        return true;
      }
      prev = cur; cur = cur.next;
    }
    return false;
  }

  getBuckets() {
    return this.table.map((node, i) => {
      const items = [];
      let cur = node;
      while (cur) { items.push(cur.key); cur = cur.next; }
      return { idx: i, items };
    });
  }
}

// ── 4. MERGE SORT ─────────────────────────────────────────
function mergeSort(arr, steps = []) {
  if (arr.length <= 1) return arr;
  const mid = Math.floor(arr.length / 2);
  const L = mergeSort(arr.slice(0, mid), steps);
  const R = mergeSort(arr.slice(mid), steps);
  return _merge(L, R, steps);
}

function _merge(L, R, steps) {
  const result = [];
  let i = 0, j = 0;
  while (i < L.length && j < R.length) {
    if (L[i].cgpa >= R[j].cgpa) {
      steps && steps.push(`Comparing ${L[i].name}(${L[i].cgpa}) >= ${R[j].name}(${R[j].cgpa}) → take ${L[i].name}`);
      result.push(L[i++]);
    } else {
      steps && steps.push(`Comparing ${L[i].name}(${L[i].cgpa}) < ${R[j].name}(${R[j].cgpa}) → take ${R[j].name}`);
      result.push(R[j++]);
    }
  }
  while (i < L.length) result.push(L[i++]);
  while (j < R.length) result.push(R[j++]);
  return result;
}

// ── 5. MAX-HEAP (Priority Queue) ──────────────────────────
class MaxHeap {
  constructor() { this.heap = []; }

  get size() { return this.heap.length; }
  isEmpty()  { return this.heap.length === 0; }
  peek()     { return this.heap[0] || null; }

  insert(req) {
    this.heap.push({ ...req });
    this._siftUp(this.heap.length - 1);
  }

  extractMax() {
    if (this.isEmpty()) return null;
    const top = this.heap[0];
    const last = this.heap.pop();
    if (this.heap.length > 0) { this.heap[0] = last; this._siftDown(0); }
    return top;
  }

  _siftUp(i) {
    while (i > 0) {
      const parent = Math.floor((i - 1) / 2);
      if (this.heap[parent].priority < this.heap[i].priority) {
        [this.heap[parent], this.heap[i]] = [this.heap[i], this.heap[parent]];
        i = parent;
      } else break;
    }
  }

  _siftDown(i) {
    const n = this.heap.length;
    while (true) {
      let largest = i, l = 2*i+1, r = 2*i+2;
      if (l < n && this.heap[l].priority > this.heap[largest].priority) largest = l;
      if (r < n && this.heap[r].priority > this.heap[largest].priority) largest = r;
      if (largest !== i) {
        [this.heap[i], this.heap[largest]] = [this.heap[largest], this.heap[i]];
        i = largest;
      } else break;
    }
  }
}

// ============================================================
//  GLOBAL STATE & C++ BACKEND CLIENT
// ============================================================
const masterList = new DoublyLinkedList();
const rollBST    = new BST();
const emailHash  = new HashTable();
const helpdesk   = new MaxHeap();

// C++ REST Backend configuration
const API_BASE = window.location.origin && window.location.origin.includes('localhost:8080')
  ? ''
  : 'http://localhost:8080';

let isBackendConnected = false;

async function checkBackendHealth(showToast = false) {
  const badge = document.getElementById('backend-status');
  const text  = document.getElementById('backend-status-text');
  if (badge) badge.className = 'backend-status-pill checking';
  if (text) text.textContent = 'Connecting C++...';

  try {
    const res = await fetch(`${API_BASE}/api/status`, { cache: 'no-store' });
    if (res.ok) {
      const data = await res.json();
      if (data.backend) {
        isBackendConnected = true;
        if (badge) badge.className = 'backend-status-pill online';
        if (text) text.textContent = '⚡ C++ Backend (Port 8080)';
        if (showToast) toast('Connected to C++ Backend Server (Native DSA Engine)', 'success');
        await syncFromCppBackend();
        return true;
      }
    }
  } catch (err) {
    // Offline or server not yet running
  }

  isBackendConnected = false;
  if (badge) badge.className = 'backend-status-pill offline';
  if (text) text.textContent = '⚪ C++ Offline (Run server.exe)';
  if (showToast) toast('C++ server offline. Run .\\server.exe to activate C++ backend.', 'warning');
  return false;
}

async function syncFromCppBackend() {
  if (!isBackendConnected) return;
  try {
    const res = await fetch(`${API_BASE}/api/students?order=forward`);
    if (res.ok) {
      const data = await res.json();
      if (data.students) {
        masterList.head = masterList.tail = null;
        masterList.size = 0;
        rollBST.root = null;
        for (let i = 0; i < HASH_SIZE; ++i) emailHash.table[i] = null;
        data.students.forEach(syncInsert);
        updateStats();
      }
    }
    const hdRes = await fetch(`${API_BASE}/api/helpdesk`);
    if (hdRes.ok) {
      const hdData = await hdRes.json();
      if (hdData.requests) {
        helpdesk.heap = [...hdData.requests];
        updateStats();
      }
    }
  } catch (e) {
    console.warn('Sync from C++ backend failed:', e);
  }
}

// ── Local Fallback Seed Data ──────────────────────────────
function syncInsert(s) {
  masterList.insertBack(s);
  rollBST.insert(s);
  emailHash.insert(s);
}

function syncDelete(roll, email) {
  masterList.deleteByRoll(roll);
  rollBST.deleteNode(roll);
  emailHash.remove(email);
}

const seedData = [
  { rollNo:101, name:'Aryan Mehta',    email:'aryan.mehta@college.edu',    branch:'CSE',   year:2, cgpa:9.10 },
  { rollNo:102, name:'Priya Sharma',   email:'priya.sharma@college.edu',   branch:'ECE',   year:3, cgpa:8.75 },
  { rollNo:103, name:'Rohan Das',      email:'rohan.das@college.edu',      branch:'MECH',  year:1, cgpa:7.50 },
  { rollNo:104, name:'Sneha Patil',    email:'sneha.patil@college.edu',    branch:'CSE',   year:4, cgpa:9.45 },
  { rollNo:105, name:'Kiran Kumar',    email:'kiran.kumar@college.edu',    branch:'CIVIL', year:2, cgpa:6.80 },
  { rollNo:106, name:'Aisha Siddiqui',email:'aisha.siddiqui@college.edu', branch:'CSE',   year:3, cgpa:8.20 },
  { rollNo:107, name:'Dev Nair',       email:'dev.nair@college.edu',       branch:'ECE',   year:2, cgpa:7.90 },
  { rollNo:108, name:'Meera Iyer',     email:'meera.iyer@college.edu',     branch:'CSE',   year:1, cgpa:8.60 },
];
seedData.forEach(syncInsert);

// ============================================================
//  UI UTILITIES
// ============================================================
function updateStats() {
  document.getElementById('stat-records').textContent = masterList.size;
  document.getElementById('stat-queue').textContent   = helpdesk.size;
}

function showPage(page) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));

  const el = document.getElementById('page-' + page);
  if (el) el.classList.add('active');

  const btn = document.getElementById('nav-' + page);
  if (btn) btn.classList.add('active');

  const titles = {
    'add': 'Add New Student', 'view': 'View All Students',
    'search-roll': 'Search by Roll (BST)', 'search-email': 'Search by Email (Hash)',
    'update': 'Update CGPA', 'delete': 'Delete Student',
    'leaderboard': 'Leaderboard — Merge Sort', 'bst': 'BST In-Order View',
    'helpdesk': 'Helpdesk — Max-Heap', 'visualizer': 'DSA Visualizer'
  };
  document.getElementById('page-title').textContent = titles[page] || page;

  if (page === 'view') viewForward();
  if (page === 'leaderboard') showLeaderboard();
  if (page === 'bst') showBSTInOrder();
  if (page === 'visualizer') initVisualizer();
}

function toggleSidebar() {
  const sidebar = document.getElementById('sidebar');
  const main    = document.querySelector('.main-content');
  sidebar.classList.toggle('collapsed');
  sidebar.classList.toggle('open');
  main.classList.toggle('expanded');
}

function toast(msg, type = 'info') {
  const tc = document.getElementById('toast-container');
  const t  = document.createElement('div');
  t.className = `toast ${type}`;
  const icon = type === 'success' ? '✓' : type === 'error' ? '✕' : 'ℹ';
  t.innerHTML = `<span>${icon}</span><span>${msg}</span>`;
  tc.appendChild(t);
  setTimeout(() => t.remove(), 3500);
}

function feedback(id, msg, type = 'info') {
  const icons = { success:'✓', error:'✕', info:'ℹ', warning:'⚠' };
  document.getElementById(id).innerHTML =
    `<div class="alert alert-${type}"><span class="alert-icon">${icons[type]}</span><span>${msg}</span></div>`;
}

function clearFeedback(id) { document.getElementById(id).innerHTML = ''; }

function cgpaBadge(cgpa) {
  const cls = cgpa >= 8.5 ? 'cgpa-high' : cgpa >= 7.0 ? 'cgpa-mid' : 'cgpa-low';
  return `<span class="cgpa-badge ${cls}">${cgpa.toFixed(2)}</span>`;
}

function rankBadge(i) {
  if (i === 0) return `<span class="rank-badge rank-1">🥇</span>`;
  if (i === 1) return `<span class="rank-badge rank-2">🥈</span>`;
  if (i === 2) return `<span class="rank-badge rank-3">🥉</span>`;
  return `<span class="rank-badge rank-n">${i+1}</span>`;
}

function renderTable(containerId, rows, showRank = false) {
  if (!rows.length) {
    document.getElementById(containerId).innerHTML =
      `<div class="empty-state"><span class="empty-icon">📭</span>No records found.</div>`;
    return;
  }
  const ths = showRank
    ? ['Rank','Roll','Name','Email','Branch','Year','CGPA']
    : ['Roll','Name','Email','Branch','Year','CGPA'];

  const thead = `<thead><tr>${ths.map(h=>`<th>${h}</th>`).join('')}</tr></thead>`;
  const tbody = `<tbody>${rows.map((s, i) => `<tr>
    ${showRank ? `<td>${rankBadge(i)}</td>` : ''}
    <td><code style="color:var(--cyan)">${s.rollNo}</code></td>
    <td style="font-weight:600">${s.name}</td>
    <td style="color:var(--text-secondary);font-size:.8rem">${s.email}</td>
    <td><span class="chip chip-purple" style="font-size:.7rem">${s.branch}</span></td>
    <td style="color:var(--text-secondary)">Year ${s.year}</td>
    <td>${cgpaBadge(s.cgpa)}</td>
  </tr>`).join('')}</tbody>`;

  document.getElementById(containerId).innerHTML =
    `<div class="table-wrap"><table>${thead}${tbody}</table></div>`;
}

// ============================================================
//  STUDENT MANAGEMENT
// ============================================================

// ============================================================
//  STUDENT MANAGEMENT (Connected to C++ Backend)
// ============================================================

// ADD STUDENT
async function addStudent() {
  const roll   = parseInt(document.getElementById('add-roll').value);
  const name   = document.getElementById('add-name').value.trim();
  const email  = document.getElementById('add-email').value.trim();
  const branch = document.getElementById('add-branch').value.trim();
  const year   = parseInt(document.getElementById('add-year').value);
  const cgpa   = parseFloat(document.getElementById('add-cgpa').value);

  if (!roll || !name || !email || !branch || !year || isNaN(cgpa)) {
    feedback('add-feedback', 'Please fill in all fields.', 'error'); return;
  }
  if (year < 1 || year > 4) { feedback('add-feedback', 'Year must be 1–4.', 'error'); return; }
  if (cgpa < 0 || cgpa > 10) { feedback('add-feedback', 'CGPA must be 0.0–10.0.', 'error'); return; }

  // 1. Try C++ Backend if active
  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ rollNo: roll, name, email, branch, year, cgpa })
      });
      const data = await res.json();
      if (!res.ok || !data.success) {
        feedback('add-feedback', data.error || 'Failed to add student in C++ backend.', 'error');
        return;
      }
      await syncFromCppBackend();
      feedback('add-feedback', `Student "${name}" added via C++ Backend (DLL, BST & Hash Table synced)!`, 'success');
      toast(`${name} added! [C++ Backend]`, 'success');
      ['add-roll','add-name','add-email','add-branch','add-year','add-cgpa'].forEach(id => {
        document.getElementById(id).value = '';
      });
      return;
    } catch (e) {
      console.warn('C++ backend call failed, falling back to local JS:', e);
    }
  }

  // 2. Local Fallback
  if (rollBST.search(roll)) {
    feedback('add-feedback', `Roll Number ${roll} already exists!`, 'error'); return;
  }
  if (emailHash.search(email)) {
    feedback('add-feedback', `Email "${email}" already registered!`, 'error'); return;
  }
  const s = { rollNo: roll, name, email, branch, year, cgpa };
  syncInsert(s);
  updateStats();
  feedback('add-feedback', `Student "${name}" added locally to DLL, BST & Hash Table!`, 'success');
  toast(`${name} added!`, 'success');
  ['add-roll','add-name','add-email','add-branch','add-year','add-cgpa'].forEach(id => {
    document.getElementById(id).value = '';
  });
}

// VIEW ALL
let currentDir = 'forward';

async function viewForward() {
  currentDir = 'forward';
  document.getElementById('btn-fwd').classList.add('active');
  document.getElementById('btn-bwd').classList.remove('active');

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students?order=forward`);
      if (res.ok) {
        const data = await res.json();
        document.getElementById('view-count').textContent = `${data.count} record(s) [C++ DLL Forward]`;
        renderTable('view-table-wrap', data.students);
        return;
      }
    } catch (e) {
      console.warn('C++ view failed:', e);
    }
  }

  const rows = masterList.toArrayForward();
  document.getElementById('view-count').textContent = `${rows.length} record(s)`;
  renderTable('view-table-wrap', rows);
}

async function viewBackward() {
  currentDir = 'backward';
  document.getElementById('btn-bwd').classList.add('active');
  document.getElementById('btn-fwd').classList.remove('active');

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students?order=backward`);
      if (res.ok) {
        const data = await res.json();
        document.getElementById('view-count').textContent = `${data.count} record(s) (reversed) [C++ DLL Backward]`;
        renderTable('view-table-wrap', data.students);
        return;
      }
    } catch (e) {
      console.warn('C++ view backward failed:', e);
    }
  }

  const rows = masterList.toArrayBackward();
  document.getElementById('view-count').textContent = `${rows.length} record(s) (reversed)`;
  renderTable('view-table-wrap', rows);
}

// SEARCH BY ROLL
async function searchByRoll() {
  const roll = parseInt(document.getElementById('sr-roll').value);
  if (!roll) { feedback('sr-feedback','Enter a roll number.','error'); return; }

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students/search/roll?roll=${roll}`);
      const data = await res.json();
      if (res.ok && data.success && data.student) {
        feedback('sr-feedback', `Found! C++ BST search complete — O(log n)`, 'success');
        renderTable('sr-result', [data.student]);
        return;
      } else {
        feedback('sr-feedback', data.error || `No student found with Roll ${roll} in C++ BST.`, 'error');
        document.getElementById('sr-result').innerHTML = '';
        return;
      }
    } catch (e) {
      console.warn('C++ BST search failed:', e);
    }
  }

  const node = rollBST.search(roll);
  if (!node) {
    feedback('sr-feedback', `No student found with Roll ${roll} (BST search complete).`, 'error');
    document.getElementById('sr-result').innerHTML = '';
  } else {
    feedback('sr-feedback', `Found! BST traversal complete — O(log n)`, 'success');
    renderTable('sr-result', [node.data]);
  }
}

// SEARCH BY EMAIL
async function searchByEmail() {
  const email = document.getElementById('se-email').value.trim();
  if (!email) { feedback('se-feedback','Enter an email address.','error'); return; }

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students/search/email?email=${encodeURIComponent(email)}`);
      const data = await res.json();
      if (res.ok && data.success && data.student) {
        feedback('se-feedback', `Found via C++ Hash Table (djb2) — O(1) lookup!`, 'success');
        renderTable('se-result', [data.student]);
        return;
      } else {
        feedback('se-feedback', data.error || `No student found with email "${email}" in C++ Hash Table.`, 'error');
        document.getElementById('se-result').innerHTML = '';
        return;
      }
    } catch (e) {
      console.warn('C++ Hash search failed:', e);
    }
  }

  const s = emailHash.search(email);
  if (!s) {
    feedback('se-feedback', `No student found with email "${email}" (Hash lookup complete).`, 'error');
    document.getElementById('se-result').innerHTML = '';
  } else {
    feedback('se-feedback', `Found via Hash Table — O(1) lookup!`, 'success');
    renderTable('se-result', [s]);
  }
}

// UPDATE CGPA
async function updateCGPA() {
  const roll    = parseInt(document.getElementById('up-roll').value);
  const newCGPA = parseFloat(document.getElementById('up-cgpa').value);

  if (!roll) { feedback('up-feedback','Enter a roll number.','error'); return; }
  if (isNaN(newCGPA) || newCGPA < 0 || newCGPA > 10) {
    feedback('up-feedback','CGPA must be 0.0–10.0.','error'); return;
  }

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students/update-cgpa`, {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ rollNo: roll, cgpa: newCGPA })
      });
      const data = await res.json();
      if (res.ok && data.success) {
        await syncFromCppBackend();
        feedback('up-feedback',
          `Updated ${data.student.name}'s CGPA: ${data.oldCgpa.toFixed(2)} → ${data.newCgpa.toFixed(2)} [C++ DLL + BST + Hash updated]`,
          'success');
        toast(`CGPA updated in C++ backend for ${data.student.name}`, 'success');
        document.getElementById('up-roll').value = '';
        document.getElementById('up-cgpa').value = '';
        return;
      } else {
        feedback('up-feedback', data.error || `Student Roll ${roll} not found.`, 'error');
        return;
      }
    } catch (e) {
      console.warn('C++ CGPA update failed:', e);
    }
  }

  const bstNode = rollBST.search(roll);
  if (!bstNode) { feedback('up-feedback',`Student Roll ${roll} not found.`,'error'); return; }

  const oldCGPA = bstNode.data.cgpa;
  bstNode.data.cgpa = newCGPA;
  const dllNode = masterList.findByRoll(roll);
  if (dllNode) dllNode.data.cgpa = newCGPA;
  const hashData = emailHash.search(bstNode.data.email);
  if (hashData) hashData.cgpa = newCGPA;

  feedback('up-feedback',
    `Updated ${bstNode.data.name}'s CGPA: ${oldCGPA.toFixed(2)} → ${newCGPA.toFixed(2)} (DLL + BST + Hash updated)`,
    'success');
  toast(`CGPA updated for ${bstNode.data.name}`, 'success');
  document.getElementById('up-roll').value = '';
  document.getElementById('up-cgpa').value = '';
}

// DELETE STUDENT
let _delTarget = null;

async function deleteStudent() {
  const roll = parseInt(document.getElementById('del-roll').value);
  if (!roll) { feedback('del-feedback','Enter a roll number.','error'); return; }

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students/search/roll?roll=${roll}`);
      const data = await res.json();
      if (res.ok && data.success && data.student) {
        _delTarget = { ...data.student };
      } else {
        feedback('del-feedback', `Student Roll ${roll} not found in C++ backend.`, 'error');
        return;
      }
    } catch (e) {
      const node = rollBST.search(roll);
      if (!node) { feedback('del-feedback',`Student Roll ${roll} not found.`,'error'); return; }
      _delTarget = { ...node.data };
    }
  } else {
    const node = rollBST.search(roll);
    if (!node) { feedback('del-feedback',`Student Roll ${roll} not found.`,'error'); return; }
    _delTarget = { ...node.data };
  }

  clearFeedback('del-feedback');
  document.getElementById('del-confirm').className = '';
  document.getElementById('del-confirm').innerHTML = `
    <div class="confirm-box">
      <p>⚠️ Are you sure you want to delete <strong>${_delTarget.name}</strong> (Roll: ${_delTarget.rollNo})?<br/>
      This will remove the record from <strong>DLL, BST, and Hash Table</strong> in the C++ backend.</p>
      <div class="confirm-actions">
        <button class="btn btn-confirm" onclick="confirmDelete()">Yes, Delete</button>
        <button class="btn btn-cancel" onclick="cancelDelete()">Cancel</button>
      </div>
    </div>`;
}

async function confirmDelete() {
  if (!_delTarget) return;

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students?roll=${_delTarget.rollNo}`, { method: 'DELETE' });
      const data = await res.json();
      if (res.ok && data.success) {
        await syncFromCppBackend();
        document.getElementById('del-confirm').className = 'hidden';
        document.getElementById('del-confirm').innerHTML = '';
        document.getElementById('del-roll').value = '';
        feedback('del-feedback', `Student "${_delTarget.name}" deleted from C++ DLL, BST, and Hash Table.`, 'success');
        toast(`${_delTarget.name} deleted [C++ Backend]`, 'error');
        _delTarget = null;
        return;
      }
    } catch (e) {
      console.warn('C++ delete failed:', e);
    }
  }

  syncDelete(_delTarget.rollNo, _delTarget.email);
  updateStats();
  document.getElementById('del-confirm').className = 'hidden';
  document.getElementById('del-confirm').innerHTML = '';
  document.getElementById('del-roll').value = '';
  feedback('del-feedback', `Student "${_delTarget.name}" deleted from DLL, BST, and Hash Table.`, 'success');
  toast(`${_delTarget.name} deleted`, 'error');
  _delTarget = null;
}

function cancelDelete() {
  document.getElementById('del-confirm').className = 'hidden';
  feedback('del-feedback','Deletion cancelled.','info');
  _delTarget = null;
}

// LEADERBOARD
async function showLeaderboard() {
  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/leaderboard`);
      if (res.ok) {
        const data = await res.json();
        if (data.students && data.students.length) {
          if (data.sortSteps) _sortSteps = data.sortSteps;
          renderTable('leaderboard-table', data.students, true);
          return;
        }
      }
    } catch (e) {
      console.warn('C++ leaderboard failed:', e);
    }
  }

  const arr = masterList.toArrayForward();
  if (!arr.length) {
    document.getElementById('leaderboard-table').innerHTML =
      `<div class="empty-state"><span class="empty-icon">📭</span>No records to sort.</div>`;
    return;
  }
  const sorted = mergeSort([...arr]);
  renderTable('leaderboard-table', sorted, true);
}

// BST IN-ORDER
async function showBSTInOrder() {
  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/students/bst-inorder`);
      if (res.ok) {
        const data = await res.json();
        if (data.students) {
          renderTable('bst-table', data.students);
          return;
        }
      }
    } catch (e) {
      console.warn('C++ BST inorder failed:', e);
    }
  }

  const rows = rollBST.inOrder();
  renderTable('bst-table', rows);
}

// ============================================================
//  HELPDESK (Max-Heap Priority Queue)
// ============================================================
function updatePriorityLabel(val) {
  const lbl = document.getElementById('hd-priority-label');
  lbl.textContent = val;
  lbl.style.color = val >= 8 ? 'var(--red)' : val >= 5 ? 'var(--yellow)' : 'var(--green)';
}

async function submitHelpdeskRequest() {
  const roll     = parseInt(document.getElementById('hd-roll').value);
  const priority = parseInt(document.getElementById('hd-priority').value);
  const issue    = document.getElementById('hd-issue').value.trim();

  if (!roll) { feedback('hd-submit-feedback','Enter your roll number.','error'); return; }
  if (!issue) { feedback('hd-submit-feedback','Describe your issue.','error'); return; }

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/helpdesk`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ rollNo: roll, priority, issue })
      });
      const data = await res.json();
      if (res.ok && data.success) {
        await syncFromCppBackend();
        feedback('hd-submit-feedback',
          `Request submitted to C++ Max-Heap! Priority: ${priority}/10 · Queue size: ${data.queueSize}`, 'success');
        toast(`Submitted to C++ Helpdesk (Priority ${priority})`, 'success');
        document.getElementById('hd-roll').value  = '';
        document.getElementById('hd-issue').value = '';
        document.getElementById('hd-priority').value = '5';
        updatePriorityLabel(5);
        showHelpdeskQueue();
        return;
      } else {
        feedback('hd-submit-feedback', data.error || 'Failed to submit helpdesk request.', 'error');
        return;
      }
    } catch (e) {
      console.warn('C++ helpdesk submit failed:', e);
    }
  }

  const node = rollBST.search(roll);
  if (!node) { feedback('hd-submit-feedback',`Roll ${roll} not found. Please register first.`,'error'); return; }

  helpdesk.insert({ priority, rollNo: roll, studentName: node.data.name, issue });
  updateStats();
  feedback('hd-submit-feedback',
    `Request submitted! Priority: ${priority}/10 · Queue size: ${helpdesk.size}`, 'success');
  toast(`Request submitted (Priority ${priority})`, 'success');
  document.getElementById('hd-roll').value  = '';
  document.getElementById('hd-issue').value = '';
  document.getElementById('hd-priority').value = '5';
  updatePriorityLabel(5);
  showHelpdeskQueue();
}

async function processNext() {
  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/helpdesk/process`, { method: 'POST' });
      const data = await res.json();
      if (res.ok && data.success) {
        const req = data.processedRequest;
        await syncFromCppBackend();
        feedback('hd-feedback',
          `<strong>Processed via C++ Max-Heap extractMax():</strong> ${req.studentName} (Roll ${req.rollNo}) — Priority ${req.priority}/10<br/>
           <em>Issue:</em> ${req.issue}`, 'success');
        toast(`Processed: ${req.studentName} [C++ Heap]`, 'success');
        showHelpdeskQueue();
        return;
      } else {
        feedback('hd-feedback', data.error || 'No pending requests in queue.', 'warning');
        return;
      }
    } catch (e) {
      console.warn('C++ process next failed:', e);
    }
  }

  if (helpdesk.isEmpty()) { feedback('hd-feedback','No pending requests in queue.','warning'); return; }
  const req = helpdesk.extractMax();
  updateStats();
  feedback('hd-feedback',
    `<strong>Processed:</strong> ${req.studentName} (Roll ${req.rollNo}) — Priority ${req.priority}/10<br/>
     <em>Issue:</em> ${req.issue}`, 'success');
  toast(`Processed: ${req.studentName}`, 'success');
  showHelpdeskQueue();
}

async function peekQueue() {
  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/helpdesk/peek`);
      const data = await res.json();
      if (res.ok && data.success) {
        const req = data.request;
        const pc  = req.priority >= 8 ? 'priority-critical' : req.priority >= 5 ? 'priority-high' : 'priority-low';
        feedback('hd-feedback',
          `<strong>Next (C++ Max-Heap peek()):</strong> <span class="${pc}">${req.studentName}</span> (Roll ${req.rollNo}) — Priority <strong>${req.priority}/10</strong><br/>
           <em>Issue:</em> ${req.issue}`, 'info');
        return;
      } else {
        feedback('hd-feedback', data.error || 'Queue is empty.', 'warning');
        return;
      }
    } catch (e) {
      console.warn('C++ peek failed:', e);
    }
  }

  if (helpdesk.isEmpty()) { feedback('hd-feedback','Queue is empty.','warning'); return; }
  const req = helpdesk.peek();
  const pc  = req.priority >= 8 ? 'priority-critical' : req.priority >= 5 ? 'priority-high' : 'priority-low';
  feedback('hd-feedback',
    `<strong>Next (Peek):</strong> <span class="${pc}">${req.studentName}</span> (Roll ${req.rollNo}) — Priority <strong>${req.priority}/10</strong><br/>
     <em>Issue:</em> ${req.issue}`, 'info');
}

async function showHelpdeskQueue() {
  const container = document.getElementById('hd-queue-display');

  if (isBackendConnected) {
    try {
      const res = await fetch(`${API_BASE}/api/helpdesk`);
      if (res.ok) {
        const data = await res.json();
        if (data.requests) {
          helpdesk.heap = [...data.requests];
          updateStats();
        }
      }
    } catch (e) {
      console.warn('C++ show queue fetch failed:', e);
    }
  }

  if (helpdesk.isEmpty()) {
    container.innerHTML = `<div class="empty-state"><span class="empty-icon">✅</span>Helpdesk queue is empty.</div>`;
    return;
  }
  const items = helpdesk.heap.map((req, i) => {
    const pc   = req.priority >= 8 ? 'p-critical' : req.priority >= 5 ? 'p-high' : 'p-low';
    const top  = i === 0 ? `style="border-color:rgba(167,139,250,0.4)"` : '';
    return `<div class="queue-item" ${top}>
      <span class="priority-pill ${pc}">P${req.priority}</span>
      <div>
        <div style="font-weight:600;font-size:.88rem">${req.studentName} <span style="color:var(--text-muted);font-size:.76rem">(Roll ${req.rollNo})</span></div>
        <div class="queue-meta">${req.issue}</div>
      </div>
      ${i === 0 ? '<span style="margin-left:auto;font-size:.7rem;color:var(--purple);font-weight:700">← MAX (C++ Heap)</span>' : ''}
    </div>`;
  }).join('');

  container.innerHTML = `<div style="font-size:.78rem;color:var(--text-muted);margin-bottom:.6rem;font-family:'JetBrains Mono',monospace">
    ${helpdesk.size} pending request(s) · C++ Max-Heap array order shown
  </div>${items}`;
}

// ============================================================
//  DSA VISUALIZER
// ============================================================
let vizInitialized = false;

function initVisualizer() {
  if (!vizInitialized) {
    renderDLLViz();
    renderHashViz();
    vizInitialized = true;
  }
}

function showVizTab(tab) {
  document.querySelectorAll('.viz-tab').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.viz-panel').forEach(p => p.classList.remove('active'));
  document.getElementById('viz-tab-' + tab).classList.add('active');
  document.getElementById('viz-' + tab).classList.add('active');
}

// ── DLL Visualizer ────────────────────────────────────────
function renderDLLViz(highlightRoll = null, deletedRoll = null, newRoll = null) {
  const arr = masterList.toArrayForward();
  const container = document.getElementById('dll-viz-nodes');
  if (!arr.length) { container.innerHTML = '<span style="color:var(--text-muted)">No nodes in list.</span>'; return; }

  let html = '';
  arr.forEach((s, i) => {
    const isHL  = s.rollNo === highlightRoll;
    const isDel = s.rollNo === deletedRoll;
    const isNew = s.rollNo === newRoll;
    const cls   = isHL ? 'highlight' : isDel ? 'deleted' : isNew ? 'new-node' : '';

    html += `<div class="dll-node">
      <div class="dll-box ${cls}">
        <div class="dll-roll">Roll ${s.rollNo}</div>
        <div class="dll-name">${s.name.split(' ')[0]}</div>
        <div class="dll-cgpa">${s.cgpa.toFixed(2)}</div>
      </div>
    </div>`;

    if (i < arr.length - 1) {
      html += `<div class="dll-arrow"><span>⇄</span></div>`;
    }
  });

  html += `<div class="dll-node" style="opacity:.3">
    <div class="dll-box" style="border-color:var(--text-muted)">
      <div class="dll-roll">NULL</div>
    </div>
  </div>`;

  container.innerHTML = html;
}

// ── BST Visualizer ────────────────────────────────────────
function renderBSTViz(highlightRoll = null) {
  const { nodes, edges, root } = rollBST.getStructure();
  const svgEl = document.getElementById('bst-svg');
  if (!nodes.length) { svgEl.innerHTML = '<text x="10" y="30" fill="var(--text-muted)">BST is empty.</text>'; return; }

  const W = 700, H = 320, nodeR = 26;
  const positions = {};

  // BFS-based layout
  const queue = [{ id: root, depth: 0, xMin: 0, xMax: W }];
  const visited = {};

  while (queue.length) {
    const { id, depth, xMin, xMax } = queue.shift();
    if (visited[id]) continue;
    visited[id] = true;

    const x = (xMin + xMax) / 2;
    const y = 40 + depth * 80;
    positions[id] = { x, y };

    const nodeObj = nodes.find(n => n.id === id);
    const bstNode = rollBST._search(rollBST.root, id);
    if (!bstNode) continue;

    if (bstNode.left)  queue.push({ id: bstNode.left.data.rollNo,  depth: depth+1, xMin, xMax: x });
    if (bstNode.right) queue.push({ id: bstNode.right.data.rollNo, depth: depth+1, xMin: x, xMax });
  }

  let svg = '';
  // Draw edges
  edges.forEach(({ from, to }) => {
    const pf = positions[from], pt = positions[to];
    if (pf && pt) svg += `<line x1="${pf.x}" y1="${pf.y}" x2="${pt.x}" y2="${pt.y}" stroke="rgba(34,211,238,0.25)" stroke-width="1.5"/>`;
  });

  // Draw nodes
  nodes.forEach(n => {
    const pos = positions[n.id];
    if (!pos) return;
    const isHL = n.id === highlightRoll;
    const fill = isHL ? 'rgba(34,211,238,0.25)' : 'rgba(34,211,238,0.07)';
    const stroke = isHL ? '#22d3ee' : 'rgba(34,211,238,0.4)';
    const sw = isHL ? '2.5' : '1.5';

    svg += `<circle cx="${pos.x}" cy="${pos.y}" r="${nodeR}" fill="${fill}" stroke="${stroke}" stroke-width="${sw}"/>`;
    svg += `<text x="${pos.x}" y="${pos.y - 5}" fill="#e8edf5" text-anchor="middle" font-family="JetBrains Mono" font-size="11" font-weight="700">${n.roll}</text>`;
    svg += `<text x="${pos.x}" y="${pos.y + 9}" fill="rgba(139,152,179,0.9)" text-anchor="middle" font-family="Inter" font-size="8">${n.name.split(' ')[0]}</text>`;
  });

  svgEl.setAttribute('viewBox', `0 0 ${W} ${H}`);
  svgEl.innerHTML = svg;
}

// ── Hash Visualizer ───────────────────────────────────────
function renderHashViz(highlightIdx = null) {
  const buckets = emailHash.getBuckets();
  const occupied = buckets.filter(b => b.items.length > 0);
  const container = document.getElementById('hash-viz-grid');

  if (!occupied.length) { container.innerHTML = '<span style="color:var(--text-muted)">Hash table is empty.</span>'; return; }

  container.innerHTML = occupied.map(b => {
    const isHL = b.idx === highlightIdx;
    return `<div class="hash-bucket occupied ${isHL ? 'highlight' : ''}">
      <div class="hash-idx">Bucket [${b.idx}]</div>
      ${b.items.map(k => `<div class="hash-val">${k}</div>`).join('<div style="color:var(--text-muted);font-size:.65rem">→ chain</div>')}
    </div>`;
  }).join('');

  document.getElementById('hash-stats').textContent =
    `${occupied.length} occupied / ${HASH_SIZE} buckets · Load factor: ${(occupied.length/HASH_SIZE).toFixed(2)}`;
}

// ── Merge Sort Visualizer ─────────────────────────────────
let _sortSteps = [];
let _sortStepIdx = 0;
let _sortArr = [];
let _sortFinalOrder = [];

function initSortViz() {
  _sortSteps = [];
  const arr = masterList.toArrayForward();
  if (!arr.length) return;
  _sortArr = [...arr];
  _sortFinalOrder = mergeSort([...arr], _sortSteps);
  _sortStepIdx = 0;
  renderSortBars(_sortArr, -1, -1);
  renderSortLog([]);
  document.getElementById('sort-step-info').textContent = `${_sortSteps.length} comparison steps · click "Next Step"`;
}

function sortNextStep() {
  if (_sortStepIdx >= _sortSteps.length) {
    renderSortBars(_sortFinalOrder, -1, -1, true);
    document.getElementById('sort-step-info').textContent = '✅ Sorting complete!';
    return;
  }
  const step = _sortSteps[_sortStepIdx];
  _sortStepIdx++;

  const logEl = document.getElementById('sort-log');
  const entry = document.createElement('div');
  entry.className = 'log-entry' + (_sortStepIdx === _sortSteps.length ? ' highlight' : '');
  entry.innerHTML = `<span class="step-n">Step ${_sortStepIdx}:</span> ${step}`;
  logEl.appendChild(entry);
  logEl.scrollTop = logEl.scrollHeight;

  document.getElementById('sort-step-info').textContent = `Step ${_sortStepIdx} / ${_sortSteps.length}`;
}

function sortRunAll() {
  renderSortBars(_sortFinalOrder, -1, -1, true);
  _sortStepIdx = _sortSteps.length;
  document.getElementById('sort-step-info').textContent = '✅ Sort complete!';
  const logEl = document.getElementById('sort-log');
  logEl.innerHTML = _sortSteps.map((s, i) =>
    `<div class="log-entry"><span class="step-n">Step ${i+1}:</span> ${s}</div>`).join('');
  logEl.scrollTop = logEl.scrollHeight;
}

function renderSortBars(arr, ci = -1, cj = -1, sorted = false) {
  const container = document.getElementById('sort-bars');
  if (!arr.length) return;
  const maxCGPA = 10;
  container.innerHTML = arr.map((s, i) => {
    const h = Math.max(20, (s.cgpa / maxCGPA) * 140);
    const cls = sorted ? 'sorted' : (i === ci || i === cj) ? 'comparing' : '';
    return `<div class="sort-bar ${cls}" style="height:${h}px" title="${s.name}: ${s.cgpa}">
      <span class="bar-name">${s.name.split(' ')[0]}</span>
      <span class="bar-label">${s.cgpa.toFixed(1)}</span>
    </div>`;
  }).join('');
}

function renderSortLog(steps) {
  document.getElementById('sort-log').innerHTML = '';
}

// ── Heap Visualizer ───────────────────────────────────────
function renderHeapViz() {
  const container = document.getElementById('heap-viz');
  if (helpdesk.isEmpty()) {
    container.innerHTML = '<div class="empty-state"><span class="empty-icon">📭</span>Heap is empty. Submit some helpdesk requests first.</div>';
    return;
  }
  container.innerHTML = helpdesk.heap.map((req, i) => {
    const pc = req.priority >= 8 ? 'var(--red)' : req.priority >= 5 ? 'var(--yellow)' : 'var(--green)';
    return `<div class="heap-node">
      <div class="hn-prio" style="color:${pc}">P${req.priority}</div>
      <div class="hn-name">${req.studentName}</div>
      <div class="hn-issue">${req.issue.slice(0, 30)}${req.issue.length > 30 ? '...' : ''}</div>
      <div style="font-size:.6rem;color:var(--text-muted);font-family:'JetBrains Mono',monospace">heap[${i}]</div>
    </div>`;
  }).join('');

  // Parent child connections description
  const treeDesc = helpdesk.heap.map((req, i) => {
    const l = 2*i+1, r = 2*i+2;
    const children = [];
    if (l < helpdesk.size) children.push(`heap[${l}]=P${helpdesk.heap[l].priority}`);
    if (r < helpdesk.size) children.push(`heap[${r}]=P${helpdesk.heap[r].priority}`);
    return children.length ? `heap[${i}]=P${req.priority} → {${children.join(', ')}}` : null;
  }).filter(Boolean);

  document.getElementById('heap-tree-desc').innerHTML = treeDesc.map(d =>
    `<div class="log-entry">${d}</div>`).join('');
}

// Demo: simulate DLL search step
function demoSearchDLL() {
  const arr = masterList.toArrayForward();
  if (!arr.length) return;
  const target = arr[Math.floor(arr.length / 2)].rollNo;
  const log = document.getElementById('dll-search-log');
  log.innerHTML = '';
  let i = 0;

  const interval = setInterval(() => {
    if (i >= arr.length) { clearInterval(interval); return; }
    renderDLLViz(arr[i].rollNo === target ? target : arr[i].rollNo);
    const entry = document.createElement('div');
    entry.className = 'log-entry' + (arr[i].rollNo === target ? ' highlight' : '');
    entry.innerHTML = `<span class="step-n">Visit:</span> Roll ${arr[i].rollNo} — ${arr[i].rollNo === target ? '✅ FOUND!' : '✗ not target'}`;
    log.appendChild(entry);
    log.scrollTop = log.scrollHeight;
    if (arr[i].rollNo === target) { clearInterval(interval); renderDLLViz(target); }
    i++;
  }, 600);
}

function demoBSTSearch() {
  const arr = rollBST.inOrder();
  if (!arr.length) return;
  const target = arr[Math.floor(arr.length / 2)].rollNo;
  const log = document.getElementById('bst-search-log');
  log.innerHTML = '';

  const path = [];
  let cur = rollBST.root;
  while (cur) {
    path.push(cur.data.rollNo);
    if (cur.data.rollNo === target) break;
    cur = target < cur.data.rollNo ? cur.left : cur.right;
  }

  let i = 0;
  const interval = setInterval(() => {
    if (i >= path.length) { clearInterval(interval); return; }
    renderBSTViz(path[i]);
    const entry = document.createElement('div');
    entry.className = 'log-entry' + (path[i] === target ? ' highlight' : '');
    entry.innerHTML = `<span class="step-n">Visit:</span> Roll ${path[i]} — ${path[i] === target ? '✅ FOUND!' : path[i] > target ? 'go left ←' : 'go right →'}`;
    log.appendChild(entry);
    log.scrollTop = log.scrollHeight;
    i++;
  }, 700);
}

function demoHashLookup() {
  const arr = masterList.toArrayForward();
  if (!arr.length) return;
  const target = arr[0];
  const k = target.email.toLowerCase();
  let h = 5381;
  for (let i = 0; i < k.length; i++) h = ((h << 5) + h) + k.charCodeAt(i);
  const idx = Math.abs(h) % HASH_SIZE;

  const log = document.getElementById('hash-search-log');
  log.innerHTML = `<div class="log-entry"><span class="step-n">Key:</span> "${k}"</div>
    <div class="log-entry"><span class="step-n">djb2 hash:</span> ${Math.abs(h)}</div>
    <div class="log-entry highlight"><span class="step-n">Bucket:</span> ${Math.abs(h)} % ${HASH_SIZE} = ${idx} ✅</div>`;
  renderHashViz(idx);
}

// ============================================================
//  INIT
// ============================================================
updateStats();
viewForward();
showPage('add');
checkBackendHealth();
