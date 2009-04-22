<Item id="Switch" width="{Groove.width}" height="{Groove.height}">
    <properties><Property name="on"/></properties>
    <Script>
        function toggle() {
            if(Switch.state == "On")
                Switch.state = "Off";
            else
                Switch.state = "On";
        }
        function dorelease() {
            print(Knob.x);
            if(Knob.x == 1) {
                if(Switch.state == "Off") 
                    return;
            }

            if(Knob.x == 78) {
                if(Switch.state == "On") 
                    return;
            }

            toggle();
        }
    </Script>
    <Image id="Groove" src="background.svg"/>
    <MouseRegion anchors.fill="{Groove}" onClicked="toggle()"/>
    <Image id="Knob" src="knob.svg" x="1" y="2"/>
    <MouseRegion anchors.fill="{Knob}"
                 onClicked="toggle()"
                 onReleased="if (!isClick) dorelease()"
                 drag.target="{Knob}"
                 drag.axis="x"
                 drag.xmin="1"
                 drag.xmax="78"/>

    <states>
        <State name="On">
            <SetProperty target="{Knob}" property="x" value="78" />
            <SetProperty target="{Switch}" property="on" value="true" />
        </State>
        <State name="Off">
            <SetProperty target="{Knob}" property="x" value="1" />
            <SetProperty target="{Switch}" property="on" value="false" />
        </State>
    </states>

    <transitions>
        <Transition>
            <NumericAnimation properties="x" easing="easeInOutQuad" duration="200"/>
        </Transition>
    </transitions>
</Item>
