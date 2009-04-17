#import "InstallerPanePane.h"
#import "helpfulfunc.h"

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

@implementation InstallerPanePane
- (NSString *)title
{
    return [[NSBundle bundleForClass:[self class]] localizedStringForKey:@"PaneTitle"
                                                                   value:nil table:nil];
}

- (id)init
{
    self = [super init];
    if (self) {
        licenseStatus = InvalidLicense;
        nameCheckOK = NO;
        fullLicenseKey = [[NSMutableString alloc] initWithCapacity:35];
        [self fillInForm];
    }
    return self;
}

- (void)fillInForm
{
    static NSString *qtLicense = [NSHomeDirectory() stringByAppendingPathComponent: @".qt-license"];
    NSData *fileData = [[NSFileManager defaultManager] contentsAtPath: qtLicense];
    NSString *fileContents = [[[NSString alloc] initWithData:fileData
                                                    encoding:NSUTF8StringEncoding] autorelease];
    if (fileContents == nil)
        return;
    NSArray *array = [fileContents componentsSeparatedByString:@"\n"];
    uint totalCount = [array count];
    initialLicensee = nil;
    initialLicenseKey = nil;
    for (uint i = 0; i < totalCount; ++i) {
        if (initialLicensee != nil && initialLicenseKey != nil)
            break;
        NSString *string = [array objectAtIndex:i];
        if (initialLicenseKey == nil) {
            NSRange range = [string rangeOfString:[NSString stringWithUTF8String:LicenseKeyExtString]];
            if (range.location != NSNotFound && range.length != 0) {
                initialLicenseKey = [string substringFromIndex:range.location + range.length];
                [initialLicenseKey retain];
                continue;
            }
        }
        if (initialLicensee == nil) {
            NSRange range = [string rangeOfString:[NSString stringWithUTF8String:LicenseeString]];
            if (range.location != NSNotFound && range.length != 0) {
                initialLicensee = [[string substringFromIndex:range.location + range.length]
                                   stringByTrimmingCharactersInSet:
                                        [NSCharacterSet characterSetWithCharactersInString:@"\""]];
                [initialLicensee retain];
                continue;
            }
        }
    }
}

- (void)willEnterPane:(InstallerSectionDirection)dir
{
    NSString *string = [nameField stringValue];
    
    if ((string == nil || [string length] == 0) && (initialLicensee != nil))
        [nameField setStringValue:initialLicensee];
    string = [LicenseField1 stringValue];
    if ((string == nil || [string length] == 0  && (initialLicenseKey != nil))) {
        NSArray *array = [initialLicenseKey componentsSeparatedByString:@"-"];
        NSArray *licenseFieldArray = [NSArray arrayWithObjects:LicenseField1, LicenseField2,
                                        LicenseField3, LicenseField4,
                                        LicenseField5, LicenseField6, LicenseField7, nil];
        uint count = [array count];
        count = (count > 7) ? 7 : count;
        for (uint i = 0; i < count; ++i)
            [[licenseFieldArray objectAtIndex:i] setStringValue:[array objectAtIndex:i]];
    }
    [self checkLicense];
}

