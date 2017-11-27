//
//  WheelSupports.cpp
//  WheelSupportTools
//
//  Created by Annop Prapasapong on 27/11/2012.
//  Copyright (c) 2012 Feral Interactive. All rights reserved.
//

#include <CoreFoundation/CFString.h>
#include "WheelSupports.h"

//=============================================================================
// Mode strings
const char *sGPLogitechModeRestricted = "RESTRICTED";
const char *sGPLogitechModeNative = "NATIVE";


//=============================================================================
//		SetApplierFunctionCopyToCFArray : Applier function for CFSetApplyFunction
//-----------------------------------------------------------------------------
void SetApplierFunctionCopyToCFArray(const void *value, void *context)
{
    CFArrayAppendValue((CFMutableArrayRef) context, value);
}



//=============================================================================
//		AllocateHIDManager
//-----------------------------------------------------------------------------
IOHIDManagerRef AllocateHIDManager()
{
	IOHIDManagerRef managerRef = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDManagerOptionNone);
	IOHIDManagerSetDeviceMatching(managerRef, NULL);
	IOHIDManagerOpen(managerRef, kIOHIDOptionsTypeSeizeDevice);
	return managerRef;
}



//=============================================================================
//		ConfigAllDevices : Scan connected devices and apply early configs as need
//										 Return true if any change is made
//-----------------------------------------------------------------------------
bool ConfigAllDevices(const DeviceMode mode)
{
	bool changed = false;
	
	// Obtain a copy of the list of connected devices
	IOHIDManagerRef managerRef = AllocateHIDManager();
	CFSetRef deviceCFSetRef = IOHIDManagerCopyDevices(managerRef);
	
	CFIndex count = CFSetGetCount(deviceCFSetRef);
	CFArrayRef devCFArrayRef = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
	CFSetApplyFunction(deviceCFSetRef, SetApplierFunctionCopyToCFArray, (void*)devCFArrayRef);
	
	// Going through the list, determining their ID and call the appropriate function;
	for(CFIndex i = 0; i < count; i++)
	{
		IOHIDDeviceRef hidDevice = (IOHIDDeviceRef) CFArrayGetValueAtIndex(devCFArrayRef, i);
		
		UInt32 vendorID = GetPropertyNumber(hidDevice, CFSTR(kIOHIDVendorIDKey));
		UInt32 productID = GetPropertyNumber(hidDevice, CFSTR(kIOHIDProductIDKey));
		DeviceID deviceID = MakeDeviceID(productID, vendorID);
		
		changed |= ConfigDevice(hidDevice, deviceID, mode);
	}
	
	// Release the list
	CFRelease(devCFArrayRef);
	CFRelease(deviceCFSetRef);
	return changed;
}



//=============================================================================
//		ConfigDevice : Config a device using current settngs
//-----------------------------------------------------------------------------
bool ConfigDevice(IOHIDDeviceRef hidDevice, DeviceID deviceID, const DeviceMode mode)
{
    
	switch(deviceID)
	{
		case kGPLogitechWheelRestricted:
			return ConfigLogitechWheels(hidDevice, deviceID, false, mode);
			break;			
		case kGPLogitechG25Native:
            printf("Logitech G25 Native mode enabled.\n");
            return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		case kGPLogitechG27Native:
            printf("Logitech G27 Native mode enabled.\n");
            return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		case kGPLogitechG29Native:
            printf("Logitech G29 Native mode enabled.\n");
            return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		case kGPLogitechDFGTNative:
            printf("Logitech Driving Force GT Native mode enabled.\n");
            return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		case kGPLogitechDFPNative:
            printf("Logitech Driving Force Pro Native mode enabled.\n");
			return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		case kGPLogitechG920Native:
			printf("Logitech G920 Native mode enabled.\n");
			return ConfigLogitechWheels(hidDevice, deviceID, true, mode);
		default:
			return false;
	}
}



