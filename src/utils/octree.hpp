#pragma once

#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <algorithm>
#include <concepts>



// --- Concept C++20 ---
template<typename T>
concept HasPosition = requires(T t) {
    { t.x } -> std::convertible_to<float>;
    { t.y } -> std::convertible_to<float>;
    { t.z } -> std::convertible_to<float>;
};

// --- Géométrie ---
struct Box {
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

    // Vérifie si un point est strictement dans la boîte
    bool contains(float x, float y, float z) const {
        return x >= minX && x <= maxX && 
               y >= minY && y <= maxY && 
               z >= minZ && z <= maxZ;
    }

    // Distance carrée minimale entre un point et la boîte (pour les requêtes de voisinage)
    float distanceSqToPoint(float x, float y, float z) const {
        float dx = 0.0f, dy = 0.0f, dz = 0.0f;
        // On accumule la distance seulement si le point est hors des bornes sur un axe
        if (x < minX) dx = minX - x; else if (x > maxX) dx = x - maxX;
        if (y < minY) dy = minY - y; else if (y > maxY) dy = y - maxY;
        if (z < minZ) dz = minZ - z; else if (z > maxZ) dz = z - maxZ;
        return dx*dx + dy*dy + dz*dz;
    }

    float sizeX() const { return maxX - minX; }
    float sizeY() const { return maxY - minY; }
    float sizeZ() const { return maxZ - minZ; }
};

// --- Octree ---
template<HasPosition T>
class Octree {
public:
    Octree(const Box& bounds, int maxCapacity = 16, int maxDepth = 8)
        : m_bounds(bounds), m_maxCapacity(maxCapacity), m_maxDepth(maxDepth) {
        m_root = std::make_unique<Node>();
        m_root->bounds = bounds;
    }

    // Insertion
    bool insert(const T& item) {
        // Sécurité : on rejette les objets hors du monde pour éviter des erreurs d'indexation
        if (!m_bounds.contains(item.x, item.y, item.z)) return false;
        insertRecursive(m_root.get(), item, 0);
        return true;
    }

    // Suppression ciblée (rapide, nécessite une position approximative)
    // Retourne true si trouvé et supprimé
    template<typename Pred>
    bool removeIf(Pred pred, float hintX, float hintY, float hintZ) {
        return removeIfRecursive(m_root.get(), pred, hintX, hintY, hintZ);
    }

    // Suppression globale (lente, parcourt tout l'arbre)
    // Utile pour nettoyer des objets dont on a perdu la trace ou qui ont "teleporté"
    template<typename Pred>
    void removeIfGlobal(Pred pred) {
        removeIfGlobalRecursive(m_root.get(), pred);
    }

    // Mise à jour intelligente
    template<typename Pred>
    void update(Pred pred, const T& newItem, float oldX, float oldY, float oldZ) {
        // 1. Tenter la suppression rapide via l'ancienne position connue
        if (!removeIf(pred, oldX, oldY, oldZ)) {
            // 2. Si échec (l'objet a bougé plus vite que prévu), suppression globale de secours
            removeIfGlobal(pred);
        }
        // 3. Insertion à la nouvelle position
        insert(newItem);
    }

    // Requête de voisinage (Sphère)
    // radius : rayon de recherche
    // outResults : vecteur où ajouter les résultats (évite l'allocation si réutilisé)
    void queryRadius(float x, float y, float z, float radius, std::vector<T>& outResults) const {
        float radiusSq = radius * radius;
        queryRadiusRecursive(m_root.get(), x, y, z, radiusSq, outResults);
    }

    // Requête de boîte (AABB) - Utile pour le Frustum Culling
    void queryBox(const Box& searchBox, std::vector<T>& outResults) const {
        queryBoxRecursive(m_root.get(), searchBox, outResults);
    }

    void clear() {
        m_root = std::make_unique<Node>();
        m_root->bounds = m_bounds;
    }

private:
    struct Node {
        Box bounds;
        std::vector<T> items;
        std::array<std::unique_ptr<Node>, 8> children; // nullptr si feuille

        bool isLeaf() const { return children[0] == nullptr; }
    };

    std::unique_ptr<Node> m_root;
    Box m_bounds;
    int m_maxCapacity;
    int m_maxDepth;

    // Calcule l'index de l'enfant (Morton code simplifié localement)
    int getChildIndex(const Box& b, float x, float y, float z) const {
        int index = 0;
        float midX = b.minX + b.sizeX() * 0.5f;
        float midY = b.minY + b.sizeY() * 0.5f;
        float midZ = b.minZ + b.sizeZ() * 0.5f; // Calculer les milieux à la volée est bon marché
        if (x >= midX) index |= 1;
        if (y >= midY) index |= 2;
        if (z >= midZ) index |= 4;
        return index;
    }

    void subdivide(Node* node) {
        float hx = node->bounds.sizeX() * 0.5f;
        float hy = node->bounds.sizeY() * 0.5f;
        float hz = node->bounds.sizeZ() * 0.5f;
        
        for (int i = 0; i < 8; ++i) {
            float minX = node->bounds.minX + (i & 1 ? hx : 0);
            float minY = node->bounds.minY + (i & 2 ? hy : 0);
            float minZ = node->bounds.minZ + (i & 4 ? hz : 0);
            
            node->children[i] = std::make_unique<Node>();
            node->children[i]->bounds = Box{minX, minY, minZ, minX + hx, minY + hy, minZ + hz};
            node->children[i]->items.reserve(m_maxCapacity / 2); // Petite réservation préventive
        }
    }

