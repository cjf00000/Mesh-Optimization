#pragma once
#include "simpleobject.h"
#include <vector>

/*
	The mesh is represented by 
	1.	Vertexes
	2. Triangles of vertex index
	3. Vertex adjacent list, for updating the triangle after edge collapse
*/
class Mesh
{
public:
	//Construct an empty mesh
	Mesh(void);
	//Construct from an obj file
	Mesh(const char* fn);
	//!!!This function unimplemented for convenience
	~Mesh(void);

	//Use CSimpleObject to initialize Mesh
	bool LoadFromObj(const char* fn);

	SimpleOBJ::Vec3f normal(SimpleOBJ::Array<int, 3> f);

public:
	//Vertexes
	std::vector<SimpleOBJ::Vec3f> v;
	//Faces, stores the index of each vertex
	std::vector<SimpleOBJ::Array<int,3>> f;
	//Adjacent list, adj[i] is all vertexes which are adjacent to vertex i
	std::vector<std::vector<int> > adj;
};