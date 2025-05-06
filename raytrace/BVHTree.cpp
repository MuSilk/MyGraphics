#include "BVHTree.h"

#include <algorithm>

void BVHTree::build(const Mesh<MeshPoint>& mesh ) {
	const auto& indices = mesh.indices;
	const auto& vertices = mesh.vertices;

	for (int i = 0; i < indices.size()/3; i+=1) {
		bounds.push_back({ i,Bound3d::Union({
			mesh.getPoint(indices[i * 3 + 0]).V,
			mesh.getPoint(indices[i * 3 + 1]).V,
			mesh.getPoint(indices[i * 3 + 2]).V,
		}) });
	}
	recursiveBuild(bounds.begin(),bounds.end());

}

uint32_t BVHTree::recursiveBuild(std::vector<BVHLeafInfo>::iterator bg, std::vector<BVHLeafInfo>::iterator ed) {
	BVHNode node;

	Bound3d bound;
	for (auto i = bg; i != ed; i++) {
		bound = Bound3d::Union(bound, i->second);
	}
	node.bound = bound;
	node.children[0] = node.children[1] = bg->first;
	node.axis = 4;

	uint32_t idx = nodearray.size();
	nodearray.push_back(node);

	if (ed-bg == 1) {
		return idx;
	}

	Bound3d centroidBounds;
	for (auto i = bg; i != ed; i++) {
		bound = Bound3d::Union(bound, i->second.centroid());
	}
	int dim = centroidBounds.MaxExtent();
	
	nodearray[idx].axis = dim;

	auto mid = bg + (ed - bg) / 2;
	std::nth_element(bg, mid, ed, [dim](const BVHLeafInfo& a, const BVHLeafInfo& b) {
		return a.second.centroid()[dim] < b.second.centroid()[dim];
	});
	
	nodearray[idx].children[0] = recursiveBuild(bg, mid);
	nodearray[idx].children[1] = recursiveBuild(mid, ed);

	return idx;
}