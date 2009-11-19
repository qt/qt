This test launches gui applications (tools, demos and prominent examples),
keeps them running a while (grabbing their top level from the window manager) 
and sends them a Close event via window manager. 

It checks that they do not crash nor produce unexpected error output.

Note: Do not play with the machine while it is running as otherwise
the top-level find algorithm might get confused (especially on Windows).

Environment variables are checked to turned off some tests (see code).

It is currently implemented for X11 (Skips unless DISPLAY is set) and
Windows, pending an implementation of the WindowManager class and deployment
on the other platforms.
