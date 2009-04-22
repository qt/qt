<Item height="30" width="200" id="field">
    <properties>
        <Property name="value" onValueChanged="fieldText.text=field.value"/>
        <Property name="icon"/>
        <Property name="editable" value="0"/>
        <Property name="label"/>
    </properties>
    <Item id="fieldSelector" width="30" height="30"
        x="0"
        anchors.top="{parent.top}"
        anchors.bottom="{parent.bottom}">
        <Image src="{field.icon}"
            anchors.verticalCenter="{parent.verticalCenter}"
            anchors.horizontalCenter="{parent.horizontalCenter}"/>
    </Item>
    <FieldText id="fieldText"
        label="{field.label}"
        width="{field.width-70}"
        anchors.left="{fieldSelector.right}"
        anchors.leftMargin="5"
        anchors.top="{parent.top}"
        anchors.bottom="{parent.bottom}"
        onTextEdited="field.value = fieldText.text"/>
    <FieldRemover id="fieldRemover"
        anchors.left="{fieldText.right}"
        anchors.leftMargin="5"
        anchors.top="{parent.top}"
        anchors.bottom="{parent.bottom}"
        onConfirm="fieldText.text = ''"
        opacity="{field.editable}"/>
    <states>
        <State name="textFill" when="{fieldText.open == 'true'}">
            <SetProperty target="{fieldText}" property="width" value="{field.width}"/>
            <SetProperty target="{fieldText}" property="x" value="0"/>
            <SetProperty target="{fieldRemover}" property="opacity" value="0"/>
            <SetProperty target="{fieldSelector}" property="opacity" value="0"/>
            <SetProperty target="{fieldSelector}" property="x" value="{-5-fieldSelector.width}"/>
        </State>
        <State name="removerFill" when="{fieldRemover.open == 'true'}">
            <SetProperty target="{fieldRemover}" property="width" value="{field.width}"/>
            <SetProperty target="{fieldRemover}" property="x" value="0"/>
            <SetProperty target="{fieldText}" property="opacity" value="0"/>
            <SetProperty target="{fieldSelector}" property="opacity" value="0"/>
            <SetProperty target="{fieldSelector}" property="x" value="{-10-fieldText.width-fieldSelector.width}"/>
        </State>
    </states>
    <transitions>
        <Transition fromState="*" toState="*">
            <NumericAnimation target="{fieldSelector}" properties="width,x,opacity" duration="200"/>
            <NumericAnimation target="{fieldText}" properties="width,x,opacity" duration="200"/>
            <NumericAnimation target="{fieldRemover}" properties="width,x,opacity" duration="200"/>
        </Transition>
    </transitions>
</Item>
