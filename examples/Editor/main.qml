import QtQuick 2.1

Rectangle {
    id: rectangle
    color: "red"
    width: 140
    height: 100
    z : 2

    Text {
        id: text
        text: "This is QML code.\n(Click to pause)"
        font.pointSize: 14
        anchors.centerIn: parent
        PropertyAnimation {
            id: animation
            target: text
            property: "rotation"
            from: 0; to: 360; duration: 5000
            loops: Animation.Infinite
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: animation.paused ? animation.resume() : animation.pause()
    }
    Component.onCompleted: animation.start()
}
