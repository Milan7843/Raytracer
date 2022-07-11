#pragma once

#include "Application.h"

/* CONTROLS */
/*
 - W to move forward
 - S to move backward
 - A to move left
 - D to move right
 - E to move up
 - Q to move down
 - R to enable/disable settings and info menu (coming soon)
 - V to enable/disable raytraced view
 - Mouse to look around
*/

// Window size
const int WIDTH = 1200, HEIGHT = 700;

int main()
{
	Application application(WIDTH, HEIGHT);
	return application.Start();
}
