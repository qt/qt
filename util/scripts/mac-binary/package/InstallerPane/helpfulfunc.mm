/*
 *  helpfulfunc.c
 *  InstallerPane
 *
 *  Created by Trenton Schulz on 7/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "helpfulfunc.h"
#include "keydec.h"

#include <Cocoa/Cocoa.h>

const char LicenseKeyExtString[] = "LicenseKeyExt=";
const char LicenseeString[] = "Licensee=";

int validateLicense(const char *string)
{
    KeyDecoder key(string);
    int ret = InvalidLicense;
    int validSchema =
#ifdef QT_EVAL
        (KeyDecoder::SupportedEvaluation
         | KeyDecoder::UnsupportedEvaluation | KeyDecoder::FullSourceEvaluation);
#else
       (KeyDecoder::FullSourceEvaluation | KeyDecoder::Academic 
        | KeyDecoder::Educational | KeyDecoder::FullCommercial);
#endif
    if (key.IsValid()) {
        if (!(key.getProducts() & (KeyDecoder::QtUniversal | KeyDecoder::QtDesktop
                                   | KeyDecoder::QtDesktopLight | KeyDecoder::QtConsole))) {
            ret = InvalidProduct;
        } else {
            if (!(key.getPlatforms()
                    & (KeyDecoder::Mac | KeyDecoder::Embedded | KeyDecoder::WinCE))) {
                ret = InvalidPlatform;
            } else {

                if (!(key.getLicenseSchema() & validSchema)) {
                    ret = InvalidType;
                } else {
                    ret = LicenseOK;
                }
            }
        }
    }

    if (ret == LicenseOK) {
        // We need to check ourselves against the build date
        // First read in the build date and then use it to compare with the
        // Expiry Date if it's commercial or the current date if it's an eval.
        CDate date = key.getExpiryDate();
        NSCalendarDate *expiryDate = [NSCalendarDate dateWithYear:date.year()
                                                            month:date.month() day:date.day()
                                                             hour:23 minute:59 second:59 
                                                         timeZone:[NSTimeZone systemTimeZone]];
        NSCalendarDate *compareDay = 0;
#ifdef QT_EVAL
        compareDay = [NSCalendarDate calendarDate];
#else
        // There's a lot of bundles here, so we have to make sure we get the correct one
        NSBundle *bundle = nil;
        NSArray *bundles = [NSBundle allBundles];
        for (uint i = 0; i < [bundles count]; ++i) {
            NSBundle *bun = [bundles objectAtIndex: i];
            NSRange location = [[bun bundleIdentifier] rangeOfString:@"com.trolltech.qt4."];
            if (location.length == 0)
                location = [[bun bundleIdentifier] rangeOfString:@"Trolltech Qt Packages"];

            if (location.length != 0) {
                bundle = bun;
                break;
            }

        }
        NSString *contents = [NSString stringWithContentsOfFile:[bundle pathForResource:@".package_date" ofType:nil]
                                                    encoding:NSUTF8StringEncoding error:0];
        compareDay = [NSCalendarDate dateWithString: contents calendarFormat:@"%Y-%m-%d"];
#endif
        if ([expiryDate laterDate: compareDay] != expiryDate)
            ret = LicenseExpired;
    }
    return ret;
}
