#include "Surface.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

void Surface::init(){
    glObject::init(reinterpret_cast<float*>(vertices.data()),vertices.size(),{3,3},indices);
}

void Surface::defaultrender(
	std::map<std::string,Shader>& shaderManager,
	glm::mat4 model,glm::mat4 view,glm::mat4 projection,
	glm::vec3 viewPos,glm::vec3 lightDir,
	glm::vec3 materialDiffuse,glm::vec3 materialSpecular,float materialShininess,
	glm::vec3 lightColor,glm::vec3 ambientColor){

	if(shaderManager.find("SURFACE_DEFAULT_SHADER")==shaderManager.end()){
		shaderManager["SURFACE_DEFAULT_SHADER"] = Surface::defaultShader();
	}
	Shader shader=shaderManager["SURFACE_DEFAULT_SHADER"];
	shader.use();

    shader.setMat4("model",model);
    shader.setMat4("view",view);
    shader.setMat4("projection",projection);

    shader.setVec3("viewPos",viewPos);
    shader.setVec3("material.diffuse",glm::vec3(0.4f));
    shader.setVec3("material.specular",glm::vec3(0.4f));
    shader.set1f("material.shininess",20.0f);
    shader.setVec3("light.direction",glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)));
    shader.setVec3("light.color",glm::vec3(1.0f));
    shader.setVec3("ambientColor",glm::vec3(0.5f));

    render(GL_TRIANGLES);
}

void Surface::defaultrender_T(
	std::map<std::string,Shader>& shaderManager,
	glm::mat4 model,glm::mat4 view,glm::mat4 projection,
	glm::vec3 viewPos,glm::vec3 lightDir,
	glm::vec3 materialDiffuse,glm::vec3 materialSpecular,float materialShininess,
	glm::vec3 lightColor,glm::vec3 ambientColor){

	defaultrender(shaderManager,model,view,projection,viewPos,lightDir,materialDiffuse,materialSpecular,materialShininess,lightColor,ambientColor);
	
	if(shaderManager.find("SURFACE_DEFAULT_SHADER_N")==shaderManager.end()){
		shaderManager["SURFACE_DEFAULT_SHADER_N"] = Surface::defaultShader_N();
	}
	Shader shader=shaderManager["SURFACE_DEFAULT_SHADER_N"];
	shader.use();
    shader.setMat4("model",glm::mat4(1.0f));
    shader.setMat4("view",view);
    shader.setMat4("projection",projection);
	glObject::render(GL_TRIANGLES);
}

Surface Surface::evalQuad(){
    Surface R;
	R.vertices.insert(R.vertices.end(), {
		{glm::vec3(-1,0,-1),glm::vec3(0,1,0)},
		{glm::vec3(+1,0,-1),glm::vec3(0,1,0)},
		{glm::vec3(+1,0,+1),glm::vec3(0,1,0)},
		{glm::vec3(-1,0,+1),glm::vec3(0,1,0)},
	});

	R.indices.insert(R.indices.end(), {
		0,1,2,
		0,2,3
	});
	
	return R;
}

Surface Surface::evalCube(){
	Surface R;
	R.vertices.insert(R.vertices.end(), {
		{glm::vec3(-1,-1,+1),glm::vec3(0,0,1)},
		{glm::vec3(+1,-1,+1),glm::vec3(0,0,1)},
		{glm::vec3(+1,+1,+1),glm::vec3(0,0,1)},
		{glm::vec3(-1,+1,+1),glm::vec3(0,0,1)},

		{glm::vec3(-1,-1,-1),glm::vec3(0,0,-1)},
		{glm::vec3(+1,-1,-1),glm::vec3(0,0,-1)},
		{glm::vec3(+1,+1,-1),glm::vec3(0,0,-1)},
		{glm::vec3(-1,+1,-1),glm::vec3(0,0,-1)},

		{glm::vec3(-1,+1,-1),glm::vec3(0,1,0)},
		{glm::vec3(+1,+1,-1),glm::vec3(0,1,0)},
		{glm::vec3(+1,+1,+1),glm::vec3(0,1,0)},
		{glm::vec3(-1,+1,+1),glm::vec3(0,1,0)},

		{glm::vec3(-1,-1,-1),glm::vec3(0,-1,0)},
		{glm::vec3(+1,-1,-1),glm::vec3(0,-1,0)},
		{glm::vec3(+1,-1,+1),glm::vec3(0,-1,0)},
		{glm::vec3(-1,-1,+1),glm::vec3(0,-1,0)},

		{glm::vec3(+1,-1,-1),glm::vec3(1,0,0)},
		{glm::vec3(+1,+1,-1),glm::vec3(1,0,0)},
		{glm::vec3(+1,+1,+1),glm::vec3(1,0,0)},
		{glm::vec3(+1,-1,+1),glm::vec3(1,0,0)},

		{glm::vec3(-1,-1,-1),glm::vec3(-1,0,0)},
		{glm::vec3(-1,+1,-1),glm::vec3(-1,0,0)},
		{glm::vec3(-1,+1,+1),glm::vec3(-1,0,0)},
		{glm::vec3(-1,-1,+1),glm::vec3(-1,0,0)},

		});

	R.indices.insert(R.indices.end(), {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10,8, 10,11,
		12,13,14,12,14,15,
		16,17,18,16,18,19,
		20,21,22,20,22,23
		});

	return R;
}

