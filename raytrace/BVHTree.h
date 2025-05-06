#pragma once

#include <surface/Mesh.h>

struct Bound3d{
    glm::vec3 pMin,pMax;
    
    Bound3d(){
        const float minNum=std::numeric_limits<float>::min();
        const float maxNum=std::numeric_limits<float>::max();
        pMin=glm::vec3(maxNum,maxNum,maxNum);
        pMax=glm::vec3(minNum,minNum,minNum);
    }

    glm::vec3 centroid() const {return (pMax+pMin)*0.5f;}

    int MaxExtent(){
        glm::vec3 size=pMax-pMin;
        if(size.x>size.y&&size.x>size.z)return 0;
        else if(size.y>size.z)return 1;
        else return 2;
    }

    static glm::vec3 Min(const glm::vec3& p1, const glm::vec3& p2) {
		return glm::vec3(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z));
	}
	static glm::vec3 Max(const glm::vec3& p1, const glm::vec3& p2) {
		return glm::vec3(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z));
	}
	static Bound3d Union(const Bound3d& b1, const Bound3d& b2) {
		Bound3d ret;
		ret.pMin = Min(b1.pMin, b2.pMin);
		ret.pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}
	static Bound3d Union(const Bound3d& b, const glm::vec3& p) {
		Bound3d ret;
		ret.pMin = Min(b.pMin, p);
		ret.pMax = Max(b.pMax, p);
		return ret;
	}

	static Bound3d Union(std::initializer_list<glm::vec3> list) {
		Bound3d ret;
		for (auto i : list) {
			ret = Union(ret, i);
		}
		return ret;
	}
};

struct BVHNode{
    Bound3d bound;
    uint32_t children[2];
    uint32_t axis;
};

class BVHTree{
    typedef std::pair<int32_t,Bound3d> BVHLeafInfo;
public:
    void build(const Mesh<MeshPoint>& mesh);
    std::vector<BVHNode> nodearray;
private:
    std::vector<BVHLeafInfo> bounds;

    uint32_t recursiveBuild(std::vector<BVHLeafInfo>::iterator bg, std::vector<BVHLeafInfo>::iterator ed);
};

class BVHTreeTBO {
	glTBO ptr, bound;
public:
	void init(const Mesh<MeshPoint>& mesh) {
		BVHTree tree;
		tree.build(mesh);
		std::vector<uint32_t> ptrdata;
		std::vector<float> bounddata;
		//std::cout << tree.nodearray.size() << "\n";
		for (auto& i : tree.nodearray) {
			//std::cout << i.children[0] << " " << i.children[1] << "\n";
			ptrdata.insert(ptrdata.end(), { i.axis,i.children[0],i.children[1] });
			bounddata.insert(bounddata.end(), {
				i.bound.pMin[0],i.bound.pMin[1],i.bound.pMin[2],
				i.bound.pMax[0],i.bound.pMax[1],i.bound.pMax[2] 
			});
		}
		ptr.init((void*)ptrdata.data(), ptrdata.size() * sizeof(uint32_t), GL_R32UI);
		bound.init((void*)bounddata.data(), bounddata.size() * sizeof(float), GL_R32F);
	}
	void use() {
		ptr.bind(3);
		bound.bind(4);
	}
	void release() {
		ptr.release();
		bound.release();
	}
};
