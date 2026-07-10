#include <vitaGL.h>

int main(int argc, char *argv[]) 
{
	// graphic device init
	vglInit(0x800000);

	// vsync
	vglWaitVblankStart(GL_TRUE);

	// blue screen
	glClearColor(0.118f, 0.353f, 0.706f, 1.0f);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		vglSwapBuffers(GL_FALSE);
	}

    return 0;
}