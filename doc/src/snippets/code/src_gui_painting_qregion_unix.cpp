//! [0]
QRegion r1(10, 10, 20, 20);
r1.isNull();                // false
r1.isEmpty();               // false

QRegion r2(40, 40, 20, 20);
QRegion r3;
r3.isNull();                // true
r3.isEmpty();               // true

r3 = r1.intersected(r2);    // r3: intersection of r1 and r2
r3.isNull();                // false
r3.isEmpty();               // true

r3 = r1.united(r2);         // r3: union of r1 and r2
r3.isNull();                // false
r3.isEmpty();               // false
//! [0]
