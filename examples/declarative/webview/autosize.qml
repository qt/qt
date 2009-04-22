<!-- The WebView size is determined by the width, height,
     idealWidth, and idealHeight properties. -->
<Rect id="Rect" color="white" width="200" height="{Layout.height}">
    <VerticalLayout id="Layout" spacing="2">
        <WebView>
            <html><![CDATA[
                No width defined.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
        <WebView width="{Rect.width}">
            <html><![CDATA[
                The width is full.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
        <WebView width="{Rect.width/2}">
            <html><![CDATA[
                The width is half.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
        <WebView idealWidth="{Rect.width/2}">
            <html><![CDATA[
                The idealWidth is half.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
        <WebView idealWidth="{Rect.width/2}">
            <html><![CDATA[
                The_idealWidth_is_half.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
        <WebView width="{Rect.width/2}">
            <html><![CDATA[
                The_width_is_half.
            ]]></html>
            <Rect color="#10000000" anchors.fill="{parent}"/>
        </WebView>
    </VerticalLayout>
</Rect>
