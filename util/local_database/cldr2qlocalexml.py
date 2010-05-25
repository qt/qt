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

import os
import sys
import enumdata
import xpathlite
from  xpathlite import DraftResolution
from dateconverter import convert_date
import re

findEntry = xpathlite.findEntry
findEntryInFile = xpathlite._findEntryInFile

def ordStr(c):
    if len(c) == 1:
        return str(ord(c))
    return "##########"

# the following functions are supposed to fix the problem with QLocale
# returning a character instead of strings for QLocale::exponential()
# and others. So we fallback to default values in these cases.
def fixOrdStrExp(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord('e'))
def fixOrdStrPercent(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord('%'))
def fixOrdStrList(c):
    if len(c) == 1:
        return str(ord(c))
    return str(ord(';'))

def generateLocaleInfo(path):
    (dir_name, file_name) = os.path.split(path)

    if not path.endswith(".xml"):
        return {}
    language_code = findEntryInFile(path, "identity/language", attribute="type")[0]
    if language_code == 'root':
        # just skip it
        return {}
    country_code = findEntryInFile(path, "identity/territory", attribute="type")[0]
    script_code = findEntryInFile(path, "identity/script", attribute="type")[0]
    variant_code = findEntryInFile(path, "identity/variant", attribute="type")[0]

    # we should handle fully qualified names with the territory
    if not country_code:
        return {}

    # we do not support scripts and variants
    if variant_code or script_code:
        return {}

    language_id = enumdata.languageCodeToId(language_code)
    if language_id == -1:
        sys.stderr.write("unnknown language code \"" + language_code + "\"\n")
        return {}
    language = enumdata.language_list[language_id][0]

    country_id = enumdata.countryCodeToId(country_code)
    country = ""
    if country_id != -1:
        country = enumdata.country_list[country_id][0]
    if country == "":
        sys.stderr.write("unnknown country code \"" + country_code + "\"\n")
        return {}

    # So we say we accept only those values that have "contributed" or
    # "approved" resolution. see http://www.unicode.org/cldr/process.html
    # But we only respect the resolution for new datas for backward
    # compatibility.
    draft = DraftResolution.contributed

    result = {}
    result['language'] = language
    result['country'] = country
    result['language_code'] = language_code
    result['country_code'] = country_code
    result['script_code'] = script_code
    result['variant_code'] = variant_code
    result['language_id'] = language_id
    result['country_id'] = country_id

    numbering_system = None
    try:
        numbering_system = findEntry(path, "numbers/defaultNumberingSystem")
    except:
        pass
    def get_number_in_system(path, xpath, numbering_system):
        if numbering_system:
            try:
                return findEntry(path, xpath + "[numberSystem=" + numbering_system + "]")
            except xpathlite.Error:
                pass
        return findEntry(path, xpath)
    result['decimal'] = get_number_in_system(path, "numbers/symbols/decimal", numbering_system)
    result['group'] = get_number_in_system(path, "numbers/symbols/group", numbering_system)
    result['list'] = get_number_in_system(path, "numbers/symbols/list", numbering_system)
    result['percent'] = get_number_in_system(path, "numbers/symbols/percentSign", numbering_system)
    result['zero'] = get_number_in_system(path, "numbers/symbols/nativeZeroDigit", numbering_system)
    result['minus'] = get_number_in_system(path, "numbers/symbols/minusSign", numbering_system)
    result['plus'] = get_number_in_system(path, "numbers/symbols/plusSign", numbering_system)
    result['exp'] = get_number_in_system(path, "numbers/symbols/exponential", numbering_system).lower()
    result['am'] = findEntry(path, "dates/calendars/calendar[gregorian]/dayPeriods/dayPeriodContext[format]/dayPeriodWidth[wide]/dayPeriod[am]", draft)
    result['pm'] = findEntry(path, "dates/calendars/calendar[gregorian]/dayPeriods/dayPeriodContext[format]/dayPeriodWidth[wide]/dayPeriod[pm]", draft)
    result['longDateFormat'] = convert_date(findEntry(path, "dates/calendars/calendar[gregorian]/dateFormats/dateFormatLength[full]/dateFormat/pattern"))
    result['shortDateFormat'] = convert_date(findEntry(path, "dates/calendars/calendar[gregorian]/dateFormats/dateFormatLength[short]/dateFormat/pattern"))
    result['longTimeFormat'] = convert_date(findEntry(path, "dates/calendars/calendar[gregorian]/timeFormats/timeFormatLength[full]/timeFormat/pattern"))
    result['shortTimeFormat'] = convert_date(findEntry(path, "dates/calendars/calendar[gregorian]/timeFormats/timeFormatLength[short]/timeFormat/pattern"))

    standalone_long_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[stand-alone]/monthWidth[wide]/month"
    result['standaloneLongMonths'] \
        = findEntry(path, standalone_long_month_path + "[1]") + ";" \
        + findEntry(path, standalone_long_month_path + "[2]") + ";" \
        + findEntry(path, standalone_long_month_path + "[3]") + ";" \
        + findEntry(path, standalone_long_month_path + "[4]") + ";" \
        + findEntry(path, standalone_long_month_path + "[5]") + ";" \
        + findEntry(path, standalone_long_month_path + "[6]") + ";" \
        + findEntry(path, standalone_long_month_path + "[7]") + ";" \
        + findEntry(path, standalone_long_month_path + "[8]") + ";" \
        + findEntry(path, standalone_long_month_path + "[9]") + ";" \
        + findEntry(path, standalone_long_month_path + "[10]") + ";" \
        + findEntry(path, standalone_long_month_path + "[11]") + ";" \
        + findEntry(path, standalone_long_month_path + "[12]") + ";"

    standalone_short_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[stand-alone]/monthWidth[abbreviated]/month"
    result['standaloneShortMonths'] \
        = findEntry(path, standalone_short_month_path + "[1]") + ";" \
        + findEntry(path, standalone_short_month_path + "[2]") + ";" \
        + findEntry(path, standalone_short_month_path + "[3]") + ";" \
        + findEntry(path, standalone_short_month_path + "[4]") + ";" \
        + findEntry(path, standalone_short_month_path + "[5]") + ";" \
        + findEntry(path, standalone_short_month_path + "[6]") + ";" \
        + findEntry(path, standalone_short_month_path + "[7]") + ";" \
        + findEntry(path, standalone_short_month_path + "[8]") + ";" \
        + findEntry(path, standalone_short_month_path + "[9]") + ";" \
        + findEntry(path, standalone_short_month_path + "[10]") + ";" \
        + findEntry(path, standalone_short_month_path + "[11]") + ";" \
        + findEntry(path, standalone_short_month_path + "[12]") + ";"

    standalone_narrow_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[stand-alone]/monthWidth[narrow]/month"
    result['standaloneNarrowMonths'] \
        = findEntry(path, standalone_narrow_month_path + "[1]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[2]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[3]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[4]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[5]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[6]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[7]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[8]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[9]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[10]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[11]") + ";" \
        + findEntry(path, standalone_narrow_month_path + "[12]") + ";"

    long_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[format]/monthWidth[wide]/month"
    result['longMonths'] \
        = findEntry(path, long_month_path + "[1]") + ";" \
        + findEntry(path, long_month_path + "[2]") + ";" \
        + findEntry(path, long_month_path + "[3]") + ";" \
        + findEntry(path, long_month_path + "[4]") + ";" \
        + findEntry(path, long_month_path + "[5]") + ";" \
        + findEntry(path, long_month_path + "[6]") + ";" \
        + findEntry(path, long_month_path + "[7]") + ";" \
        + findEntry(path, long_month_path + "[8]") + ";" \
        + findEntry(path, long_month_path + "[9]") + ";" \
        + findEntry(path, long_month_path + "[10]") + ";" \
        + findEntry(path, long_month_path + "[11]") + ";" \
        + findEntry(path, long_month_path + "[12]") + ";"

    short_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[format]/monthWidth[abbreviated]/month"
    result['shortMonths'] \
        = findEntry(path, short_month_path + "[1]") + ";" \
        + findEntry(path, short_month_path + "[2]") + ";" \
        + findEntry(path, short_month_path + "[3]") + ";" \
        + findEntry(path, short_month_path + "[4]") + ";" \
        + findEntry(path, short_month_path + "[5]") + ";" \
        + findEntry(path, short_month_path + "[6]") + ";" \
        + findEntry(path, short_month_path + "[7]") + ";" \
        + findEntry(path, short_month_path + "[8]") + ";" \
        + findEntry(path, short_month_path + "[9]") + ";" \
        + findEntry(path, short_month_path + "[10]") + ";" \
        + findEntry(path, short_month_path + "[11]") + ";" \
        + findEntry(path, short_month_path + "[12]") + ";"

    narrow_month_path = "dates/calendars/calendar[gregorian]/months/monthContext[format]/monthWidth[narrow]/month"
    result['narrowMonths'] \
        = findEntry(path, narrow_month_path + "[1]") + ";" \
        + findEntry(path, narrow_month_path + "[2]") + ";" \
        + findEntry(path, narrow_month_path + "[3]") + ";" \
        + findEntry(path, narrow_month_path + "[4]") + ";" \
        + findEntry(path, narrow_month_path + "[5]") + ";" \
        + findEntry(path, narrow_month_path + "[6]") + ";" \
        + findEntry(path, narrow_month_path + "[7]") + ";" \
        + findEntry(path, narrow_month_path + "[8]") + ";" \
        + findEntry(path, narrow_month_path + "[9]") + ";" \
        + findEntry(path, narrow_month_path + "[10]") + ";" \
        + findEntry(path, narrow_month_path + "[11]") + ";" \
        + findEntry(path, narrow_month_path + "[12]") + ";"

    long_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[format]/dayWidth[wide]/day"
    result['longDays'] \
        = findEntry(path, long_day_path + "[sun]") + ";" \
        + findEntry(path, long_day_path + "[mon]") + ";" \
        + findEntry(path, long_day_path + "[tue]") + ";" \
        + findEntry(path, long_day_path + "[wed]") + ";" \
        + findEntry(path, long_day_path + "[thu]") + ";" \
        + findEntry(path, long_day_path + "[fri]") + ";" \
        + findEntry(path, long_day_path + "[sat]") + ";"

    short_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[format]/dayWidth[abbreviated]/day"
    result['shortDays'] \
        = findEntry(path, short_day_path + "[sun]") + ";" \
        + findEntry(path, short_day_path + "[mon]") + ";" \
        + findEntry(path, short_day_path + "[tue]") + ";" \
        + findEntry(path, short_day_path + "[wed]") + ";" \
        + findEntry(path, short_day_path + "[thu]") + ";" \
        + findEntry(path, short_day_path + "[fri]") + ";" \
        + findEntry(path, short_day_path + "[sat]") + ";"

    narrow_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[format]/dayWidth[narrow]/day"
    result['narrowDays'] \
        = findEntry(path, narrow_day_path + "[sun]") + ";" \
        + findEntry(path, narrow_day_path + "[mon]") + ";" \
        + findEntry(path, narrow_day_path + "[tue]") + ";" \
        + findEntry(path, narrow_day_path + "[wed]") + ";" \
        + findEntry(path, narrow_day_path + "[thu]") + ";" \
        + findEntry(path, narrow_day_path + "[fri]") + ";" \
        + findEntry(path, narrow_day_path + "[sat]") + ";"

    standalone_long_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[stand-alone]/dayWidth[wide]/day"
    result['standaloneLongDays'] \
        = findEntry(path, standalone_long_day_path + "[sun]") + ";" \
        + findEntry(path, standalone_long_day_path + "[mon]") + ";" \
        + findEntry(path, standalone_long_day_path + "[tue]") + ";" \
        + findEntry(path, standalone_long_day_path + "[wed]") + ";" \
        + findEntry(path, standalone_long_day_path + "[thu]") + ";" \
        + findEntry(path, standalone_long_day_path + "[fri]") + ";" \
        + findEntry(path, standalone_long_day_path + "[sat]") + ";"

    standalone_short_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[stand-alone]/dayWidth[abbreviated]/day"
    result['standaloneShortDays'] \
        = findEntry(path, standalone_short_day_path + "[sun]") + ";" \
        + findEntry(path, standalone_short_day_path + "[mon]") + ";" \
        + findEntry(path, standalone_short_day_path + "[tue]") + ";" \
        + findEntry(path, standalone_short_day_path + "[wed]") + ";" \
        + findEntry(path, standalone_short_day_path + "[thu]") + ";" \
        + findEntry(path, standalone_short_day_path + "[fri]") + ";" \
        + findEntry(path, standalone_short_day_path + "[sat]") + ";"

    standalone_narrow_day_path = "dates/calendars/calendar[gregorian]/days/dayContext[stand-alone]/dayWidth[narrow]/day"
    result['standaloneNarrowDays'] \
        = findEntry(path, standalone_narrow_day_path + "[sun]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[mon]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[tue]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[wed]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[thu]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[fri]") + ";" \
        + findEntry(path, standalone_narrow_day_path + "[sat]") + ";"


    return result

