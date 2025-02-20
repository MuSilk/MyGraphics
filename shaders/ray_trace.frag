#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform struct Camera {
	vec3 camPos;
	vec3 front,right,up;
	vec2 grid;
}camera;

uniform float randOrigin;
uint wseed;
float rand(void);

struct Bound3d{
	vec3 pMin,pMax;
};

struct LinearBVHNode{
	vec3 pMin,pMax;
	int axis;
	int children[2];
};

struct Sphere{
	vec3 center;
	float radius;
	vec3 albedo;
	int materialIndex;
};

struct Triangle{
	vec3 v[3],n[3];
};

uniform Triangle triFloor[2];

uniform samplerBuffer texMeshVertex;
uniform usamplerBuffer texMeshFaceIndex;
uniform int meshFaceNum;

uniform usamplerBuffer bvhTreePtr;
uniform samplerBuffer bvhTreeBounds;

struct Ray{
	vec3 origin;
	vec3 direction;
	float hitMin;
};

struct hitRecord{
	vec3 Normal;
	vec3 Pos;
	vec3 albedo;
	int materialIndex;
	float rayHitMin;
};
hitRecord rec;

float hitSphere(Sphere s,Ray r);
hitRecord hitTriangle(Triangle tri,Ray r);
bool hitScene(Ray r);
vec3 shading(Ray r);

bool IntersectBound(Bound3d bound,Ray ray,vec3 invDir,bool dirIsNeg[3]);

uniform int LoopNum;
uniform sampler2D historyTexture;

void main() {
	wseed = uint(float(randOrigin*float(6.95857)) * (TexCoords.x * TexCoords.y));
	vec3 hist=texture(historyTexture,TexCoords).rgb;

	Ray cameraRay;
	cameraRay.origin=camera.camPos;
	cameraRay.direction = normalize(camera.front + 
		(TexCoords.x-0.5)*camera.grid.x*camera.right + 
		(TexCoords.y-0.5)*camera.grid.y*camera.up);
	cameraRay.hitMin=100000.0;

	vec3 curColor = shading(cameraRay);
	curColor = (1.0 / float(LoopNum))*curColor + (float(LoopNum - 1) / float(LoopNum))*hist;
	FragColor = vec4(curColor, 1.0);
}

float randcore(uint seed) {
	seed = (seed ^ uint(61)) ^ (seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> uint(4));
	seed *= uint(0x27d4eb2d);
	wseed = seed ^ (seed >> uint(15));
	return float(wseed) * (1.0 / 4294967296.0);
}

float rand() {
	return randcore(wseed);
}

LinearBVHNode getLinearBVHNode(int offset){
	LinearBVHNode node;
	node.pMin=vec3(
		texelFetch(bvhTreeBounds,offset*6+0).x,
		texelFetch(bvhTreeBounds,offset*6+1).x,
		texelFetch(bvhTreeBounds,offset*6+2).x);
	node.pMax=vec3(
		texelFetch(bvhTreeBounds,offset*6+3).x,
		texelFetch(bvhTreeBounds,offset*6+4).x,
		texelFetch(bvhTreeBounds,offset*6+5).x);

	node.axis=int(texelFetch(bvhTreePtr,offset*3+0).x);
	node.children[0]=int(texelFetch(bvhTreePtr,offset*3+1).x);
	node.children[1]=int(texelFetch(bvhTreePtr,offset*3+2).x);
	return node;
}

float hitSphere(Sphere s,Ray r){
	vec3 oc = r.origin - s.center;
	float a = dot(r.direction, r.direction);
	float b = 2.0 * dot(oc, r.direction);
	float c = dot(oc, oc) - s.radius * s.radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0.0) {
		float dis = (-b - sqrt(discriminant)) / (2.0 * a);
		if (dis > 0.0) return dis;
		else return -1.0;
	}else{
		return -1.0;
	}
}

hitRecord hitTriangle(Triangle tri,Ray r){
	hitRecord rec;
	rec.rayHitMin=-1;

	vec3 S=r.origin-tri.v[0];
    vec3 E1=tri.v[1]-tri.v[0];
    vec3 E2=tri.v[2]-tri.v[0];
    vec3 S1=cross(r.direction,E2);
    vec3 S2=cross(S,E1);

    float S1E1=dot(S1,E1);
    if(abs(S1E1)<1e-8)return rec;
    float t=dot(S2,E2)/S1E1;
    float b1=dot(S1,S)/S1E1;
    float b2=dot(S2,r.direction)/S1E1;

	if(b1>=0.0f&&b2>=0.0f&&(1-b1-b2)>=0.0f){
		rec.rayHitMin=t - 0.00001;
		rec.Normal=(1-b1-b2)*tri.n[0]+b1*tri.n[1]+b2*tri.n[2];
	}

	return rec;
}

