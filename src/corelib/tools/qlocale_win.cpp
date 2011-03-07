/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocale_p.h"
#include "qlocale_tools_p.h"

#include "qstringlist.h"
#include "qvariant.h"
#include "qdatetime.h"

#include "private/qsystemlibrary_p.h"

#include "qdebug.h"

#if defined(Q_WS_WIN)
#   include "qt_windows.h"
#   include <time.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef MUI_LANGUAGE_NAME
#define MUI_LANGUAGE_NAME 0x8
#endif

static const char *winLangCodeToIsoName(int code);
static QString winIso639LangName(LCID id = LOCALE_USER_DEFAULT);
static QString winIso3116CtryName(LCID id = LOCALE_USER_DEFAULT);

static QString qt_getLocaleInfo(LCID lcid, LCTYPE type, int maxlen = 0)
{
    QVarLengthArray<wchar_t, 64> buf(maxlen ? maxlen : 64);
    if (!GetLocaleInfo(lcid, type, buf.data(), buf.size()))
        return QString();
    return QString::fromWCharArray(buf.data());
}
static int qt_getLocaleInfo_int(LCID lcid, LCTYPE type, int maxlen = 0)
{
    QString str = qt_getLocaleInfo(lcid, type, maxlen);
    bool ok = false;
    int v = str.toInt(&ok);
    return ok ? v : 0;
}

static QString getWinLocaleInfo(LCTYPE type)
{
    LCID id = GetUserDefaultLCID();
    int cnt = GetLocaleInfo(id, type, 0, 0) * 2;

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", (int)type);
        return QString();
    }

    QByteArray buff(cnt, 0);

    cnt = GetLocaleInfo(id, type, reinterpret_cast<wchar_t*>(buff.data()), buff.size() / 2);

    if (cnt == 0) {
        qWarning("QLocale: empty windows locale info (%d)", (int)type);
        return QString();
    }

    return QString::fromWCharArray(reinterpret_cast<const wchar_t *>(buff.data()));
}

static QByteArray envVarLocale()
{
    static QByteArray lang = qgetenv("LANG");
    return lang;
}

QByteArray getWinLocaleName(LCID id = LOCALE_USER_DEFAULT)
{
    QByteArray result;
    if (id == LOCALE_USER_DEFAULT) {
        result = envVarLocale();
        QString lang, script, cntry;
        if ( result == "C" || (!result.isEmpty()
                && splitLocaleName(QString::fromLocal8Bit(result), lang, script, cntry)) ) {
            long id = 0;
            bool ok = false;
            id = qstrtoll(result.data(), 0, 0, &ok);
            if ( !ok || id == 0 || id < INT_MIN || id > INT_MAX )
                return result;
            else
                return winLangCodeToIsoName( (int)id );
        }
    }

#if defined(Q_OS_WINCE)
    result = winLangCodeToIsoName(id != LOCALE_USER_DEFAULT ? id : GetUserDefaultLCID());
#else
    if (id == LOCALE_USER_DEFAULT)
        id = GetUserDefaultLCID();
    QString resultuage = winIso639LangName(id);
    QString country = winIso3116CtryName(id);
    result = resultuage.toLatin1();
    if (!country.isEmpty()) {
        result += '_';
        result += country.toLatin1();
    }
#endif

    return result;
}

Q_CORE_EXPORT QLocale qt_localeFromLCID(LCID id)
{
    return QLocale(QString::fromLatin1(getWinLocaleName(id)));
}

