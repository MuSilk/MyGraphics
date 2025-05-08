#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalQuad(){
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
    auto& R=*result;
	R.geometry="Quad";
	R.attrs.setAttr("Geometry","Quad");
	R.vertices=std::vector<float>{
		-1.0f,0.0f,-1.0f,	0.0f,1.0f,0.0f,
		+1.0f,0.0f,-1.0f,	0.0f,1.0f,0.0f,
		+1.0f,0.0f,+1.0f,	0.0f,1.0f,0.0f,
		-1.0f,0.0f,+1.0f,	0.0f,1.0f,0.0f,
	};

	R.indices.insert(R.indices.end(), {
		0,1,2,
		0,2,3
	});
	
	return result;
}

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalCube(){
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
    auto& R=*result;
	R.geometry="Cube";
	R.vertices=std::vector<float>{
		-1,-1,+1,0,0,1,
		+1,-1,+1,0,0,1,
		+1,+1,+1,0,0,1,
		-1,+1,+1,0,0,1,

		-1,-1,-1,0,0,-1,
		+1,-1,-1,0,0,-1,
		+1,+1,-1,0,0,-1,
		-1,+1,-1,0,0,-1,

		-1,+1,-1,0,1,0,
		+1,+1,-1,0,1,0,
		+1,+1,+1,0,1,0,
		-1,+1,+1,0,1,0,

		-1,-1,-1,0,-1,0,
		+1,-1,-1,0,-1,0,
		+1,-1,+1,0,-1,0,
		-1,-1,+1,0,-1,0,

		+1,-1,-1,1,0,0,
		+1,+1,-1,1,0,0,
		+1,+1,+1,1,0,0,
		+1,-1,+1,1,0,0,

		-1,-1,-1,-1,0,0,
		-1,+1,-1,-1,0,0,
		-1,+1,+1,-1,0,0,
		-1,-1,+1,-1,0,0,
	};

	R.indices.insert(R.indices.end(), {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10,8, 10,11,
		12,13,14,12,14,15,
		16,17,18,16,18,19,
		20,21,22,20,22,23
		});
	return result;
}

