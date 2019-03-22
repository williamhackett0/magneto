/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

//#include "pxt.h"

//using namespace pxt;

#include "MicroBit.h"
#include <string>
#include <stdlib.h>
#include <time.h>
#include <list>

//Magneto Variables
#define MAGNETO_PROGRAM_RATE 50

//Direction values
#define LEFT_DIR 0
#define RIGHT_DIR 1
#define TOP_DIR 2
#define BOTTOM_DIR 3

//Frequency rate values
#define PULSE_LENGTH 50
#define PULSE_DELAY_AFTER_SUCESS 5 //Multiplied by pulse lnegth in miliseconds.

//Detection / Capturing of magnetic data
#define SENSITIVITY 800 //Higher the value the stronger the magnetic field change must be.
#define MAGNETIC_FIELD_RATE 10 // The amount of data which should be used for magnetic field averaging.


namespace magneto
{
    //General variables
    MicroBit uBit;
    ManagedString myID;
    
    //Pulsing Variables
    int detectedThisPulse = false;
    bool pulsing = false;
    bool isSomeonePulsing = false;
    bool electromagnetState = false;
    int delayBeforeNextPulse = 0;
    int currentPulse = 0;
    int errorCounter = 0;
    
    //Proximity variables
    bool someoneIsNearMe = false;
    string currentPuslingDevice = "";
    
    //================================================= Classes / Structs =================================================
    
    /*
     * Holds x y z data of the magnetometer
     */
    class Magnetometer
    {
    public:
        int x;
        int y;
        int z;
    };
    
    //Magnetic Field
    Magnetometer averageField;
    
    /*
     * Holds the radio response data (Command, ID, Message) etc
     */
    class RadioResponse
    {
    public:
        string command;
        string ID;
        string message;
    };
    
    //Radio Response
    RadioResponse radioResponse;
    
    /*
     * Holds proximity data
     */
    class Proximity
    {
    public:
        bool operator == (const Proximity& p) const { return ID == p.ID; };
        bool operator != (const Proximity& p) const { return !operator==(p); };
        string ID;
        int strength;
        int direction;
    };
    
    //Devices near me
    list<Proximity> devicesNearMe;
    
    //================================================= Debug Code =================================================
    
    /*
     * For debugging what devices are near the Micro:bit
     */
    void PrintNearList()
    {
        list<Proximity> :: iterator it;
        uBit.serial.printf("Devices: ");
        for(it = devicesNearMe.begin(); it != devicesNearMe.end(); ++it)
        {
            Proximity device = *it;
            uBit.serial.printf("%s {%i}, ", device.ID.c_str(), device.direction);
        }
        uBit.serial.printf("\r\n");
    }
    
    void EnableDisplayLed(int x, int y, bool state)
    {
        if(state)
        {
            uBit.display.image.setPixelValue(x,y,255);
        }
        else
        {
            uBit.display.image.setPixelValue(x,y,0);
        }
    }
    
    void LeftIndicator(bool state)
    {
        EnableDisplayLed(0,2, state);
    }
    
    void RightIndicator(bool state)
    {
        EnableDisplayLed(4,2, state);
    }
    
    void TopIndicator(bool state)
    {
        EnableDisplayLed(2,0, state);
    }
    
    void BottomIndicator(bool state)
    {
        EnableDisplayLed(2,4, state);
    }
    
    void MiddleIndicator(bool state)
    {
        EnableDisplayLed(2,2, state);
    }
    
    void EnablePinOut(bool state)
    {
        uBit.io.P2.setDigitalValue(!state);
    }
    
    //%
    void setMicrobitName(string name)
    {
        myID = name.c_str();
    }
    
    /*
     * Parses information recieved by radio reciever into a radio response class
     */
    void ParseRadioResponse(ManagedString s)
    {
        const char *responseInArray = s.toCharArray();
        string response = responseInArray;
        
        //Parse response
        int position = 0;
        string breakString = "|";
        string token;
        
        int count = 0;
        while ((position = response.find(breakString)) != -1)
        {
            token = response.substr(0, position);
            if(count == 0)
            {
                radioResponse.command = token;
                //uBit.serial.printf("Command: %s \r\n", radioResponse.command.c_str());
            }
            else if(count == 1)
            {
                radioResponse.ID = token;
                //uBit.serial.printf("ID: %s \r\n", radioResponse.ID.c_str());
            }
            else
            {
                radioResponse.message = token;
                //uBit.serial.printf("Message: %s \r\n", radioResponse.message.c_str());
            }
            
            response.erase(0, position + breakString.length());
            
            count++;
        }
    }
    
