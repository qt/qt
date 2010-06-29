TEMPLATE = subdirs
wince*:{
  SUBDIRS = test
} else {
  SUBDIRS = test stdinprocess
}

!symbian:SUBDIRS += largefile