std::shared_ptr<Mesh<TexturedMeshPoint>> Mesh<TexturedMeshPoint>::evalCube(){
	auto result=std::make_shared<Mesh<TexturedMeshPoint>>();
	auto& R=*result;
	R.geometry="Cube";
	R.vertices=std::vector<float>{
		-1,-1,+1,	0,0,1,	0,0,
		+1,-1,+1,	0,0,1,	1,0,
		+1,+1,+1,	0,0,1,	1,1,
		-1,+1,+1,	0,0,1,	0,1,

		-1,-1,-1,	0,0,-1, 0,0,
		+1,-1,-1,	0,0,-1, 1,0,
		+1,+1,-1,	0,0,-1, 1,1,
		-1,+1,-1,	0,0,-1, 0,1,

		-1,+1,-1,	0,1,0,	0,0,
		+1,+1,-1,	0,1,0,	1,0,
		+1,+1,+1,	0,1,0,	1,1,
		-1,+1,+1,	0,1,0,	0,1,

		-1,-1,-1,	0,-1,0,	0,0,
		+1,-1,-1,	0,-1,0, 1,0,
		+1,-1,+1,	0,-1,0, 1,1,
		-1,-1,+1,	0,-1,0, 0,1,

		+1,-1,-1,	1,0,0,	0,0,
		+1,+1,-1,	1,0,0,	1,0,
		+1,+1,+1,	1,0,0, 	1,1,
		+1,-1,+1,	1,0,0,	0,1,

		-1,-1,-1,	-1,0,0,	0,0,
		-1,+1,-1,	-1,0,0,	1,0,
		-1,+1,+1,	-1,0,0, 1,1,
		-1,-1,+1,	-1,0,0,	0,1,
	};

	R.indices.insert(R.indices.end(), {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10,8, 10,11,
		12,13,14,12,14,15,
		16,17,18,16,18,19,
		20,21,22,20,22,23
	});
	return result;
} 

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalCircle(float radius,uint32_t steps){
	auto c=Curve::evalCircle(radius,steps);
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
	auto& R=*result;
	R.geometry="Circle";
	R.attrs.setAttr("Radius",radius);
	R.attrs.setAttr("Steps",steps);
	for(size_t i=0;i<c->size();i++){
		R.vertices.insert(R.vertices.end(),{
			(*c)[i].V.x,(*c)[i].V.y,(*c)[i].V.z,
			(*c)[i].B.x,(*c)[i].B.y,(*c)[i].B.z,
		});
	}
	for(size_t i=1;i+1<c->size();i++){
		R.indices.push_back(0);
		R.indices.push_back(i);
		R.indices.push_back(i+1);
	}
	return result;
}

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalSphere(float radius,uint32_t steps){
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
	auto& R=*result;
	R.geometry="Sphere";
	R.attrs.setAttr("Radius",radius);
	R.attrs.setAttr("Steps",steps);

	for (size_t i = 0; i < steps; ++i){
		float t1 = 2.0f * glm::pi<float>() * float(i) / steps;
		for (size_t j = 0; j <= steps / 2; ++j) {
			float t2 = 1.0f * glm::pi<float>() * float(j) / (steps / 2);
			auto V=radius * glm::vec3(sin(t2) * cos(t1),sin(t2) * sin(t1),cos(t2));
			auto N=glm::vec3(sin(t2) * cos(t1),sin(t2) * sin(t1),cos(t2));
			R.vertices.insert(R.vertices.end(),{V.x,V.y,V.z});
			R.vertices.insert(R.vertices.end(),{N.x,N.y,N.z});
		}

		uint32_t l_now = i * (steps / 2 + 1);
		uint32_t l_nxt = (i + 1) % steps * (steps / 2 + 1);
		for (uint32_t j = 0; j + 1 <= steps / 2; ++j) {
			R.indices.insert(R.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			R.indices.insert(R.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}
	return result;
}

static bool checkFlat(const Curve& profile)
{
	for (unsigned i = 0; i < profile.size(); i++)
		if (profile[i].V[2] != 0.0 ||
			profile[i].T[2] != 0.0 ||
			profile[i].N[2] != 0.0)
			return false;

	return true;
}

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalSweepSurf(const Curve& profile, unsigned steps)
{
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
	auto& mesh=*result;
	mesh.geometry="SweepSurf";

	if (!checkFlat(profile)){
		std::cerr << "surfRev profile curve must be flat on xy plane." << std::endl;
		return nullptr;
	}

	for (size_t i = 0; i < steps; ++i) {
		float t = 2 * glm::pi<float>() * i / steps;
		glm::mat4 M(1.0f);
		M = glm::rotate(M, t, glm::vec3(0, 1, 0));
		glm::mat4 M_norm = glm::transpose(glm::inverse(M));
		for (size_t j = 0; j < profile.size(); ++j) {
			auto V=glm::vec3(M * glm::vec4(profile[j].V,1.0f));
			auto N=glm::vec3(M_norm * glm::vec4(-profile[j].N,1.0f));
			mesh.vertices.insert(mesh.vertices.end(),{
				V.x,V.y,V.z,
				N.x,N.y,N.z
			});
		}

		uint32_t l_now = i * profile.size();
		uint32_t l_nxt = (i + 1) % steps * profile.size();
		for (uint32_t j = 0; j + 1 < profile.size(); ++j) {
			mesh.indices.insert(mesh.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			mesh.indices.insert(mesh.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}

	return result;
}

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalCylinder(const Curve& profile, const Curve& sweep)
{
	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
	auto& mesh=*result;
	mesh.geometry="Cylinder";

	if (!checkFlat(profile)){
		std::cerr << "surfRev profile curve must be flat on xy plane." << std::endl;
		return nullptr;
	}

	for (size_t i = 0; i < sweep.size(); ++i) {
		glm::mat4 M{
			glm::vec4(sweep[i].N, 0.0f),
			glm::vec4(sweep[i].B, 0.0f),
			glm::vec4(sweep[i].T, 0.0f),
			glm::vec4(sweep[i].V, 1.0f)
		};
		glm::mat4 M_norm = glm::transpose(glm::inverse(M));

		for (size_t j = 0; j < profile.size(); ++j) {
			auto V=glm::vec3(M * glm::vec4(profile[j].V,1.0f));
			auto N=glm::vec3(M_norm * glm::vec4(-profile[j].N,1.0f));
			mesh.vertices.insert(mesh.vertices.end(),{
				V.x,V.y,V.z,
				N.x,N.y,N.z
			});
		}
	}

	for (uint32_t i = 0; i + 1 < sweep.size(); ++i) {
		uint32_t l_now = i * profile.size();
		uint32_t l_nxt = (i + 1) % sweep.size() * profile.size();
		for (uint32_t j = 0; j + 1 < profile.size(); ++j) {
			mesh.indices.insert(mesh.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			mesh.indices.insert(mesh.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}

	return result;
}

std::shared_ptr<Mesh<MeshPoint>> Mesh<MeshPoint>::evalModel(const char* filename) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
		throw std::runtime_error(warn + err);
	}

	std::shared_ptr<Mesh<MeshPoint>> result=std::make_shared<Mesh<MeshPoint>>();
	auto& mesh=*result;
	mesh.geometry="Model";
	mesh.attrs.setAttr("Source",filename);
	std::vector<int> cnt(attrib.vertices.size() / 3);

	for (size_t i = 0; i < attrib.vertices.size(); i+=3) {
		auto V= glm::vec3(attrib.vertices[i],attrib.vertices[i + 1],attrib.vertices[i + 2]);
		auto N=glm::vec3(0);
		mesh.vertices.insert(mesh.vertices.end(),{
			V.x,V.y,V.z,
			N.x,N.y,N.z
		});
	}

	for (const auto& shape : shapes) {
		for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
			glm::vec3 v[3];
			uint32_t ind[3];
			for (size_t j = 0; j < 3; j++) {
				ind[j] = shape.mesh.indices[i + j].vertex_index;
				v[j] = {
					attrib.vertices[3 * ind[j] + 0],
					attrib.vertices[3 * ind[j] + 1],
					attrib.vertices[3 * ind[j] + 2],
				};
				mesh.indices.push_back(shape.mesh.indices[i + j].vertex_index);
			}
			glm::vec3 a = v[1] - v[0];
			glm::vec3 b = v[2] - v[0];
			glm::vec3 normal = glm::normalize(glm::cross(a, b));
			for (size_t j = 0; j < 3; j++) {
				mesh[ind[j]].N += normal;
				cnt[ind[j]]++;
			}
		}
	}
	for (size_t i = 0; i < mesh.size();i++) {
		mesh[i].N /= cnt[i];
	}
	return result;
}

void MeshTBO::init(const Mesh<MeshPoint>& mesh){
	FaceNum = mesh.indices.size() / 3;
	vertices.init((void*)mesh.vertices.data(),mesh.vertices.size() * 6 * sizeof(float),GL_R32F);
	indices.init((void*)mesh.indices.data(),mesh.indices.size() * sizeof(uint32_t),GL_R32UI);
}

void MeshTBO::use(){
	vertices.bind(1);
	indices.bind(2);
}

void MeshTBO::release(){
	vertices.release();
	indices.release();
}

uint32_t MeshTBO::getFaceNum(){return FaceNum;}