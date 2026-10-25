# ⚡ Smart Student Record System (SSRS)
> **DSA Mini Project** — High-Performance C++ REST API Backend + Dynamic Dual-Theme Web Interface + Real-Time DSA Visualizer.

[![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Docker](https://img.shields.io/badge/Docker-Ready-2496ED.svg?logo=docker&logoColor=white)](./Dockerfile)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

---

## 📌 1. Project Overview
The **Smart Student Record System** is an end-to-end academic record management and data structure learning system. Core data structures are implemented **completely from scratch in C++** without using high-level STL containers (`std::map`, `std::list`, `std::priority_queue`, etc.) to highlight core pointer manipulation, dynamic memory allocation, and algorithmic design.

The system connects this native C++ engine to an interactive modern web frontend through a REST API built on `cpp-httplib`, featuring real-time record synchronization, multi-key search, academic leaderboard generation, an urgent ticketing helpdesk, and an interactive **DSA Visualizer**.

---

## 🧠 2. Data Structures & Algorithmic Complexity

| Data Structure | Underlying Technique | Key Operations | Time Complexity | System Responsibility |
|---|---|---|---|---|
| **Doubly Linked List (DLL)** | Bidirectional pointers (`prev`, `next`), `head` & `tail` | `insertBack`<br>`deleteByRoll`<br>`traverse` | Insert: $\mathcal{O}(1)$<br>Delete: $\mathcal{O}(n)$<br>Traverse: $\mathcal{O}(n)$ | Master store preserving sequence; supports forward & backward traversal. |
| **Binary Search Tree (BST)** | Dynamic pointer tree keyed on `rollNo` | `insert`<br>`search`<br>`deleteNode`<br>`inOrder` | Search: $\mathcal{O}(\log n)$ avg<br>Insert: $\mathcal{O}(\log n)$ avg<br>In-Order: $\mathcal{O}(n)$ | Fast Roll Number lookups, duplicate roll validation, and ascending roll listing. |
| **Hash Table** | 101 prime buckets, `djb2` hash function, Separate Chaining | `insert`<br>`search`<br>`remove` | Search: $\mathcal{O}(1)$ avg<br>Insert: $\mathcal{O}(1)$ avg<br>Delete: $\mathcal{O}(1)$ avg | Instant lookup by email address; enforces unique student email registration. |
| **Merge Sort** | Divide-and-conquer on dynamic array | `mergeSort`<br>`merge` | Time: $\mathcal{O}(n \log n)$<br>Space: $\mathcal{O}(n)$ aux | Stable sorting of student records by CGPA in descending order for the merit leaderboard. |
| **Max-Heap** | Array-based complete binary tree | `insert` (`siftUp`)<br>`extractMax` (`siftDown`)<br>`peek` | Insert: $\mathcal{O}(\log n)$<br>ExtractMax: $\mathcal{O}(\log n)$<br>Peek: $\mathcal{O}(1)$ | Student Helpdesk Priority Queue; resolves critical tickets (Priority 1–10) first. |

---

## 🚀 3. Features
- **3-Way Real-Time Data Sync**: Adding or deleting a student synchronously updates the Doubly Linked List, BST, and Hash Table.
- **Dual-Key Lookup**: Search by numeric Roll Number via BST ($\mathcal{O}(\log n)$) or by Email Address via Hash Table ($\mathcal{O}(1)$).
- **Merit Leaderboard**: Sorts all student records dynamically with Merge Sort and displays rankings with medal badges.
- **Student Helpdesk**: Priority queue ticketing system powered by Max-Heap (`siftUp` and `siftDown`).
- **Interactive DSA Visualizer**:
  - *DLL*: Animated linear search pointer walk across bidirectional nodes.
  - *BST*: SVG-rendered binary search tree showing dynamic branch traversal.
  - *Hash Table*: Live bucket breakdown and `djb2` hash computation steps.
  - *Merge Sort*: Visual animated comparison bars with divide-and-conquer logs.
  - *Max-Heap*: Logical tree structure and heap array mapping.
- **Dual Light / Dark Theme**: Smooth theme switcher with local storage persistence.
- **Cross-Platform C++ Backend**: Native C++ REST server with automated offline fallback.

---

## 💻 4. Running Locally

### Prerequisites
- GCC / MinGW (`g++` supporting C++17)
- Any modern web browser (Chrome, Edge, Firefox)

### Option A: 1-Click Launch (Windows)
Double-click [`start_server.bat`](./start_server.bat). It will:
1. Compile `server.cpp` using `g++` (if not already compiled).
2. Start the C++ REST backend on port `8080`.
3. Open `http://localhost:8080/` in your default browser.

### Option B: Terminal Command Line
```powershell
# 1. Compile C++ Backend
g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32

# 2. Run Backend Server
.\server.exe 8080

# 3. Open in Browser
# Navigate to: http://localhost:8080/
```

### Option C: Standalone C++ Terminal Console Edition
If you prefer running the traditional terminal console menu:
```powershell
g++ main.cpp -o smart_student_system.exe
.\smart_student_system.exe
```

---

## 🌐 5. Deploying Full-Stack (C++ Container 24/7)

The project includes a multi-stage production [`Dockerfile`](./Dockerfile) and a [`render.yaml`](./render.yaml) blueprint to run the C++ backend on the cloud.

### Deploying on Render (Free Tier)
1. Push this repository to your GitHub account:
   ```bash
   git add .
   git commit -m "Deploy Smart Student Record System"
   git push origin main
   ```
2. Log into [Render.com](https://render.com/).
3. Click **New +** → **Web Service**.
4. Select **Build and deploy from a Git repository** and connect your `Smart-student-record-system` repository.
5. Render will automatically detect the `Dockerfile`:
   - **Environment**: `Docker`
   - **Instance Type**: `Free`
6. Click **Create Web Service**.
7. Render will build the container, compile `server.cpp`, and provide you with a live HTTPS URL (e.g. `https://smart-student-record-system.onrender.com`).

### Deploying on Railway
1. Go to [Railway.app](https://railway.app/).
2. Click **New Project** → **Deploy from GitHub repo**.
3. Select this repository.
4. Railway will automatically build the `Dockerfile` and deploy the service.

---

## 📁 Repository Structure
```
├── server.cpp              # C++ REST API Backend Server (Multi-threaded HTTP)
├── main.cpp                # C++ Interactive Console Terminal Application
├── httplib.h               # Single-header C++ HTTP/HTTPS server library
├── index.html              # Modern Web Application UI & Visualizer Layout
├── style.css               # Dual-theme styling (Light & Dark glassmorphic design)
├── app.js                  # Frontend client & algorithm visualization engine
├── start_server.bat        # 1-click Windows runner script
├── Dockerfile              # Multi-stage Linux build container for cloud deployment
├── render.yaml             # Render infrastructure-as-code deployment blueprint
├── .gitignore              # Git ignore rules for compiled binaries
└── README.md               # Project documentation
```

---

## 📄 License
This project is open-source under the MIT License.