def addEscapes(s):
    result = ''
    for c in s:
        n = ord(c)
        if n < 128:
            result += c
        else:
            result += "\\x"
            result += "%02x" % (n)
    return result

def unicodeStr(s):
    utf8 = s.encode('utf-8')
    return "<size>" + str(len(utf8)) + "</size><data>" + addEscapes(utf8) + "</data>"

def usage():
    print "Usage: cldr2qlocalexml.py <path-to-cldr-main>"
    sys.exit()

if len(sys.argv) != 2:
    usage()

cldr_dir = sys.argv[1]

if not os.path.isdir(cldr_dir):
    usage()

cldr_files = os.listdir(cldr_dir)

locale_database = {}
for file in cldr_files:
    l = generateLocaleInfo(cldr_dir + "/" + file)
    if not l:
        sys.stderr.write("skipping file \"" + file + "\"\n")
        continue

    locale_database[(l['language_id'], l['country_id'], l['script_code'], l['variant_code'])] = l

locale_keys = locale_database.keys()
locale_keys.sort()

print "<localeDatabase>"
print "    <languageList>"
for id in enumdata.language_list:
    l = enumdata.language_list[id]
    print "        <language>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </language>"
print "    </languageList>"

print "    <countryList>"
for id in enumdata.country_list:
    l = enumdata.country_list[id]
    print "        <country>"
    print "            <name>" + l[0] + "</name>"
    print "            <id>" + str(id) + "</id>"
    print "            <code>" + l[1] + "</code>"
    print "        </country>"
