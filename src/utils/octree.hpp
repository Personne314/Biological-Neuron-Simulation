#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <algorithm>
#include <concepts>



/**
 * @brief Concept to force octree element to have a position.
 */
template<typename T>
concept HasPosition = requires(T t)
{
	{ t.x } -> std::convertible_to<float>;
	{ t.y } -> std::convertible_to<float>;
	{ t.z } -> std::convertible_to<float>;
};



/**
 * @struct Box
 * @brief 3D space subdivision to represent octree subspaces.
 */
struct Box
{
	
	float min_x; // Box min x coordinate. Included.
	float min_y; // Box min y coordinate. Included.
	float min_z; // Box min z coordinate. Included.
	float max_x; // Box max x coordinate. Included.
	float max_y; // Box max y coordinate. Included.
	float max_z; // Box max z coordinate. Included.

	/**
	 * @brief Check if a point is in the box.
	 * @param x The x coordinate of the point.
	 * @param y The y coordinate of the point.
	 * @param z The z coordinate of the point.
	 * @return true if the point is in the box.
	 */
	bool contains(float x, float y, float z) const
	{
		return 
			x >= min_x && x <= max_x && 
			y >= min_y && y <= max_y && 
			z >= min_z && z <= max_z;
	}

	/**
	 * @brief Return the min squared distance bitween the point and this box.
	 * @param x The x coordinate of the point.
	 * @param y The y coordinate of the point.
	 * @param z The z coordinate of the point.
	 * @return The computed squared distance.
	 */
	float dist2(float x, float y, float z) const
	{
		float dx = 0.0f, dy = 0.0f, dz = 0.0f;
		if (x < min_x) dx = min_x - x; else if (x > max_x) dx = x - max_x;
		if (y < min_y) dy = min_y - y; else if (y > max_y) dy = y - max_y;
		if (z < min_z) dz = min_z - z; else if (z > max_z) dz = z - max_z;
		return dx*dx + dy*dy + dz*dz;
	}

	/**
	 * @brief Return the size of the box on the x axis.
	 * @return The calculated size.
	 */
	float size_x() const
	{
		return max_x - min_x;
	}

	/**
	 * @brief Return the size of the box on the y axis.
	 * @return The calculated size.
	 */
	float size_y() const
	{
		return max_y - min_y;
	}

	/**
	 * @brief Return the size of the box on the z axis.
	 * @return The calculated size.
	 */
	float size_z() const
	{
		return max_z - min_z;
	}

};



/**
 * @class Octree
 * @brief Used to store elements in a 3D space to reduce complexity of spatial research.
 */
template<HasPosition T>
class Octree
{
public:

	/**
	 * @brief Construct an octree.
	 * @param bounds Bounding box of the tree.
	 * @param cap The maximum capacity of a node before subdivision.
	 * @param depth Max depth of the tree.
	 */
	Octree(const Box &bounds, int32_t cap = 16, int32_t depth = 8) : 
		m_bounds(bounds),
		m_cap(cap),
		m_depth(depth) 
	{
		m_root = std::make_unique<Node>();
		m_root->bounds = bounds;
	}

	/**
	 * @brief Insert an element into the tree.
	 * @param item The element to insert.
	 * @return true if there was an error.
	 */
	bool insert(const T &item)
	{
		if (!m_bounds.contains(item.x, item.y, item.z)) return true;
		rec_insert(m_root.get(), item, 0);
		return false;
	}

	/**
	 * @brief Remove an element from the tree if the predicate is true.
	 * @param pred The predicate to find the element to remove.
	 * @param hint_x An estimation of the x coordinate.
	 * @param hint_y An estimation of the y coordinate.
	 * @param hint_z An estimation of the z coordinate.
	 * @return true if there was an error.
	 */
	template<typename Pred>
	bool remove_if(Pred pred, float hint_x, float hint_y, float hint_z)
	{
		return remove_if_recursive(m_root.get(), pred, hint_x, hint_y, hint_z);
	}

