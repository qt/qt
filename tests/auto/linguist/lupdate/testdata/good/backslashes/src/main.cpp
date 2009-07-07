// IMPORTANT!!!! If you want to add testdata to this file,
// always add it to the end in order to not change the linenumbers of translations!!!
//
//
//
//

QString qt_detectRTLLanguage()
{
     return QApplication::tr("QT_LAYOUT_DIRECTION",
                         "Translate this string to the string 'LTR' in left-to-right"
                         " languages or to 'RTL' in right-to-left languages (such as Hebrew"
                         " and Arabic) to get proper widget layout.") == QLatin1String("RTL");
}

