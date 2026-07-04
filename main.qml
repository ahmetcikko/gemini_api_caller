import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: window
    visible: true
    width: Screen.desktopAvailableWidth * 0.25
    height: Screen.desktopAvailableHeight * 0.6
    property bool recording: false
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    title: "Gemini Chatbot"
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            orientation: Gradient.Vertical
            GradientStop { position: 0.0; color: '#ce1d1b1b' }
            GradientStop { position: 0.5; color: '#e52c2c2e' }
            GradientStop { position: 1.0; color: '#141416' }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: parent.width * 0.07
        spacing: window.width * 0.04
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: window.width * 0.04
            color: "#12ffffff"
            border.color: "#26ffffff"
            border.width: 1
            ScrollView {
                anchors.fill: parent
                anchors.margins: window.width * 0.04
                clip: true
                Text {
                    id: responseLabel
                    width: parent.width
                    wrapMode: Text.Wrap
                    font.pixelSize: window.height * 0.03
                    color: "#ffffff"
                    text: ""
                }
            }
        }
        Text {
            id: statusLabel
            font.pixelSize: window.height * 0.015
            color: "#99ffffff"
            text: backend.busy ? "Gemini is thinking..." : ""
            visible: backend.busy
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: window.width * 0.03
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: window.height * 0.3
                radius: window.width * 0.04
                color: "#12ffffff"
                border.color: "#26ffffff"
                border.width: 1
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: window.width * 0.02
                    clip: true
                    TextArea {
                        id: userInput
                        wrapMode: Text.Wrap
                        placeholderText: "Send a message to AI..."
                        color: "#ffffff"
                        font.pixelSize: window.height * 0.025
                        background: null
                        placeholderTextColor: "#66ffffff"
                    }
                }
            }
            ColumnLayout {    
                Rectangle {
                    Layout.preferredWidth: window.height * 0.12
                    Layout.preferredHeight: window.height * 0.12
                    radius: window.width * 0.04
                    color: textMouseArea.pressed ? "#2effffff" : "#12ffffff"
                    border.color: "#26ffffff"
                    border.width: 1
                    Behavior on color { ColorAnimation { duration: 100 } }
                    Image {
                        source: "qrc:/send.svg"
                        width: parent.width * 0.5
                        height: width
                        anchors.centerIn: parent
                        opacity: backend.busy ? 0.3 : 1.0
                    }
                    MouseArea {
                        id: textMouseArea
                        anchors.fill: parent
                        enabled: !backend.busy
                        onClicked: {
                            if (userInput.text.trim() === "") return
                            backend.prompt(userInput.text)
                            userInput.text = ""
                        }
                    }
                }
                Rectangle {
                    id: micBtn
                    Layout.preferredWidth: window.height * 0.12
                    Layout.preferredHeight: window.height * 0.12
                    radius: window.width * 0.04
                    color: recording ? "#cf1010" : (micMouseArea.pressed ? "#2effffff" : "#12ffffff")
                    border.color: "#26ffffff"
                    border.width: 1
                    Behavior on color { ColorAnimation { duration: 100 } }
                    Image {
                        source: "qrc:/mic.svg"
                        width: parent.width * 0.5
                        height: width
                        anchors.centerIn: parent
                        opacity: backend.busy ? 0.3 : 1.0
                    }
                    MouseArea {
                        id: micMouseArea
                        anchors.fill: parent
                        enabled: !backend.busy
                        onClicked: {
                            if (recording) {
                                backend.stopRecording()
                                recording = false
                            }
                            else {
                                backend.startRecording()
                                recording = true
                            }
                        }
                    }
                }
            }
        }
    }
    Popup {
        id: devicePicker
        anchors.centerIn: parent
        modal: true
        visible: false
        background: Rectangle {
            radius: window.width * 0.04
            color: "#2c2c2e"
            border.color: "#26ffffff"
            border.width: 1
        }
        contentItem: Column {
            spacing: window.height * 0.015
            padding: window.width * 0.04
            Text {
                text: "Select Microphone"
                color: "#99ffffff"
                font.pixelSize: window.height * 0.02
            }
            Repeater {
                model: backend.devicenames
                delegate: Rectangle {
                    width: window.width * 0.5
                    height: window.height * 0.07
                    radius: window.width * 0.03
                    color: deviceMouseArea.pressed ? "#2effffff" : "#12ffffff"
                    border.color: "#26ffffff"
                    border.width: 1
                    Behavior on color { ColorAnimation { duration: 100 } }
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: "#ffffff"
                        font.pixelSize: window.height * 0.022
                        wrapMode: Text.Wrap
                        width: parent.width * 0.9
                        horizontalAlignment: Text.AlignHCenter
                    }
                    MouseArea {
                        id: deviceMouseArea
                        anchors.fill: parent
                        onClicked: {
                            backend.select_device(index)
                            devicePicker.close()
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: {
        if (backend.devicenames.length > 1) devicePicker.open()
    }
    Text {
        text: "Version " + backend.version()
        color: "#b0adad"
        font.pixelSize: window.height * 0.02
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: window.width * 0.03
    }
    Connections {
        target: backend
        function onResponseReceived(text) {
            responseLabel.text = text
        }
        function onError(error) {
            responseLabel.text = "Error accessing AI: " + error
        }
    }
}
