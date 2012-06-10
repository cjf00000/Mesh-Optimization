#pragma once

#include <Windows.h>

class Mesh;

//Display a mesh with OpenGL
class Display
{
public:
	Display(int width, int height, HINSTANCE hInstance);
	~Display(void);
	BOOL CreateGLWindow(char* title, int width, int height);
	void KillGLWindow(void);	

private:
	int width, height;
	Mesh* mesh;

	HDC			hDC;		// Private GDI Device Context
	HGLRC		hRC;		// Permanent Rendering Context
	HWND		hWnd;		// Holds Our Window Handle
	HINSTANCE hInstance;

	int InitGL(void)	;
	int DrawGLScene(void);	
	void ReSizeGLScene(int width, int height);
};

