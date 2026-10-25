// ============================================================
//   Smart Student Record System (SSRS) — C++ REST API Backend
//   DSA Mini Project Backend Server
//
//   Data Structures implemented from scratch (no STL containers):
//     1. Doubly Linked List  — master record store (forward / backward)
//     2. Binary Search Tree  — O(log n) roll number lookup & in-order
//     3. Hash Table          — O(1) email lookup (djb2 + separate chaining)
//     4. Merge Sort          — O(n log n) CGPA leaderboard sorting
//     5. Max-Heap            — O(log n) Helpdesk priority queue
// ============================================================

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <mutex>
#include <cctype>
#include <cstdlib>
#include <algorithm>

#include "httplib.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

// ============================================================
//  CORE DATA STRUCTURE: STUDENT RECORD
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

// JSON string escaping
string escapeJson(const string& s) {
    std::ostringstream oss;
    for (char c : s) {
        if (c == '"') oss << "\\\"";
        else if (c == '\\') oss << "\\\\";
        else if (c == '\b') oss << "\\b";
        else if (c == '\f') oss << "\\f";
        else if (c == '\n') oss << "\\n";
        else if (c == '\r') oss << "\\r";
        else if (c == '\t') oss << "\\t";
        else oss << c;
    }
    return oss.str();
}

string studentToJson(const Student& s) {
    std::ostringstream oss;
    oss << "{"
        << "\"rollNo\":" << s.rollNo << ","
        << "\"name\":\"" << escapeJson(s.name) << "\","
        << "\"email\":\"" << escapeJson(s.email) << "\","
        << "\"branch\":\"" << escapeJson(s.branch) << "\","
        << "\"year\":" << s.year << ","
        << "\"cgpa\":" << std::fixed << std::setprecision(2) << s.cgpa
        << "}";
    return oss.str();
}

string studentListToJson(const vector<Student>& list) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < list.size(); ++i) {
        if (i > 0) oss << ",";
        oss << studentToJson(list[i]);
    }
    oss << "]";
    return oss.str();
}

// Helpers for simple JSON body parsing
string extractJsonString(const string& json, const string& key) {
    string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == string::npos) return "";
    pos = json.find(':', pos);
    if (pos == string::npos) return "";
    pos = json.find('"', pos);
    if (pos == string::npos) return "";
    size_t end = json.find('"', pos + 1);
    if (end == string::npos) return "";
    return json.substr(pos + 1, end - pos - 1);
}

int extractJsonInt(const string& json, const string& key, int defaultVal = 0) {
    string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == string::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == string::npos) return defaultVal;
    pos = json.find_first_of("-0123456789", pos);
    if (pos == string::npos) return defaultVal;
    size_t end = json.find_first_not_of("0123456789", pos + (json[pos] == '-' ? 1 : 0));
    try {
        return std::stoi(json.substr(pos, end == string::npos ? string::npos : end - pos));
    } catch (...) {
        return defaultVal;
    }
}

double extractJsonDouble(const string& json, const string& key, double defaultVal = 0.0) {
    string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == string::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == string::npos) return defaultVal;
    pos = json.find_first_of("-0123456789.", pos);
    if (pos == string::npos) return defaultVal;
    size_t end = json.find_first_not_of("0123456789.", pos);
    try {
        return std::stod(json.substr(pos, end == string::npos ? string::npos : end - pos));
    } catch (...) {
        return defaultVal;
    }
}