Triangle getTriangle(int index){
	Triangle tri;
	uint ind0=texelFetch(texMeshFaceIndex,index*3).x;
	uint ind1=texelFetch(texMeshFaceIndex,index*3+1).x;
	uint ind2=texelFetch(texMeshFaceIndex,index*3+2).x;

	tri.v[0].x=texelFetch(texMeshVertex,int(ind0)*6).x;
	tri.v[0].y=texelFetch(texMeshVertex,int(ind0)*6+1).x;
	tri.v[0].z=texelFetch(texMeshVertex,int(ind0)*6+2).x;
	tri.n[0].x=texelFetch(texMeshVertex,int(ind0)*6+3).x;
	tri.n[0].y=texelFetch(texMeshVertex,int(ind0)*6+4).x;
	tri.n[0].z=texelFetch(texMeshVertex,int(ind0)*6+5).x;

	tri.v[1].x=texelFetch(texMeshVertex,int(ind1)*6).x;
	tri.v[1].y=texelFetch(texMeshVertex,int(ind1)*6+1).x;
	tri.v[1].z=texelFetch(texMeshVertex,int(ind1)*6+2).x;
	tri.n[1].x=texelFetch(texMeshVertex,int(ind1)*6+3).x;
	tri.n[1].y=texelFetch(texMeshVertex,int(ind1)*6+4).x;
	tri.n[1].z=texelFetch(texMeshVertex,int(ind1)*6+5).x;

	tri.v[2].x=texelFetch(texMeshVertex,int(ind2)*6).x;
	tri.v[2].y=texelFetch(texMeshVertex,int(ind2)*6+1).x;
	tri.v[2].z=texelFetch(texMeshVertex,int(ind2)*6+2).x;
	tri.n[2].x=texelFetch(texMeshVertex,int(ind2)*6+3).x;
	tri.n[2].y=texelFetch(texMeshVertex,int(ind2)*6+4).x;
	tri.n[2].z=texelFetch(texMeshVertex,int(ind2)*6+5).x;

	return tri;
}

vec3 getTriangleNormal(Triangle tri) {
	return normalize(cross(tri.v[2] - tri.v[0], tri.v[1] - tri.v[0]));
}

bool IntersectBVH(Ray ray){
    bool hit=false;

	vec3 invDir = vec3(1.0 / ray.direction.x, 1.0 / ray.direction.y, 1.0 / ray.direction.z);
	bool dirIsNeg[3];
	dirIsNeg[0] = (invDir.x < 0.0); dirIsNeg[1] = (invDir.y < 0.0); dirIsNeg[2] = (invDir.z < 0.0);
	int toVisitOffset=0,currentNodeIndex=0;
	int nodesToVist[64];

	while(true){
		LinearBVHNode node=getLinearBVHNode(currentNodeIndex);

		Bound3d bound; bound.pMin = node.pMin; bound.pMax = node.pMax;
		if(IntersectBound(bound,ray,invDir,dirIsNeg)){
			if(node.axis==4){
				int offset=node.children[0];
				Triangle tri_t=getTriangle(offset);

				hitRecord hit_t=hitTriangle(tri_t,ray);
				if (hit_t.rayHitMin > 0 && hit_t.rayHitMin < ray.hitMin) {
					ray.hitMin=hit_t.rayHitMin;
					rec.Normal = hit_t.Normal;
					hit = true;
				}
				if(toVisitOffset==0)break;
				currentNodeIndex=nodesToVist[--toVisitOffset];
			}
			else{
				if(bool(dirIsNeg[node.axis])){
					nodesToVist[toVisitOffset++]=node.children[0];
					currentNodeIndex=node.children[1];
				}
				else{
					nodesToVist[toVisitOffset++]=node.children[1];
					currentNodeIndex=node.children[0];
				}
			}
		}
		else{
			if(toVisitOffset==0)break;
			currentNodeIndex=nodesToVist[--toVisitOffset];
		}
	}
	/*for(int i=0;i<meshFaceNum;i++){
		Triangle tri_t=getTriangle(i);
		float dis_t=hitTriangle(tri_t,ray);
		if (dis_t > 0 && dis_t < ray.hitMin) {
			ray.hitMin=dis_t;
			tri=tri_t;
			hit = true;
		}
	}*/

	if(hit){
		rec.Pos = ray.origin + ray.hitMin * ray.direction;
		rec.albedo = vec3(0.83, 0.73, 0.1);
		rec.rayHitMin = ray.hitMin;
		rec.materialIndex = 0;
	}

	return hit;
}