Surface Surface::evalSphere(float radius,uint32_t steps){
	Surface R;

	for (size_t i = 0; i < steps; ++i){
		float t1 = 2.0f * glm::pi<float>() * float(i) / steps;
		for (size_t j = 0; j <= steps / 2; ++j) {
			float t2 = 1.0f * glm::pi<float>() * float(j) / (steps / 2);
			R.vertices.push_back({
				radius * glm::vec3(sin(t2) * cos(t1),sin(t2) * sin(t1),cos(t2)) ,
				glm::vec3(sin(t2) * cos(t1),sin(t2) * sin(t1),cos(t2)) ,
			});
		}

		uint32_t l_now = i * (steps / 2 + 1);
		uint32_t l_nxt = (i + 1) % steps * (steps / 2 + 1);
		for (uint32_t j = 0; j + 1 <= steps / 2; ++j) {
			R.indices.insert(R.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			R.indices.insert(R.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}
	return R;
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

Surface Surface::evalSweepSurf(const Curve& profile, unsigned steps)
{
	Surface surface;

	if (!checkFlat(profile)){
		std::cerr << "surfRev profile curve must be flat on xy plane." << std::endl;
		return surface;
	}

	for (size_t i = 0; i < steps; ++i) {
		float t = 2 * glm::pi<float>() * i / steps;
		glm::mat4 M(1.0f);
		M = glm::rotate(M, t, glm::vec3(0, 1, 0));
		glm::mat4 M_norm = glm::transpose(glm::inverse(M));
		for (size_t j = 0; j < profile.size(); ++j) {
			surface.vertices.push_back({
				glm::vec3(M * glm::vec4(profile[j].V,1.0f)),
				glm::vec3(M_norm * glm::vec4(-profile[j].N,1.0f))
			});
		}

		uint32_t l_now = i * profile.size();
		uint32_t l_nxt = (i + 1) % steps * profile.size();
		for (uint32_t j = 0; j + 1 < profile.size(); ++j) {
			surface.indices.insert(surface.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			surface.indices.insert(surface.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}

	return surface;
}

Surface Surface::evalCylinder(const Curve& profile, const Curve& sweep)
{
	Surface surface;

	if (!checkFlat(profile)){
		std::cerr << "surfRev profile curve must be flat on xy plane." << std::endl;
		return surface;
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
			surface.vertices.push_back({
				glm::vec3(M * glm::vec4(profile[j].V,1.0f)),
				glm::vec3(M_norm * glm::vec4(-profile[j].N,1.0f))
			});
		}
	}

	for (uint32_t i = 0; i + 1 < sweep.size(); ++i) {
		uint32_t l_now = i * profile.size();
		uint32_t l_nxt = (i + 1) % sweep.size() * profile.size();
		for (uint32_t j = 0; j + 1 < profile.size(); ++j) {
			surface.indices.insert(surface.indices.end(), { l_now + j, l_now + j + 1, l_nxt + j });
			surface.indices.insert(surface.indices.end(), { l_nxt + j, l_now + j + 1, l_nxt + j + 1 });
		}
	}

	return surface;
}

Surface Surface::evalModel(const char* filename) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
		throw std::runtime_error(warn + err);
	}

	Surface surface;
	std::vector<int> cnt(attrib.vertices.size() / 3);

	for (size_t i = 0; i < attrib.vertices.size(); i+=3) {
		surface.vertices.push_back({
			glm::vec3(attrib.vertices[i],attrib.vertices[i + 1],attrib.vertices[i + 2]),
			glm::vec3(0)
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
				surface.indices.push_back(shape.mesh.indices[i + j].vertex_index);
			}
			glm::vec3 a = v[1] - v[0];
			glm::vec3 b = v[2] - v[0];
			glm::vec3 normal = glm::normalize(glm::cross(a, b));
			for (size_t j = 0; j < 3; j++) {
				surface.vertices[ind[j]].N += normal;
				cnt[ind[j]]++;
			}
		}
	}
	for (size_t i = 0; i < surface.vertices.size();i++) {
		surface.vertices[i].N /= cnt[i];
	}
	return surface;
}

#include <shaders/rc.h>

Shader Surface::defaultShader(){
	Shader shader;
	shader.compile((char*)SURFACE_DEFAULT_VERT_DATA,nullptr,(char*)SURFACE_DEFAULT_FRAG_DATA);
	return shader;
}

Shader Surface::defaultShader_N(){
	Shader shader;
	shader.compile(
		(char*)SURFACE_DEFAULT_N_VERT_DATA,
		(char*)SURFACE_DEFAULT_N_GEOM_DATA,
		(char*)SURFACE_DEFAULT_N_FRAG_DATA
	);
	return shader;
}