static QString winToQtFormat(const QString &sys_fmt)
{
    QString result;
    int i = 0;

    while (i < sys_fmt.size()) {
        if (sys_fmt.at(i).unicode() == QLatin1Char('\'')) {
            QString text = readEscapedFormatString(sys_fmt, &i);
            if (text == QLatin1String("'"))
                result += QLatin1String("''");
            else
                result += QString(QLatin1Char('\'') + text + QLatin1Char('\''));
            continue;
        }

        QChar c = sys_fmt.at(i);
        int repeat = repeatCount(sys_fmt, i);

        switch (c.unicode()) {
            // Date
            case 'y':
                if (repeat > 5)
                    repeat = 5;
                else if (repeat == 3)
                    repeat = 2;
                switch (repeat) {
                    case 1:
                        result += QLatin1String("yy"); // "y" unsupported by Qt, use "yy"
                        break;
                    case 5:
                        result += QLatin1String("yyyy"); // "yyyyy" same as "yyyy" on Windows
                        break;
                    default:
                        result += QString(repeat, QLatin1Char('y'));
                        break;
                }
                break;
            case 'g':
                if (repeat > 2)
                    repeat = 2;
                switch (repeat) {
                    case 2:
                        break; // no equivalent of "gg" in Qt
                    default:
                        result += QLatin1Char('g');
                        break;
                }
                break;
            case 't':
                if (repeat > 2)
                    repeat = 2;
                result += QLatin1String("AP"); // "t" unsupported, use "AP"
                break;
            default:
                result += QString(repeat, c);
                break;
        }

        i += repeat;
    }

    return result;
}



static QString winDateToString(const QDate &date, DWORD flags)
{
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    st.wYear = date.year();
    st.wMonth = date.month();
    st.wDay = date.day();

    LCID id = GetUserDefaultLCID();

    wchar_t buf[255];
    if (GetDateFormat(id, flags, &st, 0, buf, 255))
        return QString::fromWCharArray(buf);

    return QString();
}

static QString winTimeToString(const QTime &time, bool longFormat)
{
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    st.wHour = time.hour();
    st.wMinute = time.minute();
    st.wSecond = time.second();
    st.wMilliseconds = 0;

    DWORD flags = 0;
    if (!longFormat && QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7)
        flags = 2; // TIME_NOSECONDS

    LCID id = GetUserDefaultLCID();

    wchar_t buf[255];
    if (GetTimeFormat(id, flags, &st, 0, buf, 255))
        return QString::fromWCharArray(buf);

    return QString();
}

static QString winDayName(int day, bool short_format)
{
    static const LCTYPE short_day_map[]
        = { LOCALE_SABBREVDAYNAME1, LOCALE_SABBREVDAYNAME2,
            LOCALE_SABBREVDAYNAME3, LOCALE_SABBREVDAYNAME4, LOCALE_SABBREVDAYNAME5,
            LOCALE_SABBREVDAYNAME6, LOCALE_SABBREVDAYNAME7 };

    static const LCTYPE long_day_map[]
        = { LOCALE_SDAYNAME1, LOCALE_SDAYNAME2,
            LOCALE_SDAYNAME3, LOCALE_SDAYNAME4, LOCALE_SDAYNAME5,
            LOCALE_SDAYNAME6, LOCALE_SDAYNAME7 };

    day -= 1;

    LCTYPE type = short_format
                    ? short_day_map[day] : long_day_map[day];
    return getWinLocaleInfo(type);
}

static QString winMonthName(int month, bool short_format)
{
    static const LCTYPE short_month_map[]
        = { LOCALE_SABBREVMONTHNAME1, LOCALE_SABBREVMONTHNAME2, LOCALE_SABBREVMONTHNAME3,
            LOCALE_SABBREVMONTHNAME4, LOCALE_SABBREVMONTHNAME5, LOCALE_SABBREVMONTHNAME6,
            LOCALE_SABBREVMONTHNAME7, LOCALE_SABBREVMONTHNAME8, LOCALE_SABBREVMONTHNAME9,
            LOCALE_SABBREVMONTHNAME10, LOCALE_SABBREVMONTHNAME11, LOCALE_SABBREVMONTHNAME12 };

    static const LCTYPE long_month_map[]
        = { LOCALE_SMONTHNAME1, LOCALE_SMONTHNAME2, LOCALE_SMONTHNAME3,
            LOCALE_SMONTHNAME4, LOCALE_SMONTHNAME5, LOCALE_SMONTHNAME6,
            LOCALE_SMONTHNAME7, LOCALE_SMONTHNAME8, LOCALE_SMONTHNAME9,
            LOCALE_SMONTHNAME10, LOCALE_SMONTHNAME11, LOCALE_SMONTHNAME12 };

    month -= 1;
    if (month < 0 || month > 11)
    return QString();

    LCTYPE type = short_format ? short_month_map[month] : long_month_map[month];
    return getWinLocaleInfo(type);
}

