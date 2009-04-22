<Rect width="400" height="200" color="white">
    <Rect width="40" height="40" y="80" color="#FF0000" radius="10">

        <x>
            <!--
            Animate the x property.  Setting repeat to true makes the
            animation repeat indefinitely, otherwise it would only run once.
            -->
            <SequentialAnimation running="true" repeat="true">
                <!-- Move from 0 to 360 in 500ms, using the easeInOutQuad easing function -->
                <NumericAnimation from="0" to="360" easing="easeInOutQuad" duration="500"/>
                <!-- Then pause for 200ms -->
                <PauseAnimation duration="200"/>
                <!-- Then move back to 0 in 2 seconds, using the easeInOutElastic easing function -->
                <NumericAnimation from="360" to="0" easing="easeInOutElastic" duration="2000"/>
            </SequentialAnimation>
        </x>

        <color>
            <!-- Alternate color between red and green -->
            <SequentialAnimation running="true" repeat="true">
                <ColorAnimation from="#FF0000" to="#00FF00" duration="5000"/>
                <ColorAnimation from="#00FF00" to="#FF0000" duration="5000"/>
            </SequentialAnimation>
        </color>

    </Rect>
</Rect>