//=============================================================================
//		OpenDevice : Open given device if it is not opened yet
//-----------------------------------------------------------------------------
IOReturn OpenDevice(IOHIDDeviceRef hidDevice)
{
	IOReturn result = IOHIDDeviceOpen(hidDevice, kIOHIDOptionsTypeSeizeDevice);
	if(result != kIOReturnSuccess)
	{
		printf("ERROR: OpenDevice failed with result: %x\n", result);
		return result;
	}
	return kIOReturnSuccess;
}



//=============================================================================
//		CloseDevice : Close given device if it is opened
//-----------------------------------------------------------------------------
IOReturn CloseDevice(IOHIDDeviceRef hidDevice)
{
	return IOHIDDeviceClose(hidDevice, 0);
}



//=============================================================================
//		SendCommands : Send given commands to the device
//-----------------------------------------------------------------------------
IOReturn SendCommands(IOHIDDeviceRef hidDevice, CCommands *commands)
{
	for(int i=0; i<commands->count; ++i)
	{
		UInt8 *cmd = commands->cmds[i];
		IOReturn result = IOHIDDeviceSetReport(hidDevice, kIOHIDReportTypeOutput, time(NULL), cmd, kGPCommandMaxLength);
		if (result != kIOReturnSuccess)
		{
			printf("WARNING: SendCommand failed with result: %x\n", result);
			return result;
		}
	}
	return kIOReturnSuccess;
}



//=============================================================================
//		GetPropertyString : Obtain the property string data of the device
//-----------------------------------------------------------------------------
CFStringRef GetPropertyString(IOHIDDeviceRef hidDevice, CFStringRef property)
{
	// Obtain the data as untyped pointer and convert it to CString
	CFTypeRef dataRef = IOHIDDeviceGetProperty(hidDevice, property);
	return CFStringCreateCopy(kCFAllocatorDefault, CFStringRef(dataRef));
}



//=============================================================================
//		GetPropertyString : Obtain the property number data of the device
//-----------------------------------------------------------------------------
UInt32 GetPropertyNumber(IOHIDDeviceRef hidDevice, CFStringRef property)
{
	// Obtain the data as untyped pointer and convert it to CString
	CFTypeRef dataRef = IOHIDDeviceGetProperty(hidDevice, property);
	if(dataRef && (CFNumberGetTypeID() == CFGetTypeID(dataRef)))
	{
		UInt32 number;
		CFNumberGetValue((CFNumberRef)dataRef, kCFNumberSInt32Type, &number);
		return number;
	}
	return 0;
}



