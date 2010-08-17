#include <qglobal.h>

struct StringCollection
{
    int len;
    int offset1, offset2;
    ushort align1, align2;
};

extern const ushort stringCollectionData[];
extern StringCollection stringCollection[];
extern const int stringCollectionCount;
