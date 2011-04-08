import QtQuick 2.0

Item{
    id: container
    //TODO: Somehow get particles into this?
    property list<Component> pages
    property Item cur: null
    property int at: 0
    function advance(){
        if(cur != null)
            cur.destroy();
        cur = pages[at++].createObject(container);
    }
}
