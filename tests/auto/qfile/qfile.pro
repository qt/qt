TEMPLATE = subdirs
wince*:{
  SUBDIRS = test
} else {
  SUBDIRS = test stdinprocess
}

SUBDIRS += largefile

