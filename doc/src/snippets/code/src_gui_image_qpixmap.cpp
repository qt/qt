//! [0]
static const char * const start_xpm[]={
    "16 15 8 1",
    "a c #cec6bd",
....
//! [0]


//! [1]
QPixmap myPixmap;
myPixmap->setMask(myPixmap->createHeuristicMask());
//! [1]

//! [2]
QPixmap pixmap("background.png");
QRegion exposed;
pixmap.scroll(10, 10, pixmap.rect(), &exposed);
//! [2]
