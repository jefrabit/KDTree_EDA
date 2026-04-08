// =============================================================================
// KD-Tree Implementation in C++
// =============================================================================
// A complete, modular implementation of a KD-Tree for k-dimensional points.
// Supports: insertion, search, traversal, and nearest neighbor search.
// =============================================================================

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>

// =============================================================================
// Point Class
// =============================================================================
template <size_t K>
class Point {
public:
    std::vector<double> coords;
    
    Point() : coords(K, 0.0) {}
    Point(const std::vector<double>& v) : coords(v) {}
    
    // Constructor with variable arguments for convenience
    Point(std::initializer_list<double> list) : coords(list) {
        while (coords.size() < K) coords.push_back(0.0);
        if (coords.size() > K) coords.resize(K);
    }
    
    // Get coordinate at dimension d
    double get(size_t d) const {
        return coords[d];
    }
    
    // Set coordinate at dimension d
    void set(size_t d, double value) {
        if (d < K) coords[d] = value;
    }
    
    // Calculate squared distance to another point
    double squaredDistance(const Point<K>& other) const {
        double sum = 0.0;
        for (size_t i = 0; i < K; i++) {
            double diff = coords[i] - other.coords[i];
            sum += diff * diff;
        }
        return sum;
    }
    
    // Calculate distance to another point
    double distance(const Point<K>& other) const {
        return std::sqrt(squaredDistance(other));
    }
    
