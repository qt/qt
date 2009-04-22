<Rect color="white" width="200" height="200">
    <Rect width="50" height="50" color="red">
        <Text text="Click" anchors.centeredIn="{parent}"/>
        <MouseRegion onPressed="print('press (x: ' + x + ' y: ' + y + ')')"
                     onReleased="print('release (x: ' + x + ' y: ' + y + ' isClick: ' + isClick + ' followsPressAndHold: ' + followsPressAndHold + ')')"
                     onClicked="print('click (x: ' + x + ' y: ' + y + ' followsPressAndHold: ' + followsPressAndHold + ')')"
                     onDoubleClicked="print('double click (x: ' + x + ' y: ' + y + ')')"
                     onPressAndHold="print('press and hold')"
                     onExitedWhilePressed="print('exiting while pressed')"
                     onReenteredWhilePressed="print('reentering while pressed')" anchors.fill="{parent}"/>
    </Rect>
    <Rect y="100" width="50" height="50" color="blue">
        <Text text="Drag" anchors.centeredIn="{parent}"/>
        <MouseRegion drag.target="{parent}"
                     drag.axis="x" drag.xmin="0" drag.xmax="150"
                     onPressed="print('press')"
                     onReleased="print('release (isClick: ' + isClick + ') (followsPressAndHold: ' + followsPressAndHold + ')')"
                     onClicked="print('click' + '(followsPressAndHold: ' + followsPressAndHold + ')')"
                     onDoubleClicked="print('double click')"
                     onPressAndHold="print('press and hold')"
                     onExitedWhilePressed="print('exiting while pressed')"
                     onReenteredWhilePressed="print('reentering while pressed')" anchors.fill="{parent}"/>
    </Rect>
</Rect>