- (BOOL)shouldExitPane:(InstallerSectionDirection)dir
{ 
    if ((dir == InstallerDirectionForward)) {
        if ((licenseStatus != LicenseOK) && !nameCheckOK)
            return NO;
        
        // Copy the file first
        NSFileManager *fm = [NSFileManager defaultManager];
        NSString *qtLicense = [NSHomeDirectory() stringByAppendingPathComponent: @".qt-license"];
#if 0
        if ([fm fileExistsAtPath:qtLicense]) {
            NSMutableString *alternatePlace = [NSMutableString stringWithCapacity: 256];
            [alternatePlace setString : [NSHomeDirectory() stringByAppendingPathComponent: @".qt-license.bak"]];
            NSNumber *backNumber = [NSNumber numberWithInt:1];
            int index = [alternatePlace length] - 1;
            
            while ([fm fileExistsAtPath:alternatePlace]) {
                [alternatePlace insertString:[backNumber stringValue] atIndex:index];
                int foo = [backNumber intValue];
                ++foo;
                backNumber = [NSNumber numberWithInt:foo];
            }
            [fm copyPath:qtLicense toPath:alternatePlace handler:nil];
        }
#endif

        NSMutableString *finalString = [NSMutableString stringWithCapacity:256];
        NSMutableString *nameString = [NSMutableString stringWithCapacity:128];
        [nameString appendString: [nameField stringValue]];
        [nameString replaceOccurrencesOfString:@"\"" withString:@"'"
                           options:NSLiteralSearch range:NSMakeRange(0, [nameString length])];
        
        [finalString appendString:[NSString stringWithUTF8String: LicenseeString]];
        [finalString appendString:@"\""];
        [finalString appendString:nameString];
        [finalString appendString:@"\""];
        [finalString appendString:@"\n"];
        [finalString appendString:[NSString stringWithUTF8String: LicenseKeyExtString]];
        [finalString appendString:fullLicenseKey];
        [finalString appendString:@"\n"];
            
        const char *finalFinalString = [finalString UTF8String];
        
        NSData *data = [NSData dataWithBytes:finalFinalString length:strlen(finalFinalString)];
        [fm createFileAtPath:qtLicense contents:data attributes:nil];
    }
    return YES; 
}

- (void)didEnterPane:(InstallerSectionDirection)dir
{
    [self checkName];
    [self checkLicense];
    [nameField selectText:self];
}

- (void)manipulateEditorString:(NSTextField *) textField
{
    NSString *string = [[textField currentEditor] string];
    NSRange range = [string rangeOfString:@"-"];
    if (range.location == NSNotFound) {
        return;
    } else {
        NSString *theBeginning = [string substringToIndex:range.location];
        NSString *theRest = [string substringFromIndex:range.location + 1];
        NSText *ed = [textField currentEditor];
        [ed setString:theBeginning];
        NSView *nextView = [textField nextValidKeyView];
        if (nextView != nil && [nextView class] == [textField class]) {
            NSTextField *nextTextField = (NSTextField *)nextView;
            [[nextTextField window] makeFirstResponder: nextTextField];
            ed = [nextTextField currentEditor];
            [ed setString:theRest];
            // Bubble this along to catch extra dashes
            [self manipulateEditorString:nextTextField];
        }
    }
}

- (void)controlTextDidChange:(NSNotification *)aNotification
{
    NSTextField *itemThatChanged = [aNotification object];
    if (itemThatChanged == nameField) {
        [self checkName];
    } else if (itemThatChanged != nil) {
        [self manipulateEditorString: itemThatChanged];
        [self checkLicense];
    }
}


- (void)checkLicense
{
    int i;
    [fullLicenseKey setString:@""];
    NSTextField *theLicenseFields[] = { LicenseField1, LicenseField2, LicenseField3, LicenseField4,
                                        LicenseField5, LicenseField6, LicenseField7 };
    
    for (i = 0; i < sizeof(theLicenseFields) / sizeof(NSTextField*); ++i) {
        NSString *part = [[theLicenseFields[i] stringValue]
                            stringByTrimmingCharactersInSet:[NSCharacterSet
                                                                whitespaceAndNewlineCharacterSet]];
        if ([part length] <= 0) {
            licenseStatus = InvalidLicense;
            [self tryEnable];
            return;
        }
        if (i != 0)
            [fullLicenseKey appendString:@"-"];
        [fullLicenseKey appendString:[part uppercaseString]];
    }
    
    licenseStatus = validateLicense([fullLicenseKey UTF8String]);
    [self tryEnable];
}

- (void)checkName
{
    NSString *name = [nameField stringValue];
    nameCheckOK = [name length] > 0;
    [self tryEnable];
}


- (void)tryEnable
{
    [self setNextEnabled:(licenseStatus == LicenseOK) && nameCheckOK];
    switch (licenseStatus) {
    case LicenseOK:
    case InvalidLicense:
        [errorField setStringValue:@""];
        break;
    case InvalidType:
        [errorField setStringValue:@"This license cannot be used with this version of Qt"];
        break;
    case InvalidPlatform:
        [errorField setStringValue:@"This license cannot be used for this Qt Platform"];
        break;
    case LicenseExpired:
        [errorField setStringValue:@"This license has expired for this product,\n contact sales@trolltech.com for an upgrade"];
        break;
    }
}


@end


