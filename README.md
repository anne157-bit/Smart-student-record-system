# ⚡ Smart Student Record System (SSRS)
> **DSA Mini Project** — High-Performance C++ REST API Backend + Dynamic Dual-Theme Web Interface + Real-Time DSA Visualizer.

[![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Docker](https://img.shields.io/badge/Docker-Ready-2496ED.svg?logo=docker&logoColor=white)](./Dockerfile)
[![Vercel](https://img.shields.io/badge/Vercel-Deployed-black.svg?logo=vercel&logoColor=white)](https://vercel.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

---

## 📌 1. Description
The **Smart Student Record System (SSRS)** is a full-stack academic management and data structures learning system. It bridges a **high-performance C++ backend** running fundamental Data Structures & Algorithms (built entirely from scratch without STL containers like `std::map`, `std::list`, or `std::priority_queue`) with an interactive, modern **web interface**. The system provides real-time student record management, instant multi-key lookups, academic analytics (leaderboard generation), priority-based student helpdesk ticketing, and an interactive **DSA Visualizer** demonstrating each algorithm in real time.

---

## 🎯 2. Problem Statement
Educational institutions handle high volumes of student records requiring frequent insertions, updates, deletions, and multi-criteria queries (e.g., lookup by numeric Roll Number vs. lookup by string Email Address). Furthermore, student grievance handling requires prioritizing inquiries by urgency rather than simple arrival time, and academic departments need fast ranking computations for merit evaluation.

Standard monolithic arrays or unsorted lists suffer from $\mathcal{O}(n)$ search and deletion bottlenecks. This project solves these challenges by combining specialized data structures—**Doubly Linked List**, **Binary Search Tree**, **Hash Table**, **Merge Sort**, and **Max-Heap**—into a unified system accessible through an intuitive graphical web interface and a terminal console menu.

---

## 🎯 3. Objectives
- **Zero-Dependency DSA Implementation**: Build Doubly Linked List, Binary Search Tree, Hash Table, Max-Heap, and Merge Sort from scratch in pure C++ to demonstrate memory management and pointer manipulation.
- **Multi-Index Synchronization**: Maintain three synchronized indices (DLL for sequence traversal, BST for numeric roll lookup, Hash Table for email lookup) so records remain consistent across additions, updates, and deletions.
- **Optimal Algorithmic Complexity**:
  - $\mathcal{O}(1)$ sequential insertion and email lookup.
  - $\mathcal{O}(\log n)$ roll number search and deletion.
  - $\mathcal{O}(n \log n)$ stable sorting for academic rank calculation.
  - $\mathcal{O}(\log n)$ priority queue enqueue/dequeue operations for urgent helpdesk ticket resolution.
- **Full-Stack Architecture**: Expose the C++ data structures via a lightweight C++ HTTP REST API server connected to an interactive, dual-themed (Light/Dark) web interface.
- **Pedagogical Visualization**: Provide a live visualizer illustrating pointer walks, tree branches, hash buckets, merge-sort divide-and-conquer steps, and heap array representations.

---

## 🛠️ 4. Technology Used
* **Programming Language**: C++ (C++17 Standard), JavaScript (ES6+ client engine)
* **Standard Libraries**: `<iostream>`, `<string>`, `<vector>`, `<sstream>`, `<iomanip>`, `<mutex>`, `<algorithm>`, `<cctype>`, `<cstdlib>`
* **Interface**: 
  - Dynamic Web Interface (HTML5, Vanilla CSS3 with Dual Light/Dark glassmorphic styling, and asynchronous JavaScript)
  - Interactive Console Terminal Menu (ANSI colored CLI)
* **Development Environment**: Visual Studio Code, GCC / MinGW-w64 (`g++ 15.2.0`), Windows PowerShell, Docker
* **Data Management**: In-memory storage using custom dynamic pointer structures (Nodes, Binary Trees, Separate Chaining Bucket Lists, and Binary Heap Arrays) with thread-safe synchronization
* **Networking & Deployment**: `cpp-httplib` (single-header C++ multi-threaded HTTP server), Docker multi-stage build, Render Blueprint, Vercel

---

## 🧠 5. Data Structures Used

| Data Structure | Underlying Technique | Key Operations | Time Complexity | System Responsibility |
|---|---|---|---|---|
| **Doubly Linked List (DLL)** | Bidirectional pointers (`prev`, `next`), `head` & `tail` | `insertBack`<br>`deleteByRoll`<br>`traverse` | Insert: $\mathcal{O}(1)$<br>Delete: $\mathcal{O}(n)$<br>Traverse: $\mathcal{O}(n)$ | Master store preserving sequence; supports forward & backward traversal. |
| **Binary Search Tree (BST)** | Dynamic pointer tree keyed on `rollNo` | `insert`<br>`search`<br>`deleteNode`<br>`inOrder` | Search: $\mathcal{O}(\log n)$ avg<br>Insert: $\mathcal{O}(\log n)$ avg<br>In-Order: $\mathcal{O}(n)$ | Fast Roll Number lookups, duplicate roll validation, and ascending roll listing. |
| **Hash Table** | 101 prime buckets, `djb2` hash function, Separate Chaining | `insert`<br>`search`<br>`remove` | Search: $\mathcal{O}(1)$ avg<br>Insert: $\mathcal{O}(1)$ avg<br>Delete: $\mathcal{O}(1)$ avg | Instant lookup by email address; enforces unique student email registration. |
| **Merge Sort** | Divide-and-conquer on dynamic array | `mergeSort`<br>`merge` | Time: $\mathcal{O}(n \log n)$<br>Space: $\mathcal{O}(n)$ aux | Stable sorting of student records by CGPA in descending order for the merit leaderboard. |
| **Max-Heap** | Array-based complete binary tree (`parent=(i-1)/2`, `left=2i+1`, `right=2i+2`) | `insert` (`siftUp`)<br>`extractMax` (`siftDown`)<br>`peek` | Insert: $\mathcal{O}(\log n)$<br>ExtractMax: $\mathcal{O}(\log n)$<br>Peek: $\mathcal{O}(1)$ | Student Helpdesk Priority Queue; resolves critical tickets (Priority 1–10) first. |

---

## 🚀 6. Main Features
1. **3-Way Real-Time Data Sync**: Adding or deleting a student synchronously updates the Doubly Linked List, BST, and Hash Table.
2. **Dual-Key Lookup**: Search by numeric Roll Number via BST ($\mathcal{O}(\log n)$) or by Email Address via Hash Table ($\mathcal{O}(1)$).
3. **Merit Leaderboard**: Sorts all student records dynamically with Merge Sort and displays rankings with medal badges (🥇, 🥈, 🥉).
4. **Student Helpdesk**: Priority queue ticketing system powered by Max-Heap (`siftUp` and `siftDown`).
5. **Interactive DSA Visualizer**:
   - *DLL*: Animated linear search pointer walk across bidirectional nodes.
   - *BST*: SVG-rendered binary search tree showing dynamic branch traversal.
   - *Hash Table*: Live bucket breakdown and `djb2` hash computation steps.
   - *Merge Sort*: Visual animated comparison bars with divide-and-conquer logs.
   - *Max-Heap*: Logical tree structure and heap array mapping.
6. **Dual Light & Dark Theme**: Smooth theme switcher with local storage persistence.
7. **Native C++ REST API**: Real-time HTTP endpoints with automatic offline fallback to browser-side algorithms if the server is stopped.

---

## 📡 7. REST API Endpoints

| Method | Endpoint | Description | Complexity |
|---|---|---|---|
| `GET` | `/api/status` | System health, record count, and queue size | $\mathcal{O}(1)$ |
| `GET` | `/api/students?order=forward\|backward` | List all records via DLL traversal | $\mathcal{O}(n)$ |
| `POST` | `/api/students` | Add new record (syncs to DLL, BST, Hash) | $\mathcal{O}(\log n)$ |
| `GET` | `/api/students/search/roll?roll=:roll` | Search student by Roll Number (BST) | $\mathcal{O}(\log n)$ |
| `GET` | `/api/students/search/email?email=:email` | Search student by Email (Hash Table) | $\mathcal{O}(1)$ |
| `PUT` | `/api/students/update-cgpa` | Update student CGPA across all structures | $\mathcal{O}(\log n)$ |
| `DELETE` | `/api/students?roll=:roll` | Delete record from all structures | $\mathcal{O}(n)$ |
| `GET` | `/api/leaderboard` | Get students sorted by CGPA via Merge Sort | $\mathcal{O}(n \log n)$ |
| `GET` | `/api/students/bst-inorder` | Get students sorted by Roll via BST in-order | $\mathcal{O}(n)$ |
| `POST` | `/api/helpdesk` | Submit ticket into Max-Heap priority queue | $\mathcal{O}(\log n)$ |
| `POST` | `/api/helpdesk/process` | Extract highest priority ticket (`extractMax`) | $\mathcal{O}(\log n)$ |
| `GET` | `/api/helpdesk/peek` | Inspect top ticket without removal | $\mathcal{O}(1)$ |
| `GET` | `/api/helpdesk` | View all pending tickets in heap array order | $\mathcal{O}(n)$ |
| `GET` | `/api/dsa/viz-data` | Complete live structural data for visualizer | $\mathcal{O}(n)$ |

---

## 💻 8. Running Locally

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

## 🌐 9. Cloud Deployment

### Option A: Deploy Full-Stack C++ Container on Render
The project includes a multi-stage [`Dockerfile`](./Dockerfile) and [`render.yaml`](./render.yaml):
1. Push code to GitHub:
   ```bash
   git add .
   git commit -m "Deploy SSRS"
   git push origin main
   ```
2. On [Render.com](https://dashboard.render.com/), click **New +** → **Web Service**.
3. Connect your repository: `Smart-student-record-system`.
4. Render automatically detects the `Dockerfile`:
   - **Environment**: `Docker`
   - **Region**: Any (e.g., Oregon or Frankfurt)
   - **Instance Type**: `Free`
5. Click **Create Web Service**. Render builds the image, starts the C++ server on `$PORT`, and assigns a public live URL.

### Option B: Deploy Free Frontend on Vercel
1. Log into [Vercel.com](https://vercel.com/) with GitHub.
2. Click **Add New Project** → Import `Smart-student-record-system`.
3. Framework Preset: **Other** → Click **Deploy**.
4. The site goes live in seconds with full browser-side DSA engine execution.

### Option C: Deploy Free on GitHub Pages
1. Go to your repository on GitHub → **Settings** → **Pages**.
2. Under **Build and deployment** → **Branch**, select `main` and `/ (root)`.
3. Click **Save**. The website is live at `https://<username>.github.io/<repo-name>/`.

---

## 📁 10. Repository Structure
```
├── server.cpp              # C++ REST API Backend Server (Multi-threaded HTTP)
├── main.cpp                # C++ Interactive Console Terminal Application
├── httplib.h               # Single-header C++ HTTP/HTTPS server library
├── index.html              # Modern Web Application UI & Visualizer Layout
├── style.css               # Dual-theme styling (Light & Dark glassmorphic design)
├── app.js                  # Frontend client & algorithm visualization engine
├── start_server.bat        # 1-click Windows runner script
├── Dockerfile              # Multi-stage Linux build container (debian:bookworm)
├── render.yaml             # Render infrastructure-as-code deployment blueprint
├── vercel.json             # Vercel zero-config routing configuration
├── .gitignore              # Git ignore rules for compiled binaries
└── README.md               # Complete project documentation
```

---

## 🏁 11. Conclusion
The Smart Student Record System shows how theoretical computer science concepts translate directly into practical software architecture. By pairing specialized C++ data structures with specific functional requirements:
- Retrieval times for key attributes are minimized ($\mathcal{O}(1)$ for email and $\mathcal{O}(\log n)$ for roll number).
- Sorting achieves deterministic $\mathcal{O}(n \log n)$ performance without unnecessary swaps.
- Emergency requests are handled systematically via priority heap invariant maintenance.

The architecture cleanly separates the high-speed algorithmic execution engine (C++ Backend) from the presentation layer (Web Frontend), resulting in an educational, robust, and modern system.

---

## 📄 License
This project is open-source under the [MIT License](https://opensource.org/licenses/MIT).
