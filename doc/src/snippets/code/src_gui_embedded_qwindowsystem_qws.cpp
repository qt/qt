//! [0]
bool MyScreenSaver::save( int level )
{
    switch ( level ) {
        case 0:
            if ( dim_enabled ) {
                // dim the screen
            }
            return true;
        case 1:
            if ( screenoff_enabled ) {
                // turn off the screen
            }
            return true;
        case 2:
            if ( suspend_enabled ) {
                // suspend
            }
            return true;
        default:
            return false;
    }
}

...

int timings[4];
timings[0] = 5000;  // dim after 5 seconds
timings[1] = 10000; // light off after 15 seconds
timings[2] = 45000; // suspend after 60 seconds
timings[3] = 0;
QWSServer::setScreenSaverIntervals( timings );

// ignore the key/mouse event that turns on the screen
int blocklevel = 1;
if ( !screenoff_enabled ) {
    // screenoff is disabled, ignore the key/mouse event that wakes from suspend
    blocklevel = 2;
    if ( !suspend_enabled ) {
        // suspend is disabled, never ignore events
        blocklevel = -1;
    }
}
QWSServer::setScreenSaverBlockLevel( blocklevel );
//! [0]
