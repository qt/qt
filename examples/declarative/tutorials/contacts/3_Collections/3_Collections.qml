<Rect id="page" width="{layout.width}" height="{layout.height}" color='white'>
    <GridLayout id="layout" width="{contents.width}" height="{contents.height}">
        <FocusRealm id="realm1" focus="false" width="280" height="320">
            <GroupBox contents="ContactView1.qml" label="something" anchors.fill="{parent}"/>
            <Rect id="box1" color="black" anchors.fill="{parent}" opacity="0.3">
                <MouseRegion anchors.fill="{parent}" onClicked="print('1'); realm1.focus=true; realm2.focus=false; realm3.focus=false; box1.opacity='0'; box2.opacity='0.3'; box3.opacity='0.3'" onPressed="" onPositionChanged=""/>
                <opacity>
                    <Behaviour>
                        <NumericAnimation property="opacity" duration="250"/>
                    </Behaviour>
                </opacity>
            </Rect>
        </FocusRealm>
        <FocusRealm id="realm2" focus="false" width="280" height="320">
            <GroupBox contents="ContactView2.qml" label="something" anchors.fill="{parent}"/>
            <Rect id="box2" color="black" anchors.fill="{parent}" opacity="0.3">
                <MouseRegion anchors.fill="{parent}" onClicked="realm1.focus=false; realm2.focus=true; realm3.focus=false; box1.opacity='0.3'; box2.opacity='0'; box3.opacity='0.3'" onPressed="" onPositionChanged=""/>
                <opacity>
                    <Behaviour>
                        <NumericAnimation property="opacity" duration="250"/>
                    </Behaviour>
                </opacity>
            </Rect>
        </FocusRealm>
        <FocusRealm id="realm3" focus="true" width="280" height="320">
            <GroupBox contents="ContactView3.qml" label="something" anchors.fill="{parent}"/>
            <Rect id="box3" color="black" anchors.fill="{parent}" opacity="0.3">
                <MouseRegion anchors.fill="{parent}" onClicked="realm1.focus=false; realm2.focus=false; realm3.focus=true; box1.opacity='0.3'; box2.opacity='0.3'; box3.opacity='0'" onPressed="" onPositionChanged=""/>
                <opacity>
                    <Behaviour>
                        <NumericAnimation property="opacity" duration="250"/>
                    </Behaviour>
                </opacity>
            </Rect>
        </FocusRealm>
    </GridLayout>
</Rect>