//=============================================================================
//		ConfigLogitechWheels : for Logitech wheels
//-----------------------------------------------------------------------------
bool ConfigLogitechWheels(IOHIDDeviceRef hidDevice, DeviceID deviceID, bool native, const DeviceMode targetMode)
{
	if(targetMode == DeviceModeInfoOnly)
	{
		CFStringRef productID = GetPropertyString(hidDevice, CFSTR(kIOHIDProductKey));
		const char *mode = native ? sGPLogitechModeNative : sGPLogitechModeRestricted;

		char sProductID[256];
		CFStringGetCString(productID, sProductID, 256, kCFStringEncodingASCII);

		printf("Device ID=%x   Product ID=%s (%s)\n", deviceID, sProductID, mode);
		return false;
	}
	
	if(!native)
	{
		// Don't do anything is the feature is not enabled
		if(targetMode == DeviceModeStandard)
		{
			return false;
		}
		
		// Determine proper native device ID from the device's Product ID string
		// As the restricted device ID are the same for all 4 supported devices
		CFStringRef productID = GetPropertyString(hidDevice, CFSTR(kIOHIDProductKey));
		DeviceID targetDeviceID;
		if(CFStringFind(productID, CFSTR(kGPLogitechG25ProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechG25Native;
            printf("Logitech G25 Native mode enabled.\n");
		}
		else if(CFStringFind(productID, CFSTR(kGPLogitechG27ProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechG27Native;
            printf("Logitech G27 Native mode enabled.\n");
		}
		else if(CFStringFind(productID, CFSTR(kGPLogitechG29ProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechG29Native;
            printf("Logitech G29 Native mode enabled.\n");
		}
		else if(CFStringFind(productID, CFSTR(kGPLogitechDFGTProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechDFGTNative;
            printf("Logitech Driving Force GT Native mode enabled.\n");
		}
		else if(CFStringFind(productID, CFSTR(kGPLogitechDFPProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechDFPNative;
            printf("Logitech Driving Force Pro Native mode enabled.\n");
		}
		else if(CFStringFind(productID, CFSTR(kGPLogitechG920ProductID), 0).location == 0)
		{
			targetDeviceID = kGPLogitechG920Native;
			printf("Logitech G920 Native mode enabled.\n");
		}
		else return false;
		
		// Activate full native and 900 degree mode
		if(OpenDevice(hidDevice) == kIOReturnSuccess)
		{
			CCommands commands;
			GetCmdLogitechWheelRange(&commands, targetDeviceID, kGPLogitechWheelRangeMax);
			SendCommands(hidDevice, &commands);
			printf("Calibrated full wheel range. (VendorID/DeviceID %x)\n", deviceID);
			
			GetCmdLogitechWheelNative(&commands, targetDeviceID);
			SendCommands(hidDevice, &commands);
			printf("Enabled native mode. (VendorID/DeviceID %x)\n", deviceID);
			
                     
			CloseDevice(hidDevice);
			return true;
		}
	}
	else
	{
		if(OpenDevice(hidDevice) == kIOReturnSuccess)
		{
			CCommands commands;
			bool changed = false;
			
			if(targetMode == DeviceModeFull)
			{
				// Activate full 900 degree mode, as being in native mode doesn't guarantee the wheel range
				GetCmdLogitechWheelRange(&commands, deviceID, kGPLogitechWheelRangeMax);
				SendCommands(hidDevice, &commands);
				
                printf("Calibrated 900 degree wheel movement. (VendorID/DeviceID %x)\n", deviceID);
				changed = true;
			}
			else
			{
				// We don't know how to go back to restricted mode, but we can set the wheel back to 240 degree range
				GetCmdLogitechWheelRange(&commands, deviceID, kGPLogitechWheelRangeStandard);
				SendCommands(hidDevice, &commands);
				
                printf("Reset device to default 320 degree wheel movement. (VendorID/DeviceID %x)\n", deviceID);
				changed = true;
			}
			
			CloseDevice(hidDevice);
			return changed;
		}
	}
	
	return false;
}



//=============================================================================
//		GetCmdLogitechWheelNative : Activate native mode for given device ID
//-----------------------------------------------------------------------------
void GetCmdLogitechWheelNative(CCommands *c, const DeviceID deviceID)
{
	// Fill the command array with null
	memset(c->cmds, 0, kGPCommandsDataSize);
	
	// Fill in the command value according to target deviceID
	switch (deviceID) {
		case kGPLogitechG25Native:
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x10;
			c->count = 1;
			break;
		case kGPLogitechG27Native:
		case kGPLogitechG29Native:


            // Original commands
//			c->cmds[0][0] = 0xf8;
//			c->cmds[0][1] = 0x0a;
//			c->cmds[1][0] = 0xf8;
//			c->cmds[1][1] = 0x09;
//			c->cmds[1][2] = 0x04;
//			c->cmds[1][3] = 0x01;
//			c->count = 1;
            
            // https://github.com/TripleSpeeder/LTWheelConf/blob/master/wheels.c
            
            c->cmds[0][0] = 0xf8;
            c->cmds[0][1] = 0x0a;
            c->cmds[0][2] = 0x00;
            c->cmds[0][3] = 0x00;
            c->cmds[0][4] = 0x00;
            c->cmds[0][5] = 0x00;
            c->cmds[0][6] = 0x00;
            c->cmds[0][7] = 0x00;

            // Partial button mapping
            c->cmds[1][0] = 0xf8;
            c->cmds[1][1] = 0x01;
            c->cmds[1][2] = 0x00;
            c->cmds[1][3] = 0x00;
            c->cmds[1][4] = 0x00;
            c->cmds[1][5] = 0x00;
            c->cmds[1][6] = 0x00;
            c->cmds[1][7] = 0x00;
            
            // Full button mapping with clutch
//            c->cmds[1][0] = 0xf8;
//            c->cmds[1][1] = 0x09;
//            c->cmds[1][2] = 0x04;
//            c->cmds[1][3] = 0x01;
//            c->cmds[1][4] = 0x00;
//            c->cmds[1][5] = 0x00;
//            c->cmds[1][6] = 0x00;
//            c->cmds[1][7] = 0x00;

			c->count = 2;
            
			break;
		case kGPLogitechDFGTNative:
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x0a;
			c->cmds[1][0] = 0xf8;
			c->cmds[1][1] = 0x09;
			c->cmds[1][2] = 0x03;
			c->cmds[1][3] = 0x01;
			c->count = 2;
			break;
		case kGPLogitechDFPNative:
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x01;
			c->count = 1;
			break;
		case kGPLogitechG920Native:
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x0a;
			c->cmds[0][2] = 0x00;
			c->cmds[0][3] = 0x00;
			c->cmds[0][4] = 0x00;
			c->cmds[0][5] = 0x00;
			c->cmds[0][6] = 0x00;
			c->cmds[0][7] = 0x00;

			c->count = 2;
			break;
		default:
			c->count = 0;
			break;
	}
}



//=============================================================================
//		GetCmdLogitechWheelRange : Set wheel range for given device ID
//-----------------------------------------------------------------------------
void GetCmdLogitechWheelRange(CCommands *c, const DeviceID deviceID, int range)
{
	// Fill the command array with null
	memset(c->cmds, 0, kGPCommandsDataSize);
	
	// Fill in the command value according to target deviceID
	switch (deviceID) {
		case kGPLogitechG25Native:
		case kGPLogitechG27Native:
		case kGPLogitechG29Native:
		case kGPLogitechDFGTNative:
		{
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x81;
			c->cmds[0][2] = (range & 0x00ff);
			c->cmds[0][3] = (range & 0xff00) >> 8;
			c->cmds[0][4] = 0x00;
			c->cmds[0][5] = 0x00;
			c->cmds[0][6] = 0x00;
			c->cmds[0][7] = 0x00;
            
//            c->cmds[1][0] = 0xfe;
//            c->cmds[1][1] = 0x0d;
//            c->cmds[1][2] = 0 & 0x0f;
//            c->cmds[1][3] = 0 & 0x0f;
//            c->cmds[1][4] = 0 & 0xff;
//            c->cmds[1][5] = 0x00;
//            c->cmds[1][6] = 0x00;
//            c->cmds[1][7] = 0x00;
            
			c->count = 1;
			break;
		}
		case kGPLogitechDFPNative:
		{
			c->cmds[0][0] = 0xf8;
			c->cmds[1][0] = 0x81;
			c->cmds[1][1] = 0x0b;
			c->count = 2;
			
			// Determine the needs for range limiter
			int rampLeft;
			int rampRight;
			int fullRange = 0;
			
			if (range > 200)
			{
				c->cmds[0][1] = 0x03;
				fullRange = 900;
			}
			else
			{
				c->cmds[0][1] = 0x02;
				fullRange = 200;
			}
			
			// If target range less than full range; Apply limiter command
			if (range < fullRange)
			{
				rampLeft = (((fullRange - range + 1) * 2047) / fullRange);
				rampRight = 0xfff - rampLeft;
				
				c->cmds[1][2] = rampLeft >> 4;
				c->cmds[1][3] = rampRight >> 4;
				c->cmds[1][4] = 0xff;
				c->cmds[1][5] = (rampRight & 0xe) << 4 | (rampLeft & 0xe);
				c->cmds[1][6] = 0xff;
			}
			break;
		}
		case kGPLogitechG920Native:
		{
			c->cmds[0][0] = 0xf8;
			c->cmds[0][1] = 0x61;
			c->cmds[0][2] = (range & 0x00ff);
			c->cmds[0][3] = (range & 0xff00) >> 8;
			c->cmds[0][4] = 0x00;
			c->cmds[0][5] = 0x00;
			c->cmds[0][6] = 0x00;
			c->cmds[0][7] = 0x00;
		}
		default:
			c->count = 0;
			break;
	}
}
