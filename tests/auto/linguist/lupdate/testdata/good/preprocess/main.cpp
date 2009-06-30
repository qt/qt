// IMPORTANT!!!! If you want to add testdata to this file,
// always add it to the end in order to not change the linenumbers of translations!!!


void func1() {
    QApplication::tr("Hello world", "Platform-independent file");
}




void func2() {
#ifdef Q_OS_WIN
    QApplication::tr("Kind", "Windows only, see Type");
#else
    QApplication::tr("Type", "Not used on windows, see Kind");
#endif

}



void stringconcatenation()
{
    QApplication::tr("One string,"
    " three"
    " lines");

    QApplication::tr("a backslash followed by newline \
should be ignored \
and the next line should be syntactically considered to be \
on the same line");

}