static QLocale::MeasurementSystem winSystemMeasurementSystem()
{
    LCID id = GetUserDefaultLCID();
    wchar_t output[2];

    if (GetLocaleInfo(id, LOCALE_IMEASURE, output, 2)) {
        QString iMeasure = QString::fromWCharArray(output);
        if (iMeasure == QLatin1String("1")) {
            return QLocale::ImperialSystem;
        }
    }

    return QLocale::MetricSystem;
}

static QString winSystemAMText()
{
    LCID id = GetUserDefaultLCID();
    wchar_t output[15]; // maximum length including  terminating zero character for Win2003+

    if (GetLocaleInfo(id, LOCALE_S1159, output, 15)) {
        return QString::fromWCharArray(output);
    }

    return QString();
}

static QString winSystemPMText()
{
    LCID id = GetUserDefaultLCID();
    wchar_t output[15]; // maximum length including  terminating zero character for Win2003+

    if (GetLocaleInfo(id, LOCALE_S2359, output, 15)) {
        return QString::fromWCharArray(output);
    }

    return QString();
}

static quint8 winSystemFirstDayOfWeek()
{
    LCID id = GetUserDefaultLCID();
    wchar_t output[4]; // maximum length including  terminating zero character for Win2003+

    if (GetLocaleInfo(id, LOCALE_IFIRSTDAYOFWEEK, output, 4))
        return QString::fromWCharArray(output).toUInt()+1;

    return 1;
}

QString winCurrencySymbol(QLocale::CurrencySymbolFormat format)
{
    LCID lcid = GetUserDefaultLCID();
    wchar_t buf[13];
    switch (format) {
    case QLocale::CurrencySymbol:
        if (GetLocaleInfo(lcid, LOCALE_SCURRENCY, buf, 13))
            return QString::fromWCharArray(buf);
        break;
    case QLocale::CurrencyIsoCode:
        if (GetLocaleInfo(lcid, LOCALE_SINTLSYMBOL, buf, 9))
            return QString::fromWCharArray(buf);
        break;
    case QLocale::CurrencyDisplayName: {
        QVarLengthArray<wchar_t, 64> buf(64);
        if (!GetLocaleInfo(lcid, LOCALE_SNATIVECURRNAME, buf.data(), buf.size())) {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                break;
            buf.resize(255); // should be large enough, right?
            if (!GetLocaleInfo(lcid, LOCALE_SNATIVECURRNAME, buf.data(), buf.size()))
                break;
        }
        return QString::fromWCharArray(buf.data());
    }
    default:
        break;
    }
    return QString();
}

