#ifndef __DDHELPER__
#define __DDHELPER__

extern uchar* q_frameBuffer();

extern int q_screenDepth();

extern int q_screenWidth();

extern int q_screenHeight();

extern int q_linestep();

extern void q_initDD();

extern void q_unlock();

extern void q_lock();

#endif //__DDHELPER__

