#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>

#include "sensel.h"
#include "sensel_device.h"

namespace sensosc
{
	class Morph
	{
	private:
		SENSEL_HANDLE handle = NULL;
		unsigned int num_frames = 0;
		SenselFrameData *frame = NULL;

		unsigned short maxLEDBrightness = 0;
		unsigned char numLEDs = 0;
	public:

		Morph();
		// ~Morph();
		int open();
		void init();

		// TODO: this is the index of the morph (in case there are several)
		int index = 0;
		// idx wraps around
		// brightness between [0..1]
		void setLED(int idx, float brightness);
		void setScanDetail(int level);

		// how many unprocessed frames?
		int getNumFrames();

		// calling this will pop current state from stack
		SenselFrameData* getFrame();

		// don't use me if you want to do anything else than printing
		void printAllVals();
	};
}