#ifndef QT_NO_SYSTEMLOCALE
static QString winFormatCurrency(const QSystemLocale::CurrencyToStringArgument &arg)
{
    QString value;
    switch (arg.value.type()) {
    case QVariant::Int:
        value = QLocalePrivate::longLongToString(QLatin1Char('0'), QLatin1Char(','), QLatin1Char('+'), QLatin1Char('-'),
                                                 arg.value.toInt(), -1, 10, -1, QLocale::OmitGroupSeparator);
        break;
    case QVariant::UInt:
        value = QLocalePrivate::unsLongLongToString(QLatin1Char('0'), QLatin1Char(','), QLatin1Char('+'),
                                                    arg.value.toUInt(), -1, 10, -1, QLocale::OmitGroupSeparator);
        break;
    case QVariant::Double:
        value = QLocalePrivate::doubleToString(QLatin1Char('0'), QLatin1Char('+'), QLatin1Char('-'),
                                               QLatin1Char(' '), QLatin1Char(','), QLatin1Char('.'),
                                               arg.value.toDouble(), -1, QLocalePrivate::DFDecimal, -1, QLocale::OmitGroupSeparator);
        break;
    case QVariant::LongLong:
        value = QLocalePrivate::longLongToString(QLatin1Char('0'), QLatin1Char(','), QLatin1Char('+'), QLatin1Char('-'),
                                                 arg.value.toLongLong(), -1, 10, -1, QLocale::OmitGroupSeparator);
        break;
    case QVariant::ULongLong:
        value = QLocalePrivate::unsLongLongToString(QLatin1Char('0'), QLatin1Char(','), QLatin1Char('+'),
                                                    arg.value.toULongLong(), -1, 10, -1, QLocale::OmitGroupSeparator);
        break;
    default:
        return QString();
    }

    QVarLengthArray<wchar_t, 64> out(64);
    LCID lcid = GetUserDefaultLCID();

    QString decimalSep;
    QString thousandSep;
    CURRENCYFMT format;
    CURRENCYFMT *pformat = NULL;
    if (!arg.symbol.isEmpty()) {
        format.NumDigits = qt_getLocaleInfo_int(lcid, LOCALE_ICURRDIGITS);
        format.LeadingZero = qt_getLocaleInfo_int(lcid, LOCALE_ILZERO);
        decimalSep = qt_getLocaleInfo(lcid, LOCALE_SMONDECIMALSEP);
        format.lpDecimalSep = (wchar_t *)decimalSep.utf16();
        thousandSep = qt_getLocaleInfo(lcid, LOCALE_SMONTHOUSANDSEP);
        format.lpThousandSep = (wchar_t *)thousandSep.utf16();
        format.NegativeOrder = qt_getLocaleInfo_int(lcid, LOCALE_INEGCURR);
        format.PositiveOrder = qt_getLocaleInfo_int(lcid, LOCALE_ICURRENCY);
        format.lpCurrencySymbol = (wchar_t *)arg.symbol.utf16();

        // grouping is complicated and ugly:
        // int(0)  == "123456789.00"    == string("0")
        // int(3)  == "123,456,789.00"  == string("3;0")
        // int(30) == "123456,789.00"   == string("3;0;0")
        // int(32) == "12,34,56,789.00" == string("3;2;0")
        // int(320)== "1234,56,789.00"  == string("3;2")
        QString groupingStr = qt_getLocaleInfo(lcid, LOCALE_SMONGROUPING);
        format.Grouping = groupingStr.remove(QLatin1Char(';')).toInt();
        if (format.Grouping % 10 == 0) // magic
            format.Grouping /= 10;
        else
            format.Grouping *= 10;
        pformat = &format;
    }

    int ret = ::GetCurrencyFormat(lcid, 0, reinterpret_cast<const wchar_t *>(value.utf16()),
                                  pformat, out.data(), out.size());
    if (ret == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        ret = ::GetCurrencyFormat(lcid, 0, reinterpret_cast<const wchar_t *>(value.utf16()),
                                  pformat, out.data(), 0);
        out.resize(ret);
        ::GetCurrencyFormat(lcid, 0, reinterpret_cast<const wchar_t *>(value.utf16()),
                            pformat, out.data(), out.size());
    }

    return QString::fromWCharArray(out.data());
}