print "    </countryList>"

print \
"    <defaultCountryList>\n\
        <defaultCountry>\n\
            <language>Afrikaans</language>\n\
            <country>SouthAfrica</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Afan</language>\n\
            <country>Ethiopia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Afar</language>\n\
            <country>Djibouti</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Arabic</language>\n\
            <country>SaudiArabia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Chinese</language>\n\
            <country>China</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Dutch</language>\n\
            <country>Netherlands</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>English</language>\n\
            <country>UnitedStates</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>French</language>\n\
            <country>France</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>German</language>\n\
            <country>Germany</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Greek</language>\n\
            <country>Greece</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Italian</language>\n\
            <country>Italy</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Malay</language>\n\
            <country>Malaysia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Portuguese</language>\n\
            <country>Portugal</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Russian</language>\n\
            <country>RussianFederation</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Serbian</language>\n\
            <country>SerbiaAndMontenegro</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>SerboCroatian</language>\n\
            <country>SerbiaAndMontenegro</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Somali</language>\n\
            <country>Somalia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Spanish</language>\n\
            <country>Spain</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Swahili</language>\n\
            <country>Kenya</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Swedish</language>\n\
            <country>Sweden</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Tigrinya</language>\n\
            <country>Eritrea</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Uzbek</language>\n\
            <country>Uzbekistan</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Persian</language>\n\
            <country>Iran</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Mongolian</language>\n\
            <country>Mongolia</country>\n\
        </defaultCountry>\n\
        <defaultCountry>\n\
            <language>Nepali</language>\n\
            <country>Nepal</country>\n\
        </defaultCountry>\n\
    </defaultCountryList>"

