#include "Mesh.h"
#include <cassert>
#include "SimpleObject.h"
#include <iostream>
#include <fstream>
#include "LinearEquation_Special.h"
using namespace SimpleOBJ;
using namespace std;

Matrix addMatrix( Matrix a, Matrix b )
{
	Matrix ret;
	clearMatrix(ret);
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			ret[i][j] = a[i][j] + b[i][j];
	return ret;
}			

Mesh::Mesh(void)
{
	seed = 1992;
	iterTimes = 0;
	totFaces = 0;
	workMode = NoOptimize;
}

Mesh::Mesh( const char* fn )
{
	seed = 1992;
	iterTimes = 0;
	totFaces = 0;
	bool success = LoadFromObj(fn);
	assert(success);
	workMode = NoOptimize;
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
	f.clear();
	relTri.clear();
	eLastUpdate.clear();
	while (!nextEdgeCollapse.empty()) nextEdgeCollapse.pop();

	//Create a new CSimpleObject
	CSimpleObject *simpleObject = new CSimpleObject();
	bool success = simpleObject->LoadFromObj(fn);
	//Copy the Information from CSimpleObject		
	for ( int i=0; i<simpleObject->m_nVertices; ++i )
	{
		v.push_back(simpleObject->m_pVertexList[i]);									//Copy vertexes
	}
	adj.resize(v.size());	
	relTri.resize(v.size());
	vExist = vector<bool>(v.size(), true);
	totFaces = simpleObject->m_nTriangles;
	for ( int i=0; i<simpleObject->m_nTriangles; ++i )
	{					
		Triangle t = Triangle(simpleObject->m_pTriangleList[i]);
		f.push_back(t);																					//Copy faces
		for ( int p1=0; p1<3; ++p1 )																//Construct adjacent list
			for ( int p2=0; p2<3; ++p2 )
				if ( p1!=p2 )
					adj[t.p[p1]].push_back(t.p[p2]);

		for (int p1=0; p1<3; ++p1)																//Construct triangle relevant list
			relTri[t.p[p1]].push_back(i);

		for (int p1=0; p1<3; ++p1)																//Initial edge last update list
			for (int p2=0; p2<3; ++p2)
				if (p1!=p2)
					eLastUpdate[std::make_pair(t.p[p1], t.p[p2])] = iterTimes;
	}
	for (unsigned int i=0; i<v.size(); ++i)													//Sort the lists
	{
		sort(adj[i].begin(), adj[i].end());
		sort(relTri[i].begin(), relTri[i].end());
	}
	vQ.resize(v.size());
	for (unsigned int i=0; i<v.size(); ++i)													//Init Vertex Q
		vQ[i] = Q(i);
	for ( int i=0; i<simpleObject->m_nTriangles; ++i )
	{					
		Triangle t = Triangle(simpleObject->m_pTriangleList[i]);
		for (int p1=0; p1<3; ++p1)																//Push edge collapses into the queue
			for (int p2=p1+1; p2<3; ++p2)
				addEdgeCollapse(t.p[p1], t.p[p2]);
	}
	
	//Release simpleObject
	delete simpleObject;
	//Return the result
	return success;
}

void Mesh::setWorkMode( WorkMode workMode )
{
	this->workMode = workMode;
}

SimpleOBJ::Vec3f Mesh::normal(Triangle f)
{
	return Vec3f::cross((v[f.p[2]]-v[f.p[0]]).Normalize(), (v[f.p[1]]-v[f.p[0]]).Normalize()).Normalize();
}

Matrix Mesh::Q( Triangle tri )
{
	float v4[4];
	Vec3f norm = normal(tri);
	v4[0] = norm._p[0]; 
	v4[1] = norm._p[1];
	v4[2] = norm._p[2];
	v4[3] = -Vec3f::dot(norm, v[tri.p[0]]);
	Matrix ret;
	clearMatrix(ret);
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			ret[i][j] = v4[i]*v4[j];
	return ret;
}

