@rem The qt_pkg_setup is a .bat file that must be in your PATH
@rem It should set the proper environment for the compiler requested
call qt_pkg_setup mingw

c:
cd \installer\win-binary
call iwmake.bat mingw-opensource >out.txt
pscp -batch -i c:\key1.ppk c:\iwmake\*.exe qt@tirion:public_html/packages/%QT_VERSION%

