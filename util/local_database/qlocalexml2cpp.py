#!/usr/bin/env python
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the test suite of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the Technology Preview License Agreement accompanying
## this package.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
##
##
##
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################

import sys
import xml.dom.minidom

def check_static_char_array_length(name, array):
    # some compilers like VC6 doesn't allow static arrays more than 64K bytes size.
    size = reduce(lambda x, y: x+len(escapedString(y)), array, 0)
    if size > 65535:
        print "\n\n\n#error Array %s is too long! " % name
        sys.stderr.write("\n\n\nERROR: the content of the array '%s' is too long: %d > 65535 " % (name, size))
        sys.exit(1)

def wrap_list(lst):
    def split(lst, size):
        for i in range(len(lst)/size+1):
            yield lst[i*size:(i+1)*size]
    return ",\n".join(map(lambda x: ", ".join(x), split(lst, 20)))

def firstChildElt(parent, name):
    child = parent.firstChild
    while child:
        if child.nodeType == parent.ELEMENT_NODE \
            and (not name or child.nodeName == name):
            return child
        child = child.nextSibling
    return False

def nextSiblingElt(sibling, name):
    sib = sibling.nextSibling
    while sib:
        if sib.nodeType == sibling.ELEMENT_NODE \
            and (not name or sib.nodeName == name):
            return sib
        sib = sib.nextSibling
    return False

def eltText(elt):
    result = ""
    child = elt.firstChild
    while child:
        if child.nodeType == elt.TEXT_NODE:
            if result:
                result += " "
            result += child.nodeValue
        child = child.nextSibling
    return result

def loadLanguageMap(doc):
    result = {}

    language_list_elt = firstChildElt(doc.documentElement, "languageList")
    language_elt = firstChildElt(language_list_elt, "language")
    while language_elt:
        language_id = int(eltText(firstChildElt(language_elt, "id")))
        language_name = eltText(firstChildElt(language_elt, "name"))
        language_code = eltText(firstChildElt(language_elt, "code"))
        result[language_id] = (language_name, language_code)
        language_elt = nextSiblingElt(language_elt, "language")

    return result

def loadCountryMap(doc):
    result = {}

    country_list_elt = firstChildElt(doc.documentElement, "countryList")
    country_elt = firstChildElt(country_list_elt, "country")
    while country_elt:
        country_id = int(eltText(firstChildElt(country_elt, "id")))
        country_name = eltText(firstChildElt(country_elt, "name"))
        country_code = eltText(firstChildElt(country_elt, "code"))
        result[country_id] = (country_name, country_code)
        country_elt = nextSiblingElt(country_elt, "country")

    return result

def loadDefaultMap(doc):
    result = {}

    list_elt = firstChildElt(doc.documentElement, "defaultCountryList")
    elt = firstChildElt(list_elt, "defaultCountry")
    while elt:
        country = eltText(firstChildElt(elt, "country"));
        language = eltText(firstChildElt(elt, "language"));
        result[language] = country;
        elt = nextSiblingElt(elt, "defaultCountry");
    return result

def fixedCountryName(name, dupes):
    if name in dupes:
        return name.replace(" ", "") + "Country"
    return name.replace(" ", "")

def fixedLanguageName(name, dupes):
    if name in dupes:
        return name.replace(" ", "") + "Language"
    return name.replace(" ", "")

def findDupes(country_map, language_map):
    country_set = set([ v[0] for a, v in country_map.iteritems() ])
    language_set = set([ v[0] for a, v in language_map.iteritems() ])
    return country_set & language_set

def languageNameToId(name, language_map):
    for key in language_map.keys():
        if language_map[key][0] == name:
            return key
    return -1

def countryNameToId(name, country_map):
    for key in country_map.keys():
        if country_map[key][0] == name:
            return key
    return -1