    /*
     * Detection algorithm. Returns best detected result from the
     * magnetometer.
     */
    Magnetometer BestFit()
    {
        int highestXValue = 0;
        int highestYValue = 0;
        
        Magnetometer xCapture;
        Magnetometer yCapture;
        
        int count = 0;
        
        while(count < 10)
        {
            //Get values from Magnetometer
            Magnetometer capturedData;
            capturedData.x = uBit.compass.getX();
            capturedData.y = uBit.compass.getY();
            capturedData.z = uBit.compass.getZ();
            
            //X Axis detection
            int xStrength = capturedData.x - averageField.x;
            
            if(xStrength > highestXValue)
            {
                highestXValue = xStrength;
                xCapture = capturedData;
            }
            
            //Y Axis Detection
            int yStrength = capturedData.y - averageField.y;
            
            if(yStrength < 0)
            {
                yStrength = -yStrength;
            }
            
            if(yStrength > highestYValue)
            {
                highestYValue = yStrength;
                yCapture = capturedData;
            }
            
            count++;
        }
        
        //Return best detected axis from detection result.
        if(highestXValue > highestYValue)
        {
            return xCapture;
        }
        else
        {
            return yCapture;
        }
    }
    
    /*
     * Used to get an average of the magnetic field around the BBC micro:bit.
     * returns An array of size 3, containing the x,y,z values of the compass.
     */
    Magnetometer GetAverage()
    {
        static Magnetometer fieldStrengths;
        
        //Create empty average values
        int xAvg = 0;
        int yAvg = 0;
        int zAvg = 0;
        
        for (int i = 0; i < MAGNETIC_FIELD_RATE; i++)
        {
            //Get inital values from compass
            int x = uBit.compass.getX();
            int y = uBit.compass.getY();
            int z = uBit.compass.getZ();
            
            xAvg += x;
            yAvg += y;
            zAvg += z;
        }
        
        xAvg = xAvg / 10;
        yAvg = yAvg / 10;
        zAvg = zAvg / 10;
        
        fieldStrengths.x = xAvg;
        fieldStrengths.y = yAvg;
        fieldStrengths.z = zAvg;
        
        return fieldStrengths;
    }
    
    /*
     * Sends a radio signal containing the relevant data.
     */
    void SendDataRadio(ManagedString command, ManagedString message)
    {
        uBit.radio.datagram.send(command + "|" + myID + "|" + message + "|");
    }
    
    /*
     * Sends an electromagnetic pulse, as well as a radio signal
     */
    void SendPulse()
    {
        //Do one last check to insure we're the only one pulsing.
        if(!isSomeonePulsing)
        {
            //Send pulse
            SendDataRadio("Pulse", "Active");
            
            //uBit.serial.printf("I'm pulsing \r\n");
            
            //Enable or disable electromagnet
            electromagnetState = true;
            MiddleIndicator(electromagnetState);
            EnablePinOut(electromagnetState);
            
            uBit.sleep(PULSE_LENGTH);
            
            //Tell everyone I've finished
            SendDataRadio("Pulse", "Stopping");
            //uBit.serial.printf("I'm stopping \r\n");
            
            electromagnetState = false;
            MiddleIndicator(electromagnetState);
            EnablePinOut(electromagnetState);
            
            //Set delay
            delayBeforeNextPulse = PULSE_DELAY_AFTER_SUCESS;
            
            //Reset state of Micro:bit
            electromagnetState = false;
            pulsing = false;
        }
        else
        {
            pulsing = false;
        }
    }
    
    /*
     * Called when data recieved from radio is detected
     */
    void onData(MicroBitEvent e)
    {
        ManagedString s = uBit.radio.datagram.recv();
        ParseRadioResponse(s); //Sets it to global variable
        
        ManagedString command = radioResponse.command.c_str();
        ManagedString ID = radioResponse.ID.c_str();
        ManagedString message = radioResponse.message.c_str();
        
        //Recieved data is about magnetic pulsing
        if(command == "Pulse")
        {
            if(message == "Active")
            {
                //Someone is pulsing
                errorCounter = 0;
                isSomeonePulsing = true;
                //uBit.serial.printf("Device: %s is pulsing \r\n", radioResponse.ID.c_str());
                detectedThisPulse = false;
                currentPuslingDevice = radioResponse.ID;
            }
            else if(message == "Stopping")
            {
                //Stopping
                isSomeonePulsing = false;
                //uBit.serial.printf("Device: %s stopped pulsing \r\n", radioResponse.ID.c_str());
                currentPuslingDevice = "";
            }
        }
        else //Anything else is check against own ID.
        {
            
        }
    }
    
    /*
     * Updates the device specified by their ID within the list containing all nearby mirco:bits
     */
    void updateDeviceValues(string ID, int strength, int direction)
    {
        list<Proximity> :: iterator it;
        for(it = devicesNearMe.begin(); it != devicesNearMe.end(); ++it)
        {
            Proximity device = *it;
            
            if(device.ID == ID)
            {
                device.strength = strength;
                device.direction = direction;
                return;
            }
        }
    }
    
