TEMPLATE = subdirs


symbian: {
SYMBIANTRANSLATIONSFILES = qt
SYMBIANTRANSLATIONS = ur fa ar he fr pl ru zh_cn zh_tw cs da de es gl hu ja pt sk sl sv uk

CONFIG = loc

for( FILE, SYMBIANTRANSLATIONSFILES ) {
    for( LANGID, SYMBIANTRANSLATIONS ) {
        TRANSLATIONS += $${EPOCROOT}epoc32/include/platform/qt/translations/$${FILE}_$${LANGID}.ts
    }
}

}