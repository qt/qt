/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

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