Matrix Mesh::Q( int x )
{
	Matrix ret;
	clearMatrix(ret);
	for (unsigned int i=0; i<relTri[x].size(); ++i )
		if (f[relTri[x][i]].alive)
		{
			Matrix Qi = Q(f[relTri[x][i]]);
			ret = addMatrix(ret, Qi);
		}
	return ret;
}

float Mesh::cost(int v1, int v2, Vec3f &targetV)
{
	float cost = 0;
	if (workMode==Quadratic)
	{
		targetV = (v[v1]+v[v2])/2;
		float V[4];
		V[0] = targetV[0];
		V[1] = targetV[1];
		V[2] = targetV[2];
		V[3] = 1;
		Matrix q = addMatrix(vQ[v1], vQ[v2]);	
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				cost += V[i] * q[i][j] * V[j];
	}
	else if (workMode==QuadraticOpt)
	{
		Matrix q = addMatrix(vQ[v1], vQ[v2]);	
		vector<float> b(4);
		b[0] = b[1] = b[2] = 0; b[3] = 1;
		Matrix qs = q;
		qs[3] = b;
		vector<float> res;		

		if (!LinearEquation_Special::solve(qs, b, res))
			targetV = (v[v1]+v[v2])/2;
		else
			targetV = Vec3f(res[0], res[1], res[2]);

		float V[4];
		V[0] = targetV[0];
		V[1] = targetV[1];
		V[2] = targetV[2];
		V[3] = 1;
		
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				cost += V[i] * q[i][j] * V[j];
	}	
	else
	{
	}
	return cost;
}

void Mesh::addEdgeCollapse( int v1, int v2 )
{
	if (workMode==RandomOptimize)
	{
		EdgeCollapse op;
		op.cost = rand();
		op.p[0] = v1; op.p[1] = v2;
		op.targetV = (v[v1]+v[v2])/2;
		op.createTime = iterTimes;
		nextEdgeCollapse.push(op);
		eLastUpdate[std::make_pair(v1, v2)] = eLastUpdate[std::make_pair(v2, v1)] = iterTimes;
	}
	else if (workMode==Quadratic || workMode==QuadraticOpt)
	{
		EdgeCollapse op;		
		op.cost = cost(v1, v2, op.targetV);
		op.p[0] = v1; op.p[1] = v2;
		op.createTime = iterTimes;
		nextEdgeCollapse.push(op);
		eLastUpdate[std::make_pair(v1, v2)] = eLastUpdate[std::make_pair(v2, v1)] = iterTimes;
	}
	else if (workMode==EdgeLength)
	{
		EdgeCollapse op;
		op.cost = (v[v2]-v[v1]).L2Norm_Sqr();
		op.p[0] = v1; op.p[1] = v2;
		op.targetV = (v[v1]+v[v2])/2;
		op.createTime = iterTimes;
		nextEdgeCollapse.push(op);
		eLastUpdate[std::make_pair(v1, v2)] = eLastUpdate[std::make_pair(v2, v1)] = iterTimes;
	}
	else
	{
	}
}

