/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing The Base Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "Mesh.h"
#include <string>
using namespace SimpleOBJ;
using namespace std;

Mesh *mesh;

vector<string> Split(const string& s,
	const string& match,
	bool removeEmpty=false,
	bool fullMatch=false);

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{	
	int nCmdLineLength = strlen(lpCmdLine);
	string cmdLine = string(lpCmdLine, lpCmdLine+nCmdLineLength);
	if (cmdLine.empty())
	{
		printf("Usage: \n");
		printf("MeshOptimization animate input                        Create an animation to compare the algorithms. \n");
		printf("MeshOptimization show mode input percentage           Reduce input model to percentage, show the image.\n");
		printf("MeshOptimization model mode input output percentage   Output the reduced model to output.\n\n");
		printf("Mode Parameters:\n");
		printf("        l        use edge length as heuristics. v=(v1+v2)/2.\n");
		printf("        q     use the quadratic cost function as heuristics. v=(v1+v2)/2.\n");
		printf("        qopt     use the quadratic cost function as heuristics. v is optimal.\n\n");
		printf("Examples:\n");
		printf("MeshOptimization animate sphere.obj\n");
		printf("MeshOptimization show \n");
		printf("MeshOptimization model mode input output percentage   Output the reduced model to output.\n\n");
		return 0;
	}
	vector<string> cmd = Split(cmdLine, " ");

		mesh = new Mesh();
		if (cmd[0]=="l")
			mesh->setWorkMode(Mesh::EdgeLength);
		else if (cmd[0]=="q")
			mesh->setWorkMode(Mesh::Quadratic);
		else
			mesh->setWorkMode(Mesh::QuadraticOpt);
		mesh->LoadFromObj(cmd[1].c_str());

		float p = float(atoi(cmd[3].c_str())) / 100;
		int targetFaces = mesh->totFaces*p;

		while (mesh->totFaces > targetFaces)
		{
			mesh->collapse();
		}

		mesh->SaveToObj(cmd[2].c_str());
		MessageBox(0, "Finished.", "Notice", 0);
		return 0;							// Exit The Program
}
	typedef string::size_type (string::*find_t)(const string& delim, 
		string::size_type offset) const;
	/// <summary>
	/// Splits the string s on the given delimiter(s) and
	/// returns a list of tokens without the delimiter(s)
	/// </summary>
	/// <param name=s>The string being split</param>
	/// <param name=match>The delimiter(s) for splitting</param>
	/// <param name=removeEmpty>Removes empty tokens from the list</param>
	/// <param name=fullMatch>
	/// True if the whole match string is a match, false
	/// if any character in the match string is a match
	/// </param>
	/// <returns>A list of tokens</returns>
	vector<string> Split(const string& s,
		const string& match,
		bool removeEmpty,
		bool fullMatch)
	{
		vector<string> result;                 // return container for tokens
		string::size_type start = 0,           // starting position for searches
			skip = 1;            // positions to skip after a match
		find_t pfind = &string::find_first_of; // search algorithm for matches
		if (fullMatch)
		{
			// use the whole match string as a key
			// instead of individual characters
			// skip might be 0. see search loop comments
			skip = match.length();
			pfind = &string::find;
		}
		while (start != string::npos)
		{
			// get a complete range [start..end)
			string::size_type end = (s.*pfind)(match, start);
			// null strings always match in string::find, but
			// a skip of 0 causes infinite loops. pretend that
			// no tokens were found and extract the whole string
			if (skip == 0) end = string::npos;
			string token = s.substr(start, end - start);
			if (!(removeEmpty && token.empty()))
			{
				// extract the token and add it to the result list
				result.push_back(token);
			}
			// start the next range
			if ((start = end) != string::npos) start += skip;
		}		
		return result;
	}
	