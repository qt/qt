TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = standalone

CREATOR_SRC = $$(CREATOR_SRC_DIR)
CREATOR_BUILD = $$(CREATOR_BUILD_DIR)
!isEmpty(CREATOR_SRC):!isEmpty(CREATOR_BUILD) {
    SUBDIRS += creatorplugin
}
