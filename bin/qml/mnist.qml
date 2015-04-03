import QtQuick 2.2
import QtQuick.Window 2.1

Window {
    visible: true
    width: 28*16
    height: 28*16

    Rectangle {
        anchors.fill: parent;
        color: Qt.rgba(0.8, 0.8, 0.8, 1);
    }

    Canvas {
        id: canvas
        anchors.top: parent.top;
        anchors.left: parent.left;
        width: 28*8
        height: 28*8
        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = Qt.rgba(0.8, 0.8, 0.8, 1);
            ctx.fillRect(0, 0, width, height);

            var state = cpp.getState();

            for (var i in state) {
                var fanal = state[i];

                var pos = Math.floor(fanal/81);
                var x = pos%7;
                var y = Math.floor(pos / 7);

                var colors = fanal % 81;
                var upLeft = Math.floor(colors/27);
                var upRight = Math.floor((colors%27) / 9);
                var botLeft = Math.floor((colors%9) / 3);
                var botRight = colors % 3;

                ctx.fillStyle = Qt.rgba(1-upLeft*0.5, 1-upLeft*0.5, 1-upLeft*0.5, 1);
                ctx.fillRect(x * 32, y*32, 16, 16);
                ctx.fillStyle = Qt.rgba(1-upRight*0.5, 1-upRight*0.5, 1-upRight*0.5, 1);
                ctx.fillRect(x * 32 + 16, y*32, 16, 16);
                ctx.fillStyle = Qt.rgba(1-botLeft*0.5, 1-botLeft*0.5, 1-botLeft*0.5, 1);
                ctx.fillRect(x * 32, y*32 + 16, 16, 16);
                ctx.fillStyle = Qt.rgba(1-botRight*0.5, 1-botRight*0.5, 1-botRight*0.5, 1);
                ctx.fillRect(x * 32 + 16, y*32 + 16, 16, 16);
            }
        }
    }

    Canvas {
        id: canvas2
        anchors.bottom: parent.bottom;
        anchors.right: parent.right
        width: 28*8
        height: 28*8
        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = Qt.rgba(0.8, 0.8, 0.8, 1);
            ctx.fillRect(0, 0, width, height);

            var state = cpp.getExpected();

            for (var i in state) {
                var fanal = state[i];

                var pos = Math.floor(fanal/81);
                var x = pos%7;
                var y = Math.floor(pos / 7);

                var colors = fanal % 81;
                var upLeft = Math.floor(colors/27);
                var upRight = Math.floor((colors%27) / 9);
                var botLeft = Math.floor((colors%9) / 3);
                var botRight = colors % 3;

                ctx.fillStyle = Qt.rgba(1-upLeft*0.5, 1-upLeft*0.5, 1-upLeft*0.5, 1);
                ctx.fillRect(x * 32, y*32, 16, 16);
                ctx.fillStyle = Qt.rgba(1-upRight*0.5, 1-upRight*0.5, 1-upRight*0.5, 1);
                ctx.fillRect(x * 32 + 16, y*32, 16, 16);
                ctx.fillStyle = Qt.rgba(1-botLeft*0.5, 1-botLeft*0.5, 1-botLeft*0.5, 1);
                ctx.fillRect(x * 32, y*32 + 16, 16, 16);
                ctx.fillStyle = Qt.rgba(1-botRight*0.5, 1-botRight*0.5, 1-botRight*0.5, 1);
                ctx.fillRect(x * 32 + 16, y*32 + 16, 16, 16);
            }
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
            cpp.advanceState();
            canvas.markDirty();
            canvas.requestPaint();
        }
    }
}
