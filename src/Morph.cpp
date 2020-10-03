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

#include <sensosc/Morph.hpp>


namespace sensosc
{
	static const char* CONTACT_STATE_STRING[] = { "CONTACT_INVALID","CONTACT_START", "CONTACT_MOVE", "CONTACT_END" };

	Morph::Morph(){
		open();
		init();
	}
	int Morph::open(){
		SenselDeviceList list;
		//Get a list of avaialble Sensel devices
		senselGetDeviceList(&list);
		if (list.num_devices == 0) {
			fprintf(stdout, "No device found\n");
			fprintf(stdout, "Press Enter to exit example\n");
			getchar();
			return 0;
		}
	
		senselOpenDeviceByID(&handle, list.devices[0].idx);
		return 1;
	}
	void Morph::init(){
		//Open a Sensel device by the id in the SenselDeviceList, handle initialized 

		senselGetMaxLEDBrightness(handle, &maxLEDBrightness);
		senselGetNumAvailableLEDs(handle, &numLEDs);

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
 
	    // set maximum scanning framerate 
		// max 2000Hz
	    // For higher frame rates, however,
	    // actual frame rate dependents on Scan Detail:
	    // Scan Detail == medium >> max. 250fps
	    // Scan Detail == low >> max. 1000fps
		senselSetMaxFrameRate(handle, 2000); // max rate

		// set scan detail
		setScanDetail(2);

		// set if frames are sent synchronously (default) or asynchronously
		// SCAN_MODE_DISABLE,
    	// SCAN_MODE_SYNC,
	    // SCAN_MODE_ASYNC,
		// senselSetScanMode(handle, SCAN_MODE_ASYNC);
	}; // end init()


	void Morph::setScanDetail(int level) {
		switch(level) {
			case 0  :
				senselSetScanDetail(handle, SCAN_DETAIL_LOW); // min detail
		    	break; //optional
			case 1  :
				senselSetScanDetail(handle, SCAN_DETAIL_MEDIUM); // min detail
		    	break; //optional
			case 2  :
				senselSetScanDetail(handle, SCAN_DETAIL_HIGH); // min detail
		    	break; //optional
			default : //Optional
				senselSetScanDetail(handle, SCAN_DETAIL_HIGH); // min detail
		}

				// senselSetScanDetail(handle, SCAN_DETAIL_HIGH); // max detail
		senselSetScanDetail(handle, SCAN_DETAIL_MEDIUM); // medium detail
		// senselSetScanDetail(handle, SCAN_DETAIL_LOW); // min detail

	}

	// ~Morph(){
	// 	handle = NULL;
	// };
	int Morph::getNumFrames() {
		//Read all available data from the Sensel device
		senselReadSensor(handle);
		//Get number of frames available in the data read from the sensor
		senselGetNumAvailableFrames(handle, &num_frames);
		return num_frames;
	}
	SenselFrameData* Morph::getFrame(){
		senselGetFrame(handle, frame);
		return frame;
	} 
	void Morph::setLED(int idx, float brightness){
		senselSetLEDBrightness(handle, idx % numLEDs, (int) (brightness * maxLEDBrightness));
	}


	void Morph::printAllVals() {
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
	
}