print "    <localeList>"
print \
"        <locale>\n\
            <language>C</language>\n\
            <country>AnyCountry</country>\n\
            <decimal>46</decimal>\n\
            <group>44</group>\n\
            <list>59</list>\n\
            <percent>37</percent>\n\
            <zero>48</zero>\n\
            <minus>45</minus>\n\
            <plus>43</plus>\n\
            <exp>101</exp>\n\
            <am>AM</am>\n\
            <pm>PM</pm>\n\
            <longDateFormat>EEEE, d MMMM yyyy</longDateFormat>\n\
            <shortDateFormat>d MMM yyyy</shortDateFormat>\n\
            <longTimeFormat>HH:mm:ss z</longTimeFormat>\n\
            <shortTimeFormat>HH:mm:ss</shortTimeFormat>\n\
            <standaloneLongMonths>January;February;March;April;May;June;July;August;September;October;November;December;</standaloneLongMonths>\n\
            <standaloneShortMonths>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec;</standaloneShortMonths>\n\
            <standaloneNarrowMonths>J;F;M;A;M;J;J;A;S;O;N;D;</standaloneNarrowMonths>\n\
            <longMonths>January;February;March;April;May;June;July;August;September;October;November;December;</longMonths>\n\
            <shortMonths>Jan;Feb;Mar;Apr;May;Jun;Jul;Aug;Sep;Oct;Nov;Dec;</shortMonths>\n\
            <narrowMonths>1;2;3;4;5;6;7;8;9;10;11;12;</narrowMonths>\n\
            <longDays>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday;</longDays>\n\
            <shortDays>Sun;Mon;Tue;Wed;Thu;Fri;Sat;</shortDays>\n\
            <narrowDays>7;1;2;3;4;5;6;</narrowDays>\n\
            <standaloneLongDays>Sunday;Monday;Tuesday;Wednesday;Thursday;Friday;Saturday;</standaloneLongDays>\n\
            <standaloneShortDays>Sun;Mon;Tue;Wed;Thu;Fri;Sat;</standaloneShortDays>\n\
            <standaloneNarrowDays>S;M;T;W;T;F;S;</standaloneNarrowDays>\n\
        </locale>"