QStringList winUILanguages()
{
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA) {
        typedef BOOL (*GetUserPreferredUILanguagesFunc) (
                    DWORD dwFlags,
                    PULONG pulNumLanguages,
                    PWSTR pwszLanguagesBuffer,
                    PULONG pcchLanguagesBuffer);
        static GetUserPreferredUILanguagesFunc GetUserPreferredUILanguages_ptr = 0;
        if (!GetUserPreferredUILanguages_ptr) {
            QSystemLibrary lib(QLatin1String("kernel32"));
            if (lib.load())
                GetUserPreferredUILanguages_ptr = (GetUserPreferredUILanguagesFunc)lib.resolve("GetUserPreferredUILanguages");
        }
        if (GetUserPreferredUILanguages_ptr) {
            unsigned long cnt = 0;
            QVarLengthArray<wchar_t, 64> buf(64);
            unsigned long size = buf.size();
            if (!GetUserPreferredUILanguages_ptr(MUI_LANGUAGE_NAME, &cnt, buf.data(), &size)) {
                size = 0;
                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER &&
                    GetUserPreferredUILanguages_ptr(MUI_LANGUAGE_NAME, &cnt, NULL, &size)) {
                    buf.resize(size);
                    if (!GetUserPreferredUILanguages_ptr(MUI_LANGUAGE_NAME, &cnt, buf.data(), &size))
                        return QStringList();
                }
            }
            QStringList result;
            result.reserve(cnt);
            const wchar_t *str = buf.constData();
            for (; cnt > 0; --cnt) {
                QString s = QString::fromWCharArray(str);
                if (s.isEmpty())
                    break; // something is wrong
                result.append(s);
                str += s.size()+1;
            }
            return result;
        }
    }

    // old Windows before Vista
    return QStringList(QString::fromLatin1(winLangCodeToIsoName(GetUserDefaultUILanguage())));
}

QLocale QSystemLocale::fallbackLocale() const
{
    return QLocale(QString::fromLatin1(getWinLocaleName()));
}

QVariant QSystemLocale::query(QueryType type, QVariant in = QVariant()) const
{
    LCTYPE locale_info = 0;
    bool format_string = false;

    switch(type) {
//     case Name:
//         return getWinLocaleName();
    case DecimalPoint:
        locale_info = LOCALE_SDECIMAL;
        break;
    case GroupSeparator:
        locale_info = LOCALE_STHOUSAND;
        break;
    case NegativeSign:
        locale_info = LOCALE_SNEGATIVESIGN;
        break;
    case PositiveSign:
        locale_info = LOCALE_SPOSITIVESIGN;
        break;
    case DateFormatLong:
        locale_info = LOCALE_SLONGDATE;
        format_string = true;
        break;
    case DateFormatShort:
        locale_info = LOCALE_SSHORTDATE;
        format_string = true;
        break;
    case TimeFormatLong:
    case TimeFormatShort:
        locale_info = LOCALE_STIMEFORMAT;
        format_string = true;
        break;

    case DateTimeFormatLong:
    case DateTimeFormatShort:
        return QString(query(type == DateTimeFormatLong ? DateFormatLong : DateFormatShort).toString()
            + QLatin1Char(' ') + query(type == DateTimeFormatLong ? TimeFormatLong : TimeFormatShort).toString());
    case DayNameLong:
    case DayNameShort:
        return winDayName(in.toInt(), (type == DayNameShort));
    case MonthNameLong:
    case MonthNameShort:
        return winMonthName(in.toInt(), (type == MonthNameShort));
    case DateToStringShort:
    case DateToStringLong:
        return winDateToString(in.toDate(), type == DateToStringShort ? DATE_SHORTDATE : DATE_LONGDATE);
    case TimeToStringShort:
    case TimeToStringLong:
        return winTimeToString(in.toTime(), type == TimeToStringLong);
    case DateTimeToStringShort:
    case DateTimeToStringLong: {
        const QDateTime dt = in.toDateTime();
        return QString(winDateToString(dt.date(), type == DateTimeToStringShort ? DATE_SHORTDATE : DATE_LONGDATE)
            + QLatin1Char(' ') + winTimeToString(dt.time(), type == DateTimeToStringLong)); }

    case ZeroDigit:
        locale_info = LOCALE_SNATIVEDIGITS;
        break;

    case LanguageId:
    case CountryId: {
        QString locale = QString::fromLatin1(getWinLocaleName());
        QLocale::Language lang;
        QLocale::Script script;
        QLocale::Country cntry;
        QLocalePrivate::getLangAndCountry(locale, lang, script, cntry);
        if (type == LanguageId)
            return lang;
        if (cntry == QLocale::AnyCountry)
            return fallbackLocale().country();
        return cntry;
    }
    case ScriptId:
        return QVariant(QLocale::AnyScript);

    case MeasurementSystem:
        return QVariant(static_cast<int>(winSystemMeasurementSystem()));

    case AMText:
        return QVariant(winSystemAMText());
    case PMText:
        return QVariant(winSystemPMText());
    case FirstDayOfWeek:
        return QVariant(winSystemFirstDayOfWeek());
    case CurrencySymbol:
        return QVariant(winCurrencySymbol(QLocale::CurrencySymbolFormat(in.toUInt())));
    case CurrencyToString:
        return QVariant(winFormatCurrency(in.value<QSystemLocale::CurrencyToStringArgument>()));
    case UILanguages:
        return QVariant(winUILanguages());
    default:
        break;
    }
    if (locale_info) {
        QString result = getWinLocaleInfo(locale_info);
        if (format_string)
            result = winToQtFormat(result);
        if (!result.isEmpty())
            return result;
    }
    return QVariant();
}
#endif // QT_NO_SYSTEMLOCALE