// ============================================================
//  1. DOUBLY LINKED LIST
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

    DLLNode* findByRoll(int roll) {
        DLLNode* cur = head;
        while (cur) {
            if (cur->data.rollNo == roll) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

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

    vector<Student> toVectorForward() const {
        vector<Student> result;
        DLLNode* cur = head;
        while (cur) {
            result.push_back(cur->data);
            cur = cur->next;
        }
        return result;
    }

    vector<Student> toVectorBackward() const {
        vector<Student> result;
        DLLNode* cur = tail;
        while (cur) {
            result.push_back(cur->data);
            cur = cur->prev;
        }
        return result;
    }

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

    void insert(const Student& s) {
        root = insertRec(root, s);
    }

    BSTNode* search(int roll) const {
        return searchRec(root, roll);
    }

    void inOrder(vector<Student>& out) const {
        inOrderRec(root, out);
    }

    void deleteNode(int roll) {
        root = deleteRec(root, roll);
    }

    // Helper for visualizer tree representation
    void getStructureRec(BSTNode* node, int parentId, const string& side,
                         std::ostringstream& nodesJson,
                         std::ostringstream& edgesJson,
                         bool& firstNode, bool& firstEdge) const {
        if (!node) return;
        if (!firstNode) nodesJson << ",";
        firstNode = false;
        nodesJson << "{\"id\":" << node->data.rollNo
                  << ",\"roll\":" << node->data.rollNo
                  << ",\"name\":\"" << escapeJson(node->data.name) << "\""
                  << ",\"cgpa\":" << std::fixed << std::setprecision(2) << node->data.cgpa << "}";

        if (parentId != -1) {
            if (!firstEdge) edgesJson << ",";
            firstEdge = false;
            edgesJson << "{\"from\":" << parentId
                      << ",\"to\":" << node->data.rollNo
                      << ",\"side\":\"" << side << "\"}";
        }

        getStructureRec(node->left, node->data.rollNo, "left", nodesJson, edgesJson, firstNode, firstEdge);
        getStructureRec(node->right, node->data.rollNo, "right", nodesJson, edgesJson, firstNode, firstEdge);
    }

    string getStructureJson() const {
        std::ostringstream nodesJson, edgesJson;
        bool firstNode = true, firstEdge = true;
        getStructureRec(root, -1, "", nodesJson, edgesJson, firstNode, firstEdge);
        std::ostringstream out;
        out << "{"
            << "\"nodes\":[" << nodesJson.str() << "],"
            << "\"edges\":[" << edgesJson.str() << "],"
            << "\"root\":" << (root ? std::to_string(root->data.rollNo) : "null")
            << "}";
        return out.str();
    }

private:
    BSTNode* insertRec(BSTNode* node, const Student& s) {
        if (!node) return new BSTNode(s);
        if (s.rollNo < node->data.rollNo)
            node->left = insertRec(node->left, s);
        else if (s.rollNo > node->data.rollNo)
            node->right = insertRec(node->right, s);
        return node;
    }

    BSTNode* searchRec(BSTNode* node, int roll) const {
        if (!node || node->data.rollNo == roll) return node;
        if (roll < node->data.rollNo)
            return searchRec(node->left, roll);
        return searchRec(node->right, roll);
    }

    void inOrderRec(BSTNode* node, vector<Student>& out) const {
        if (!node) return;
        inOrderRec(node->left, out);
        out.push_back(node->data);
        inOrderRec(node->right, out);
    }

    BSTNode* minNode(BSTNode* node) const {
        while (node && node->left) node = node->left;
        return node;
    }

    BSTNode* deleteRec(BSTNode* node, int roll) {
        if (!node) return nullptr;
        if (roll < node->data.rollNo)
            node->left = deleteRec(node->left, roll);
        else if (roll > node->data.rollNo)
            node->right = deleteRec(node->right, roll);
        else {
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
            BSTNode* succ = minNode(node->right);
            node->data = succ->data;
            node->right = deleteRec(node->right, succ->data.rollNo);
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
//  3. HASH TABLE (djb2 + Separate Chaining)
// ============================================================
const int HASH_SIZE = 101;

struct HashNode {
    string key;
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

    int hash(const string& key) const {
        unsigned long h = 5381;
        for (char c : key)
            h = ((h << 5) + h) + (unsigned char)c;
        return (int)(h % HASH_SIZE);
    }

    void insert(const Student& s) {
        string k = toLower(s.email);
        int idx = hash(k);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == k) { cur->data = s; return; }
            cur = cur->next;
        }
        HashNode* node = new HashNode(k, s);
        node->next = table[idx];
        table[idx] = node;
    }

    Student* search(const string& email) {
        string k = toLower(email);
        int idx = hash(k);
        HashNode* cur = table[idx];
        while (cur) {
            if (cur->key == k) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    bool remove(const string& email) {
        string k = toLower(email);
        int idx = hash(k);
        HashNode* cur = table[idx];
        HashNode* prev = nullptr;
        while (cur) {
            if (cur->key == k) {
                if (prev) prev->next = cur->next;
                else      table[idx] = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    // Helper for visualizer
    string getBucketsJson() const {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        for (int i = 0; i < HASH_SIZE; ++i) {
            if (!table[i]) continue;
            if (!first) oss << ",";
            first = false;
            oss << "{\"idx\":" << i << ",\"items\":[";
            HashNode* cur = table[i];
            bool firstItem = true;
            while (cur) {
                if (!firstItem) oss << ",";
                firstItem = false;
                oss << "\"" << escapeJson(cur->key) << "\"";
                cur = cur->next;
            }
            oss << "]}";
        }
        oss << "]";
        return oss.str();
    }

private:
    string toLower(const string& s) const {
        string r = s;
        for (char& c : r) c = (char)tolower((unsigned char)c);
        return r;
    }
};

// ============================================================
//  4. MERGE SORT
// ============================================================
void merge(Student* arr, int left, int mid, int right, vector<string>* steps = nullptr) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Student* L = new Student[n1];
    Student* R = new Student[n2];

    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (steps) {
            std::ostringstream oss;
            oss << "Comparing " << L[i].name << " (" << std::fixed << std::setprecision(2) << L[i].cgpa << ")"
                << (L[i].cgpa >= R[j].cgpa ? " >= " : " < ")
                << R[j].name << " (" << std::fixed << std::setprecision(2) << R[j].cgpa << ")"
                << " -> choose " << (L[i].cgpa >= R[j].cgpa ? L[i].name : R[j].name);
            steps->push_back(oss.str());
        }
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

void mergeSort(Student* arr, int left, int right, vector<string>* steps = nullptr) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, steps);
    mergeSort(arr, mid + 1, right, steps);
    merge(arr, left, mid, right, steps);
}

// ============================================================
//  5. MAX-HEAP (Student Helpdesk Priority Queue)
// ============================================================
struct HelpdeskRequest {
    int    priority; // 1 (low) .. 10 (critical)
    int    rollNo;
    string studentName;
    string issue;

    HelpdeskRequest() : priority(0), rollNo(0) {}
    HelpdeskRequest(int p, int r, const string& n, const string& iss)
        : priority(p), rollNo(r), studentName(n), issue(iss) {}
};

string helpdeskToJson(const HelpdeskRequest& req) {
    std::ostringstream oss;
    oss << "{"
        << "\"priority\":" << req.priority << ","
        << "\"rollNo\":" << req.rollNo << ","
        << "\"studentName\":\"" << escapeJson(req.studentName) << "\","
        << "\"issue\":\"" << escapeJson(req.issue) << "\""
        << "}";
    return oss.str();
}

class MaxHeap {
public:
    HelpdeskRequest* heap;
    int capacity;
    int heapSize;

    MaxHeap(int cap = 200) : capacity(cap), heapSize(0) {
        heap = new HelpdeskRequest[cap];
    }

    ~MaxHeap() { delete[] heap; }

    bool isEmpty() const { return heapSize == 0; }
    int count() const { return heapSize; }

    bool insert(const HelpdeskRequest& req) {
        if (heapSize == capacity) return false;
        heap[heapSize] = req;
        siftUp(heapSize);
        ++heapSize;
        return true;
    }

    HelpdeskRequest peek() const {
        if (isEmpty()) return HelpdeskRequest();
        return heap[0];
    }

    HelpdeskRequest extractMax() {
        if (isEmpty()) return HelpdeskRequest();
        HelpdeskRequest top = heap[0];
        heap[0] = heap[--heapSize];
        siftDown(0);
        return top;
    }

    vector<HelpdeskRequest> toVector() const {
        vector<HelpdeskRequest> list;
        for (int i = 0; i < heapSize; ++i)
            list.push_back(heap[i]);
        return list;
    }

private:
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void swapNodes(int i, int j) {
        HelpdeskRequest tmp = heap[i];
        heap[i] = heap[j];
        heap[j] = tmp;
    }

    void siftUp(int i) {
        while (i > 0 && heap[parent(i)].priority < heap[i].priority) {
            swapNodes(i, parent(i));
            i = parent(i);
        }
    }

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
//  GLOBAL STATE & THREAD SYNCHRONIZATION
// ============================================================
DoublyLinkedList masterList;
BST              rollBST;
HashTable        emailHash;
MaxHeap          helpdesk;
std::mutex       dsaMutex;

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

void seedInitialData() {
    Student samples[] = {
        {101, "Aryan Mehta",    "aryan.mehta@college.edu",    "CSE",   2, 9.10},
        {102, "Priya Sharma",   "priya.sharma@college.edu",   "ECE",   3, 8.75},
        {103, "Rohan Das",      "rohan.das@college.edu",      "MECH",  1, 7.50},
        {104, "Sneha Patil",    "sneha.patil@college.edu",    "CSE",   4, 9.45},
        {105, "Kiran Kumar",    "kiran.kumar@college.edu",    "CIVIL", 2, 6.80},
        {106, "Aisha Siddiqui", "aisha.siddiqui@college.edu", "CSE",   3, 8.20},
        {107, "Dev Nair",       "dev.nair@college.edu",       "ECE",   2, 7.90},
        {108, "Meera Iyer",     "meera.iyer@college.edu",     "CSE",   1, 8.60},
    };
    for (const auto& s : samples)
        syncInsert(s);
}

// ============================================================
//  CORS & RESPONSE HELPERS
// ============================================================
void setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void sendJson(httplib::Response& res, int status, const string& jsonBody) {
    setCorsHeaders(res);
    res.status = status;
    res.set_content(jsonBody, "application/json");
}

void sendError(httplib::Response& res, int status, const string& errorMsg) {
    std::ostringstream oss;
    oss << "{\"success\":false,\"error\":\"" << escapeJson(errorMsg) << "\"}";
    sendJson(res, status, oss.str());
}

// ============================================================
//  MAIN HTTP SERVER ROUTING
// ============================================================
int main(int argc, char* argv[]) {
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    int port = 8080;
    if (const char* envPort = std::getenv("PORT")) {
        try { port = std::stoi(envPort); } catch (...) {}
    }
    if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }

    seedInitialData();

    httplib::Server svr;

    // CORS preflight options handler
    svr.Options(R"(/api/.*)", [](const httplib::Request&, httplib::Response& res) {
        setCorsHeaders(res);
        res.status = 200;
    });

    // ── 1. STATUS & METADATA ───────────────────────────────
    svr.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        std::ostringstream oss;
        oss << "{"
            << "\"success\":true,"
            << "\"backend\":\"C++ Native DSA Backend\","
            << "\"version\":\"1.0.0\","
            << "\"records\":" << masterList.size << ","
            << "\"helpdeskQueue\":" << helpdesk.count() << ","
            << "\"structures\":[\"DoublyLinkedList\",\"BinarySearchTree\",\"HashTable\",\"MergeSort\",\"MaxHeap\"]"
            << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 2. VIEW ALL STUDENTS (DLL Traversal) ───────────────
    svr.Get("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        string order = req.has_param("order") ? req.get_param_value("order") : "forward";
        vector<Student> list;
        if (order == "backward" || order == "reverse") {
            list = masterList.toVectorBackward();
        } else {
            list = masterList.toVectorForward();
        }
        std::ostringstream oss;
        oss << "{\"success\":true,\"order\":\"" << order << "\",\"count\":" << list.size()
            << ",\"students\":" << studentListToJson(list) << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 3. SEARCH BY ROLL (BST Lookup - O(log n)) ─────────
    svr.Get("/api/students/search/roll", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("roll")) {
            sendError(res, 400, "Missing 'roll' parameter.");
            return;
        }
        int roll = 0;
        try { roll = std::stoi(req.get_param_value("roll")); }
        catch (...) { sendError(res, 400, "Invalid roll number."); return; }

        std::lock_guard<std::mutex> lock(dsaMutex);
        BSTNode* node = rollBST.search(roll);
        if (!node) {
            sendError(res, 404, "Student not found in BST for Roll " + std::to_string(roll));
            return;
        }

        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"BST O(log n)\",\"student\":"
            << studentToJson(node->data) << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 4. SEARCH BY EMAIL (Hash Table Lookup - O(1)) ─────
    svr.Get("/api/students/search/email", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("email")) {
            sendError(res, 400, "Missing 'email' parameter.");
            return;
        }
        string email = req.get_param_value("email");

        std::lock_guard<std::mutex> lock(dsaMutex);
        Student* s = emailHash.search(email);
        if (!s) {
            sendError(res, 404, "Student not found in Hash Table for email: " + email);
            return;
        }

        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"Hash Table O(1) djb2\",\"student\":"
            << studentToJson(*s) << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 5. ADD NEW STUDENT (Synced Insert) ─────────────────
    svr.Post("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        int roll = extractJsonInt(body, "rollNo", 0);
        string name = extractJsonString(body, "name");
        string email = extractJsonString(body, "email");
        string branch = extractJsonString(body, "branch");
        int year = extractJsonInt(body, "year", 0);
        double cgpa = extractJsonDouble(body, "cgpa", -1.0);

        if (roll <= 0 || name.empty() || email.empty() || branch.empty() ||
            year < 1 || year > 4 || cgpa < 0.0 || cgpa > 10.0) {
            sendError(res, 400, "Invalid student details. Ensure all fields are filled properly.");
            return;
        }

        std::lock_guard<std::mutex> lock(dsaMutex);
        if (rollBST.search(roll)) {
            sendError(res, 409, "Duplicate Roll Number! Student with Roll " + std::to_string(roll) + " already exists in BST.");
            return;
        }
        if (emailHash.search(email)) {
            sendError(res, 409, "Duplicate Email! Student with email '" + email + "' already registered in Hash Table.");
            return;
        }

        Student newStudent(roll, name, email, branch, year, cgpa);
        syncInsert(newStudent);

        std::ostringstream oss;
        oss << "{\"success\":true,\"message\":\"Student successfully added to DLL, BST, and Hash Table.\","
            << "\"student\":" << studentToJson(newStudent) << "}";
        sendJson(res, 201, oss.str());
    });

    // ── 6. UPDATE CGPA (Synced Update across DLL, BST, Hash) ──
    svr.Put("/api/students/update-cgpa", [](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        int roll = extractJsonInt(body, "rollNo", 0);
        double newCGPA = extractJsonDouble(body, "cgpa", -1.0);

        if (roll <= 0 || newCGPA < 0.0 || newCGPA > 10.0) {
            sendError(res, 400, "Invalid roll number or CGPA range (0.0 - 10.0).");
            return;
        }

        std::lock_guard<std::mutex> lock(dsaMutex);
        BSTNode* bstNode = rollBST.search(roll);
        if (!bstNode) {
            sendError(res, 404, "Student with Roll " + std::to_string(roll) + " not found.");
            return;
        }

        double oldCGPA = bstNode->data.cgpa;
        bstNode->data.cgpa = newCGPA;

        DLLNode* dllNode = masterList.findByRoll(roll);
        if (dllNode) dllNode->data.cgpa = newCGPA;

        Student* hashStudent = emailHash.search(bstNode->data.email);
        if (hashStudent) hashStudent->cgpa = newCGPA;

        std::ostringstream oss;
        oss << "{\"success\":true,\"message\":\"CGPA updated successfully across all data structures.\","
            << "\"oldCgpa\":" << std::fixed << std::setprecision(2) << oldCGPA << ","
            << "\"newCgpa\":" << std::fixed << std::setprecision(2) << newCGPA << ","
            << "\"student\":" << studentToJson(bstNode->data) << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 7. DELETE STUDENT (Synced Delete) ──────────────────
    svr.Delete("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("roll")) {
            sendError(res, 400, "Missing 'roll' parameter.");
            return;
        }
        int roll = 0;
        try { roll = std::stoi(req.get_param_value("roll")); }
        catch (...) { sendError(res, 400, "Invalid roll number."); return; }

        std::lock_guard<std::mutex> lock(dsaMutex);
        BSTNode* node = rollBST.search(roll);
        if (!node) {
            sendError(res, 404, "Student with Roll " + std::to_string(roll) + " not found.");
            return;
        }

        string email = node->data.email;
        string name = node->data.name;
        syncDelete(roll, email);

        std::ostringstream oss;
        oss << "{\"success\":true,\"message\":\"Student '" << escapeJson(name)
            << "' (Roll " << roll << ") deleted from DLL, BST, and Hash Table.\"}";
        sendJson(res, 200, oss.str());
    });

    // ── 8. LEADERBOARD (Merge Sort - O(n log n)) ──────────
    svr.Get("/api/leaderboard", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        int size = 0;
        Student* arr = masterList.toArray(size);
        if (!arr || size == 0) {
            sendJson(res, 200, "{\"success\":true,\"algorithm\":\"Merge Sort O(n log n)\",\"students\":[]}");
            return;
        }

        vector<string> steps;
        mergeSort(arr, 0, size - 1, &steps);

        vector<Student> sortedList;
        for (int i = 0; i < size; ++i) sortedList.push_back(arr[i]);
        delete[] arr;

        std::ostringstream stepsJson;
        stepsJson << "[";
        for (size_t i = 0; i < steps.size(); ++i) {
            if (i > 0) stepsJson << ",";
            stepsJson << "\"" << escapeJson(steps[i]) << "\"";
        }
        stepsJson << "]";

        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"Merge Sort O(n log n)\","
            << "\"count\":" << sortedList.size() << ","
            << "\"students\":" << studentListToJson(sortedList) << ","
            << "\"sortSteps\":" << stepsJson.str() << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 9. BST IN-ORDER TRAVERSAL (Sorted by Roll) ────────
    svr.Get("/api/students/bst-inorder", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        vector<Student> sortedRoll;
        rollBST.inOrder(sortedRoll);

        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"BST In-Order Traversal O(n)\","
            << "\"count\":" << sortedRoll.size() << ","
            << "\"students\":" << studentListToJson(sortedRoll) << "}";
        sendJson(res, 200, oss.str());
    });

    // ── 10. HELPDESK (Max-Heap Priority Queue) ─────────────
    svr.Post("/api/helpdesk", [](const httplib::Request& req, httplib::Response& res) {
        string body = req.body;
        int roll = extractJsonInt(body, "rollNo", 0);
        int priority = extractJsonInt(body, "priority", 5);
        string issue = extractJsonString(body, "issue");

        if (roll <= 0 || priority < 1 || priority > 10 || issue.empty()) {
            sendError(res, 400, "Invalid helpdesk parameters. Priority must be 1-10.");
            return;
        }

        std::lock_guard<std::mutex> lock(dsaMutex);
        BSTNode* studentNode = rollBST.search(roll);
        if (!studentNode) {
            sendError(res, 404, "Roll " + std::to_string(roll) + " not found. Register student first.");
            return;
        }

        HelpdeskRequest request(priority, roll, studentNode->data.name, issue);
        if (!helpdesk.insert(request)) {
            sendError(res, 500, "Helpdesk queue is full.");
            return;
        }

        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"Max-Heap Insert (siftUp) O(log n)\","
            << "\"queueSize\":" << helpdesk.count() << ","
            << "\"request\":" << helpdeskToJson(request) << "}";
        sendJson(res, 201, oss.str());
    });

    svr.Post("/api/helpdesk/process", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        if (helpdesk.isEmpty()) {
            sendError(res, 404, "Helpdesk queue is empty.");
            return;
        }
        HelpdeskRequest processed = helpdesk.extractMax();
        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"Max-Heap extractMax (siftDown) O(log n)\","
            << "\"remainingQueue\":" << helpdesk.count() << ","
            << "\"processedRequest\":" << helpdeskToJson(processed) << "}";
        sendJson(res, 200, oss.str());
    });

    svr.Get("/api/helpdesk/peek", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        if (helpdesk.isEmpty()) {
            sendError(res, 404, "Helpdesk queue is empty.");
            return;
        }
        HelpdeskRequest top = helpdesk.peek();
        std::ostringstream oss;
        oss << "{\"success\":true,\"algorithm\":\"Max-Heap peek O(1)\","
            << "\"request\":" << helpdeskToJson(top) << "}";
        sendJson(res, 200, oss.str());
    });

    svr.Get("/api/helpdesk", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        vector<HelpdeskRequest> list = helpdesk.toVector();
        std::ostringstream oss;
        oss << "{\"success\":true,\"count\":" << list.size() << ",\"requests\":[";
        for (size_t i = 0; i < list.size(); ++i) {
            if (i > 0) oss << ",";
            oss << helpdeskToJson(list[i]);
        }
        oss << "]}";
        sendJson(res, 200, oss.str());
    });

    // ── 11. VISUALIZER DATA API ────────────────────────────
    svr.Get("/api/dsa/viz-data", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(dsaMutex);
        vector<Student> dllList = masterList.toVectorForward();
        string bstStruct = rollBST.getStructureJson();
        string hashBuckets = emailHash.getBucketsJson();
        vector<HelpdeskRequest> heapList = helpdesk.toVector();

        std::ostringstream oss;
        oss << "{"
            << "\"success\":true,"
            << "\"dll\":" << studentListToJson(dllList) << ","
            << "\"bst\":" << bstStruct << ","
            << "\"hashTable\":{\"hashSize\":" << HASH_SIZE << ",\"buckets\":" << hashBuckets << "},"
            << "\"heap\":[";
        for (size_t i = 0; i < heapList.size(); ++i) {
            if (i > 0) oss << ",";
            oss << helpdeskToJson(heapList[i]);
        }
        oss << "]}";
        sendJson(res, 200, oss.str());
    });

    // Serve static files from current directory
    svr.set_mount_point("/", "./");

    cout << "\n========================================================" << endl;
    cout << "  Smart Student Record System (SSRS) — C++ Backend Server" << endl;
    cout << "========================================================" << endl;
    cout << "  [OK] C++ DSA Engines loaded: DLL, BST, Hash Table, Merge Sort, Max-Heap" << endl;
    cout << "  [OK] Seed records loaded: " << masterList.size << endl;
    cout << "  [OK] Server listening at: http://localhost:" << port << endl;
    cout << "  [OK] Open your browser and navigate to: http://localhost:" << port << endl;
    cout << "========================================================\n" << endl;

    if (!svr.listen("0.0.0.0", port)) {
        cout << "[ERR] Failed to bind to port " << port << ". Try running with another port argument." << endl;
        return 1;
    }

    return 0;
}