bool hitScene(Ray r){
	bool ifHitTriangleMesh = false;
	bool ifHitTriangleFloor = false;
	int hitTriangleIndex;
	if(IntersectBVH(r)){
		r.hitMin=rec.rayHitMin;
		ifHitTriangleMesh=true;
	}

	for(int i=0;i<2;i++){
		hitRecord hit_t=hitTriangle(triFloor[i],r);
		if(hit_t.rayHitMin>0&&hit_t.rayHitMin<r.hitMin){
			r.hitMin=hit_t.rayHitMin;
			hitTriangleIndex=i;
			ifHitTriangleFloor=true;
		}
	}

	if (ifHitTriangleFloor) {
		rec.Pos = r.origin + r.hitMin * r.direction;
		rec.Normal = getTriangleNormal(triFloor[hitTriangleIndex]);
		rec.albedo = vec3(0.87, 0.87, 0.87);
		rec.materialIndex = 1;
		return true;
	}
	if (ifHitTriangleMesh) {
		return true;
	}
	else return false;
}

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(rand(), rand() ,rand()) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return p;
}

vec3 diffuseReflection(vec3 Normal) {
	return normalize(Normal + random_in_unit_sphere());
}

vec3 metalReflection(vec3 rayIn, vec3 Normal) {
	return normalize(rayIn - 2 * dot(rayIn, Normal) * Normal + 0.15* random_in_unit_sphere());
}

vec3 shading(Ray r){
	vec3 color = vec3(1.0,1.0,1.0);
	for (int i = 0; i < 3; i++) {
		if (hitScene(r)) {
			if(rec.materialIndex == 0)
				r.direction = diffuseReflection(rec.Normal);
			else if(rec.materialIndex == 1)
				r.direction = metalReflection(r.direction, rec.Normal);

			r.origin=rec.Pos;
			r.hitMin=100000;
			color *= rec.albedo;
		}
		else {
			if(i==1){
				vec3 lightPos = vec3(-4.0, 4.0, -4.0);
				vec3 lightDir = normalize(lightPos - rec.Pos);
				float diff = 0.5 * max(dot(rec.Normal, lightDir), 0.0) + 0.5;
				color *= vec3(diff, diff, diff);
			}
			else{
				float t = 0.5*(r.direction.y + 1.0);
				color *= (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
			}
			break;
		}
	}
	return color;
}

vec3 getBoundp(Bound3d bound, int i) {
	return (i == 0) ? bound.pMin : bound.pMax;
}
bool IntersectBound(Bound3d bounds, Ray ray, vec3 invDir, bool dirIsNeg[3]) {
	float tMin = (getBoundp(bounds, int(dirIsNeg[0])).x - ray.origin.x) * invDir.x;
	float tMax = (getBoundp(bounds, 1 - int(dirIsNeg[0])).x - ray.origin.x) * invDir.x;
	float tyMin = (getBoundp(bounds, int(dirIsNeg[1])).y - ray.origin.y) * invDir.y;
	float tyMax = (getBoundp(bounds, 1 - int(dirIsNeg[1])).y - ray.origin.y) * invDir.y;

	if (tMin > tyMax || tyMin > tMax) return false;
	if (tyMin > tMin) tMin = tyMin;
	if (tyMax < tMax) tMax = tyMax;

	float tzMin = (getBoundp(bounds, int(dirIsNeg[2])).z - ray.origin.z) * invDir.z;
	float tzMax = (getBoundp(bounds, 1 - int(dirIsNeg[2])).z - ray.origin.z) * invDir.z;

	if (tMin > tzMax || tzMin > tMax) return false;
	if (tzMin > tMin) tMin = tzMin;
	if (tzMax < tMax) tMax = tzMax;

	return tMax > 0;
}