struct WindowsToISOListElt {
    ushort windows_code;
    char iso_name[6];
};

/* NOTE: This array should be sorted by the first column! */
static const WindowsToISOListElt windows_to_iso_list[] = {
    { 0x0401, "ar_SA" },
    { 0x0402, "bg\0  " },
    { 0x0403, "ca\0  " },
    { 0x0404, "zh_TW" },
    { 0x0405, "cs\0  " },
    { 0x0406, "da\0  " },
    { 0x0407, "de\0  " },
    { 0x0408, "el\0  " },
    { 0x0409, "en_US" },
    { 0x040a, "es\0  " },
    { 0x040b, "fi\0  " },
    { 0x040c, "fr\0  " },
    { 0x040d, "he\0  " },
    { 0x040e, "hu\0  " },
    { 0x040f, "is\0  " },
    { 0x0410, "it\0  " },
    { 0x0411, "ja\0  " },
    { 0x0412, "ko\0  " },
    { 0x0413, "nl\0  " },
    { 0x0414, "no\0  " },
    { 0x0415, "pl\0  " },
    { 0x0416, "pt_BR" },
    { 0x0418, "ro\0  " },
    { 0x0419, "ru\0  " },
    { 0x041a, "hr\0  " },
    { 0x041c, "sq\0  " },
    { 0x041d, "sv\0  " },
    { 0x041e, "th\0  " },
    { 0x041f, "tr\0  " },
    { 0x0420, "ur\0  " },
    { 0x0421, "in\0  " },
    { 0x0422, "uk\0  " },
    { 0x0423, "be\0  " },
    { 0x0425, "et\0  " },
    { 0x0426, "lv\0  " },
    { 0x0427, "lt\0  " },
    { 0x0429, "fa\0  " },
    { 0x042a, "vi\0  " },
    { 0x042d, "eu\0  " },
    { 0x042f, "mk\0  " },
    { 0x0436, "af\0  " },
    { 0x0438, "fo\0  " },
    { 0x0439, "hi\0  " },
    { 0x043e, "ms\0  " },
    { 0x0458, "mt\0  " },
    { 0x0801, "ar_IQ" },
    { 0x0804, "zh_CN" },
    { 0x0807, "de_CH" },
    { 0x0809, "en_GB" },
    { 0x080a, "es_MX" },
    { 0x080c, "fr_BE" },
    { 0x0810, "it_CH" },
    { 0x0812, "ko\0  " },
    { 0x0813, "nl_BE" },
    { 0x0814, "no\0  " },
    { 0x0816, "pt\0  " },
    { 0x081a, "sr\0  " },
    { 0x081d, "sv_FI" },
    { 0x0c01, "ar_EG" },
    { 0x0c04, "zh_HK" },
    { 0x0c07, "de_AT" },
    { 0x0c09, "en_AU" },
    { 0x0c0a, "es\0  " },
    { 0x0c0c, "fr_CA" },
    { 0x0c1a, "sr\0  " },
    { 0x1001, "ar_LY" },
    { 0x1004, "zh_SG" },
    { 0x1007, "de_LU" },
    { 0x1009, "en_CA" },
    { 0x100a, "es_GT" },
    { 0x100c, "fr_CH" },
    { 0x1401, "ar_DZ" },
    { 0x1407, "de_LI" },
    { 0x1409, "en_NZ" },
    { 0x140a, "es_CR" },
    { 0x140c, "fr_LU" },
    { 0x1801, "ar_MA" },
    { 0x1809, "en_IE" },
    { 0x180a, "es_PA" },
    { 0x1c01, "ar_TN" },
    { 0x1c09, "en_ZA" },
    { 0x1c0a, "es_DO" },
    { 0x2001, "ar_OM" },
    { 0x2009, "en_JM" },
    { 0x200a, "es_VE" },
    { 0x2401, "ar_YE" },
    { 0x2409, "en\0  " },
    { 0x240a, "es_CO" },
    { 0x2801, "ar_SY" },
    { 0x2809, "en_BZ" },
    { 0x280a, "es_PE" },
    { 0x2c01, "ar_JO" },
    { 0x2c09, "en_TT" },
    { 0x2c0a, "es_AR" },
    { 0x3001, "ar_LB" },
    { 0x300a, "es_EC" },
    { 0x3401, "ar_KW" },
    { 0x340a, "es_CL" },
    { 0x3801, "ar_AE" },
    { 0x380a, "es_UY" },
    { 0x3c01, "ar_BH" },
    { 0x3c0a, "es_PY" },
    { 0x4001, "ar_QA" },
    { 0x400a, "es_BO" },
    { 0x440a, "es_SV" },
    { 0x480a, "es_HN" },
    { 0x4c0a, "es_NI" },
    { 0x500a, "es_PR" }
};