void Mesh::collapse()
{
	if (workMode==NoOptimize)
		return;
	++iterTimes;
	EdgeCollapse col;	
	while (!nextEdgeCollapse.empty())														//Find an legal edge collapse
	{
		col = nextEdgeCollapse.top();
		nextEdgeCollapse.pop();		
		if (col.createTime < eLastUpdate[std::make_pair(col.p[0], col.p[1])])	//If it is valid?
			continue;
		if (col.p[0]==col.p[1])																					//Prohibit a edge to be collapsed 2 times
			continue;
		if (!vExist[col.p[0]])
			continue;
		if (!vExist[col.p[1]])
			continue;
		break;
	}
	if (nextEdgeCollapse.empty()) return;
	int v1 = col.p[0], v2 = col.p[1];
	lastCollapsed1 = v1; lastCollapsed2 = v2;
	v[v1] = col.targetV; v[v2] = col.targetV;												//Merge V2 to V1, delete all points link to V2
	vExist[v2] = 0;
	for (unsigned int i=0; i<relTri[v1].size(); ++i)									//Update triangles, Delete triangles that degraded to edges
	{
		totFaces -= updateTriangle(f[relTri[v1][i]], v1, v2, v1);
	}
	for (unsigned int i=0; i<relTri[v2].size(); ++i)	
		totFaces -= updateTriangle(f[relTri[v2][i]], v1, v2, v1);	
	for (unsigned int i=0; i<adj[v2].size(); ++i)										//Update adjacent list, delete all links to v2
	{
		int vt = adj[v2][i];
		for (unsigned int j=0; j<adj[vt].size(); ++j)
			if (adj[vt][j]==v2)
				adj[vt][j]=v1;
	}
	int *tmp = new int[adj[v1].size()+adj[v2].size()];								//Merge Adjacent list
	int *last;
	sort(adj[v1].begin(), adj[v1].end());
	sort(adj[v2].begin(), adj[v2].end());
	last = set_union(adj[v1].begin(), adj[v1].end(), adj[v2].begin(), adj[v2].end(), tmp);
	adj[v1] = vector<int>(tmp, last);	
	delete tmp;
			
	tmp = new int[relTri[v1].size()+relTri[v2].size()];							//Merge RelTri list
	sort(relTri[v1].begin(), relTri[v1].end());
	sort(relTri[v2].begin(), relTri[v2].end());
	last = set_union(relTri[v1].begin(), relTri[v1].end(), relTri[v2].begin(), relTri[v2].end(), tmp);
	relTri[v1] = vector<int>(tmp, last);	
	delete tmp;

	vQ[v1] = Q(v1);
	for (unsigned int i=0; i<adj[v1].size(); ++i)										//Update Vertex Q
		if (vExist[adj[v1][i]])
			vQ[adj[v1][i]] = Q(adj[v1][i]);

	for (unsigned int i=0; i<adj[v1].size(); ++i)										//Update Edges (Insert new edge collapses)
		addEdgeCollapse(v1, adj[v1][i]);
	for (unsigned int i=0; i<adj[v2].size(); ++i)
		addEdgeCollapse(v2, adj[v2][i]);
}

int Mesh::updateTriangle( Triangle &tri, int v1, int v2, int v )
{
	if (!tri.alive) return 0;
	bool hit1=0, hit2=0;
	for (int p=0; p<3; ++p)
	{
		if (tri.p[p]==v1)
		{
			hit1 = 1;
			tri.p[p]=v;
		}
		if (tri.p[p]==v2)
		{
			hit2 = 1;
			tri.p[p] = v;
		}
	}
	if (hit1&&hit2) 
	{
		tri.alive = 0;
		return 1;
	}
	return 0;
}

int Mesh::rand()
{
	//return std::rand();
	return seed = (seed*10000007)  + 28374681;
}

bool Mesh::SaveToObj( const char* fn )
{
	CSimpleObject *object = new CSimpleObject;
	//Init vertex
	map<int, int> vMap;
	for ( int i=0; i<v.size(); ++i )
		if (vExist[i])
			vMap[i] = vMap.size();

	object->m_nVertices = vMap.size();
	object->m_pVertexList = new Vec3f[vMap.size()];
	for ( int i=0; i<v.size(); ++i )
		if (vExist[i])
			object->m_pVertexList[vMap[i]] = Vec3f(v[i].x, v[i].y, v[i].z);

	//Init Faces
	object->m_nTriangles = totFaces;
	object->m_pTriangleList = new Array<int,3>[totFaces];
	int fCnt = 0;
	for (int i=0; i<f.size(); ++i)
		if (f[i].alive)
		{
			for (int p=0; p<3; ++p)
				object->m_pTriangleList[fCnt][p] = vMap[f[i].p[p]];
			++fCnt;
		}
	//Save
	bool success = object->SaveToObj(fn);
	delete object;
	return success;
}

