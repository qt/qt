/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcoretextfontdatabase.h"

#include <CoreText/CoreText.h>
#include <Foundation/Foundation.h>

#include <private/qcore_mac_p.h>
#include <private/qfontengine_coretext_p.h>

QT_BEGIN_NAMESPACE

QStringList QCoreTextFontDatabase::fallbacksForFamily(const QString family,
                                                      const QFont::Style &style,
                                                      const QFont::StyleHint &styleHint,
                                                      const QUnicodeTables::Script &script) const
{
    Q_UNUSED(family);
    Q_UNUSED(style);
    Q_UNUSED(script);
    if (fallbackLists.isEmpty())
        const_cast<QCoreTextFontDatabase *>(this)->populateFontDatabase();

    return fallbackLists[styleHint];
}

static QFont::StyleHint styleHintFromNSString(NSString *style)
{
    if ([style isEqual: @"sans-serif"])
        return QFont::SansSerif;
    else if ([style isEqual: @"monospace"])
        return QFont::Monospace;
    else if ([style isEqual: @"cursive"])
        return QFont::Cursive;
    else if ([style isEqual: @"serif"])
        return QFont::Serif;
    else if ([style isEqual: @"fantasy"])
        return QFont::Fantasy;
    else
        return QFont::AnyStyle;
}

static NSInteger languageMapSort(id obj1, id obj2, void *context)
{
    NSArray *map1 = (NSArray *) obj1;
    NSArray *map2 = (NSArray *) obj2;
    NSArray *languages = (NSArray *) context;

    NSString *lang1 = [map1 objectAtIndex: 0];
    NSString *lang2 = [map2 objectAtIndex: 0];

    return [languages indexOfObject: lang1] - [languages indexOfObject: lang2];
}

static QString familyNameFromPostScriptName(QHash<QString, QString> *psNameToFamily,
                                            NSString *psName)
{
    QString name = QCFString::toQString((CFStringRef) psName);
    if (psNameToFamily->contains(name)) {
        return psNameToFamily->value(name);
    } else {
        QCFType<CTFontRef> font = CTFontCreateWithName((CFStringRef) psName, 12.0, NULL);
        if (font) {
            QCFString family = CTFontCopyFamilyName(font);
            (*psNameToFamily)[name] = family;
            return family;
        }
    }

    return name;
}

void QCoreTextFontDatabase::populateFontDatabase()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    QCFType<CTFontCollectionRef> collection = CTFontCollectionCreateFromAvailableFonts(0);
    if (!collection)
        return;
    QCFType<CFArrayRef> fonts = CTFontCollectionCreateMatchingFontDescriptors(collection);
    if (!fonts)
        return;
    QSupportedWritingSystems supportedWritingSystems;
    for (int i = 0; i < QFontDatabase::WritingSystemsCount; ++i)
        supportedWritingSystems.setSupported((QFontDatabase::WritingSystem)i, true);
    QString foundry_name = "CoreText";
    const int numFonts = CFArrayGetCount(fonts);
    QHash<QString, QString> psNameToFamily;
    for (int i = 0; i < numFonts; ++i) {
        CTFontDescriptorRef font = (CTFontDescriptorRef)CFArrayGetValueAtIndex(fonts, i);

        QCFString family_name = (CFStringRef)CTFontDescriptorCopyAttribute(font, kCTFontFamilyNameAttribute);
//        QCFString style_name = (CFStringRef)CTFontDescriptorCopyAttribute(font, kCTFontStyleNameAttribute);

        QFont::Weight fontWeight = QFont::Normal;
        QFont::Style fontStyle = QFont::StyleNormal;
        if (QCFType<CFDictionaryRef> styles = (CFDictionaryRef)CTFontDescriptorCopyAttribute(font, kCTFontTraitsAttribute)) {
            if (CFNumberRef weight = (CFNumberRef)CFDictionaryGetValue(styles, kCTFontWeightTrait)) {
                Q_ASSERT(CFNumberIsFloatType(weight));
                double d;
                if (CFNumberGetValue(weight, kCFNumberDoubleType, &d)) {
                    if (d > 0.0)
                        fontWeight = QFont::Bold;
                }
            }
            if (CFNumberRef italic = (CFNumberRef)CFDictionaryGetValue(styles, kCTFontSlantTrait)) {
                Q_ASSERT(CFNumberIsFloatType(italic));
                double d;
                if (CFNumberGetValue(italic, kCFNumberDoubleType, &d)) {
                    if (d > 0.0)
                        fontStyle = QFont::StyleItalic;
                }
            }
        }

        int pixelSize = 0;
        if (QCFType<CFNumberRef> size = (CFNumberRef)CTFontDescriptorCopyAttribute(font, kCTFontSizeAttribute)) {
            if (CFNumberIsFloatType(size)) {
                double d;
                CFNumberGetValue(size, kCFNumberDoubleType, &d);
                pixelSize = d;
            } else {
                CFNumberGetValue(size, kCFNumberIntType, &pixelSize);
            }
        }
        QString familyName = QCFString::toQString(family_name);
        registerFont(familyName,
                     foundry_name,
                     fontWeight,
                     fontStyle,
                     QFont::Unstretched,
                     true,
                     true,
                     pixelSize,
                     supportedWritingSystems,
                     0);

        CFStringRef psName = (CFStringRef) CTFontDescriptorCopyAttribute(font,
                                                                         kCTFontNameAttribute);
        psNameToFamily[QCFString::toQString(psName)] = familyName;
        CFRelease(psName);
    }

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults stringArrayForKey: @"AppleLanguages"];

    NSDictionary *fallbackDict = [NSDictionary dictionaryWithContentsOfFile: @"/System/Library/Frameworks/CoreText.framework/DefaultFontFallbacks.plist"];
    for (NSString *style in [fallbackDict allKeys]) {
        NSArray *list = [fallbackDict valueForKey: style];
        QFont::StyleHint styleHint = styleHintFromNSString(style);
        QStringList fallbackList;
        for (id item in list) {
            if ([item isKindOfClass: [NSArray class]]) {
                NSArray *langs = [(NSArray *) item sortedArrayUsingFunction: languageMapSort
                                                                             context: languages];
                for (NSArray *map in langs)
                    fallbackList.append(familyNameFromPostScriptName(&psNameToFamily, [map objectAtIndex: 1]));
            } else if ([item isKindOfClass: [NSString class]]) {
                fallbackList.append(familyNameFromPostScriptName(&psNameToFamily, item));
            }
        }

        fallbackLists[styleHint] = fallbackList;
    }

    [pool release];
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
