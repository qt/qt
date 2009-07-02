// IMPORTANT!!!! If you want to add testdata to this file, 
// always add it to the end in order to not change the linenumbers of translations!!!

// nothing here

// sickness: multi-\
line c++ comment } (with brace)

#define This is a closing brace } which was ignored
} // complain here

#define This is another \
    closing brace } which was ignored
} // complain here

#define This is another /* comment in } define */\
     something /* comment )
       spanning {multiple} lines */ \
    closing brace } which was ignored
} // complain here

#define This is another // comment in } define \
     something } comment
} // complain here

char somestring[] = "\
    continued\n\
    here and \"quoted\" to activate\n";

    NSString *scriptSource = @"\
                on SetupNewMail(theRecipientAddress, theSubject, theContent, theAttachmentPath)\n\
                    tell application \"Mail\" to activate\n\
                    tell application \"Mail\"\n\
                        set theMessage to make new outgoing message with properties {visible:true, subject:theSubject, content:theContent}\n\
                        tell theMessage\n\
                            make new to recipient at end of to recipients with properties {address:theRecipientAddress}\n\
                        end tell\n\
                        tell content of theMessage\n\
                            make new attachment with properties {file name:theAttachmentPath} at after last paragraph\n\
                        end tell\n\
                    end tell\n\
                end SetupNewMail\n";