    void insertRecursive(Node* node, const T& item, int depth) {
        if (node->isLeaf()) {
            if (node->items.size() < m_maxCapacity || depth >= m_maxDepth) {
                node->items.push_back(item);
            } else {
                subdivide(node);
                
                // Redistribuer les anciens items vers les enfants
                // On utilise std::move pour éviter les copies inutiles
                for (auto& oldItem : node->items) {
                    int idx = getChildIndex(node->bounds, oldItem.x, oldItem.y, oldItem.z);
                    // Note : On push directement dans le vecteur de l'enfant.
                    // On accepte temporairement que l'enfant dépasse la capacité. 
                    // Il se subdivisera à son tour à la prochaine insertion le ciblant.
                    node->children[idx]->items.push_back(std::move(oldItem)); 
                }
                node->items.clear();
                node->items.shrink_to_fit(); // Important pour libérer la mémoire du nœud devenu branche

                // Insérer le nouvel item dans l'enfant approprié
                int idx = getChildIndex(node->bounds, item.x, item.y, item.z);
                insertRecursive(node->children[idx].get(), item, depth + 1);
            }
        } else {
            int idx = getChildIndex(node->bounds, item.x, item.y, item.z);
            insertRecursive(node->children[idx].get(), item, depth + 1);
        }
    }

    template<typename Pred>
    bool removeIfRecursive(Node* node, Pred pred, float x, float y, float z) {
        if (node->isLeaf()) {
            // Utilisation d'un itérateur inverse pour suppression efficace ou find_if
            auto it = std::find_if(node->items.begin(), node->items.end(), pred);
            if (it != node->items.end()) {
                // Swap & Pop (O(1)) : on remplace par le dernier et on réduit la taille
                *it = std::move(node->items.back());
                node->items.pop_back();
                return true;
            }
            return false;
        }

        // Descente
        int idx = getChildIndex(node->bounds, x, y, z);
        bool removed = removeIfRecursive(node->children[idx].get(), pred, x, y, z);

        if (removed) {
            // Logique de fusion (Collapse)
            // On vérifie si on peut transformer cette branche en feuille
            int totalCount = 0;
            for (const auto& child : node->children) {
                if (!child->isLeaf()) return true; // Si un enfant est une branche, on ne peut pas fusionner
                totalCount += child->items.size();
            }

            if (totalCount <= m_maxCapacity) {
                // Fusion !
                node->items.reserve(totalCount);
                for (auto& child : node->children) {
                    // Move iterator pour transfert efficace
                    node->items.insert(node->items.end(), 
                                     std::make_move_iterator(child->items.begin()), 
                                     std::make_move_iterator(child->items.end()));
                    child.reset(); // Destruction du nœud enfant
                }
            }
        }
        return removed;
    }

    template<typename Pred>
    void removeIfGlobalRecursive(Node* node, Pred pred) {
        if (node->isLeaf()) {
            // C++20 erase_if : propre et efficace
            std::erase_if(node->items, pred);
        } else {
            bool allEmptyLeaves = true;
            int totalCount = 0;
            
            for (auto& child : node->children) {
                removeIfGlobalRecursive(child.get(), pred);
                if (!child->isLeaf()) allEmptyLeaves = false;
                totalCount += child->items.size();
            }

            // Tentative de fusion après nettoyage global
            if (allEmptyLeaves && totalCount <= m_maxCapacity) {
                node->items.reserve(totalCount);
                for (auto& child : node->children) {
                    node->items.insert(node->items.end(), 
                                     std::make_move_iterator(child->items.begin()), 
                                     std::make_move_iterator(child->items.end()));
                    child.reset();
                }
            }
        }
    }

    void queryRadiusRecursive(const Node* node, float x, float y, float z, float radiusSq, std::vector<T>& out) const {
        // Broad Phase : Test Boîte vs Sphère
        // distanceSqToPoint est très rapide et élimine les branches éloignées
        if (node->bounds.distanceSqToPoint(x, y, z) > radiusSq) return;

        if (node->isLeaf()) {
            // Narrow Phase : Test précis Point vs Sphère
            for (const auto& item : node->items) {
                float dx = item.x - x;
                float dy = item.y - y;
                float dz = item.z - z;
                if (dx*dx + dy*dy + dz*dz <= radiusSq) {
                    out.push_back(item);
                }
            }
        } else {
            // Pas d'ordre spécifique pour les enfants, une sphère peut en chevaucher plusieurs
            for (const auto& child : node->children) {
                queryRadiusRecursive(child.get(), x, y, z, radiusSq, out);
            }
        }
    }

    void queryBoxRecursive(const Node* node, const Box& searchBox, std::vector<T>& out) const {
        // Intersection simple AABB vs AABB
        if (node->bounds.minX > searchBox.maxX || node->bounds.maxX < searchBox.minX ||
            node->bounds.minY > searchBox.maxY || node->bounds.maxY < searchBox.minY ||
            node->bounds.minZ > searchBox.maxZ || node->bounds.maxZ < searchBox.minZ) return;

        if (node->isLeaf()) {
            for (const auto& item : node->items) {
                if (searchBox.contains(item.x, item.y, item.z)) {
                    out.push_back(item);
                }
            }
        } else {
            for (const auto& child : node->children) {
                queryBoxRecursive(child.get(), searchBox, out);
            }
        }
    }
};
