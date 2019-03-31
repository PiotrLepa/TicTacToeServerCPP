import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import io.qt.Backend 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Tic Tac Toe Server")

    Backend {
        id: backend
        onSmbConnected: {
            textArea.append(addMsg("Somebody has connected"));
        }
        onSmbDisconnected: {
            textArea.append(addMsg("Somebody has disconnected"));
        }
        onNewMessage: {
            textArea.append(addMsg("New message: " + msg));
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent

        TextArea {
            id: textArea
            readOnly: true
            selectByMouse : true
            font.pixelSize: 14
            wrapMode: TextInput.WrapAnywhere
            anchors.fill: parent
        }
    }

    Component.onCompleted: {
            textArea.text = addMsg("Application started\n- - - - - -");
        }

        function addMsg(someText)
        {
            return "[" + currentTime() + "] " + someText;
        }

        function currentTime()
        {
            var now = new Date();
            var nowString = ("0" + now.getHours()).slice(-2) + ":"
                    + ("0" + now.getMinutes()).slice(-2) + ":"
                    + ("0" + now.getSeconds()).slice(-2);
            return nowString;
        }
}
