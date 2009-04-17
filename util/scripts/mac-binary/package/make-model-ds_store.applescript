(* This script works by using FileStorm to create a disk image that you can then use as a model .DS_Store file in 
   the disk image. Here's what you need to do:
     
   Run this script, adjusting the path to the items and versions as necessary. If all items don't exist,
   then it won't finish correctly.
   
   Afterwords, mount the final disk image and make sure that it looks OK, sometimes FileStorm
   messes this up.
   
   If it's OK, copy /Volumes/Qt-x.y.z/.DS_Store to safe location (call it model-DS_Store.x.y.z)
   
   Edit this file and replace all occurences of backgroundXXXXXXXXXXXX (21 chars) with
   findersbackground.png.
   
   Save the file and you should be ready to go.
   
   I believe this is as automated as I can get it.
   
   FileStorm 1.9.1 has some new issues:
   * The file is invisible as well, you can use '/Developer/Tools/SetFile -a v'  to make
      it visible if you need to use a GUI app to edit it.
*)

tell application "FileStorm"
	activate
	make new document at before first document with properties {name:"diskimage"}
	
	tell first document
		set disk image name to "/Users/twschulz/Desktop/qt-mac-commercial-4.4.5.dmg"
		set {volume name} to {"Qt 4.4.5"}
		set icon path to "/Users/twschulz/troll/qt/4.4/util/scripts/mac-binary/package/backgrounds/DriveIcon.icns"
		set background image path to "/Users/twschulz/troll/qt/4.4/util/scripts/mac-binary/package/backgrounds/DiskImage-Commercial.png"
		set height to 660
		make new file at before first file with properties {file path:"/Users/twschulz/Desktop/foo/Qt.mpkg", left position:85, top position:117}
		make new file at before first file with properties {file path:"//Users/twschulz/Desktop/foo/ReadMe.txt", left position:85, top position:297}
		make new file at before first file with properties {file path:"/Users/twschulz/Desktop/foo/packages", left position:150, top position:640}
		tell application "FileStorm" to set bounds of window "diskimage" to {100, 100, 640, 660}
		finalize image with rebuilding
	end tell
	
end tell
