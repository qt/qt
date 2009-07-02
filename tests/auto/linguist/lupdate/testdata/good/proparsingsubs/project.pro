TEMPLATE = subdirs
SUBDIRS = win mac unix common

exists( project.ts ) {
    win32: system(del project.ts)
    unix:  system(rm project.ts)
}