static const int windows_to_iso_count
    = sizeof(windows_to_iso_list)/sizeof(WindowsToISOListElt);

static const char *winLangCodeToIsoName(int code)
{
    int cmp = code - windows_to_iso_list[0].windows_code;
    if (cmp < 0)
        return 0;

    if (cmp == 0)
        return windows_to_iso_list[0].iso_name;

    int begin = 0;
    int end = windows_to_iso_count;

    while (end - begin > 1) {
        uint mid = (begin + end)/2;

        const WindowsToISOListElt *elt = windows_to_iso_list + mid;
        int cmp = code - elt->windows_code;
        if (cmp < 0)
            end = mid;
        else if (cmp > 0)
            begin = mid;
        else
            return elt->iso_name;
    }

    return 0;

}

static QString winIso639LangName(LCID id)
{
    QString result;

    // Windows returns the wrong ISO639 for some languages, we need to detect them here using
    // the language code
    QString lang_code;
    wchar_t out[256];
    if (GetLocaleInfo(id, LOCALE_ILANGUAGE, out, 255))
        lang_code = QString::fromWCharArray(out);

    if (!lang_code.isEmpty()) {
        const char *endptr;
        bool ok;
        QByteArray latin1_lang_code = lang_code.toLatin1();
        int i = qstrtoull(latin1_lang_code, &endptr, 16, &ok);
        if (ok && *endptr == '\0') {
            switch (i) {
                case 0x814:
                    result = QLatin1String("nn"); // Nynorsk
                    break;
                default:
                    break;
            }
        }
    }

    if (!result.isEmpty())
        return result;

    // not one of the problematic languages - do the usual lookup
    if (GetLocaleInfo(id, LOCALE_SISO639LANGNAME , out, 255))
        result = QString::fromWCharArray(out);

    return result;
}

static QString winIso3116CtryName(LCID id)
{
    QString result;

    wchar_t out[256];
    if (GetLocaleInfo(id, LOCALE_SISO3166CTRYNAME, out, 255))
        result = QString::fromWCharArray(out);

    return result;
}

QT_END_NAMESPACE
