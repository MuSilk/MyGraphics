#include "Curve.h"

#include <glm/gtc/constants.hpp>

inline bool approx(const glm::vec3& lhs, const glm::vec3& rhs)
{
	const float eps = 1e-8f;
	return (lhs - rhs).length() < eps;
}

void Curve::init(){
    glObject::init(reinterpret_cast<float*>(this->data()),this->size(),{3,3,3,3});
}

void Curve::defaultrender(std::map<std::string,Shader>& shaderManager,glm::mat4 model,glm::mat4 view,glm::mat4 proj){
    if(shaderManager.find("CURVE_DEFAULT_SHADER")==shaderManager.end()){
		shaderManager["CURVE_DEFAULT_SHADER"] = Curve::defaultShader();
	}
	Shader shader=shaderManager["CURVE_DEFAULT_SHADER"];
	shader.use();
	glm::mat4 mvp=proj*view*model;
	shader.setMat4("mvp",mvp);
	glObject::render(GL_LINE_STRIP);
}

void Curve::defaultrender_TNB(std::map<std::string,Shader>& shaderManager,glm::mat4 model,glm::mat4 view,glm::mat4 proj){
	defaultrender(shaderManager,model,view,proj);
	if(shaderManager.find("CURVE_DEFAULT_SHADER_TNB")==shaderManager.end()){
		shaderManager["CURVE_DEFAULT_SHADER_TNB"] = Curve::defaultShader_TNB();
	}
	Shader shader=shaderManager["CURVE_DEFAULT_SHADER_TNB"];
    shader.use();
    shader.setMat4("model",glm::mat4(1.0f));
    shader.setMat4("view",view);
    shader.setMat4("projection",proj);
	glObject::render(GL_POINTS);
}

Curve Curve::evalBezier(const std::vector< glm::vec3 >& P, uint32_t steps) {
	if (P.size() < 4 || P.size() % 3 != 1) {
		std::cerr << "evalBezier must be called with 3n+1 control points." << std::endl;
		return Curve();
	}

	const glm::mat4 BEZ = glm::mat4(
		 1, 0, 0, 0,
		-3, 3, 0, 0,
		 3,-6, 3, 0,
		-1, 3,-3, 1
	);

	size_t n = (P.size() - 1) / 3;
	Curve R(n * steps + 1);
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < steps; ++j) {
			float t = 1.0f * j / steps;
			size_t p = i * steps + j;

			glm::vec4 vec_v = BEZ * glm::vec4(1, t, t * t, t * t * t);
			glm::vec4 vec_t = BEZ * glm::vec4(0, 1, 2 * t, 3 * t * t);
			R[p].V = R[p].T = glm::vec3(0);
			for (size_t k = 0; k < 4; k++) {
				R[p].V += vec_v[k] * P[3 * i + k];
				R[p].T += vec_t[k] * P[3 * i + k];
			}
			R[p].T=glm::normalize(R[p].T);
		}
	}
	float t = 1.0;
	size_t p = n * steps;
	glm::vec4 vec_v = BEZ * glm::vec4(1, t, t * t, t * t * t);
	glm::vec4 vec_t = BEZ * glm::vec4(0, 1, 2 * t, 3 * t * t);
	R[p].V = R[p].T = glm::vec3(0);
	for (size_t k = 0; k < 4; k++) {
		R[p].V += vec_v[k] * P[3 * (n - 1) + k];
		R[p].T += vec_t[k] * P[3 * (n - 1) + k];
	}
	R[p].T = glm::normalize(R[p].T);

	if (approx(R[0].T, glm::vec3(0, 0, 1)))R[0].N = glm::normalize(glm::cross(glm::vec3(1, 0, 0), R[0].T));
	else R[0].N = glm::normalize(glm::cross(glm::vec3(0, 0, 1), R[0].T));
	R[0].B = glm::normalize(glm::cross(R[0].T, R[0].N));

	for (size_t i = 1; i < n * steps + 1; ++i) {
		R[i].N = glm::normalize(glm::cross(R[i - 1].B, R[i].T));
		R[i].B = glm::normalize(glm::cross(R[i].T, R[i].N));
	}

	if (approx(P.front(), P.back())) {
		auto vb = R.front().N, ve = R.back().N;
		float cos_theta = glm::dot(ve, vb) / vb.length() / ve.length();
		std::cerr << "Curve closed and cos(theta)=" << cos_theta << std::endl;
		if (cos_theta <= 1) {
			float theta = acos(cos_theta);
			if (glm::dot(glm::cross(ve, vb), R.front().T) < 0)theta = -theta;
			for (size_t i = 0; i < R.size(); i++) {
				float t = theta * i / R.size();
				R[i].N = glm::vec3(cos(t) * R[i].N + sin(t) * R[i].B);
				R[i].B = glm::normalize(glm::cross(R[i].T, R[i].N));
			}
		}
	}
	return R;
}

