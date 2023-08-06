import QtQuick 2.6
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.0 //shortcut
import QtQuick.Shapes 1.1 // linear gradient

ApplicationWindow {
    id: applicationWindow
    x : Screen.width / 2 - width / 2
    y : Screen.height / 2 - height / 2
/////// Variables /////////////////////
    property int lScreen: 2*Screen.width/5
    property int hScreen: 3*Screen.height/5

    property int lMessage: lScreen/3
    property int hMessage: hMessage/3

////////// Taille écran //////////////////
    visible: true
    width: lScreen
    height: hScreen

    minimumWidth: 350
    minimumHeight: 410

    function doSomething()
    {
        lScreen = width;
        hScreen = height;

        lMessage = 2*lScreen/3;
        hMessage = hScreen/3;
    }
    onHeightChanged: doSomething();
    onWidthChanged: doSomething();

//////////// Background-color ////////////////

    Rectangle {
        id: fenet
        width: lScreen; height: hScreen

        gradient: Gradient {
            GradientStop {
               position: 0.000
               color: Qt.rgba(255, 0, 0, 1)
            }
            GradientStop {
               position: 0.2
               color: Qt.rgba(255, 0, 0, 1)
            }
            GradientStop {
               position: 0.21
               color: Qt.rgba(255, 255, 0, 1)
            }
            GradientStop {
               position: 0.666
               color: Qt.rgba(255, 255, 0, 1)
            }
            GradientStop {
               position: 0.67
               color: Qt.rgba(255, 0, 0, 1)
            }
            GradientStop {
               position: 1.000
               color: Qt.rgba(255, 0, 0, 1)
            }
        }
    }
//////// MenuBar /////////////////////
    MenuBar {
        id: menuBar
        Menu {
            id:menubarFile
            title: qsTr("File")
            MenuItem{
                id: actNew
                text: qsTr("&New file")
                shortcut: "Ctrl+N"
                onTriggered: qmlTranslator.nvxFiche()
            }
            MenuItem{
                id: actEncrypt
                text: qsTr("&Encrypt")
                shortcut:"Ctrl+E"
                onTriggered: qmlTranslator.crypte()
            }
            MenuItem{
                id: actDecode
                text: qsTr("&Decode")
                shortcut:"Ctrl+D"
                onTriggered: qmlTranslator.decrypte()
            }
            MenuSeparator{}
            MenuItem {
                id: actQuit
                text: qsTr("&Leave")
                shortcut:"Ctrl+L"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            id: menubarEdit
            title: qsTr("Edit")
            MenuItem {
                id: actCopy
                text: qsTr("&Copy selected Text")
                shortcut: StandardKey.Copy
                onTriggered: window.activeFocusItem.copy()
            }
        }
    }
//////// StatusBar /////////////////////
    footer: Label {
        id: statusBar
        width: lScreen
        height: 30
        padding: 20
        anchors{
            margins: 30
        }
        background: Rectangle {
            color: "white"
            width: lScreen
        }
        verticalAlignment: Qt.AlignVCenter
        color: "red"
        font.pixelSize: 18
        font.bold: true
    }

//////// ToolBar /////////////////////
    header: ToolBar {
        id: toolbar
        height: 35
        background: Rectangle {
            color: "darkred"
        }
        RowLayout {
            anchors.fill: parent
            Label {
                id: labeltoolbar
                color: "white"
                font.pixelSize: 18
                font.bold: true
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
        }
        StackView {
            id: stack
            anchors.fill: parent
        }
    }

//////////// Objets fenêtre /////////////////
    Rectangle {
        id: comboRect
        width: 100
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        ComboBox {
            id: comboBox
            height: 30
            anchors {
                top: parent.top
                right: parent.right
                margins: 30
            }
            delegate: ItemDelegate {
                width: comboBox.width
                contentItem: Text {
                    text: modelData
                    color: "deepskyblue"
                    font: comboBox.font
                }
                highlighted: comboBox.highlightedIndex == index
            }
            indicator: Canvas {
                    id: canvas
                    x: comboBox.width - width - comboBox.rightPadding
                    y: comboBox.topPadding + (comboBox.availableHeight - height) / 2
                    width: 12
                    height: 8
                    contextType: "2d"

                    Connections {
                        target: comboBox
                        function onPressedChanged() { canvas.requestPaint(); }
                    }

                    onPaint: { // fleche de la comboBox
                        context.reset();
                        context.moveTo(0, 0);
                        context.lineTo(width, 0);
                        context.lineTo(width / 2, height);
                        context.closePath();
                        context.fillStyle = comboBox.pressed ? "orange" : "blue";
                        context.fill();
                    }
            }
            contentItem: Text {
                text: comboBox.displayText
                font: comboBox.font
                color: mouseAreaComboBoxRect.containsMouse ? "orange" : "blue"
            }

            background: Rectangle {
                border.color: mouseAreaComboBoxRect.containsMouse ? "orange" : "blue"
                MouseArea{
                    id: mouseAreaComboBoxRect
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: statusBar.text=qsTr("Push to change language");
                    onExited: statusBar.text=qsTr("Ready");
                }
            }
            popup: Popup {
                y: comboBox.height - 1
                width: comboBox.width
                implicitHeight: contentItem.implicitHeight
                padding: 1
                contentItem: ListView {
                    clip: true
                    implicitHeight: contentHeight
                    model: comboBox.popup.visible ? comboBox.delegateModel : null
                    currentIndex: comboBox.highlightedIndex
                    ScrollIndicator.vertical: ScrollIndicator { }
                }
                background: Rectangle {
                    border.color: "white"
                    radius: 2
                    border.width: 3
                }
            }

            model: ["Español", "Français", "English","Pусский","Deutsch"]

            currentIndex: 2

            onCurrentTextChanged: {
                qmlTranslator.setTranslation(comboBox.currentText)
            }
        }
        Label{
            id: titre
            anchors {
                top: comboBox.top
                horizontalCenter: parent.horizontalCenter
                margins: 30
            }
            text: "Crypto'App"
            font.pixelSize: 30
            font.bold: true
            font.family: "Arial Black"
            style: Text.Outline;
            styleColor: "red"
            color: "black"
        }
        Rectangle {
            id: msg
            color: "black"
            height: hMessage
            width: lMessage
            clip: true
            radius: 10
            anchors {
                top: titre.bottom
                horizontalCenter: parent.horizontalCenter
                margins: 30

            }
            Flickable{
                id: view
                anchors.fill: parent
                contentWidth: lMessage
                contentHeight: messageAccueil.height
                states: State {
                    name: "ShowBars"
                    when: view.movingVertically //|| view.movingHorizontally
                    PropertyChanges { target: verticalScrollBar; opacity: 1 }
                }

                transitions: Transition {
                    NumberAnimation { properties: "opacity"; duration: 400 }
                }
                TextEdit{
                    id: messageAccueil
                    width:lMessage
                    readOnly: true
                    padding: 20
                    leftPadding: 30
                    wrapMode: TextEdit.Wrap
                    color: "white"
                    font.pixelSize: 18
                    //clip: true
                    MouseArea{
                        id: mouseArea1
                        anchors.fill: messageAccueil
                        cursorShape: Qt.IBeamCursor
                        onClicked: {
                            messageAccueil.color = "red"
                        }
                        onDoubleClicked: {
                            messageAccueil.color = "white"
                        }
                        onEntered: {
                            statusBar.text=qsTr("Click or double click to change the color of the message")
                        }
                        hoverEnabled: true
                        onMouseXChanged: {
                            messageAccueil.color = "orange"
                        }
                        onExited: {
                            statusBar.text=qsTr("Ready")
                            messageAccueil.color = "white"
                        }
                    }
                }
            }
            ScrollBar {
                id: verticalScrollBar
                width: 12; height: messageAccueil.height
                anchors.right: view.right
                opacity: 0
                orientation: Qt.Vertical
                position: view.visibleArea.yPosition
            }
        }
        Rectangle {
            id: rectBoutons
            height: 50
            width: lMessage
            color: "transparent"
            anchors{
                top: msg.bottom
                margins: 30
                horizontalCenter: parent.horizontalCenter
            }
            Button {
                id: encryptBut
                width: lMessage/3
                anchors{
                    left: rectBoutons.left
                }
                contentItem: Text {
                    text: encryptBut.text
                    font: encryptBut.font
                    color: mouseAreaEncrypt.containsMouse ? "orange" : "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    color: mouseAreaEncrypt.containsMouse ? "white" : "black"
                    implicitWidth: lMessage/3
                    implicitHeight: 30
                    border.color: mouseAreaEncrypt.pressed ? "purple" : "cyan"
                    border.width: 2
                    radius: 2
                }
                MouseArea{
                    id: mouseAreaEncrypt
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: qmlTranslator.crypte()
                    onEntered: statusBar.text=qsTr("Push to encrypt a file");
                    onExited: statusBar.text=qsTr("Ready");
                }
            }
            Button {
                id: decryptBut
                width: lMessage/3
                anchors{
                    left: encryptBut.right
                }
                contentItem: Text {
                    text: decryptBut.text
                    font: decryptBut.font
                    color: mouseAreaDecrypt.containsMouse ? "orange" : "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    color: mouseAreaDecrypt.containsMouse ? "white" : "black"
                    implicitWidth: lMessage/3
                    implicitHeight: 30
                    border.color: mouseAreaDecrypt.pressed ? "purple" : "cyan"
                    border.width: 2
                    radius: 2
                }
                MouseArea{
                    id: mouseAreaDecrypt
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: qmlTranslator.decrypte()
                    onEntered: statusBar.text=qsTr("Push to decode a file");
                    onExited: statusBar.text=qsTr("Ready");
                }
            }
            Button {
                id: newfileBut
                width: lMessage/3
                anchors{
                    left: decryptBut.right
                }
                contentItem: Text {
                    text: newfileBut.text
                    font: newfileBut.font
                    color: mouseAreaNewfile.containsMouse ? "orange" : "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    color: mouseAreaNewfile.containsMouse ? "white" : "black"
                    implicitWidth: lMessage/3
                    implicitHeight: 30
                    border.color: mouseAreaNewfile.pressed ? "purple" : "cyan"
                    border.width: 2
                    radius: 2
                }
                MouseArea{
                    id: mouseAreaNewfile
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: qmlTranslator.nvxFiche()
                    onEntered: statusBar.text=qsTr("Push to create a new file");
                    onExited: statusBar.text=qsTr("Ready");
                }
            }
        }
        Button {
            id: steganoBut
            width: lMessage/3
            anchors{
                top: rectBoutons.bottom
                right: parent.center
            }
            contentItem: Text {
                text: steganoBut.text
                font: steganoBut.font
                color: mouseAreaStegano.containsMouse ? "orange" : "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                color: mouseAreaStegano.containsMouse ? "white" : "black"
                implicitWidth: lMessage/3
                implicitHeight: 30
                border.color: mouseAreaStegano.pressed ? "purple" : "cyan"
                border.width: 2
                radius: 5
            }
            MouseArea{
                id: mouseAreaStegano
                anchors.fill: parent
                hoverEnabled: true
                onEntered: statusBar.text=qsTr("Push to hide a message in a picture");
                onExited: statusBar.text=qsTr("Ready");
                onClicked: qmlTranslator.stegano()
            }
        }
        Button {
            id: leaveBut
            width: lMessage/3
            anchors{
                top: rectBoutons.bottom
                right: parent.right
            }
            contentItem: Text {
                text: leaveBut.text
                font: leaveBut.font
                color: mouseAreaLeave.containsMouse ? "orange" : "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle {
                color: mouseAreaLeave.containsMouse ? "white" : "black"
                implicitWidth: lMessage/3
                implicitHeight: 30
                border.color: mouseAreaLeave.pressed ? "purple" : "cyan"
                border.width: 2
                radius: 5
            }
            MouseArea{
                id: mouseAreaLeave
                anchors.fill: parent
                hoverEnabled: true
                onEntered: statusBar.text=qsTr("Push to leave the app");
                onExited: statusBar.text=qsTr("Ready");
                onClicked: qmlTranslator.arret()
            }
        }

    }
    Connections {
        target: qmlTranslator
        onLanguageChanged: {
            retranslateUi()
        }
    }

    function retranslateUi() {
        applicationWindow.title = qsTr("Multi-Languages Crypto'Application")
        statusBar.text= qsTr("Ready")
        labeltoolbar.text=qsTr("Tool Bar")
        messageAccueil.text=qsTr("Welcome in the files encrypting app to secure your datas.\n\n"
                                  +"Encryption and decoding are released by Cesar encryption or by Vigenre encryption.\n\n"
                                  +"So the key for encryption is a number or a word.\n\n"
                                  + "This application was created by BADAJOZ Julien.\n\n "
                                  +  "A student in electronics and computer science engineering.\n\n"
                                  + "Thank you for your participation and enjoy !")
        encryptBut.text=qsTr("Encrypt File")
        decryptBut.text=qsTr("Decode File")
        newfileBut.text=qsTr("New File")
        leaveBut.text=qsTr("Close")
        steganoBut.text=qsTr("Steganography")
        actNew.text=qsTr("&New File")
        actEncrypt.text=qsTr("&Encrypt")
        actDecode.text=qsTr("&Decode")
        actQuit.text=qsTr("&Quit")
        actCopy.text=qsTr("&Copy selected Text")
        menubarFile.title=qsTr("File")
        menubarEdit.title=qsTr("Edit")
    }

    Component.onCompleted: {
        retranslateUi();
    }
}