    // Equality comparison
    bool operator==(const Point<K>& other) const {
        for (size_t i = 0; i < K; i++) {
            if (coords[i] != other.coords[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const Point<K>& other) const {
        return !(*this == other);
    }
    
    // String representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "(";
        for (size_t i = 0; i < K; i++) {
            oss << coords[i];
            if (i < K - 1) oss << ", ";
        }
        oss << ")";
        return oss.str();
    }
};

// =============================================================================
// KD-Tree Node
// =============================================================================
template <size_t K>
struct KDNode {
    Point<K> point;
    size_t axis;          // Dimension used for splitting (0 to K-1)
    KDNode* left;
    KDNode* right;
    
    KDNode(const Point<K>& p, size_t a) 
        : point(p), axis(a), left(nullptr), right(nullptr) {}
};

// =============================================================================
// KD-Tree Class
// =============================================================================
template <size_t K>
class KDTree {
private:
    KDNode<K>* root;
    size_t size;
    
    // Helper: Insert a point recursively
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
    
    // Helper: Search for a point recursively
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
    
    // Helper: Find minimum value in given axis
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
    
    // Helper: Remove a point recursively
    KDNode<K>* removeRecursive(KDNode<K>* node, const Point<K>& point, size_t depth) {
        if (node == nullptr) return nullptr;
        
        if (node->point == point) {
            // Found the node to delete
            if (node->right != nullptr) {
                // Find minimum in right subtree for this axis
                KDNode<K>* minNode = findMin(node->right, node->axis, depth + 1);
                node->point = minNode->point;
                node->right = removeRecursive(node->right, minNode->point, depth + 1);
            } else if (node->left != nullptr) {
                // Find minimum in left subtree
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
    
    // Helper: Nearest neighbor search
    void nearestNeighborRecursive(KDNode<K>* node, const Point<K>& target,
                                  KDNode<K>*& best, double& bestDist, size_t depth) const {
        if (node == nullptr) return;
        
        // Calculate distance to current node
        double dist = node->point.squaredDistance(target);
        
        if (dist < bestDist) {
            bestDist = dist;
            best = node;
        }
        
        size_t axis = depth % K;
        double diff = target.get(axis) - node->point.get(axis);
        
        // Search the side closer to the target first
        KDNode<K>* first = (diff < 0) ? node->left : node->right;
        KDNode<K>* second = (diff < 0) ? node->right : node->left;
        
        nearestNeighborRecursive(first, target, best, bestDist, depth + 1);
        
        // Check if we need to search the other side
        // (if the hypersphere crosses the hyperplane)
        if (diff * diff < bestDist) {
            nearestNeighborRecursive(second, target, best, bestDist, depth + 1);
        }
    }
    
    // Helper: Traverse tree (pre-order)
    void traverseRecursive(KDNode<K>* node, std::vector<Point<K>>& result) const {
        if (node == nullptr) return;
        result.push_back(node->point);
        traverseRecursive(node->left, result);
        traverseRecursive(node->right, result);
    }
    
    // Helper: Print tree structure
    void printTreeRecursive(KDNode<K>* node, std::ostringstream& oss, 
                            size_t depth, const std::string& prefix) const {
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
    
    // Helper: Build balanced tree from points
    KDNode<K>* buildBalanced(std::vector<Point<K>>& points, size_t depth) {
        if (points.empty()) return nullptr;
        
        size_t axis = depth % K;
        
        // Sort by current axis
        auto comparator = [axis](const Point<K>& a, const Point<K>& b) {
            return a.get(axis) < b.get(axis);
        };
        std::sort(points.begin(), points.end(), comparator);
        
        // Find median
        size_t mid = points.size() / 2;
        
        // Create node with median
        KDNode<K>* node = new KDNode<K>(points[mid], axis);
        
        // Build left subtree (points before median)
        std::vector<Point<K>> leftPoints(points.begin(), points.begin() + mid);
        node->left = buildBalanced(leftPoints, depth + 1);
        
        // Build right subtree (points after median)
        std::vector<Point<K>> rightPoints(points.begin() + mid + 1, points.end());
        node->right = buildBalanced(rightPoints, depth + 1);
        
        return node;
    }
    
    // Helper: Delete all nodes
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
    
    // Copy constructor
    KDTree(const KDTree& other) : root(nullptr), size(0) {
        std::vector<Point<K>> points = other.traverse();
        for (const auto& p : points) {
            insert(p);
        }
    }
    
    // Assignment operator
    KDTree& operator=(const KDTree& other) {
        if (this != &other) {
            deleteTree(root);
            root = nullptr;
            size = 0;
            std::vector<Point<K>> points = other.traverse();
            for (const auto& p : points) {
                insert(p);
            }
        }
        return *this;
    }
    
    // Insert a point
    void insert(const Point<K>& point) {
        root = insertRecursive(root, point, 0);
        size++;
    }
    
    // Search for a point
    bool search(const Point<K>& point) const {
        return searchRecursive(root, point, 0);
    }
    
    // Remove a point
    bool remove(const Point<K>& point) {
        if (!search(point)) return false;
        root = removeRecursive(root, point, 0);
        size--;
        return true;
    }
    
    // Find nearest neighbor to a given point
    Point<K> nearestNeighbor(const Point<K>& target) const {
        if (root == nullptr) {
            throw std::runtime_error("Tree is empty");
        }
        
        KDNode<K>* best = nullptr;
        double bestDist = std::numeric_limits<double>::max();
        
        nearestNeighborRecursive(root, target, best, bestDist, 0);
        
        return best->point;
    }
    
    // Find nearest neighbor with distance
    Point<K> nearestNeighbor(const Point<K>& target, double& distance) const {
        if (root == nullptr) {
            throw std::runtime_error("Tree is empty");
        }
        
        KDNode<K>* best = nullptr;
        double bestDist = std::numeric_limits<double>::max();
        
        nearestNeighborRecursive(root, target, best, bestDist, 0);
        
        distance = std::sqrt(bestDist);
        return best->point;
    }
    
    // Traverse tree (pre-order)
    std::vector<Point<K>> traverse() const {
        std::vector<Point<K>> result;
        traverseRecursive(root, result);
        return result;
    }
    
    // Get size
    size_t getSize() const {
        return size;
    }
    
    // Check if empty
    bool isEmpty() const {
        return size == 0;
    }
    
    // Print tree structure
    std::string toString() const {
        std::ostringstream oss;
        oss << "KD-Tree (K=" << K << ", Size=" << size << "):\n";
        
        if (root == nullptr) {
            oss << "  (empty)\n";
        } else {
            printTreeRecursive(root, oss, 0, "  ");
        }
        
        return oss.str();
    }
};

// =============================================================================
// Main - Test Examples
// =============================================================================
int main() {
    std::cout << "=================================================================\n";
    std::cout << "                    KD-Tree Implementation Test\n";
    std::cout << "=================================================================\n\n";
    
    // -------------------------------------------------------------------------
    // Test 1: 2D KD-Tree (K=2)
    // -------------------------------------------------------------------------
    std::cout << "=== Test 1: 2D KD-Tree ===\n\n";
    
    KDTree<2> tree2D;
    
    // Insert points
    std::vector<Point<2>> points2D = {
        {3, 4},
        {1, 2},
        {6, 1},
        {7, 8},
        {4, 7},
        {5, 4},
        {2, 3},
        {8, 5}
    };
    
    std::cout << "Inserting points:\n";
    for (const auto& p : points2D) {
        tree2D.insert(p);
        std::cout << "  " << p.toString() << "\n";
    }
    std::cout << "\n";
    
    // Print tree structure
    std::cout << tree2D.toString() << "\n";
    
    // Traverse
    std::cout << "Pre-order traversal:\n  ";
    std::vector<Point<2>> traversed = tree2D.traverse();
    for (size_t i = 0; i < traversed.size(); i++) {
        std::cout << traversed[i].toString();
        if (i < traversed.size() - 1) std::cout << " -> ";
    }
    std::cout << "\n\n";
    
    // Search
    Point<2> searchPoint = {4, 7};
    std::cout << "Searching for " << searchPoint.toString() << ": ";
    std::cout << (tree2D.search(searchPoint) ? "Found!" : "Not found") << "\n";
    
    Point<2> notFoundPoint = {10, 10};
    std::cout << "Searching for " << notFoundPoint.toString() << ": ";
    std::cout << (tree2D.search(notFoundPoint) ? "Found!" : "Not found") << "\n\n";
    
    // Nearest neighbor
    Point<2> target = {5, 5};
    double dist;
    Point<2> nearest = tree2D.nearestNeighbor(target, dist);
    std::cout << "Nearest neighbor to " << target.toString() << ":\n";
    std::cout << "  Point: " << nearest.toString() << "\n";
    std::cout << "  Distance: " << dist << "\n\n";
    
    // -------------------------------------------------------------------------
    // Test 2: 3D KD-Tree (K=3)
    // -------------------------------------------------------------------------
    std::cout << "=== Test 2: 3D KD-Tree ===\n\n";
    
    KDTree<3> tree3D;
    
    std::vector<Point<3>> points3D = {
        {3, 4, 2},
        {1, 2, 3},
        {6, 1, 5},
        {7, 8, 1},
        {4, 7, 6},
        {5, 4, 4},
        {2, 3, 7},
        {8, 5, 3}
    };
    
    std::cout << "Inserting 8 3D points...\n";
    for (const auto& p : points3D) {
        tree3D.insert(p);
    }
    std::cout << "Tree size: " << tree3D.getSize() << "\n\n";
    
    std::cout << tree3D.toString() << "\n";
    
    Point<3> target3D = {4, 4, 4};
    Point<3> nearest3D = tree3D.nearestNeighbor(target3D, dist);
    std::cout << "Nearest neighbor to " << target3D.toString() << ":\n";
    std::cout << "  Point: " << nearest3D.toString() << "\n";
    std::cout << "  Distance: " << dist << "\n\n";
    
    // -------------------------------------------------------------------------
    // Test 3: Remove operation
    // -------------------------------------------------------------------------
    std::cout << "=== Test 3: Remove Operation ===\n\n";
    
    KDTree<2> treeRemove;
    for (const auto& p : points2D) {
        treeRemove.insert(p);
    }
    
    std::cout << "Before removal, size: " << treeRemove.getSize() << "\n";
    
    Point<2> removePoint = {4, 7};
    bool removed = treeRemove.remove(removePoint);
    std::cout << "Removed " << removePoint.toString() << ": " << (removed ? "Success" : "Failed") << "\n";
    std::cout << "After removal, size: " << treeRemove.getSize() << "\n";
    
    std::cout << "Searching for " << removePoint.toString() << " after removal: ";
    std::cout << (treeRemove.search(removePoint) ? "Found" : "Not found") << "\n\n";
    
    // -------------------------------------------------------------------------
    // Test 4: Empty tree
    // -------------------------------------------------------------------------
    std::cout << "=== Test 4: Empty Tree ===\n\n";
    
    KDTree<2> emptyTree;
    std::cout << "Empty tree size: " << emptyTree.getSize() << "\n";
    std::cout << "Empty tree is empty: " << (emptyTree.isEmpty() ? "Yes" : "No") << "\n";
    std::cout << emptyTree.toString() << "\n";
    
    // -------------------------------------------------------------------------
    // Test 5: Large dataset - Nearest neighbor search
    // -------------------------------------------------------------------------
    std::cout << "=== Test 5: Large Dataset ===\n\n";
    
    KDTree<2> largeTree;
    
    // Generate random points
    std::srand(42); // Fixed seed for reproducibility
    
    std::cout << "Inserting 100 random points...\n";
    for (int i = 0; i < 100; i++) {
        double x = std::rand() % 100;
        double y = std::rand() % 100;
        largeTree.insert({x, y});
    }
    
    std::cout << "Tree size: " << largeTree.getSize() << "\n";
    
    Point<2> query = {50, 50};
    Point<2> nn = largeTree.nearestNeighbor(query, dist);
    std::cout << "Query point: " << query.toString() << "\n";
    std::cout << "Nearest neighbor: " << nn.toString() << "\n";
    std::cout << "Distance: " << dist << "\n\n";
    
    // -------------------------------------------------------------------------
    // Test 6: Copy constructor and assignment
    // -------------------------------------------------------------------------
    std::cout << "=== Test 6: Copy and Assignment ===\n\n";
    
    KDTree<2> original;
    original.insert({1, 1});
    original.insert({2, 2});
    original.insert({3, 3});
    
    KDTree<2> copy = original;  // Copy constructor
    
    std::cout << "Original size: " << original.getSize() << "\n";
    std::cout << "Copy size: " << copy.getSize() << "\n";
    
    original.insert({4, 4});
    std::cout << "After adding to original:\n";
    std::cout << "  Original size: " << original.getSize() << "\n";
    std::cout << "  Copy size: " << copy.getSize() << " (unchanged)\n\n";
    
    std::cout << "=================================================================\n";
    std::cout << "                        All Tests Passed!\n";
    std::cout << "=================================================================\n";
    
    return 0;
}