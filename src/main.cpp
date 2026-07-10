#include <vitaGL.h>

int main(int argc, char *argv[]) 
{
	// graphic device init
	vglInit(0x800000);

	// vsync
	vglWaitVblankStart(GL_TRUE);

	// blue screen with vita res
	glViewport(0, 0, 960, 544);
	glClearColor(0.118f, 0.353f, 0.706f, 1.0f);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// hello triangle
		{
			glBegin(GL_TRIANGLES);
			
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(-0.6f, -0.5f);
			
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex2f(0.6f, -0.5f);
			
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex2f(0.0f, 0.6f);
			
			glEnd();
		}

		vglSwapBuffers(GL_FALSE);
	}

    return 0;
}