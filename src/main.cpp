#include <psp2/kernel/processmgr.h>
#include <sstream>
#include <vector>

#include <cstdio>

#include <common/debugScreen.h>

#define printf psvDebugScreenPrintf

int main(int argc, char *argv[]) 
{
	psvDebugScreenInit();
	psvDebugScreenPrintf("Hello World!");
	
	sceKernelDelayThread(5 * 1000000); // Wait for 5 seconds
	sceKernelExitProcess(0);
    return 0;
}