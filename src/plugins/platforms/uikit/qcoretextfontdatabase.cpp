/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcoretextfontdatabase.h"

#include <CoreText/CoreText.h>

#include <private/qcore_mac_p.h>
#include <private/qfontengine_coretext_p.h>

#include <QtDebug>

QT_BEGIN_NAMESPACE

void QCoreTextFontDatabase::populateFontDatabase()
{
    QCFType<CTFontCollectionRef> collection = CTFontCollectionCreateFromAvailableFonts(0);
    if(!collection)
        return;
    QCFType<CFArrayRef> fonts = CTFontCollectionCreateMatchingFontDescriptors(collection);
    if(!fonts)
        return;
    QSupportedWritingSystems supportedWritingSystems;
    for (int i = 0; i < QFontDatabase::WritingSystemsCount; ++i)
        supportedWritingSystems.setSupported((QFontDatabase::WritingSystem)i, true);
    QString foundry_name = "CoreText";
    const int numFonts = CFArrayGetCount(fonts);
    for(int i = 0; i < numFonts; ++i) {
        CTFontDescriptorRef font = (CTFontDescriptorRef)CFArrayGetValueAtIndex(fonts, i);

        QCFString family_name = (CFStringRef)CTFontDescriptorCopyAttribute(font, kCTFontFamilyNameAttribute);
//        QCFString style_name = (CFStringRef)CTFontDescriptorCopyAttribute(font, kCTFontStyleNameAttribute);

        QFont::Weight fontWeight = QFont::Normal;
        QFont::Style fontStyle = QFont::StyleNormal;
        if(QCFType<CFDictionaryRef> styles = (CFDictionaryRef)CTFontDescriptorCopyAttribute(font, kCTFontTraitsAttribute)) {
            if(CFNumberRef weight = (CFNumberRef)CFDictionaryGetValue(styles, kCTFontWeightTrait)) {
                Q_ASSERT(CFNumberIsFloatType(weight));
                double d;
                if(CFNumberGetValue(weight, kCFNumberDoubleType, &d)) {
                    if (d > 0.0)
                        fontWeight = QFont::Bold;
                }
            }
            if(CFNumberRef italic = (CFNumberRef)CFDictionaryGetValue(styles, kCTFontSlantTrait)) {
                Q_ASSERT(CFNumberIsFloatType(italic));
                double d;
                if(CFNumberGetValue(italic, kCFNumberDoubleType, &d)) {
                    if (d > 0.0)
                        fontStyle = QFont::StyleItalic;
                }
            }
        }

        int pixelSize = 0;
        if(QCFType<CFNumberRef> size = (CFNumberRef)CTFontDescriptorCopyAttribute(font, kCTFontSizeAttribute)) {
            if(CFNumberIsFloatType(size)) {
                double d;
                CFNumberGetValue(size, kCFNumberDoubleType, &d);
                pixelSize = d;
            } else {
                CFNumberGetValue(size, kCFNumberIntType, &pixelSize);
            }
        }
        registerFont(QString(family_name),
                     foundry_name,
                     fontWeight,
                     fontStyle,
                     QFont::Unstretched,
                     true,
                     true,
                     pixelSize,
                     supportedWritingSystems,
                     0);
    }
}

QFontEngine *QCoreTextFontDatabase::fontEngine(const QFontDef &fontDef, QUnicodeTables::Script script, void *handle)
{
    Q_UNUSED(script)
    Q_UNUSED(handle)
    CTFontSymbolicTraits symbolicTraits = 0;
    if (fontDef.weight >= QFont::Bold)
        symbolicTraits |= kCTFontBoldTrait;
    switch (fontDef.style) {
    case QFont::StyleNormal:
        break;
    case QFont::StyleItalic:
    case QFont::StyleOblique:
        symbolicTraits |= kCTFontItalicTrait;
        break;
    }

    CGAffineTransform transform = CGAffineTransformIdentity;
    if (fontDef.stretch != 100) {
        transform = CGAffineTransformMakeScale(float(fontDef.stretch) / float(100), 1);
    }

    QCFType<CTFontRef> baseFont = CTFontCreateWithName(QCFString(fontDef.family), fontDef.pixelSize, &transform);
    QCFType<CTFontRef> ctFont = NULL;
    // There is a side effect in Core Text: if we apply 0 as symbolic traits to a font in normal weight,
    // we will get the light version of that font (while the way supposed to work doesn't:
    // setting kCTFontWeightTrait to some value between -1.0 to 0.0 has no effect on font selection)
    if (fontDef.weight != QFont::Normal || symbolicTraits)
        ctFont = CTFontCreateCopyWithSymbolicTraits(baseFont, fontDef.pixelSize, &transform, symbolicTraits, symbolicTraits);

    // CTFontCreateCopyWithSymbolicTraits returns NULL if we ask for a trait that does
    // not exist for the given font. (for example italic)
    if (ctFont == 0) {
        ctFont = baseFont;
    }

    if (ctFont)
        return new QCoreTextFontEngine(ctFont, fontDef);
    return 0;
}

QT_END_NAMESPACE
