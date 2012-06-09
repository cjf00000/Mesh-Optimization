#include "Mesh.h"
#include <cassert>
#include "SimpleObject.h"
using namespace SimpleOBJ;

Mesh::Mesh(void)
{
		
}

Mesh::Mesh( const char* fn )
{
	bool success = LoadFromObj(fn);
	assert(success);
}

Mesh::~Mesh(void)
{
}

bool Mesh::LoadFromObj( const char* fn )
{
	//Clear data structure
	v.clear();
	f.clear();
	adj.clear();		

	//Create a new CSimpleObject
	CSimpleObject *simpleObject = new CSimpleObject();
	bool success = simpleObject->LoadFromObj(fn);
	//Copy the Information from CSimpleObject		
	for ( int i=0; i<simpleObject->m_nVertices; ++i )
		v.push_back(simpleObject->m_pVertexList[i]);				//Copy vertexes
	adj.resize(v.size());
	for ( int i=0; i<simpleObject->m_nTriangles; ++i )
	{			
		f.push_back(simpleObject->m_pTriangleList[i]);			//Copy faces
		for ( int p1=0; p1<3; ++p1 )												//Construct adjacent list
			for ( int p2=0; p2<3; ++p2 )
				if ( p1!=p2 )
					adj[simpleObject->m_pTriangleList[i][p1]].push_back(simpleObject->m_pTriangleList[i][p2]);
	}
	//Release simpleObject
	delete simpleObject;
	//Return the result
	return success;
}

SimpleOBJ::Vec3f Mesh::normal( SimpleOBJ::Array<int, 3> f )
{
	return Vec3f::cross(v[f[2]]-v[f[0]], v[f[1]]-v[f[0]]).Normalize();
}
