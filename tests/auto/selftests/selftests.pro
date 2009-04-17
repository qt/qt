TEMPLATE = subdirs

SUBDIRS = subtest test warnings maxwarnings cmptest globaldata skipglobal skip \
          strcmp expectfail sleep fetchbogus crashes multiexec failinit failinitdata \
          skipinit skipinitdata datetime singleskip assert waitwithoutgui differentexec \
          exception qexecstringlist datatable commandlinedata\
          benchlibwalltime benchlibcallgrind benchlibeventcounter benchlibtickcounter \
          benchliboptions

INSTALLS =

QT = core