def convertFormat(format):
    result = ""
    i = 0
    while i < len(format):
        if format[i] == "'":
            result += "'"
            i += 1
            while i < len(format) and format[i] != "'":
                result += format[i]
                i += 1
            if i < len(format):
                result += "'"
                i += 1
        else:
            s = format[i:]
            if s.startswith("EEEE"):
                result += "dddd"
                i += 4
            elif s.startswith("EEE"):
                result += "ddd"
                i += 3
            elif s.startswith("a"):
                result += "AP"
                i += 1
            elif s.startswith("z"):
                result += "t"
                i += 1
            elif s.startswith("v"):
                i += 1
            else:
                result += format[i]
                i += 1

    return result

class Locale:
    def __init__(self, elt):
        self.language = eltText(firstChildElt(elt, "language"))
        self.country = eltText(firstChildElt(elt, "country"))
        self.decimal = int(eltText(firstChildElt(elt, "decimal")))
        self.group = int(eltText(firstChildElt(elt, "group")))
        self.listDelim = int(eltText(firstChildElt(elt, "list")))
        self.percent = int(eltText(firstChildElt(elt, "percent")))
        self.zero = int(eltText(firstChildElt(elt, "zero")))
        self.minus = int(eltText(firstChildElt(elt, "minus")))
        self.plus = int(eltText(firstChildElt(elt, "plus")))
        self.exp = int(eltText(firstChildElt(elt, "exp")))
        self.am = eltText(firstChildElt(elt, "am"))
        self.pm = eltText(firstChildElt(elt, "pm"))
        self.longDateFormat = convertFormat(eltText(firstChildElt(elt, "longDateFormat")))
        self.shortDateFormat = convertFormat(eltText(firstChildElt(elt, "shortDateFormat")))
        self.longTimeFormat = convertFormat(eltText(firstChildElt(elt, "longTimeFormat")))
        self.shortTimeFormat = convertFormat(eltText(firstChildElt(elt, "shortTimeFormat")))
        self.standaloneLongMonths = eltText(firstChildElt(elt, "standaloneLongMonths"))
        self.standaloneShortMonths = eltText(firstChildElt(elt, "standaloneShortMonths"))
        self.standaloneNarrowMonths = eltText(firstChildElt(elt, "standaloneNarrowMonths"))
        self.longMonths = eltText(firstChildElt(elt, "longMonths"))
        self.shortMonths = eltText(firstChildElt(elt, "shortMonths"))
        self.narrowMonths = eltText(firstChildElt(elt, "narrowMonths"))
        self.standaloneLongDays = eltText(firstChildElt(elt, "standaloneLongDays"))
        self.standaloneShortDays = eltText(firstChildElt(elt, "standaloneShortDays"))
        self.standaloneNarrowDays = eltText(firstChildElt(elt, "standaloneNarrowDays"))
        self.longDays = eltText(firstChildElt(elt, "longDays"))
        self.shortDays = eltText(firstChildElt(elt, "shortDays"))
        self.narrowDays = eltText(firstChildElt(elt, "narrowDays"))

def loadLocaleMap(doc, language_map, country_map):
    result = {}

    locale_list_elt = firstChildElt(doc.documentElement, "localeList")
    locale_elt = firstChildElt(locale_list_elt, "locale")
    while locale_elt:
        locale = Locale(locale_elt)
        language_id = languageNameToId(locale.language, language_map)
        if language_id == -1:
            sys.stderr.write("Cannot find a language id for %s\n" % locale.language)
        country_id = countryNameToId(locale.country, country_map)
        if country_id == -1:
            sys.stderr.write("Cannot find a country id for %s\n" % locale.country)
        result[(language_id, country_id)] = locale

        locale_elt = nextSiblingElt(locale_elt, "locale")

    return result

def compareLocaleKeys(key1, key2):
    if key1 == key2:
        return 0

    if key1[0] == key2[0]:
        l1 = compareLocaleKeys.locale_map[key1]
        l2 = compareLocaleKeys.locale_map[key2]

        if l1.language in compareLocaleKeys.default_map:
            default = compareLocaleKeys.default_map[l1.language]
            if l1.country == default:
                return -1
            if l2.country == default:
                return 1
    else:
        return key1[0] - key2[0]

    return key1[1] - key2[1]