Curve Curve::evalBspline(const std::vector< glm::vec3 >& P, uint32_t steps) {
	if (P.size() < 4){
		std::cerr << "evalBspline must be called with 4 or more control points." << std::endl;
		exit(0);
	}

	const glm::mat4 BEZ = glm::mat4(
		 1, 0, 0, 0,
		-3, 3, 0, 0,
		 3,-6, 3, 0,
		-1, 3,-3, 1
	);
	const glm::mat4 B = (1.0f / 6) * glm::mat4(
		 1, 4, 1, 0,
		-3, 0, 3, 0,
		 3,-6, 3, 0,
		-1, 3,-3, 1
	);
	const glm::mat4 trans_B_BEZ = B * glm::inverse(BEZ);

	size_t n = P.size() - 3;
	Curve R(n * steps + 1);
	for (size_t i = 0; i < n; i++) {
		vector<glm::vec3> pBEZ(4);
		for (size_t j = 0; j < 4; j++) {
			glm::vec4 v = trans_B_BEZ[j];
			for (size_t k = 0; k < 4; k++) {
				pBEZ[j] += v[k] * P[i + k];
			}
		}
		Curve curve = evalBezier(pBEZ, steps);
		copy(curve.begin(), curve.end(), R.begin() + i * steps);
	}

	if (approx(R[0].T, glm::vec3(0, 0, 1)))R[0].N = glm::normalize(glm::cross(glm::vec3(1, 0, 0), R[0].T));
	else R[0].N = glm::normalize(glm::cross(glm::vec3(0, 0, 1), R[0].T));
	R[0].B = glm::normalize(glm::cross(R[0].T, R[0].N));

	for (size_t i = 1; i < n * steps + 1; ++i) {
		R[i].N = glm::normalize(glm::cross(R[i - 1].B, R[i].T));
		R[i].B = glm::normalize(glm::cross(R[i].T, R[i].N));
	}

	if (approx(P.front(), P.back())) {
		auto vb = R.front().N, ve = R.back().N;
		float cos_theta = glm::dot(ve, vb) / vb.length() / ve.length();
		std::cerr << "Curve closed and cos(theta)=" << cos_theta << std::endl;
		if (cos_theta <= 1) {
			float theta = acos(cos_theta);
			if (glm::dot(glm::cross(ve, vb), R.front().T) < 0)theta = -theta;
			for (size_t i = 0; i < R.size(); i++) {
				float t = theta * i / R.size();
				R[i].N = glm::vec3(cos(t) * R[i].N + sin(t) * R[i].B);
				R[i].B = glm::normalize(glm::cross(R[i].T, R[i].N));
			}
		}
	}
	return R;
}

Curve Curve::evalCircle(float radius, uint32_t steps) {
	Curve R(steps + 1);

	for (unsigned i = 0; i <= steps; ++i)
	{
		float t = 2.0f * glm::pi<float>() * float(i) / steps;

		R[i].V = radius * glm::vec3(cos(t), sin(t), 0);

		R[i].T = glm::vec3(-sin(t), cos(t), 0);

		R[i].N = glm::vec3(-cos(t), -sin(t), 0);

		R[i].B = glm::vec3(0, 0, 1);
	}

	return R;
}

#include<shaders/rc.h>
Shader Curve::defaultShader() {
    Shader shader;
	shader.compile((char *)CURVE_DEFAULT_VERT_DATA,nullptr,(char *)CURVE_DEFAULT_FRAG_DATA);
	return shader;
}

Shader Curve::defaultShader_TNB(){
	Shader shader;
	shader.compile(
		(char *)CURVE_DEFAULT_TNB_VERT_DATA,
		(char *)CURVE_DEFAULT_TNB_GEOM_DATA,
		(char *)CURVE_DEFAULT_TNB_FRAG_DATA
	);
	return shader;
}