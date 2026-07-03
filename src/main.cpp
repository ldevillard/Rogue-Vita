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
	
	sceKernelDelayThread(3*1000000); // Wait for 3 seconds
	sceKernelExitProcess(0);
    return 0;
}