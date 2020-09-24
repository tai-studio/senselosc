#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

// OSC

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define OSC_OUT_ADDRESS "127.0.0.1"
#define OSC_OUT_PORT 7000
#define OSC_OUTPUT_BUFFER_SIZE 8192

// Sensel Morph
#include <sensosc/Morph.hpp>


// interface handling
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


int main(int argc, char* argv[])
{
    (void) argc; // suppress unused parameter warnings
    (void) argv; // suppress unused parameter warnings


    // OSC
    UdpTransmitSocket transmitSocket( IpEndpointName( OSC_OUT_ADDRESS, OSC_OUT_PORT ) );
    char buffer[OSC_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream packet( buffer, OSC_OUTPUT_BUFFER_SIZE );
    

    // Sensel Morph

    sensosc::Morph morph;

    fprintf(stdout, "Press Enter to exit\n");
    #ifdef WIN32
        HANDLE thread = CreateThread(NULL, 0, waitForEnter, NULL, 0, NULL);
    #else
        pthread_t thread;
        pthread_create(&thread, NULL, &waitForEnter, NULL);
    #endif

    while (!enter_pressed) {
        int numFrames = morph.getNumFrames();
        // fprintf(stdout, "%d\n", numFrames);
        // morph.printAllVals();
        for (int i = 0; i < numFrames; ++i) {
            SenselFrameData *frame = morph.getFrame();
            if (frame->n_contacts > 0) {
                packet << osc::BeginBundleImmediate;
                for (int c = 0; c < frame->n_contacts; c++) {
                    SenselContact contact = frame->contacts[c];
                    unsigned int state = contact.state;

                    // see sensel.h for available values
                    packet << osc::BeginMessage( "/morph" )
                        << (int) morph.index
                        << (int) contact.id
                        << (int) contact.state
                        << (float) contact.x_pos 
                        << (float) contact.y_pos
                        << (float) contact.total_force
                        << (int) contact.area
                        << (float) contact.orientation // % 360, // strange +- values occur, see http://guide.sensel.com/api/#ellipse
                        << (float) contact.major_axis
                        << (float) contact.minor_axis
                        << osc::EndMessage;

                    packet << osc::BeginMessage( "/morphDelta" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.delta_x
                        << (float) contact.delta_y
                        << (float) contact.delta_force
                        << (int) contact.delta_area
                        << osc::EndMessage;

                    packet << osc::BeginMessage( "/morphBB" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.min_x
                        << (float) contact.min_y
                        << (float) contact.max_x
                        << (float) contact.max_y
                        << osc::EndMessage;

                    packet << osc::BeginMessage( "/morphPeak" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.peak_x
                        << (float) contact.peak_y
                        << (float) contact.peak_force
                        << osc::EndMessage;

                } // rof
                packet << osc::EndBundle;
                transmitSocket.Send( packet.Data(), packet.Size() );
                packet.Clear();
            } // end if
        } // rof
    } // end while
    return 0;
}

