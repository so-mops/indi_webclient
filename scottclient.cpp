#if 0
Simple Client Tutorial
Demonstration of libindi v0.7 capabilities.

Copyright (C) 2010 Jasem Mutlaq (mutlaqja@ikarustech.com)

This library is free software;
you can redistribute it and / or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY;
without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library;
if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301  USA

#endif

/** \file tutorial_client.cpp
    \brief Construct a basic INDI client that demonstrates INDI::Client capabilities. This client must be used with tutorial_three device "Simple CCD".
    \author Jasem Mutlaq

    \example tutorial_client.cpp
    Construct a basic INDI client that demonstrates INDI::Client capabilities. This client must be used with tutorial_three device "Simple CCD".
    To run the example, you must first run tutorial_three:
    \code indiserver tutorial_three \endcode
    Then in another terminal, run the client:
    \code tutorial_client \endcode
    The client will connect to the CCD driver and attempts to change the CCD temperature.
*/

#include "tutorial_client.h"

#include "basedevice.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <uWS/uWS.h> 
#include <thread>
#include "/home/scott/git-clones/json/src/json.hpp"
using json = nlohmann::json;
#define MYCCD "Simple CCD"


/* Our client auto pointer */
std::unique_ptr<MyClient> camera_client(new MyClient());





void WSthread()
{                                                                                                      
    uWS::Hub h;
                                                                                                       
    h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		std::cout << "We got one!" << std::endl;
        ws->send(message, length, opCode);                                                             
    });                                                                                                
    h.listen(3000);                                                                                    
    h.run();                                                                                           
}                                                                                                      


/**************************************************************************************
**
***************************************************************************************/
MyClient::MyClient()
{
    ccd_simulator = nullptr;
}

/**************************************************************************************
**
***************************************************************************************/
MyClient::~MyClient()
{
}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::setTemperature()
{
    INumberVectorProperty *ccd_temperature = nullptr;

    ccd_temperature = ccd_simulator->getNumber("CCD_TEMPERATURE");

    if (ccd_temperature == nullptr)
    {
        IDLog("Error: unable to find CCD Simulator CCD_TEMPERATURE property...\n");
        return;
    }

    ccd_temperature->np[0].value = -20;
    sendNewNumber(ccd_temperature);
}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::takeExposure()
{
    INumberVectorProperty *ccd_exposure = nullptr;

    ccd_exposure = ccd_simulator->getNumber("CCD_EXPOSURE");

    if (ccd_exposure == nullptr)
    {
        IDLog("Error: unable to find CCD Simulator CCD_EXPOSURE property...\n");
        return;
    }

    // Take a 1 second exposure
    IDLog("Taking a 1 second exposure.\n");
    ccd_exposure->np[0].value = 1;
    sendNewNumber(ccd_exposure);
}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::newDevice(INDI::BaseDevice *dp)
{
    if (!strcmp(dp->getDeviceName(), MYCCD))
        IDLog("Receiving %s Device...\n", dp->getDeviceName());
	//std::cout << "The new device is " << dp->getDeviceName() << std::endl;
	
	watchDevice( dp->getDeviceName() );
    ccd_simulator = dp;
}

/**************************************************************************************
**
*************************************************************************************/
void MyClient::newProperty(INDI::Property *property)
{

	
	//std::cout << "New property name  "<< property->getName() << std::endl;
	
    //connectDevice(property->getDeviceName());
	//watchDevice( property->getDeviceName() );
    if ( !strcmp(property->getName(), "CONNECTION") )
    {
        connectDevice(property->getDeviceName());
        return;
    }

    if (!strcmp(property->getDeviceName(), MYCCD) && !strcmp(property->getName(), "CCD_TEMPERATURE"))
    {
        if (ccd_simulator->isConnected())
        {
            IDLog("CCD is connected. Setting temperature to -20 C.\n");
            setTemperature();
        }
        return;
    }
}


void MyClient::newSwitch( ISwitchVectorProperty *svp )
{
	std::cout << svp->name << std::endl;

}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::newNumber(INumberVectorProperty *nvp)
{
    // Let's check if we get any new values for CCD_TEMPERATURE
	
        //IDLog("Receving new CCD Temperature: %g C\n", nvp->np[1].value);
		json jnvp;
		json jnp;
		jnvp["name"] = nvp->name;
		jnvp["label"] = nvp->label;
		jnvp["device"] = nvp->device;
		jnvp["group"] = nvp->group;
		jnvp["np"] = jnp;
			
		INumber *np;
		for(int ii=0; ii<nvp->nnp; ii++)
		{
			np = nvp->np+ii;
			jnvp["np"][ii]["name"] = np->name;
			jnvp["np"][ii]["value"] = np->value;
			
			jnvp["np"][ii]["format"] = np->format;
			jnvp["np"][ii]["min"] = np->min;
			jnvp["np"][ii]["step"] = np->step;
			jnvp["np"][ii]["label"] = np->label;
			
			jnvp["np"][ii]["max"] = np->max;
			
		}
		
		std::cout << jnvp.dump(4) << std::endl;

}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::newMessage(INDI::BaseDevice *dp, int messageID)
{
    if (strcmp(dp->getDeviceName(), MYCCD))
        return;

    IDLog("Recveing message from Server:\n\n########################\n%s\n########################\n\n",
          dp->messageQueue(messageID).c_str());
}

/**************************************************************************************
**
***************************************************************************************/
void MyClient::newBLOB(IBLOB *bp)
{
    // Save FITS file to disk
    std::ofstream myfile;

    myfile.open("ccd_simulator.fits", std::ios::out | std::ios::binary);

    myfile.write(static_cast<char *>(bp->blob), bp->bloblen);

    myfile.close();

    IDLog("Received image, saved as ccd_simulator.fits\n");
}


int main(int /*argc*/, char ** /*argv*/)

{
    camera_client->setServer("localhost", 7624);
	
    camera_client->connectServer();
	std::vector< INDI::BaseDevice * >  devs;
	camera_client->getDevices(devs, INDI::BaseDevice::GENERAL_INTERFACE );
	std::cout << "the size is "  << devs.size() << std::endl;
	for(int i=0; i<devs.size(); i++ )
	{
		std::cout << "THe dev name is " << devs[i]->getDeviceName() << std::endl;
	}
    //camera_client->watchDevice(MYCCD);


    //camera_client->setBLOBMode(B_ALSO, MYCCD, nullptr);
	std::thread t1(WSthread);
    std::cout << "Press any key to terminate the client.\n";
    std::string term;
    std::cin >> term;
	t1.join();
}
