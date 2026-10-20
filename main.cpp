// ============================================================
//   Smart Student Record System
//   DSA Mini Project — C++ Terminal Application
//
//   DSA Concepts Implemented (all from scratch, no STL containers):
//     1. Doubly Linked List  — main record store (forward / backward nav)
//     2. Binary Search Tree  — fast lookup by Roll Number
//     3. Hash Table          — O(1) lookup by Email
//     4. Merge Sort          — sort records by CGPA for leaderboard
//     5. Max-Heap            — priority-based Helpdesk queue
// ============================================================

#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <cstdlib>

using std::string;
using std::cout;
using std::cin;
using std::endl;

// ============================================================
//  ANSI COLOR CODES
// ============================================================
#define RESET         "\033[0m"
#define BOLD          "\033[1m"
#define DIM           "\033[2m"
#define CYAN          "\033[36m"
#define BRIGHT_CYAN   "\033[96m"
#define GREEN         "\033[32m"
#define BRIGHT_GREEN  "\033[92m"
#define RED           "\033[31m"
#define BRIGHT_RED    "\033[91m"
#define YELLOW        "\033[33m"
#define BRIGHT_YELLOW "\033[93m"
#define MAGENTA       "\033[35m"
#define BRIGHT_MAGENTA "\033[95m"
#define WHITE         "\033[37m"
#define BRIGHT_WHITE  "\033[97m"

// ============================================================
//  STUDENT RECORD  (core data unit shared by all structures)
// ============================================================
struct Student {
    int    rollNo;
    string name;
    string email;
    string branch;
    int    year;
    double cgpa;

    Student() : rollNo(0), year(0), cgpa(0.0) {}

    Student(int r, const string& n, const string& e,
            const string& b, int y, double c)
        : rollNo(r), name(n), email(e), branch(b), year(y), cgpa(c) {}
};

// ============================================================
//  UTILITY — pretty-print a single student row
// ============================================================
void printStudentHeader() {
    cout << BRIGHT_CYAN << BOLD
         << std::left
         << std::setw(8)  << "Roll"
         << std::setw(22) << "Name"
         << std::setw(28) << "Email"
         << std::setw(10) << "Branch"
         << std::setw(6)  << "Year"
         << std::setw(8)  << "CGPA"
         << RESET << "\n";
    cout << CYAN << string(82, '-') << RESET << "\n";
}

void printStudent(const Student& s, const string& prefix = "") {
    cout << prefix
         << BRIGHT_WHITE << std::left
         << std::setw(8)  << s.rollNo
         << std::setw(22) << s.name
         << std::setw(28) << s.email
         << std::setw(10) << s.branch
         << std::setw(6)  << s.year
         << BRIGHT_YELLOW << std::fixed << std::setprecision(2)
         << std::setw(8)  << s.cgpa
         << RESET << "\n";
}

