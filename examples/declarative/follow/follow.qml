<Rect width="320" height="240" color="#ffffff">
    <Rect id="Rect" y="{200}" color="#00ff00" width="60" height="20">
        <y>
            <SequentialAnimation running="true" repeat="true">
            <NumericAnimation to="{200}" easing="easeOutBounce(amplitude:180)" duration="2000" />
            <PauseAnimation duration="1000" />
            </SequentialAnimation>
        </y>
    </Rect>
    <!-- Velocity -->
    <Rect x="{Rect.width}" color="#ff0000" width="{Rect.width}" height="20">
        <y>
            <Follow source="{Rect.y}" velocity="200"/>
        </y>
    </Rect>
    <Text x="{Rect.width}" y="220" text="Velocity"/>
    <!-- Spring -->
    <Rect x="{Rect.width * 2}" color="#ff0000" width="{Rect.width}" height="20">
        <y>
            <Follow source="{Rect.y}" spring="1.2" damping="0.1"/>
        </y>
    </Rect>
    <Text x="{Rect.width * 2}" y="220" text="Spring"/>
    <!-- Follow mouse -->
    <MouseRegion id="Mouse" anchors.fill="{parent}">
        <Rect width="20" height="20" radius="10" color="#0000ff">
            <x>
                <Follow source="{Mouse.mouseX-10}" spring="1.0" damping="0.05"/>
            </x>
            <y>
                <Follow source="{Mouse.mouseY-10}" spring="1.0" damping="0.05"/>
            </y>
        </Rect>
    </MouseRegion>
</Rect>
