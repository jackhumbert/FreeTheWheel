//
//  WheelSupports.h
//  WheelSupportTools
//
//  Created by Annop Prapasapong on 27/11/2012.
//  Copyright (c) 2012 Feral Interactive.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __WheelSupportTools__WheelSupports__
#define __WheelSupportTools__WheelSupports__

#include <IOKit/hid/IOHIDManager.h>


// Device IDs (format is 16-bits product ID, followed by 16-bits vender ID)
#define kGPLogitechWheelRestricted					0xc294046d
#define kGPLogitechG25Native						0xc299046d
#define kGPLogitechG27Native						0xc29b046d
#define kGPLogitechG29Native						0xc24f046d
#define kGPLogitechDFGTNative						0xc29a046d
#define kGPLogitechDFPNative						0xc298046d
#define kGPLogitechG920Native						0xc261046d

// Product IDs
#define kGPLogitechG25ProductID						"G25"
#define kGPLogitechG27ProductID						"G27"
#define kGPLogitechG29ProductID                     "G29"
#define kGPLogitechDFGTProductID					"Driving Force GT"
#define kGPLogitechDFPProductID						"Driving Force Pro"
#define kGPLogitechG920ProductID					"G920"

// Device properties
#define kGPLogitechWheelRangeStandard				240
#define kGPLogitechWheelRangeMax					900

//=============================================================================
typedef UInt32										DeviceID;

enum DeviceMode
{
	DeviceModeInfoOnly,
	DeviceModeStandard,
	DeviceModeFull
};

#define MakeDeviceID(productID, vendorID)			( (((productID) % 0xFFFF) << 16) | ((vendorID) & 0xFFFF) )

// GamePad command codes structure
#define kGPCommandsMax								4
#define kGPCommandMaxLength							8
#define kGPCommandsDataSize							sizeof(UInt8) * kGPCommandsMax * kGPCommandMaxLength

struct CCommands
{
	UInt8 cmds[kGPCommandsMax][kGPCommandMaxLength];
	UInt8 count;
};

//=============================================================================
IOHIDManagerRef AllocateHIDManager();

bool ConfigAllDevices(const DeviceMode mode);
bool ConfigDevice(IOHIDDeviceRef hidDevice, DeviceID deviceID, const DeviceMode mode);

IOReturn OpenDevice(IOHIDDeviceRef hidDevice);
IOReturn CloseDevice(IOHIDDeviceRef hidDevice);
IOReturn SendCommands(IOHIDDeviceRef hidDevice, CCommands *commands);

CFStringRef GetPropertyString(IOHIDDeviceRef hidDevice, CFStringRef property);
UInt32 GetPropertyNumber(IOHIDDeviceRef hidDevice, CFStringRef property);

bool ConfigLogitechWheels(IOHIDDeviceRef hidDevice, DeviceID deviceID, bool native, const DeviceMode targetMode);

void GetCmdLogitechWheelNative(CCommands *c, const DeviceID deviceID);
void GetCmdLogitechWheelRange(CCommands *c, const DeviceID deviceID, int range);

#endif /* defined(__WheelSupportTools__WheelSupports__) */
