/******************************************************************************************
* MIT License
*
* Copyright (c) 2013-2017 Sensel, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif
#include "sensel.h"
#include "sensel_device.h"

static const char* CONTACT_STATE_STRING[] = { "CONTACT_INVALID","CONTACT_START", "CONTACT_MOVE", "CONTACT_END" };
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

int main(int argc, char **argv)
{
	//Handle that references a Sensel device
	SENSEL_HANDLE handle = NULL;
	//List of all available Sensel devices
	SenselDeviceList list;
	//SenselFrame data that will hold the contacts
	SenselFrameData *frame = NULL;

	//Get a list of avaialble Sensel devices
	senselGetDeviceList(&list);
	if (list.num_devices == 0)
	{
		fprintf(stdout, "No device found\n");
		fprintf(stdout, "Press Enter to exit example\n");
		getchar();
		return 0;
	}

	//Open a Sensel device by the id in the SenselDeviceList, handle initialized 
	senselOpenDeviceByID(&handle, list.devices[0].idx);

	// Set the frame content
	senselSetFrameContent(handle, 
		0
		// | FRAME_CONTENT_PRESSURE_MASK // pressure values for all sensels 
		// | FRAME_CONTENT_LABELS_MASK // labels for all sensels
		| FRAME_CONTENT_CONTACTS_MASK 
		// | FRAME_CONTENT_ACCEL_MASK // accelerometer of the device
	);
    // Set the contact information reported by the sensor 
    senselSetContactsMask(handle, 
    	0
    	| CONTACT_MASK_ELLIPSE 
    	| CONTACT_MASK_DELTAS 
    	| CONTACT_MASK_BOUNDING_BOX 
    	| CONTACT_MASK_PEAK
    );

	// Set the number of frame buffers the device should store internaly.
	// default: disabled
	// max: 50
    // senselSetBufferControl(handle, 1);


	// Set contact blob merging setting (default on)
    // senselSetContactsEnableBlobMerge(handle, 0); // no merge

	// Set minimum force a contact needs to have to be reported
	// default 160 = 20g * 8
    senselSetContactsMinForce(handle, 0);

	// Allocate a frame of data, must be done before reading frame data
	senselAllocateFrameData(handle, &frame);

	//Start scanning the Sensel device
    senselStartScanning(handle);

    // Disable dynamic baselining (default on), i.e. removes unintended forces 
    // over time in order to prevent spurious contacts
	// senselSetDynamicBaselineEnabled(handle, 1);

    // Update the brightness of one LED
	// senselSetLEDBrightness(handle, )
	// senselGetLEDBrightness(handle, )
	// senselGetMaxLEDBrightness(handle)
    // senselGetNumAvailableLEDs(handle)


    // set maximum scanning framerate 
	// max 2000Hz
    // For higher frame rates, however,
    // actual frame rate dependents on Scan Detail:
    // Scan Detail == medium >> max. 250fps
    // Scan Detail == low >> max. 1000fps
	senselSetMaxFrameRate(handle, 500); // > SC control rate

	// set scan detail
	// High Detail (0), Medium Detail (1), Low Detail (2)
	senselSetScanDetail(handle, SCAN_DETAIL_HIGH); // max detail
	// senselSetScanDetail(handle, 1); // medium detail
	// senselSetScanDetail(handle, 2); // min detail

	// set if frames are sent synchronously (default) or asynchronously
	// SCAN_MODE_DISABLE,
    // SCAN_MODE_SYNC,
    // SCAN_MODE_ASYNC,
	// senselSetScanMode(handle, SCAN_MODE_ASYNC);



    fprintf(stdout, "Press Enter to exit example\n");
    #ifdef WIN32
        HANDLE thread = CreateThread(NULL, 0, waitForEnter, NULL, 0, NULL);
    #else
        pthread_t thread;
        pthread_create(&thread, NULL, &waitForEnter, NULL);
    #endif
    
    while (!enter_pressed)
    {
		unsigned int num_frames = 0;
		//Read all available data from the Sensel device
		senselReadSensor(handle);
		//Get number of frames available in the data read from the sensor
		senselGetNumAvailableFrames(handle, &num_frames);
		for (int f = 0; f < num_frames; f++)
		{
			//Read one frame of data
			senselGetFrame(handle, frame);

			// get data
			if (frame->n_contacts > 0) {
				fprintf(stdout, "Num Contacts: %d\n", frame->n_contacts);
				for (int c = 0; c < frame->n_contacts; c++)
				{
					SenselContact contact = frame->contacts[c];
					unsigned int state = contact.state;




    // unsigned char        id;                 // Contact id
    // unsigned int         state;              // Contact state (enum SenselContactState)
    // float                x_pos;              // X position in mm (0..240), upper left is minimum
    // float                y_pos;              // Y position in mm (0..139)
    // float                total_force;        // Total contact force in grams (0..8192)
    // float                area;               // Area in sensor elements (0..33360)

    // // CONTACT_MASK_ELLIPSE
    // float                orientation;        // Angle in degrees
    // float                major_axis;         // Length of the major axis in mm
    // float                minor_axis;         // Length of the minor axis in mm

    // // CONTACT_MASK_DELTAS
    // float                delta_x;            // X contact displacement in mm
    // float                delta_y;            // Y contact displacement in mm
    // float                delta_force;        // Force delta in grams
    // float                delta_area;         // Area delta in sensor elements

    // // CONTACT_MASK_BOUNDING_BOX
    // float                min_x;              // Bounding box min X coordinate in mm
    // float                min_y;              // Bounding box min Y coordinate in mm
    // float                max_x;              // Bounding box max X coordinate in mm
    // float                max_y;              // Bounding box max Y coordinate in mm

    // // CONTACT_MASK_PEAK
    // float                peak_x;             // X position of the peak in mm
    // float                peak_y;             // Y position of the peak in mm
    // float                peak_force;         // Peak force in grams


					// TODO: compile osc message
					fprintf(stdout, "Contact ID: %d %f %f %f %f State: %s\n", 
						contact.id, 
						contact.x_pos, contact.y_pos,
						contact.orientation,// % 360, // strange +- values occur, see http://guide.sensel.com/api/#ellipse
						contact.area, 
						CONTACT_STATE_STRING[state]
					);

					//Turn on LED for CONTACT_START
					if (state == CONTACT_START) {
						senselSetLEDBrightness(handle, contact.id, 100);
					}
					//Turn off LED for CONTACT_END
					else if (state == CONTACT_END) {
						senselSetLEDBrightness(handle, contact.id, 0);
					}
				}
				fprintf(stdout, "\n");
			}
		}
	}
	return 0;
}