def languageCount(language_id, locale_map):
    result = 0
    for key in locale_map.keys():
        if key[0] == language_id:
            result += 1
    return result

class StringDataToken:
    def __init__(self, index, length):
        self.index = index
        self.length = length
    def __str__(self):
        return " %d,%d " % (self.index, self.length)

class StringData:
    def __init__(self):
        self.data = []
        self.hash = {}
    def append(self, s):
        if s in self.hash:
            return self.hash[s]

        lst = map(lambda x: hex(ord(x)), s)
        index = len(self.data)
        if index >= 65535:
            print "\n\n\n#error Data index is too big!"
            sys.stderr.write ("\n\n\nERROR: index exceeds the uint16 range! index = %d\n" % index)
            sys.exit(1)
        size = len(lst)
        if size >= 65535:
            print "\n\n\n#error Data is too big!"
            sys.stderr.write ("\n\n\nERROR: data size exceeds the uint16 range! size = %d\n" % size)
            sys.exit(1)
        token = StringDataToken(index, size)
        self.hash[s] = token
        self.data += lst
        return token

def escapedString(s):
    result = ""
    i = 0
    while i < len(s):
        if s[i] == '"':
            result += '\\"'
            i += 1
        else:
            result += s[i]
            i += 1
    s = result

    line = ""
    need_escape = False
    result = ""
    for c in s:
        if ord(c) < 128 and (not need_escape or ord(c.lower()) < ord('a') or ord(c.lower()) > ord('f')):
            line += c
            need_escape = False
        else:
            line += "\\x%02x" % (ord(c))
            need_escape = True
        if len(line) > 80:
            result = result + "\n" + "\"" + line + "\""
            line = ""
    line += "\\0"
    result = result + "\n" + "\"" + line + "\""
    if result[0] == "\n":
        result = result[1:]
    return result

def printEscapedString(s):
    print escapedString(s);


