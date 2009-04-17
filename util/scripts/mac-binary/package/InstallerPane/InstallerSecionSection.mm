//
//  InstallerSecionSection.mm
//  InstallerPane
//
//  Created by Trenton Schulz on 7/27/05.
//  Copyright 2005 __MyCompanyName__. All rights reserved.
//

#import "InstallerSecionSection.h"
#import "helpfulfunc.h"
#import <stdio.h>
#import <string.h>


static BOOL checkForLicenseFile()
{
    static const int LICENSESIZE = 42;
    int value = -1;
    static NSString *qtLicense = [NSHomeDirectory() stringByAppendingPathComponent: @".qt-license"];
    NSData *fileContents = [[NSFileManager defaultManager] contentsAtPath: qtLicense];
    if (fileContents != nil) {
        const char *charBuffer = (const char *)[fileContents bytes];
        int length = [fileContents length];
        char *location = strnstr(charBuffer, LicenseKeyExtString, length);
        if (location) {
            location += strlen(LicenseKeyExtString);
            char licenseString[LICENSESIZE];
            strncpy(licenseString, location, LICENSESIZE);
            licenseString[LICENSESIZE - 1] = '\0';
            while (location = strstr(licenseString, "\n"))
                *location = '\0';
            value = validateLicense(licenseString);      
        }
    }
    return value == LicenseOK;
}


@implementation InstallerSecionSection

- (BOOL)shouldLoad
{
    return !checkForLicenseFile();
}

@end
