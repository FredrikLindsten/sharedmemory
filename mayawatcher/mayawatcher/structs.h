#include <vector>

struct vec3{
	float x, y, z;
};

struct vec2{
	float u, v;
};

struct uvSet
{
	std::vector<vec2> UVs;
};

struct Mesh{
	MFnMesh* mesh;
	MFnDagNode* node;
	int index;
};

struct faceIndices
{
	int pointID, normalID;
	std::vector<int> texCoordsID;
};

struct face
{
	faceIndices verts[3];
};

struct MeshData
{
	MDagPath mesh_path;
	MString name;
	UINT id;

	std::vector<vec3> points;
	std::vector<vec3> normals;
	std::vector<uvSet> uvSets;
	std::vector<face> faces;
};

struct MeshHeader{
	size_t id, nameLength, pointCount, normalCount, uvCount, faceCount;
};