for key in locale_keys:
    l = locale_database[key]

    print "        <locale>"
    print "            <language>" + l['language']        + "</language>"
    print "            <country>"  + l['country']         + "</country>"
    print "            <languagecode>" + l['language_code']        + "</languagecode>"
    print "            <countrycode>"  + l['country_code']         + "</countrycode>"
    print "            <decimal>"  + ordStr(l['decimal']) + "</decimal>"
    print "            <group>"    + ordStr(l['group'])   + "</group>"
    print "            <list>"     + fixOrdStrList(l['list'])    + "</list>"
    print "            <percent>"  + fixOrdStrPercent(l['percent']) + "</percent>"
    print "            <zero>"     + ordStr(l['zero'])    + "</zero>"
    print "            <minus>"    + ordStr(l['minus'])   + "</minus>"
    print "            <plus>"     + ordStr(l['plus'])   + "</plus>"
    print "            <exp>"      + fixOrdStrExp(l['exp'])     + "</exp>"
    print "            <am>"       + l['am'].encode('utf-8') + "</am>"
    print "            <pm>"       + l['pm'].encode('utf-8') + "</pm>"
    print "            <longDateFormat>"  + l['longDateFormat'].encode('utf-8')  + "</longDateFormat>"
    print "            <shortDateFormat>" + l['shortDateFormat'].encode('utf-8') + "</shortDateFormat>"
    print "            <longTimeFormat>"  + l['longTimeFormat'].encode('utf-8')  + "</longTimeFormat>"
    print "            <shortTimeFormat>" + l['shortTimeFormat'].encode('utf-8') + "</shortTimeFormat>"
    print "            <standaloneLongMonths>" + l['standaloneLongMonths'].encode('utf-8')      + "</standaloneLongMonths>"
    print "            <standaloneShortMonths>"+ l['standaloneShortMonths'].encode('utf-8')      + "</standaloneShortMonths>"
    print "            <standaloneNarrowMonths>"+ l['standaloneNarrowMonths'].encode('utf-8')      + "</standaloneNarrowMonths>"
    print "            <longMonths>"      + l['longMonths'].encode('utf-8')      + "</longMonths>"
    print "            <shortMonths>"     + l['shortMonths'].encode('utf-8')     + "</shortMonths>"
    print "            <narrowMonths>"     + l['narrowMonths'].encode('utf-8')     + "</narrowMonths>"
    print "            <longDays>"        + l['longDays'].encode('utf-8')        + "</longDays>"
    print "            <shortDays>"       + l['shortDays'].encode('utf-8')       + "</shortDays>"
    print "            <narrowDays>"       + l['narrowDays'].encode('utf-8')       + "</narrowDays>"
    print "            <standaloneLongDays>" + l['standaloneLongDays'].encode('utf-8')        + "</standaloneLongDays>"
    print "            <standaloneShortDays>" + l['standaloneShortDays'].encode('utf-8')       + "</standaloneShortDays>"
    print "            <standaloneNarrowDays>" + l['standaloneNarrowDays'].encode('utf-8')       + "</standaloneNarrowDays>"
    print "        </locale>"
print "    </localeList>"
print "</localeDatabase>"
