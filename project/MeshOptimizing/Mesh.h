#pragma once
#include "simpleobject.h"
#include <vector>
#include <map>
#include <queue>


struct Triangle{
	Triangle(int a, int b, int c){
		alive = true;
		p[0] = a;
		p[1] = b;
		p[2] = c;
	}
	Triangle(SimpleOBJ::Array<int,3> f)
	{
		alive = true;
		p[0] = f[0];
		p[1] = f[1];
		p[2] = f[2];
	}
	int p[3];
	bool alive;
};

///Represents an edge collapse 
///Used for priority queue
struct EdgeCollapse
{
	float cost;
	int p[2];
	SimpleOBJ::Vec3f targetV;
	int createTime;
	///A is prior than b    <=>  A costs litter than B
	friend bool operator < (const EdgeCollapse &a, const EdgeCollapse &b)
	{
		return a.cost > b.cost;
	}
};

typedef std::vector<std::vector<float> > Matrix;
#define clearMatrix(a) a = std::vector<std::vector<float> >(4, std::vector<float>(4,0))
extern Matrix addMatrix(Matrix a, Matrix b);

///Mesh data structure
///
///The mesh is represented by 
///1.	Vertexes
///2. Triangles of vertex index
///3. Vertex adjacent list, for updating the triangle after edge collapse
class Mesh
{	
public:
	enum WorkMode{NoOptimize, RandomOptimize, Quadratic, QuadraticOpt, EdgeLength};

	///Construct an empty mesh
	Mesh(void);
	///Construct from an obj file
	Mesh(const char* fn);
	///!!!This function unimplemented for convenience
	~Mesh(void);

	///Use CSimpleObject to initialize Mesh
	bool LoadFromObj(const char* fn);
	bool SaveToObj(const char* fn);

	///Set work mode: NoOptimize, RandomOptimize, Quadratic
	void setWorkMode(WorkMode workMode);
	///Return normal of f
	SimpleOBJ::Vec3f normal(Triangle f);
	///Collapse one edge
	void collapse();

	int qSize(){ return nextEdgeCollapse.size(); }

public:		
	///Total faces of  the mesh, the size of f is not credible
	int totFaces;
	///How many edge collapses occurred 
	int iterTimes;
	///Vertexes
	std::vector<SimpleOBJ::Vec3f> v;
	///Faces, stores the index of each vertex
	std::vector<Triangle> f;

	int lastCollapsed1, lastCollapsed2;

private:
	Matrix Q(Triangle tri);
	Matrix Q(int v);
	void addEdgeCollapse(int v1, int v2);
	///Cost of --Moving v1,v2 to v-- is a quadratic form vT (Q(v1)+Q(v2)) v
	///targetV is the best move target point
	float cost(int v1, int v2, SimpleOBJ::Vec3f &targetV);
	//Find vertex v's father
	//int findFather(int v);
	///merge v1, v2 to v. Return number of deleted triangles(0 or 1).
	int updateTriangle(Triangle &tri, int v1, int v2, int v);

private:	
	WorkMode workMode;
	///Adjacent list, adj[i] is all vertexes which are adjacent to vertex i
	std::vector<std::vector<int> > adj;
	///Relevant triangle list. A vertex is relevant with a triangle if it is belongs to this triangle
	std::vector<std::vector<int> > relTri;
	///Stores the iterTimes (time stamp)  when edge(v1, v2) is updated.
	///Use to check if an edge in queue is valid
	///If Collapse Item's create time = Edge Update Time, then the item is valid
	std::map<std::pair<int, int>,int> eLastUpdate;
	///Edge priority queue
	std::priority_queue<EdgeCollapse> nextEdgeCollapse;
	///If the vertex still exists
	std::vector<bool> vExist;

	std::vector<Matrix> vQ;

	int rand();
	int seed;
	//Find union set
	//std::vector<int> father;
};