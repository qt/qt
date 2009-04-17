TEMPLATE = subdirs
contains(kbd-plugins, usb): SUBDIRS += usb
contains(kbd-plugins, sl5000): SUBDIRS += sl5000
contains(kbd-plugins, vr41xx): SUBDIRS += vr41xx
contains(kbd-plugins, yopy): SUBDIRS += yopy
contains(kbd-plugins, linuxis): SUBDIRS += linuxis