def main():
    doc = xml.dom.minidom.parse("locale.xml")
    language_map = loadLanguageMap(doc)
    country_map = loadCountryMap(doc)
    default_map = loadDefaultMap(doc)
    locale_map = loadLocaleMap(doc, language_map, country_map)
    dupes = findDupes(language_map, country_map)

    # Language enum
    print "enum Language {"
    language = ""
    for key in language_map.keys():
        language = fixedLanguageName(language_map[key][0], dupes)
        print "    " + language + " = " + str(key) + ","
    print "    LastLanguage = " + language
    print "};"

    print

    # Country enum
    print "enum Country {"
    country = ""
    for key in country_map.keys():
        country = fixedCountryName(country_map[key][0], dupes)
        print "    " + country + " = " + str(key) + ","
    print "    LastCountry = " + country
    print "};"

    print

    # Locale index
    print "static const quint16 locale_index[] = {"
    print "     0, // unused"
    index = 0
    for key in language_map.keys():
        i = 0
        count = languageCount(key, locale_map)
        if count > 0:
            i = index
            index += count
        print "%6d, // %s" % (i, language_map[key][0])
    print "     0 // trailing 0"
    print "};"

    print

    date_format_data = StringData()
    time_format_data = StringData()
    months_data = StringData()
    standalone_months_data = StringData()
    days_data = StringData()
    am_data = StringData()
    pm_data = StringData()

    # Locale data
    print "static const QLocalePrivate locale_data[] = {"
    print "//      lang   terr    dec  group   list  prcnt   zero  minus  plus    exp sDtFmt lDtFmt sTmFmt lTmFmt ssMonth slMonth  sMonth lMonth  sDays  lDays  am,len      pm,len"

    locale_keys = locale_map.keys()
    compareLocaleKeys.default_map = default_map
    compareLocaleKeys.locale_map = locale_map
    locale_keys.sort(compareLocaleKeys)

    for key in locale_keys:
        l = locale_map[key]

        print "    { %6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%6d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s }, // %s/%s" \
                    % (key[0], key[1],
                        l.decimal,
                        l.group,
                        l.listDelim,
                        l.percent,
                        l.zero,
                        l.minus,
                        l.plus,
                        l.exp,
                        date_format_data.append(l.shortDateFormat),
                        date_format_data.append(l.longDateFormat),
                        time_format_data.append(l.shortTimeFormat),
                        time_format_data.append(l.longTimeFormat),
                        standalone_months_data.append(l.standaloneShortMonths),
                        standalone_months_data.append(l.standaloneLongMonths),
                        standalone_months_data.append(l.standaloneNarrowMonths),
                        months_data.append(l.shortMonths),
                        months_data.append(l.longMonths),
                        months_data.append(l.narrowMonths),
                        days_data.append(l.standaloneShortDays),
                        days_data.append(l.standaloneLongDays),
                        days_data.append(l.standaloneNarrowDays),
                        days_data.append(l.shortDays),
                        days_data.append(l.longDays),
                        days_data.append(l.narrowDays),
                        am_data.append(l.am),
                        pm_data.append(l.pm),
                        l.language,
                        l.country)
    print "    {      0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,0,     0,0,     0,0,     0,0,     0,0,     0,0,     0,0,    0,0,    0,0,    0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0,   0,0 }  // trailing 0s"
    print "};"

    print

    # Date format data
    #check_static_char_array_length("date_format", date_format_data.data)
    print "static const ushort date_format_data[] = {"
    print wrap_list(date_format_data.data)
    print "};"

    print

    # Time format data
    #check_static_char_array_length("time_format", time_format_data.data)
    print "static const ushort time_format_data[] = {"
    print wrap_list(time_format_data.data)
    print "};"

    print

    # Months data
    #check_static_char_array_length("months", months_data.data)
    print "static const ushort months_data[] = {"
    print wrap_list(months_data.data)
    print "};"

    print

    # Standalone months data
    #check_static_char_array_length("standalone_months", standalone_months_data.data)
    print "static const ushort standalone_months_data[] = {"
    print wrap_list(standalone_months_data.data)
    print "};"

    print

    # Days data
    #check_static_char_array_length("days", days_data.data)
    print "static const ushort days_data[] = {"
    print wrap_list(days_data.data)
    print "};"

    print

    # AM data
    #check_static_char_array_length("am", am_data.data)
    print "static const ushort am_data[] = {"
    print wrap_list(am_data.data)
    print "};"

    print

    # PM data
    #check_static_char_array_length("pm", am_data.data)
    print "static const ushort pm_data[] = {"
    print wrap_list(pm_data.data)
    print "};"

    print

    # Language name list
    print "static const char language_name_list[] ="
    print "\"Default\\0\""
    for key in language_map.keys():
        print "\"" + language_map[key][0] + "\\0\""
    print ";"

    print

    # Language name index
    print "static const quint16 language_name_index[] = {"
    print "     0, // Unused"
    index = 8
    for key in language_map.keys():
        language = language_map[key][0]
        print "%6d, // %s" % (index, language)
        index += len(language) + 1
    print "};"

    print

    # Country name list
    print "static const char country_name_list[] ="
    print "\"Default\\0\""
    for key in country_map.keys():
        if key == 0:
            continue
        print "\"" + country_map[key][0] + "\\0\""
    print ";"

    print

    # Country name index
    print "static const quint16 country_name_index[] = {"
    print "     0, // AnyCountry"
    index = 8
    for key in country_map.keys():
        if key == 0:
            continue
        country = country_map[key][0]
        print "%6d, // %s" % (index, country)
        index += len(country) + 1
    print "};"

    print

    # Language code list
    print "static const unsigned char language_code_list[] ="
    print "\"  \\0\" // Unused"
    for key in language_map.keys():
        code = language_map[key][1]
        if len(code) == 2:
            code += r"\0"
        print "\"%2s\" // %s" % (code, language_map[key][0])
    print ";"

    print

    # Country code list
    print "static const unsigned char country_code_list[] ="
    for key in country_map.keys():
        code = country_map[key][1]
        if len(code) == 2:
            code += "\\0"
        print "\"%2s\" // %s" % (code, country_map[key][0])
    print ";"


if __name__ == "__main__":
    main()
