TEMPLATE = app
LANGUAGE = C++

FORMS += project.ui

TRANSLATIONS        = project.ts

exists( $$TRANSLATIONS ) {
    win32 : system(del $$TRANSLATIONS)
    unix :  system(rm $$TRANSLATIONS)
}


