#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <cmath>

#include "sensel.h"
#include "sensel_device.h"

#define MAX_CONTACTS 16

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
		~Morph();
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

		// calculate additional states
		void calcState();

		// additional information, calculated in calcState()
		float x_center_of_mass = .0;
		float y_center_of_mass = .0;
		float average_force = .0;
		float average_area = .0;

		float average_distance = .0;

		// array of distances between contacts and CoM
		// array has same order as frame->contacts and as many valid entries
		float *distances_to_center_of_mass = NULL;


		// don't use me if you want to do anything else than printing
		void printAllVals();
	};
}