    /*
     * Returns true or false depending if the device is in the list or not
     */
    bool isNearMe(string ID)
    {
        list<Proximity> :: iterator it;
        for(it = devicesNearMe.begin(); it != devicesNearMe.end(); ++it)
        {
            Proximity device = *it;
            
            if(device.ID == ID)
            {
                return true;
            }
        }
        return false;
    }
    
    /*
     * Creates a device which can be put in the near list
     */
    Proximity createDevice(string ID, int strength, int direction)
    {
        Proximity device;
        device.ID = ID;
        device.strength = strength;
        device.direction = direction;
        return device;
    }

    
    //%
    int testFunction(){
        return 1;
    }
    
    void main_function()
    {
        //If no one is pulsing and I'm not pulsing, do magnetic field average
        if(!isSomeonePulsing && !pulsing)
        {
            LeftIndicator(false);
            RightIndicator(false);
            TopIndicator(false);
            BottomIndicator(false);
            
            //Update magnetic field average
            averageField = GetAverage();
            //uBit.serial.printf("Average Field: %i %i %i \r\n", averageField.x, averageField.y, averageField.z);
            
            //Debug list
            //PrintNearList();
        }
        
        //If someone is pulsing check values
        if(isSomeonePulsing && !detectedThisPulse)
        {
            detectedThisPulse = true;
            
            Magnetometer capturedData = BestFit();
            //uBit.serial.printf("(Someone is pulsing) Magnetic Field: %i %i %i \r\n", capturedData.x, capturedData.y, capturedData.z);
            
            bool detected = false;
            Proximity detectedDevice;
            
            int xStrength = 0;
            
            //Left or right
            if(capturedData.x < (averageField.x - SENSITIVITY) && capturedData.y < (averageField.y - (SENSITIVITY / 4)))
            {
                RightIndicator(true);
                detected = true;
                int xRightStrength = capturedData.x - (averageField.x + SENSITIVITY);
                xStrength = xRightStrength;
                detectedDevice = createDevice(currentPuslingDevice, xRightStrength, RIGHT_DIR);
            }
            else if(capturedData.x < (averageField.x - SENSITIVITY))
            {
                LeftIndicator(true);
                detected = true;
                int xLeftStrength = capturedData.x - (averageField.x + SENSITIVITY);
                xStrength = xLeftStrength;
                detectedDevice = createDevice(currentPuslingDevice, xLeftStrength, LEFT_DIR);
            }
            
            //Top and bottom
            if(capturedData.x > (averageField.x + SENSITIVITY) && capturedData.y < (averageField.y - (SENSITIVITY / 2)))
            {
                int yTopStrength = capturedData.y - (averageField.y + SENSITIVITY);
                
                if(yTopStrength > xStrength)
                {
                    TopIndicator(true);
                    detected = true;
                    detectedDevice = createDevice(currentPuslingDevice, yTopStrength, TOP_DIR);
                }
            }
            else if(capturedData.x > (averageField.x + SENSITIVITY))
            {
                int yBottomStrength = (averageField.y + SENSITIVITY) - capturedData.y;
                
                if(yBottomStrength > xStrength)
                {
                    BottomIndicator(true);
                    detected = true;
                    detectedDevice = createDevice(currentPuslingDevice, yBottomStrength, BOTTOM_DIR);
                }
            }
            
            if(!detected)
            {
                //Didn't detect anything, remove if the device used to be near us
                if(isNearMe(currentPuslingDevice) == true)
                {
                    //Remove device from list
                    Proximity temp = createDevice(currentPuslingDevice, 0, 0);
                    devicesNearMe.remove(temp);
                }
            }
            else if(detected)
            {
                //We have detected something, have we seen it before? If so then update the direction and distance.
                if(isNearMe(currentPuslingDevice) == true)
                {
                    //Update direction and strength
                    updateDeviceValues(currentPuslingDevice, detectedDevice.strength, detectedDevice.direction);
                }
                else
                {
                    //Haven't seen this before so add it to the near list
                    devicesNearMe.push_back(detectedDevice);
                }
            }
            
            //uBit.serial.printf("Detecting electromagnet -------- Finished \r\n");
        }
        
        //If someone isn't pulsing or I'm pulsing
        if(!isSomeonePulsing && delayBeforeNextPulse <= 0 || pulsing)
        {
            pulsing = true;
            SendPulse();
        }
        else if(delayBeforeNextPulse <= 0) // If someone is pulsing and my timer is 0, reset to random value.
        {
            delayBeforeNextPulse = rand() % 5 + 3;
        }
        
        //Decrement delay counter
        if(delayBeforeNextPulse > 0)
        {
            delayBeforeNextPulse--;
        }
        
        errorCounter++;
        
        if(isSomeonePulsing && errorCounter > 1000)
        {
            //Micro:bit hasn't sent stopping pulse.
            uBit.serial.printf("Error correction \r\n");
            isSomeonePulsing = false;
        }
        
        uBit.sleep(MAGNETO_PROGRAM_RATE);
    }
}
