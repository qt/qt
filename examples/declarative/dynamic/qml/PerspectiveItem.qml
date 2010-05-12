import Qt 4.7

Image {
    id: rootItem

    property bool created: false
    property string image 

    property double scaledBottom: y + (height + height*scale) / 2 
    property bool onLand: scaledBottom > window.height / 2

    source: image
    opacity: onLand ? 1 : 0.25
    scale: Math.max((y + height - 250) * 0.01, 0.3)
    smooth: true

    onCreatedChanged: {
        if (created && !onLand)
            rootItem.destroy();
        else
            z = scaledBottom;
    }

    onYChanged: z = scaledBottom;
}
