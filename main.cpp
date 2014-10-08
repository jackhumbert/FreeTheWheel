//
//  main.cpp
//  WheelSupportTools
//
//  Created by Annop Prapasapong on 27/11/2012
//  Usability edits Edwin 7/12/2012
//  Copyright (c) 2012 Feral Interactive. All rights reserved.
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


#include <iostream>
#include "WheelSupports.h"

int main(int argc, const char * argv[])
{
	printf("================================================================================\n");
	printf("=                               Free The Wheel (FTW)                           =\n");
	printf("=                             © Feral Interactive 2012                         =\n");
	printf("=    This program comes with ABSOLUTELY NO WARRANTY; This is free software,    =\n"); 
    printf("=            You are welcome to redistribute it under certain conditions,      =\n");
	printf("=                            Released under GNU License                        =\n");
    printf("================================================================================\n");
	
	DeviceMode configMode = DeviceModeFull;
	if(argc > 1)
	{
		if(strcmp(argv[1], "--info") == 0)
		{
			configMode = DeviceModeInfoOnly;
		}
		else if(strcmp(argv[1], "--restore") == 0)
		{
			configMode = DeviceModeStandard;
		}
	}
	else
	{
		//printf("=                               :Advanced Options:                             =\n");
		printf("=   --info       - display list of supported devices.                          =\n");
		printf("=   --restore    - Restore your wheel to restricted (default) mode.            =\n");
        printf("================================================================================\n");
	}

	switch (configMode) {
		case DeviceModeFull:
			printf("Looking for supported wheels to enable in NATIVE mode. . .\n\n");
			break;
			
		case DeviceModeStandard:
			printf("Returning wheels to RESTRICTED (Default) mode  . . .\n\n");
			break;
			
		case DeviceModeInfoOnly:
			printf("Displaying list of supported wheels:\n\n");
			break;
	}
	ConfigAllDevices(configMode);
	printf("\nDone.\n");
    return 0;
}

