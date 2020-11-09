#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iterator>

#ifdef WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

// cmd-line options
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <fmt/format.h>

// TODO
// add time delta to messages
// #include <chrono>
// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
// std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

// using namespace std;

// OSC

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#define OSC_OUT_ADDRESS "127.0.0.1"
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
    int osc_outPort = 7000;
    bool notBundled = false;
    int scanDetail = 2;

    try {
        po::options_description desc("options");
        desc.add_options()
            ("help,h", "this help message")
            ("nobundle,x", "send messages unbundled")
            ("scandetail,d", po::value<int>(), 
                "set scanning detail (0: low, 1:med, 2:high), defaults to high"
            )
            ("port,p", po::value<int>(), 
                fmt::format("set output port number (default: {})", osc_outPort).c_str()
            )
        ;


        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout << "Usage: senselosc [options]\n";
            std::cout << desc << "\n";
            std::cout << R"(OSC messages sent:
    /contactAvg   <index> <num_contacts> <x> <y> <wx> <wy> <force> <area> <distance>
    /contact      <index> <id> <state> <x> <y> <force> <area> <distance> <orient> <major_axis> <minor_axis>
    /contactDelta <index> <id> <d_x> <d_y> <d_force> <d_area>
    /contactBB    <index> <id> <min_x> <min_y> <max_x> <max_y>
    /contactPeak  <index> <id> <peak_x> <peak_y> <peak_force>
    /sync         <index> <updated_0> ... <updated_15>)" 
            << "\n" 
            << "see README.md for details."
            << "\n\n" 
            << "http://tai-studio.org"
            << "\n";
            return 0;
        }

        if (vm.count("nobundle")) {
            notBundled = true;
        }

        if (vm.count("port")) {
            osc_outPort = vm["port"].as<int>();
            std::cout << "port set to " 
                 << osc_outPort << ".\n";
        } else {
            std::cout << "using default port " << osc_outPort << ".\n";
        }
        if (vm.count("scandetail")) {
            scanDetail = vm["scandetail"].as<int>();
            std::cout << "scanning detail set to " 
                 << scanDetail << ".\n";
        } else {
            std::cout << "using default scanning detail " << scanDetail << ".\n";
        }
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

    // OSC
    UdpTransmitSocket transmitSocket( IpEndpointName( OSC_OUT_ADDRESS, osc_outPort ) );
    char buffer[OSC_OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream packet( buffer, OSC_OUTPUT_BUFFER_SIZE );
    

    // Sensel Morph

    sensosc::Morph morph;
    morph.setScanDetail(scanDetail);

    fprintf(stdout, "Press Enter to exit\n");
    #ifdef WIN32
        HANDLE thread = CreateThread(NULL, 0, waitForEnter, NULL, 0, NULL);
    #else
        pthread_t thread;
        pthread_create(&thread, NULL, &waitForEnter, NULL);
    #endif

    bool lastFrameValid = false;
    while (!enter_pressed) {
        int numFrames = morph.getNumFrames();
        int contact_updated[MAX_CONTACTS] = {};

        // fprintf(stdout, "%d\n", numFrames);
        // morph.printAllVals();
        for (int i = 0; i < numFrames; ++i) {
            SenselFrameData *frame = morph.getFrame();
            if (frame->n_contacts > 0) {
                lastFrameValid = true;
                if (!notBundled) {
                    packet << osc::BeginBundleImmediate;
                }

                packet << osc::BeginMessage( "/contactAvg" )
                    << (int) morph.index
                    << (int) frame->n_contacts
                    << (float) morph.x_com
                    << (float) morph.y_com
                    << (float) morph.average_force
                    << (float) morph.average_distance // to CoM
                    << (float) morph.average_area
                    << (float) morph.x_wcom
                    << (float) morph.y_wcom
                    << (float) morph.total_force
                    << (float) morph.average_wdistance // to weighted CoM
                    << osc::EndMessage;
                if (notBundled) {
                    transmitSocket.Send( packet.Data(), packet.Size() );
                    packet.Clear();
                }

                for (int c = 0; c < frame->n_contacts; c++) {
                    SenselContact contact = frame->contacts[c];
                    unsigned int state = contact.state;

                    // set to 1 if updated, 0 otherwise
                    contact_updated[contact.id] = 1;

                    // see sensel.h for available values
                    packet << osc::BeginMessage( "/contact" )
                        << (int) morph.index
                        << (int) contact.id
                        << (int) contact.state
                        << (float) contact.x_pos 
                        << (float) contact.y_pos
                        << (float) contact.total_force
                        << (int) contact.area
                        << (float) morph.distances_to_com[c]
                        << (float) morph.distances_to_wcom[c]
                        << (float) contact.orientation // % 360, // strange +- values occur, see http://guide.sensel.com/api/#ellipse
                        << (float) contact.major_axis
                        << (float) contact.minor_axis
                        << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    }

                    packet << osc::BeginMessage( "/contactDelta" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.delta_x
                        << (float) contact.delta_y
                        << (float) contact.delta_force
                        << (int) contact.delta_area
                        << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    }

                    packet << osc::BeginMessage( "/contactBB" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.min_x
                        << (float) contact.min_y
                        << (float) contact.max_x
                        << (float) contact.max_y
                        << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    }

                    packet << osc::BeginMessage( "/contactPeak" )
                        << (int) morph.index
                        << (int) contact.id
                        << (float) contact.peak_x
                        << (float) contact.peak_y
                        << (float) contact.peak_force
                        << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    }

                } // rof contacts

                packet << osc::BeginMessage( "/sync" )
                    << (int) morph.index;
                    for (int i = 0; i < MAX_CONTACTS; ++i) {
                        packet << (int) contact_updated[i];
                    }
                    packet << osc::EndMessage;

                if (notBundled) {
                    transmitSocket.Send( packet.Data(), packet.Size() );
                    packet.Clear();
                } else { // send bundle
                    packet << osc::EndBundle;
                    transmitSocket.Send( packet.Data(), packet.Size() );
                    packet.Clear();
                } 
            } else {
                if (lastFrameValid) {
                    lastFrameValid = false;

                    if (!notBundled) {
                        packet << osc::BeginBundleImmediate;
                    }

                    packet << osc::BeginMessage( "/contactAvg" )
                        << (int) morph.index
                        << (int)   0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << (float) .0
                        << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    }

                    packet << osc::BeginMessage( "/sync" )
                        << (int) morph.index;
                    for (int i = 0; i < MAX_CONTACTS; ++i) {
                        packet << (int) contact_updated[i];
                    }
                    packet << osc::EndMessage;
                    if (notBundled) {
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    } else { // send bundle
                        packet << osc::EndBundle;
                        transmitSocket.Send( packet.Data(), packet.Size() );
                        packet.Clear();
                    } 
                }
            } // end if
        } // rof
    } // end while
    return 0;
}

