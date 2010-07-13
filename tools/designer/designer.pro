TEMPLATE = subdirs

CONFIG += qt

SUBDIRS = src

TR_DIR = $$PWD/../../translations
TRANSLATIONS = \
    $$TR_DIR/designer_cs.ts \
    $$TR_DIR/designer_de.ts \
    $$TR_DIR/designer_fr.ts \
    $$TR_DIR/designer_hu.ts \
    $$TR_DIR/designer_ja.ts \
    $$TR_DIR/designer_pl.ts \
    $$TR_DIR/designer_ru.ts \
    $$TR_DIR/designer_sl.ts \
    $$TR_DIR/designer_zh_CN.ts \
    $$TR_DIR/designer_zh_TW.ts