	/**
	 * @brief Remove an element from the tree if the predicate is true.
	 * @param pred The predicate to find the element to remove.
	 * @return true if there was an error.
	 */
	template<typename Pred>
	void remove_if_global(Pred pred)
	{
		remove_if_global_recursive(m_root.get(), pred);
	}

	/**
	 * @brief Update an element and replace it by a new one.
	 * @param pred The predicate to find the element to remove.
	 * @param new_item The new item to use to replace the one found with pred.
	 * @param hint_x An estimation of the x coordinate.
	 * @param hint_y An estimation of the y coordinate.
	 * @param hint_z An estimation of the z coordinate.
	 */
	template<typename Pred>
	void update(Pred pred, const T &new_item, float hint_x, float hint_y, float hint_z)
	{
		if (!remove_if(pred, hint_x, hint_y, hint_z)) {
			remove_if_global(pred);
		}
		insert(new_item);
	}

	/**
	 * @brief Request (Sphere).
	 * @param x The sphere x coordinate.
	 * @param y The sphere y coordinate.
	 * @param z The sphere z coordinate.
	 * @param radius The sphere radius.
	 * @param out_results The results of the research.
	 */
	void query_radius(float x, float y, float z, float radius, std::vector<T> &out_results) const
	{
		float radius2 = radius * radius;
		query_radius_recursive(m_root.get(), x, y, z, radius2, out_results);
	}

	/**
	 * @brief Request (AABB).
	 * @param search_box The box to get the content of.
	 * @param out_results The results of the search.
	 */
	void query_box(const Box &search_box, std::vector<T> &out_results) const
	{
		query_box_recursive(m_root.get(), search_box, out_results);
	}

	/**
	 * @brief Clear the tree and destroy all its nodes.
	 */
	void clear()
	{
		m_root = std::make_unique<Node>();
		m_root->bounds = m_bounds;
	}
	
private:

	/**
	 * @struct Node
	 * @brief One node of the octree. This can have 8 children to subdivide space.
	 */
	struct Node 
	{

		Box bounds;           // Bounding box of this node.
		std::vector<T> items; // All item stored in this node.
		std::array<std::unique_ptr<Node>, 8> children; // This node children or nullptr.

		/**
		 * @brief Check if the node is a leaf.
		 * @return true if the node is a leaf.
		 */
		bool leaf() const 
		{ 
			return children[0] == nullptr; 
		}

	};

	std::unique_ptr<Node> m_root; // Root node of the octree.
	Box m_bounds;    // Bounding box of the whole octree.
	int32_t m_cap;   // Maximum capacity of one node of the octree before subdivision.
	int32_t m_depth; // Maximum depth of the octree.

	

	/**
	 * @brief Compute the child index of a point in a node.
	 * @param b The bounding box of the node.
	 * @param x The point x coordinate.
	 * @param y The point y coordinate.
	 * @param z The point z coordinate.
	 * @note This suppose that the point is in b.
	 * @return The child index of the point in the node. 
	 */
	int32_t child_index(const Box &b, float x, float y, float z) const
	{
		int32_t id = 0;
		float half_x = b.min_x + b.size_x() * 0.5f;
		float half_y = b.min_y + b.size_y() * 0.5f;
		float half_z = b.min_z + b.size_z() * 0.5f;
		if (x >= half_x) id |= 1;
		if (y >= half_y) id |= 2;
		if (z >= half_z) id |= 4;
		return id;
	}

	/**
	 * @brief Subdivide the node into 8 leaves.
	 * @param node The node to subdivide.
	 */
	void subdivide(Node *node)
	{
		float half_x = node->bounds.size_x() * 0.5f;
		float half_y = node->bounds.size_y() * 0.5f;
		float half_z = node->bounds.size_z() * 0.5f;
		
		for (int32_t i = 0; i < 8; ++i) {
			float min_x = node->bounds.min_x + (i & 1 ? half_x : 0);
			float min_y = node->bounds.min_y + (i & 2 ? half_y : 0);
			float min_z = node->bounds.min_z + (i & 4 ? half_z : 0);
			node->children[i] = std::make_unique<Node>();
			node->children[i]->bounds = Box{min_x, min_y, min_z, min_x + half_x, min_y + half_y, min_z + half_z};
			node->children[i]->items.reserve(m_cap / 2);
		}
	}

