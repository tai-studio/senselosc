#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

#include <sensosc/Morph.hpp>

static bool enter_pressed = false;


#ifdef WIN32
DWORD WINAPI waitForEnter()
#else
void * waitForEnter(void * arg)
#endif
{
    getchar();
    enter_pressed = true;
    return 0;
}


int main(int argc, char const *argv[])
{
	sensosc::Morph sensel;

	fprintf(stdout, "class-based test.\n");

	fprintf(stdout, "Press Enter to exit example\n");
    #ifdef WIN32
        HANDLE thread = CreateThread(NULL, 0, waitForEnter, NULL, 0, NULL);
    #else
        pthread_t thread;
        pthread_create(&thread, NULL, &waitForEnter, NULL);
    #endif
    
    while (!enter_pressed) {
		sensel.getNumFrames();
		sensel.printAllVals();
	}
	return 0;
}