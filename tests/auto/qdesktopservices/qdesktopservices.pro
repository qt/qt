CONFIG += qttest_p4

SOURCES		+= tst_qdesktopservices.cpp
TARGET		= tst_qdesktopservices
symbian: {
   dummy.sources = text\\testfile.txt
   dummy.path = .

   text.sources = text\\*
   text.path = \\data\\others

   image.sources = image\\*
   image.path = \\data\\images

   audio.sources = audio\\*
   audio.path = \\data\\sounds

   video.sources = video\\*
   video.path = \\data\\videos

   install.sources = install\\*
   install.path = \\data\\installs

   DEPLOYMENT += image audio video install

   # These are only needed for manual tests
   #DEPLOYMENT += dummy text
   }

