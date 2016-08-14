//******************************************************************************
//render/AppGLFW.h**************************************************************
//******************************************************************************

#pragma once

//Render Library Includes
#include <render/math.h>
#include <render/time.h>
#include <render/gl.h>

//Standard Library Includes
#include <iostream>
#include <vector>

//GLFW Include
#include <GLFW/glfw3.h>

//Parent Include
#include "AppBase.h"

//Namespaces
using namespace std;

namespace render
{

class AppGLFW : public AppBase
{
private:
	GLFWwindow *window;

public:
	//Constructor
	AppGLFW(const string &title, int w, int h, bool resizable = false);
	~AppGLFW();

protected:

	void system_init(const char *, unsigned int, unsigned int);
	void system_poll();
	void system_swap();
	void system_quit();
	void system_title(const string &title);
	void system_update();

	bool keyDown(Key key) const;
	bool keyUp(Key key) const;
	bool keyPressed(Key key) const;
	bool keyReleased(Key key) const;

	bool mouseDown(MouseButton button) const;
	bool mouseUp(MouseButton button) const;
	bool mousePressed(MouseButton button) const;
	bool mouseReleased(MouseButton button) const;
};
//End class AppGLFW

};
//End namespace render
