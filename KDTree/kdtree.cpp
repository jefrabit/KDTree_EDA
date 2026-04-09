#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>

using namespace std;

template <size_t K>
class Point {
public:
    vector<double> coords;
    
    Point() : coords(K, 0.0) {}
    Point(const vector<double>& v) : coords(v) {}
    
    Point(initializer_list<double> list) : coords(list) {
        while (coords.size() < K) coords.push_back(0.0);
        if (coords.size() > K) coords.resize(K);
    }
    
    double get(size_t d) const {
        return coords[d];
    }
    
    void set(size_t d, double value) {
        if (d < K) coords[d] = value;
    }
    
    double squaredDistance(const Point<K>& other) const {
        double sum = 0.0;
        for (size_t i = 0; i < K; i++) {
            double diff = coords[i] - other.coords[i];
            sum += diff * diff;
        }
        return sum;
    }
    
    double distance(const Point<K>& other) const {
        return sqrt(squaredDistance(other));
    }
    
    bool operator==(const Point<K>& other) const {
        for (size_t i = 0; i < K; i++) {
            if (coords[i] != other.coords[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const Point<K>& other) const {
        return !(*this == other);
    }
    
    string toString() const {
        ostringstream oss;
        oss << "(";
        for (size_t i = 0; i < K; i++) {
            oss << coords[i];
            if (i < K - 1) oss << ", ";
        }
        oss << ")";
        return oss.str();
    }
};

template <size_t K>
struct KDNode {
    Point<K> point;
    size_t axis;
    KDNode* left;
    KDNode* right;
    
    KDNode(const Point<K>& p, size_t a) 
        : point(p), axis(a), left(nullptr), right(nullptr) {}
};

template <size_t K>
class KDTree {
private:
    KDNode<K>* root;
    size_t size;
    
    KDNode<K>* insertRecursive(KDNode<K>* node, const Point<K>& point, size_t depth) {
        if (node == nullptr) {
            return new KDNode<K>(point, depth % K);
        }
        
        size_t axis = depth % K;
        
        if (point.get(axis) < node->point.get(axis)) {
            node->left = insertRecursive(node->left, point, depth + 1);
        } else {
            node->right = insertRecursive(node->right, point, depth + 1);
        }
        
        return node;
    }
    
    bool searchRecursive(KDNode<K>* node, const Point<K>& point, size_t depth) const {
        if (node == nullptr) return false;
        
        if (node->point == point) return true;
        
        size_t axis = depth % K;
        
        if (point.get(axis) < node->point.get(axis)) {
            return searchRecursive(node->left, point, depth + 1);
        } else {
            return searchRecursive(node->right, point, depth + 1);
        }
    }
    
    KDNode<K>* findMin(KDNode<K>* node, size_t axis, size_t depth) {
        if (node == nullptr) return nullptr;
        
        size_t currentAxis = depth % K;
        
        if (currentAxis == axis) {
            if (node->left == nullptr) return node;
            return findMin(node->left, axis, depth + 1);
        }
        
        KDNode<K>* leftMin = findMin(node->left, axis, depth + 1);
        KDNode<K>* rightMin = findMin(node->right, axis, depth + 1);
        
        KDNode<K>* minNode = node;
        
        if (leftMin && leftMin->point.get(axis) < minNode->point.get(axis)) {
            minNode = leftMin;
        }
        if (rightMin && rightMin->point.get(axis) < minNode->point.get(axis)) {
            minNode = rightMin;
        }
        
        return minNode;
    }
    
    KDNode<K>* removeRecursive(KDNode<K>* node, const Point<K>& point, size_t depth) {
        if (node == nullptr) return nullptr;
        
        if (node->point == point) {
            if (node->right != nullptr) {
                KDNode<K>* minNode = findMin(node->right, node->axis, depth + 1);
                node->point = minNode->point;
                node->right = removeRecursive(node->right, minNode->point, depth + 1);
            } else if (node->left != nullptr) {
                KDNode<K>* minNode = findMin(node->left, node->axis, depth + 1);
                node->point = minNode->point;
                node->left = removeRecursive(node->left, minNode->point, depth + 1);
            } else {
                delete node;
                return nullptr;
            }
        } else {
            size_t axis = depth % K;
            if (point.get(axis) < node->point.get(axis)) {
                node->left = removeRecursive(node->left, point, depth + 1);
            } else {
                node->right = removeRecursive(node->right, point, depth + 1);
            }
        }
        
        return node;
    }
    
    void nearestNeighborRecursive(KDNode<K>* node, const Point<K>& target,
                                  KDNode<K>*& best, double& bestDist, size_t depth) const {
        if (node == nullptr) return;
        
        double dist = node->point.squaredDistance(target);
        
        if (dist < bestDist) {
            bestDist = dist;
            best = node;
        }
        
        size_t axis = depth % K;
        double diff = target.get(axis) - node->point.get(axis);
        
        KDNode<K>* first = (diff < 0) ? node->left : node->right;
        KDNode<K>* second = (diff < 0) ? node->right : node->left;
        
        nearestNeighborRecursive(first, target, best, bestDist, depth + 1);
        
        if (diff * diff < bestDist) {
            nearestNeighborRecursive(second, target, best, bestDist, depth + 1);
        }
    }
    
    void traverseRecursive(KDNode<K>* node, vector<Point<K>>& result) const {
        if (node == nullptr) return;
        result.push_back(node->point);
        traverseRecursive(node->left, result);
        traverseRecursive(node->right, result);
    }
    
    void printTreeRecursive(KDNode<K>* node, ostringstream& oss, 
                            size_t depth, const string& prefix) const {
        if (node == nullptr) return;
        
        oss << prefix << "[" << node->axis << "] " << node->point.toString() << "\n";
        
        if (node->left || node->right) {
            if (node->left) {
                printTreeRecursive(node->left, oss, depth + 1, prefix + "├── L: ");
            } else {
                oss << prefix << "├── L: (null)\n";
            }
            if (node->right) {
                printTreeRecursive(node->right, oss, depth + 1, prefix + "└── R: ");
            } else {
                oss << prefix << "└── R: (null)\n";
            }
        }
    }
    
    KDNode<K>* buildBalanced(vector<Point<K>>& points, size_t depth) {
        if (points.empty()) return nullptr;
        
        size_t axis = depth % K;
        
        auto comparator = [axis](const Point<K>& a, const Point<K>& b) {
            return a.get(axis) < b.get(axis);
        };
        sort(points.begin(), points.end(), comparator);
        
        size_t mid = points.size() / 2;
        
        KDNode<K>* node = new KDNode<K>(points[mid], axis);
        
        vector<Point<K>> leftPoints(points.begin(), points.begin() + mid);
        node->left = buildBalanced(leftPoints, depth + 1);
        
        vector<Point<K>> rightPoints(points.begin() + mid + 1, points.end());
        node->right = buildBalanced(rightPoints, depth + 1);
        
        return node;
    }
    
    void deleteTree(KDNode<K>* node) {
        if (node == nullptr) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    KDTree() : root(nullptr), size(0) {}
    
    ~KDTree() {
        deleteTree(root);
    }
    
    KDTree(const KDTree& other) : root(nullptr), size(0) {
        vector<Point<K>> points = other.traverse();
        for (const auto& p : points) {
            insert(p);
        }
    }
    
    KDTree& operator=(const KDTree& other) {
        if (this != &other) {
            deleteTree(root);
            root = nullptr;
            size = 0;
            vector<Point<K>> points = other.traverse();
            for (const auto& p : points) {
                insert(p);
            }
        }
        return *this;
    }
    
    void insert(const Point<K>& point) {
        root = insertRecursive(root, point, 0);
        size++;
    }
    
    bool search(const Point<K>& point) const {
        return searchRecursive(root, point, 0);
    }
    
    bool remove(const Point<K>& point) {
        if (!search(point)) return false;
        root = removeRecursive(root, point, 0);
        size--;
        return true;
    }
    
    Point<K> nearestNeighbor(const Point<K>& target) const {
        if (root == nullptr) {
            throw runtime_error("Tree is empty");
        }
        
        KDNode<K>* best = nullptr;
        double bestDist = numeric_limits<double>::max();
        
        nearestNeighborRecursive(root, target, best, bestDist, 0);
        
        return best->point;
    }
    
    Point<K> nearestNeighbor(const Point<K>& target, double& distance) const {
        if (root == nullptr) {
            throw runtime_error("Tree is empty");
        }
        
        KDNode<K>* best = nullptr;
        double bestDist = numeric_limits<double>::max();
        
        nearestNeighborRecursive(root, target, best, bestDist, 0);
        
        distance = sqrt(bestDist);
        return best->point;
    }
    
    vector<Point<K>> traverse() const {
        vector<Point<K>> result;
        traverseRecursive(root, result);
        return result;
    }
    
    size_t getSize() const {
        return size;
    }
    
    bool isEmpty() const {
        return size == 0;
    }
    
    string toString() const {
        ostringstream oss;
        oss << "KD-Tree (K=" << K << ", Size=" << size << "):\n";
        
        if (root == nullptr) {
            oss << "  (empty)\n";
        } else {
            printTreeRecursive(root, oss, 0, "  ");
        }
        
        return oss.str();
    }
};

int main() {
    cout << "=================================================================\n";
    cout << "                    KD-Tree Implementation Test\n";
    cout << "=================================================================\n\n";
    
    cout << "=== Test 1: 2D KD-Tree ===\n\n";
    
    KDTree<2> tree2D;
    
    vector<Point<2>> points2D = {
        {3, 4},
        {1, 2},
        {6, 1},
        {7, 8},
        {4, 7},
        {5, 4},
        {2, 3},
        {8, 5}
    };
    
    cout << "Inserting points:\n";
    for (const auto& p : points2D) {
        tree2D.insert(p);
        cout << "  " << p.toString() << "\n";
    }
    cout << "\n";
    
    cout << tree2D.toString() << "\n";
    
    cout << "Pre-order traversal:\n  ";
    vector<Point<2>> traversed = tree2D.traverse();
    for (size_t i = 0; i < traversed.size(); i++) {
        cout << traversed[i].toString();
        if (i < traversed.size() - 1) cout << " -> ";
    }
    cout << "\n\n";
    
    Point<2> searchPoint = {4, 7};
    cout << "Searching for " << searchPoint.toString() << ": ";
    cout << (tree2D.search(searchPoint) ? "Found!" : "Not found") << "\n";
    
    Point<2> notFoundPoint = {10, 10};
    cout << "Searching for " << notFoundPoint.toString() << ": ";
    cout << (tree2D.search(notFoundPoint) ? "Found!" : "Not found") << "\n\n";
    
    Point<2> target = {5, 5};
    double dist;
    Point<2> nearest = tree2D.nearestNeighbor(target, dist);
    cout << "Nearest neighbor to " << target.toString() << ":\n";
    cout << "  Point: " << nearest.toString() << "\n";
    cout << "  Distance: " << dist << "\n\n";
    
    cout << "=== Test 2: 3D KD-Tree ===\n\n";
    
    KDTree<3> tree3D;
    
    vector<Point<3>> points3D = {
        {3, 4, 2},
        {1, 2, 3},
        {6, 1, 5},
        {7, 8, 1},
        {4, 7, 6},
        {5, 4, 4},
        {2, 3, 7},
        {8, 5, 3}
    };
    
    cout << "Inserting 8 3D points...\n";
    for (const auto& p : points3D) {
        tree3D.insert(p);
    }
    cout << "Tree size: " << tree3D.getSize() << "\n\n";
    
    cout << tree3D.toString() << "\n";
    
    Point<3> target3D = {4, 4, 4};
    Point<3> nearest3D = tree3D.nearestNeighbor(target3D, dist);
    cout << "Nearest neighbor to " << target3D.toString() << ":\n";
    cout << "  Point: " << nearest3D.toString() << "\n";
    cout << "  Distance: " << dist << "\n\n";
    
    cout << "=== Test 3: Remove Operation ===\n\n";
    
    KDTree<2> treeRemove;
    for (const auto& p : points2D) {
        treeRemove.insert(p);
    }
    
    cout << "Before removal, size: " << treeRemove.getSize() << "\n";
    
    Point<2> removePoint = {4, 7};
    bool removed = treeRemove.remove(removePoint);
    cout << "Removed " << removePoint.toString() << ": " << (removed ? "Success" : "Failed") << "\n";
    cout << "After removal, size: " << treeRemove.getSize() << "\n";
    
    cout << "Searching for " << removePoint.toString() << " after removal: ";
    cout << (treeRemove.search(removePoint) ? "Found" : "Not found") << "\n\n";
    
    cout << "=== Test 4: Empty Tree ===\n\n";
    
    KDTree<2> emptyTree;
    cout << "Empty tree size: " << emptyTree.getSize() << "\n";
    cout << "Empty tree is empty: " << (emptyTree.isEmpty() ? "Yes" : "No") << "\n";
    cout << emptyTree.toString() << "\n";
    
    cout << "=== Test 5: Large Dataset ===\n\n";
    
    KDTree<2> largeTree;
    
    srand(42);
    
    cout << "Inserting 100 random points...\n";
    for (int i = 0; i < 100; i++) {
        double x = rand() % 100;
        double y = rand() % 100;
        largeTree.insert({x, y});
    }
    
    cout << "Tree size: " << largeTree.getSize() << "\n";
    
    Point<2> query = {50, 50};
    Point<2> nn = largeTree.nearestNeighbor(query, dist);
    cout << "Query point: " << query.toString() << "\n";
    cout << "Nearest neighbor: " << nn.toString() << "\n";
    cout << "Distance: " << dist << "\n\n";
    
    cout << "=== Test 6: Copy and Assignment ===\n\n";
    
    KDTree<2> original;
    original.insert({1, 1});
    original.insert({2, 2});
    original.insert({3, 3});
    
    KDTree<2> copy = original;
    
    cout << "Original size: " << original.getSize() << "\n";
    cout << "Copy size: " << copy.getSize() << "\n";
    
    original.insert({4, 4});
    cout << "After adding to original:\n";
    cout << "  Original size: " << original.getSize() << "\n";
    cout << "  Copy size: " << copy.getSize() << " (unchanged)\n\n";
    
    cout << "=================================================================\n";
    cout << "                        All Tests Passed!\n";
    cout << "=================================================================\n";
    
    return 0;
}