	/**
	 * @brief Insert recursively an element into the tree.
	 * @param node The node where to insert the element.
	 * @param item The element to insert.
	 * @param depth The depth of the current node.
	 */
	void rec_insert(Node *node, const T &item, int32_t depth)
	{

		// If the node is not a leaf find the right child and sent it the item.
		if (!node->leaf()) {
			int32_t idx = child_index(node->bounds, item.x, item.y, item.z);
			rec_insert(node->children[idx].get(), item, depth + 1);
			return;
		}

		// Else if we are in a leaf at the max depth or if the cap is not reached,
		// the element can directly be added into the current node. 
		if ((int32_t)node->items.size() < m_cap || depth >= m_depth) {
			node->items.push_back(item);
			return;
		}
		
		// Else we need to subdivide and send the elements into the right child.
		// If the child contain more element than cap, it will be subdivided
		// during the next insert.
		subdivide(node);
		for (auto &old : node->items) {
			int32_t idx = child_index(node->bounds, old.x, old.y, old.z);
			node->children[idx]->items.push_back(std::move(old)); 
		}
		node->items.clear();
		node->items.shrink_to_fit(); 
		
		// Insert the new element in the right child.
		int32_t idx = child_index(node->bounds, item.x, item.y, item.z);
		rec_insert(node->children[idx].get(), item, depth + 1);
		
	}














	template<typename Pred>
	bool remove_if_recursive(Node* node, Pred pred, float x, float y, float z) {
		if (node->leaf()) {
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
		int32_t idx = child_index(node->bounds, x, y, z);
		bool removed = remove_if_recursive(node->children[idx].get(), pred, x, y, z);

		if (removed) {
			// Logique de fusion (Collapse)
			// On vérifie si on peut transformer cette branche en feuille
			int32_t totalCount = 0;
			for (const auto& child : node->children) {
				if (!child->isLeaf()) return true; // Si un enfant est une branche, on ne peut pas fusionner
				totalCount += child->items.size();
			}

			if (totalCount <= m_cap) {
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
	void remove_if_global_recursive(Node* node, Pred pred) {
		if (node->leaf()) {
			// C++20 erase_if : propre et efficace
			std::erase_if(node->items, pred);
		} else {
			bool allEmptyLeaves = true;
			int32_t totalCount = 0;
			
			for (auto& child : node->children) {
				remove_if_global_recursive(child.get(), pred);
				if (!child->isLeaf()) allEmptyLeaves = false;
				totalCount += child->items.size();
			}

			// Tentative de fusion après nettoyage global
			if (allEmptyLeaves && totalCount <= m_cap) {
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










	void query_radius_recursive(const Node* node, float x, float y, float z, float radiusSq, std::vector<T>& out) const {
		// Broad Phase : Test Boîte vs Sphère
		// distanceSqToPoint est très rapide et élimine les branches éloignées
		if (node->bounds.dist2(x, y, z) > radiusSq) return;

		if (node->leaf()) {
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
				query_radius_recursive(child.get(), x, y, z, radiusSq, out);
			}
		}
	}




	
	void query_box_recursive(const Node* node, const Box& searchBox, std::vector<T>& out) const {
		// Intersection simple AABB vs AABB
		if (node->bounds.min_x > searchBox.max_x || node->bounds.max_x < searchBox.min_x ||
			node->bounds.min_y > searchBox.max_y || node->bounds.max_y < searchBox.min_y ||
			node->bounds.min_z > searchBox.max_z || node->bounds.max_z < searchBox.min_z) return;

		if (node->leaf()) {
			for (const auto& item : node->items) {
				if (searchBox.contains(item.x, item.y, item.z)) {
					out.push_back(item);
				}
			}
		} else {
			for (const auto& child : node->children) {
				query_box_recursive(child.get(), searchBox, out);
			}
		}
	}

};
