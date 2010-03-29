#include <qscreenvfb_qws.h>
#include "qgraphicssystem_blittableqvfb.h"

class QBlittableVFbScreen : public QVFbScreen
{
public:
    explicit QBlittableVFbScreen(int display_id)
        : QVFbScreen(display_id)
    {
        setGraphicsSystem(QBlittableVFbGraphicsSystem::instance());

    }

};