// ============================================================
//  1. DOUBLY LINKED LIST
//     — stores the master list of all student records
//     — supports add-front/back, delete, forward/backward traversal
//     — Time: Insert O(1) front/back | Delete O(n) | Traverse O(n)
// ============================================================
struct DLLNode {
    Student data;
    DLLNode* prev;
    DLLNode* next;
    explicit DLLNode(const Student& s)
        : data(s), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
public:
    DLLNode* head;
    DLLNode* tail;
    int size;

    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~DoublyLinkedList() {
        DLLNode* cur = head;
        while (cur) {
            DLLNode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }

    // O(1) — insert at tail
    void insertBack(const Student& s) {
        DLLNode* node = new DLLNode(s);
        if (!tail) {
            head = tail = node;
        } else {
            node->prev = tail;
            tail->next = node;
            tail = node;
        }
        ++size;
    }

    // O(n) — find node by roll number
    DLLNode* findByRoll(int roll) {
        DLLNode* cur = head;
        while (cur) {
            if (cur->data.rollNo == roll) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    // O(n) — delete node by roll number; returns true if found
    bool deleteByRoll(int roll) {
        DLLNode* node = findByRoll(roll);
        if (!node) return false;
        if (node->prev) node->prev->next = node->next;
        else            head = node->next;
        if (node->next) node->next->prev = node->prev;
        else            tail = node->prev;
        delete node;
        --size;
        return true;
    }

    // O(n) — forward traversal (displays all records)
    void traverseForward() const {
        if (!head) {
            cout << YELLOW << "  [!] No records found.\n" << RESET;
            return;
        }
        printStudentHeader();
        DLLNode* cur = head;
        int idx = 1;
        while (cur) {
            string prefix = (idx % 2 == 0) ? DIM : "";
            printStudent(cur->data, prefix);
            cur = cur->next;
            ++idx;
        }
    }

    // O(n) — backward traversal (reverse order)
    void traverseBackward() const {
        if (!tail) {
            cout << YELLOW << "  [!] No records found.\n" << RESET;
            return;
        }
        printStudentHeader();
        DLLNode* cur = tail;
        while (cur) {
            printStudent(cur->data);
            cur = cur->prev;
        }
    }

    // O(n) — copy all students into a plain array (for merge sort)
    // Caller is responsible for deleting the returned array
    Student* toArray(int& outSize) const {
        outSize = size;
        if (size == 0) return nullptr;
        Student* arr = new Student[size];
        DLLNode* cur = head;
        for (int i = 0; i < size; ++i, cur = cur->next)
            arr[i] = cur->data;
        return arr;
    }
};

// ============================================================
//  2. BINARY SEARCH TREE (BST)
//     — indexed on rollNo for fast search / in-order display
//     — Time: Insert/Search O(log n) avg, O(n) worst
//             In-order traversal O(n)
// ============================================================
struct BSTNode {
    Student data;
    BSTNode* left;
    BSTNode* right;
    explicit BSTNode(const Student& s)
        : data(s), left(nullptr), right(nullptr) {}
};

class BST {
public:
    BSTNode* root;

    BST() : root(nullptr) {}

    ~BST() { destroyTree(root); }

    // O(log n) average — insert a student
    void insert(const Student& s) {
        root = insertRec(root, s);
    }

    // O(log n) average — search by roll number
    BSTNode* search(int roll) const {
        return searchRec(root, roll);
    }

    // O(n) — in-order traversal (sorted by roll number)
    void inOrder() const {
        if (!root) {
            cout << YELLOW << "  [!] BST is empty.\n" << RESET;
            return;
        }
        printStudentHeader();
        inOrderRec(root);
    }

    // O(log n) average — delete a node by roll number
    void deleteNode(int roll) {
        root = deleteRec(root, roll);
    }

private:
    BSTNode* insertRec(BSTNode* node, const Student& s) {
        if (!node) return new BSTNode(s);
        if (s.rollNo < node->data.rollNo)
            node->left  = insertRec(node->left,  s);
        else if (s.rollNo > node->data.rollNo)
            node->right = insertRec(node->right, s);
        return node; // duplicate roll numbers ignored
    }

    BSTNode* searchRec(BSTNode* node, int roll) const {
        if (!node || node->data.rollNo == roll) return node;
        if (roll < node->data.rollNo)
            return searchRec(node->left,  roll);
        return searchRec(node->right, roll);
    }

    void inOrderRec(BSTNode* node) const {
        if (!node) return;
        inOrderRec(node->left);
        printStudent(node->data);
        inOrderRec(node->right);
    }

    // Find the minimum-value node in a subtree (used in deletion)
    BSTNode* minNode(BSTNode* node) const {
        while (node->left) node = node->left;
        return node;
    }

    BSTNode* deleteRec(BSTNode* node, int roll) {
        if (!node) return nullptr;
        if (roll < node->data.rollNo)
            node->left  = deleteRec(node->left,  roll);
        else if (roll > node->data.rollNo)
            node->right = deleteRec(node->right, roll);
        else {
            // Node found — handle 3 cases
            if (!node->left) {
                BSTNode* tmp = node->right;
                delete node;
                return tmp;
            }
            if (!node->right) {
                BSTNode* tmp = node->left;
                delete node;
                return tmp;
            }
            // Two children: replace with in-order successor
            BSTNode* succ = minNode(node->right);
            node->data    = succ->data;
            node->right   = deleteRec(node->right, succ->data.rollNo);
        }
        return node;
    }

    void destroyTree(BSTNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
};

// ============================================================
//  3. HASH TABLE
//     — keyed on email string (lowercase) for O(1) lookup
//     — Collision resolution: Separate Chaining (linked lists)
//     — Time: Insert/Search/Delete O(1) avg, O(n) worst
// ============================================================
const int HASH_SIZE = 101; // prime number reduces collisions

struct HashNode {
    string  key;   // email (lowercase)
    Student data;
    HashNode* next;
    HashNode(const string& k, const Student& s)
        : key(k), data(s), next(nullptr) {}
};

class HashTable {
public:
    HashNode* table[HASH_SIZE];

    HashTable() {
        for (int i = 0; i < HASH_SIZE; ++i)
            table[i] = nullptr;
    }

    ~HashTable() {
        for (int i = 0; i < HASH_SIZE; ++i) {
            HashNode* cur = table[i];
            while (cur) {
                HashNode* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
    }

    // djb2 hash function — O(k) where k = key length
    int hash(const string& key) const {
        unsigned long h = 5381;
        for (char c : key)
            h = ((h << 5) + h) + (unsigned char)c; // h * 33 + c
        return (int)(h % HASH_SIZE);
    }

    // O(1) avg — insert (or update) a student by email
    void insert(const Student& s) {
        string k = toLower(s.email);
        int    idx = hash(k);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == k) { cur->data = s; return; } // update
            cur = cur->next;
        }
        // Prepend new node (O(1))
        HashNode* node  = new HashNode(k, s);
        node->next      = table[idx];
        table[idx]      = node;
    }

    // O(1) avg — search by email; returns nullptr if not found
    Student* search(const string& email) {
        string k   = toLower(email);
        int    idx = hash(k);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == k) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    // O(1) avg — remove entry by email
    bool remove(const string& email) {
        string k   = toLower(email);
        int    idx = hash(k);
        HashNode* cur  = table[idx];
        HashNode* prev = nullptr;
        while (cur) {
            if (cur->key == k) {
                if (prev) prev->next = cur->next;
                else       table[idx] = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur  = cur->next;
        }
        return false;
    }

private:
    string toLower(const string& s) const {
        string r = s;
        for (char& c : r) c = (char)tolower((unsigned char)c);
        return r;
    }
};

// ============================================================
//  4. MERGE SORT (on a plain array of Student objects)
//     — sorts by CGPA descending for the leaderboard
//     — Time: O(n log n) always | Space: O(n) auxiliary
// ============================================================

// Merge two sorted halves — O(n) per level
void merge(Student* arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Student* L = new Student[n1];
    Student* R = new Student[n2];

    for (int i = 0; i < n1; ++i) L[i] = arr[left  + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    // Sort descending by CGPA (highest first)
    while (i < n1 && j < n2) {
        if (L[i].cgpa >= R[j].cgpa)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

// O(n log n) — recursive merge sort
void mergeSort(Student* arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left,  mid);
    mergeSort(arr, mid+1, right);
    merge(arr, left, mid, right);
}

// ============================================================
//  5. MAX-HEAP (Priority Queue) — Student Helpdesk
//     — Each request has a priority (1-10, higher = more urgent)
//     — extractMax always dequeues the highest-priority request
//     — Time: Insert O(log n) | extractMax O(log n) | peek O(1)
// ============================================================
struct HelpdeskRequest {
    int    priority;   // 1 (low) .. 10 (critical)
    int    rollNo;
    string studentName;
    string issue;

    HelpdeskRequest() : priority(0), rollNo(0) {}
    HelpdeskRequest(int p, int r, const string& n, const string& iss)
        : priority(p), rollNo(r), studentName(n), issue(iss) {}
};

class MaxHeap {
public:
    HelpdeskRequest* heap;
    int capacity;
    int heapSize;

    MaxHeap(int cap = 200)
        : capacity(cap), heapSize(0) {
        heap = new HelpdeskRequest[cap];
    }

    ~MaxHeap() { delete[] heap; }

    bool isEmpty()    const { return heapSize == 0; }
    int  count()      const { return heapSize; }

    // O(log n) — insert a new request (heapify-up)
    bool insert(const HelpdeskRequest& req) {
        if (heapSize == capacity) return false; // heap full
        heap[heapSize] = req;
        siftUp(heapSize);
        ++heapSize;
        return true;
    }

    // O(1) — peek at highest-priority request without removing
    HelpdeskRequest peek() const {
        return heap[0];
    }

    // O(log n) — remove and return the highest-priority request
    HelpdeskRequest extractMax() {
        HelpdeskRequest top = heap[0];
        heap[0] = heap[--heapSize];
        siftDown(0);
        return top;
    }

    // O(n) — display all pending requests
    void display() const {
        if (isEmpty()) {
            cout << YELLOW << "  [!] Helpdesk queue is empty.\n" << RESET;
            return;
        }
        cout << BRIGHT_MAGENTA << BOLD
             << std::left
             << std::setw(5)  << "#"
             << std::setw(12) << "Priority"
             << std::setw(8)  << "Roll"
             << std::setw(22) << "Student"
             << "Issue\n" << RESET;
        cout << MAGENTA << string(75, '-') << RESET << "\n";
        for (int i = 0; i < heapSize; ++i) {
            string pColor = (heap[i].priority >= 8) ? BRIGHT_RED :
                            (heap[i].priority >= 5) ? BRIGHT_YELLOW : GREEN;
            cout << BRIGHT_WHITE << std::setw(5) << (i + 1)
                 << pColor << std::setw(12) << heap[i].priority
                 << BRIGHT_WHITE
                 << std::setw(8)  << heap[i].rollNo
                 << std::setw(22) << heap[i].studentName
                 << heap[i].issue
                 << RESET << "\n";
        }
    }

private:
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i)   { return 2 * i + 1; }
    int rightChild(int i)  { return 2 * i + 2; }

    void swapNodes(int i, int j) {
        HelpdeskRequest tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;
    }

    // Sift element up to restore heap property — O(log n)
    void siftUp(int i) {
        while (i > 0 && heap[parent(i)].priority < heap[i].priority) {
            swapNodes(i, parent(i));
            i = parent(i);
        }
    }

    // Sift element down to restore heap property — O(log n)
    void siftDown(int i) {
        int largest = i;
        int l = leftChild(i), r = rightChild(i);
        if (l < heapSize && heap[l].priority > heap[largest].priority)
            largest = l;
        if (r < heapSize && heap[r].priority > heap[largest].priority)
            largest = r;
        if (largest != i) {
            swapNodes(i, largest);
            siftDown(largest);
        }
    }
};

// ============================================================
//  GLOBAL SYSTEM STATE
// ============================================================
DoublyLinkedList masterList;
BST              rollBST;
HashTable        emailHash;
MaxHeap          helpdesk;

// ============================================================
//  UI HELPERS
// ============================================================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    cout << DIM << "\n  Press [Enter] to continue..." << RESET;
    cin.ignore();
    cin.get();
}

void printBanner() {
    cout << BRIGHT_CYAN << BOLD << R"(
  +=============================================================+
  |        ~  SMART STUDENT RECORD SYSTEM  ~                   |
  |        DSA Mini Project -- C++ Terminal Edition             |
  +=============================================================+
)" << RESET;
}

void printDivider(const string& title = "") {
    if (title.empty()) {
        cout << CYAN << "  " << string(70, '-') << RESET << "\n";
    } else {
        cout << CYAN << "\n  +--[ " << BOLD << BRIGHT_CYAN << title
             << RESET << CYAN << " ]" << "\n" << RESET;
    }
}

void success(const string& msg) {
    cout << BRIGHT_GREEN << "\n  [OK] " << msg << RESET << "\n";
}

void error(const string& msg) {
    cout << BRIGHT_RED << "\n  [ERR] " << msg << RESET << "\n";
}

void info(const string& msg) {
    cout << BRIGHT_YELLOW << "\n  [i] " << msg << RESET << "\n";
}

// Safe integer read — re-prompts on invalid input
int readInt(const string& prompt) {
    int val;
    while (true) {
        cout << BRIGHT_WHITE << "  " << prompt << RESET;
        cin >> val;
        if (cin.good()) { cin.ignore(); return val; }
        cin.clear();
        cin.ignore(10000, '\n');
        error("Invalid input. Please enter an integer.");
    }
}

// Safe double read — re-prompts on invalid input
double readDouble(const string& prompt) {
    double val;
    while (true) {
        cout << BRIGHT_WHITE << "  " << prompt << RESET;
        cin >> val;
        if (cin.good()) { cin.ignore(); return val; }
        cin.clear();
        cin.ignore(10000, '\n');
        error("Invalid input. Please enter a number.");
    }
}

string readLine(const string& prompt) {
    string val;
    cout << BRIGHT_WHITE << "  " << prompt << RESET;
    std::getline(cin, val);
    return val;
}

// ============================================================
//  SYNC — keeps all 3 data structures consistent
// ============================================================
void syncInsert(const Student& s) {
    masterList.insertBack(s);
    rollBST.insert(s);
    emailHash.insert(s);
}

void syncDelete(int roll, const string& email) {
    masterList.deleteByRoll(roll);
    rollBST.deleteNode(roll);
    emailHash.remove(email);
}

// ============================================================
//  FEATURE FUNCTIONS
// ============================================================

// --- Add a new student ---
void addStudent() {
    printDivider("ADD NEW STUDENT");

    int roll = readInt("Roll Number   : ");
    // Check duplicate in O(log n) using BST
    if (rollBST.search(roll)) {
        error("A student with Roll Number " + std::to_string(roll) + " already exists!");
        pressEnter();
        return;
    }

    string name   = readLine("Full Name      : ");
    string email  = readLine("Email Address  : ");

    // Check duplicate email in O(1) using Hash Table
    if (emailHash.search(email)) {
        error("A student with this email already exists!");
        pressEnter();
        return;
    }

    string branch = readLine("Branch/Dept    : ");
    int    year   = readInt("Year (1-4)     : ");
    double cgpa   = readDouble("CGPA (0.0-10.0): ");

    if (year < 1 || year > 4) { error("Year must be between 1 and 4."); pressEnter(); return; }
    if (cgpa < 0.0 || cgpa > 10.0) { error("CGPA must be between 0.0 and 10.0."); pressEnter(); return; }

    Student s(roll, name, email, branch, year, cgpa);
    syncInsert(s);

    success("Student '" + name + "' added successfully to all data structures!");
    pressEnter();
}

// --- View all students (DLL forward traversal) ---
void viewAllStudents() {
    printDivider("ALL STUDENTS -- DOUBLY LINKED LIST (Forward)");
    cout << BRIGHT_CYAN << "  Total records: " << masterList.size << RESET << "\n\n";
    masterList.traverseForward();
    pressEnter();
}

// --- View all students in reverse (DLL backward traversal) ---
void viewAllReverse() {
    printDivider("ALL STUDENTS -- DOUBLY LINKED LIST (Reverse)");
    cout << BRIGHT_CYAN << "  Total records: " << masterList.size << RESET << "\n\n";
    masterList.traverseBackward();
    pressEnter();
}

// --- Search by Roll Number using BST ---
void searchByRoll() {
    printDivider("SEARCH BY ROLL NUMBER -- BST  [O(log n)]");
    int roll = readInt("Enter Roll Number: ");

    BSTNode* node = rollBST.search(roll);
    if (!node) {
        error("No student found with Roll Number " + std::to_string(roll) + ".");
    } else {
        success("Student found!");
        printStudentHeader();
        printStudent(node->data);
    }
    pressEnter();
}

// --- Search by Email using Hash Table ---
void searchByEmail() {
    printDivider("SEARCH BY EMAIL -- HASH TABLE  [O(1)]");
    string email = readLine("Enter Email Address: ");

    Student* s = emailHash.search(email);
    if (!s) {
        error("No student found with email '" + email + "'.");
    } else {
        success("Student found!");
        printStudentHeader();
        printStudent(*s);
    }
    pressEnter();
}

// --- Delete a student ---
void deleteStudent() {
    printDivider("DELETE STUDENT RECORD");
    int roll = readInt("Enter Roll Number to delete: ");

    BSTNode* node = rollBST.search(roll);
    if (!node) {
        error("Student with Roll Number " + std::to_string(roll) + " not found.");
        pressEnter();
        return;
    }

    string name  = node->data.name;
    string email = node->data.email;

    cout << BRIGHT_YELLOW << "\n  [!] Are you sure you want to delete '"
         << name << "'?  [y/N]: " << RESET;
    char c; cin >> c; cin.ignore();
    if (c != 'y' && c != 'Y') {
        info("Deletion cancelled.");
        pressEnter();
        return;
    }

    syncDelete(roll, email);
    success("Student '" + name + "' deleted from all data structures.");
    pressEnter();
}

// --- Update a student's CGPA ---
void updateCGPA() {
    printDivider("UPDATE STUDENT CGPA");
    int roll = readInt("Enter Roll Number: ");

    BSTNode* node = rollBST.search(roll);
    if (!node) {
        error("Student not found.");
        pressEnter();
        return;
    }

    cout << BRIGHT_WHITE << "\n  Current CGPA of " << node->data.name
         << ": " << BRIGHT_YELLOW << node->data.cgpa << RESET << "\n";

    double newCGPA = readDouble("New CGPA (0.0-10.0): ");
    if (newCGPA < 0.0 || newCGPA > 10.0) {
        error("Invalid CGPA.");
        pressEnter();
        return;
    }

    // Update all three structures
    DLLNode* dllNode = masterList.findByRoll(roll);
    if (dllNode) dllNode->data.cgpa = newCGPA;
    node->data.cgpa = newCGPA;

    // Hash table: update in-place
    Student* hs = emailHash.search(node->data.email);
    if (hs) hs->cgpa = newCGPA;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << newCGPA;
    success("CGPA updated to " + oss.str() + " successfully.");
    pressEnter();
}

// --- Top Performers Leaderboard via Merge Sort ---
void showLeaderboard() {
    printDivider("TOP PERFORMERS LEADERBOARD -- MERGE SORT  [O(n log n)]");

    int size;
    Student* arr = masterList.toArray(size);
    if (!arr || size == 0) {
        error("No student records to sort.");
        pressEnter();
        return;
    }

    mergeSort(arr, 0, size - 1); // sort descending by CGPA

    cout << BRIGHT_GREEN << BOLD << "\n  Leaderboard (Sorted by CGPA -- Highest First)\n\n" << RESET;
    cout << BRIGHT_YELLOW << BOLD
         << std::left
         << std::setw(6)  << "Rank"
         << std::setw(8)  << "Roll"
         << std::setw(22) << "Name"
         << std::setw(12) << "Branch"
         << std::setw(6)  << "Year"
         << "CGPA\n" << RESET;
    cout << GREEN << string(62, '-') << RESET << "\n";

    for (int i = 0; i < size; ++i) {
        string rankColor = (i == 0) ? BRIGHT_YELLOW :
                           (i == 1) ? "\033[37m"     :
                           (i == 2) ? "\033[33m"     : BRIGHT_WHITE;
        string medal = (i == 0) ? " #1" : (i == 1) ? " #2" : (i == 2) ? " #3" : "   ";
        cout << rankColor << std::left
             << std::setw(4)  << (i + 1) << std::setw(3) << medal
             << std::setw(8)  << arr[i].rollNo
             << std::setw(22) << arr[i].name
             << std::setw(12) << arr[i].branch
             << std::setw(6)  << arr[i].year
             << BRIGHT_YELLOW << std::fixed << std::setprecision(2) << arr[i].cgpa
             << RESET << "\n";
    }

    delete[] arr;
    pressEnter();
}

// --- BST in-order display (sorted by roll number) ---
void viewBSTInOrder() {
    printDivider("STUDENTS SORTED BY ROLL NUMBER -- BST In-Order  [O(n)]");
    rollBST.inOrder();
    pressEnter();
}

// ============================================================
//  HELPDESK SUBMENU
// ============================================================
void helpdeskMenu() {
    bool running = true;
    while (running) {
        clearScreen();
        printBanner();
        printDivider("STUDENT HELPDESK -- MAX-HEAP PRIORITY QUEUE");

        cout << BRIGHT_MAGENTA << BOLD << "\n"
             << "  +---------------------------------------------+\n"
             << "  |  [1]  Submit New Request                    |\n"
             << "  |  [2]  Process Next Request (Extract Max)    |\n"
             << "  |  [3]  View Highest-Priority Request (Peek)  |\n"
             << "  |  [4]  View All Pending Requests             |\n"
             << "  |  [0]  Back to Main Menu                     |\n"
             << "  +---------------------------------------------+\n"
             << RESET << "\n";

        int choice = readInt("Select an option: ");
        clearScreen();

        switch (choice) {
        case 1: {
            printDivider("SUBMIT HELPDESK REQUEST");
            int roll = readInt("Your Roll Number  : ");
            BSTNode* node = rollBST.search(roll);
            if (!node) {
                error("Roll Number not found. Register yourself first.");
                pressEnter();
                break;
            }
            int priority = readInt("Priority (1=low, 10=critical): ");
            if (priority < 1 || priority > 10) {
                error("Priority must be between 1 and 10.");
                pressEnter();
                break;
            }
            string issue = readLine("Describe your issue: ");
            HelpdeskRequest req(priority, roll, node->data.name, issue);
            if (helpdesk.insert(req))
                success("Request submitted! Position in queue: " +
                         std::to_string(helpdesk.count()));
            else
                error("Helpdesk queue is full.");
            pressEnter();
            break;
        }
        case 2: {
            printDivider("PROCESSING NEXT REQUEST (Highest Priority First)");
            if (helpdesk.isEmpty()) {
                info("No pending requests in the helpdesk queue.");
                pressEnter();
                break;
            }
            HelpdeskRequest req = helpdesk.extractMax();
            cout << BRIGHT_GREEN << "\n  [OK] Processing request:\n" << RESET;
            cout << BRIGHT_WHITE
                 << "     Student  : " << BRIGHT_YELLOW << req.studentName << "\n" << BRIGHT_WHITE
                 << "     Roll No  : " << BRIGHT_YELLOW << req.rollNo     << "\n" << BRIGHT_WHITE
                 << "     Priority : " << BRIGHT_RED    << req.priority   << "/10\n" << BRIGHT_WHITE
                 << "     Issue    : " << BRIGHT_CYAN   << req.issue      << "\n" << RESET;
            cout << BRIGHT_GREEN << "\n  Request resolved and removed from queue!\n" << RESET;
            pressEnter();
            break;
        }
        case 3: {
            printDivider("HIGHEST PRIORITY REQUEST (Peek)");
            if (helpdesk.isEmpty()) {
                info("No pending requests.");
                pressEnter();
                break;
            }
            HelpdeskRequest req = helpdesk.peek();
            cout << BRIGHT_WHITE
                 << "\n  Next to be processed:\n"
                 << "     Student  : " << BRIGHT_YELLOW << req.studentName << "\n" << BRIGHT_WHITE
                 << "     Roll No  : " << BRIGHT_YELLOW << req.rollNo     << "\n" << BRIGHT_WHITE
                 << "     Priority : " << BRIGHT_RED    << req.priority   << "/10\n" << BRIGHT_WHITE
                 << "     Issue    : " << BRIGHT_CYAN   << req.issue      << "\n" << RESET;
            pressEnter();
            break;
        }
        case 4:
            printDivider("ALL PENDING HELPDESK REQUESTS");
            cout << BRIGHT_MAGENTA << "  Pending: " << helpdesk.count() << " request(s)\n\n" << RESET;
            helpdesk.display();
            pressEnter();
            break;
        case 0:
            running = false;
            break;
        default:
            error("Invalid option. Please try again.");
            pressEnter();
        }
    }
}

// ============================================================
//  SEED DATA — pre-loads sample students for demonstration
// ============================================================
void seedData() {
    Student samples[] = {
        {101, "Aryan Mehta",    "aryan.mehta@college.edu",    "CSE",  2, 9.10},
        {102, "Priya Sharma",   "priya.sharma@college.edu",   "ECE",  3, 8.75},
        {103, "Rohan Das",      "rohan.das@college.edu",      "MECH", 1, 7.50},
        {104, "Sneha Patil",    "sneha.patil@college.edu",    "CSE",  4, 9.45},
        {105, "Kiran Kumar",    "kiran.kumar@college.edu",    "CIVIL",2, 6.80},
        {106, "Aisha Siddiqui", "aisha.siddiqui@college.edu", "CSE",  3, 8.20},
        {107, "Dev Nair",       "dev.nair@college.edu",       "ECE",  2, 7.90},
        {108, "Meera Iyer",     "meera.iyer@college.edu",     "CSE",  1, 8.60},
    };
    for (auto& s : samples)
        syncInsert(s);
}

// ============================================================
//  MAIN MENU
// ============================================================
void mainMenu() {
    bool running = true;
    while (running) {
        clearScreen();
        printBanner();

        cout << BRIGHT_CYAN << BOLD << "\n  DSA Structures: "
             << BRIGHT_WHITE << "DLL  |  BST  |  Hash Table  |  Merge Sort  |  Max-Heap\n" << RESET;

        cout << BRIGHT_CYAN << "  Records in system: "
             << BRIGHT_YELLOW << masterList.size
             << BRIGHT_CYAN   << "   |   Helpdesk Queue: "
             << BRIGHT_YELLOW << helpdesk.count()
             << " pending\n" << RESET;

        cout << BRIGHT_WHITE << BOLD << "\n"
             << "  +============================================================+\n"
             << "  |                     MAIN MENU                             |\n"
             << "  +============================================================+\n"
             << "  |  STUDENT MANAGEMENT                                       |\n"
             << "  |   [1]  Add New Student                                    |\n"
             << "  |   [2]  View All Students (DLL -- Forward)                 |\n"
             << "  |   [3]  View All Students (DLL -- Reverse)                 |\n"
             << "  |   [4]  Search by Roll Number (BST)                        |\n"
             << "  |   [5]  Search by Email (Hash Table)                       |\n"
             << "  |   [6]  Update Student CGPA                                |\n"
             << "  |   [7]  Delete Student Record                              |\n"
             << "  +------------------------------------------------------------+\n"
             << "  |  REPORTS & ANALYTICS                                      |\n"
             << "  |   [8]  Top Performers Leaderboard (Merge Sort)            |\n"
             << "  |   [9]  View Records Sorted by Roll No. (BST In-Order)     |\n"
             << "  +------------------------------------------------------------+\n"
             << "  |  HELPDESK                                                 |\n"
             << "  |   [10] Student Helpdesk (Max-Heap Priority Queue)         |\n"
             << "  +------------------------------------------------------------+\n"
             << "  |   [0]  Exit                                               |\n"
             << "  +============================================================+\n"
             << RESET << "\n";

        int choice = readInt("Select an option: ");
        clearScreen();

        switch (choice) {
        case 1:  addStudent();      break;
        case 2:  viewAllStudents(); break;
        case 3:  viewAllReverse();  break;
        case 4:  searchByRoll();    break;
        case 5:  searchByEmail();   break;
        case 6:  updateCGPA();      break;
        case 7:  deleteStudent();   break;
        case 8:  showLeaderboard(); break;
        case 9:  viewBSTInOrder();  break;
        case 10: helpdeskMenu();    break;
        case 0:
            cout << BRIGHT_GREEN << BOLD
                 << "\n  Thank you for using Smart Student Record System. Goodbye!\n\n"
                 << RESET;
            running = false;
            break;
        default:
            error("Invalid option. Please choose a number from the menu.");
            pressEnter();
        }
    }
}

// ============================================================
//  ENTRY POINT
// ============================================================
int main() {
    // Enable UTF-8 and ANSI on Windows
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    seedData();   // load demo records into all 3 data structures
    mainMenu();   // launch the interactive menu

    return 0;
}
