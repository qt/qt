//! [0]
process->tryTerminate();
QTimer::singleShot( 5000, process, SLOT(kill()) );
//! [0]
