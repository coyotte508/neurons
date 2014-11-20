import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Window {
    visible: true
    width: 1000
    height: 750

    RowLayout {
        id: layout
        anchors.fill: parent
        anchors.bottomMargin: 50
        spacing: 0

        ScrollView {
            id: controls
            Layout.preferredWidth: cpp.cliqueCount() > 0 ? 100 : 0
            Layout.fillHeight: true

            ListView {
                id: list;
                anchors.fill: parent;
                model: cpp.cliqueCount();
                delegate: Rectangle {
                    height: 50;
                    width: parent.width;
                    color: (model.index %2 === 0 ? "darkgray" : "lightgray");

                    Text {
                        anchors.centerIn: parent
                        text: "Clique " + model.index
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            cpp.setClique(index);
                        }

                        onHoveredChanged: {
                            if (containsMouse) {
                                lastInfo = index;

                                var clique = cpp.clique(index);

                                info = {}

                                for (var n in clique) {
                                    info[clique[n]] = true;
                                }

                                overlapCanvas.requestPaint();
                            } else if (lastInfo == index) {
                                info = {};
                                overlapCanvas.requestPaint();
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: graphics
            Layout.fillHeight: true
            //Layout.preferredWidth: 900
            Layout.fillWidth: true

            MouseArea {
                acceptedButtons: Qt.RightButton | Qt.LeftButton
                anchors.fill: parent
                onClicked: {
                    //cpp.iterate(1);
                    var nclusters = cpp.clusters();
                    var nfanals = cpp.fanals();

                    for (var counter = 0; counter < nclusters*nfanals; counter = counter + 1) {
                        if (mouseX >= positions[counter][0] && mouseY >= positions[counter][1]
                                && mouseX <= positions[counter][0]+fmin && mouseY <= positions[counter][1]+fmin)
                        {
                            if (mouse.button == Qt.LeftButton) {
                                cpp.addInput(counter);
                            } else {
                                var connections = cpp.connections(counter);
                                console.log(JSON.stringify(connections));
                            }

                            break;
                        }
                    }
                }
            }

            Canvas{
                id: overlapCanvas
                z: canvas.z + 1
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.lineWidth = 0;
                    ctx.fillStyle = Qt.rgba(1, 0.7, 1, 1);

                    for (var neuron in info) {
                        var x = positions[neuron][0];
                        var y = positions[neuron][1];

                        ctx.beginPath();
                        ctx.ellipse(x+2, y+2, fmin-4, fmin-4);
                        ctx.fill();
                        ctx.closePath();
                    }
                }
            }

            Canvas {
                id: canvas
                anchors.fill: parent
                onPaint: {
                    var nclusters = cpp.clusters();
                    var nfanals = cpp.fanals();

                    var clustersParam = getLinesColumn(nclusters, width, height, 2);
                    var fanalsParam = getLinesColumn(nfanals, clustersParam.width, clustersParam.height, 2);

                    //console.log(JSON.stringify(clustersParam));
                    //console.log(JSON.stringify(fanalsParam));

                    var cperline = clustersParam.columns;
                    var fperline = fanalsParam.columns;

                    var fwidth = fanalsParam.width;
                    var fheight = fanalsParam.height;
                    fmin = fwidth < fheight ? fwidth : fheight;

                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);
                    ctx.strokeStyle = Qt.rgba(0.28, 0.28, 0.28, 1);
                    ctx.lineWidth = 1;

                    positions = [];
                    for (var i = 0; i < clustersParam.lines; i++) {
                        for (var j = 0; j < cperline && i*cperline+j < nclusters; j++) {
                            var cy = 3 + i * (clustersParam.height + 2);
                            var cx = 3 + j * (clustersParam.width + 2);

                            for (var fi = 0; fi < fanalsParam.lines; fi++) {
                                for (var fj = 0; fj < fperline && fi*fperline+fj < nfanals; fj++) {
                                    var fy = fi * (fheight + 2);
                                    var fx = fj * (fwidth + 2);

                                    positions.push([cx+fx, cy+fy]);
                                }
                            }
                        }
                    }

                    for (var k in connections) {
                        for (var v in connections[k]) {
                            var dest = connections[k][v];
                            ctx.moveTo(positions[k][0] + fmin/2, positions[k][1] + fmin/2);
                            ctx.lineTo(positions[dest][0] + fmin/2, positions[dest][1] + fmin/2);
                        }
                    }

                    ctx.stroke();

                    for (var counter = 0; counter < nclusters*nfanals; counter = counter + 1) {
                        var x = positions[counter][0];
                        var y = positions[counter][1];

                        ctx.ellipse(x, y, fmin, fmin);

                        if (counter in neurons) {
                            //console.log("neuron");
                            ctx.stroke();
                            ctx.beginPath();
                            ctx.ellipse(x, y, fmin, fmin);
                            ctx.fillStyle = Qt.rgba(0, 0, 0, 1);
                            ctx.fill();
                            ctx.closePath();
                            ctx.beginPath();
                        } else if (counter in noise) {
                            ctx.stroke();
                            ctx.beginPath();
                            ctx.ellipse(x, y, fmin, fmin);
                            ctx.fillStyle = Qt.rgba(0.75, 0.75, 0.75, 1);
                            ctx.fill();
                            ctx.closePath();
                            ctx.beginPath();
                        }

                        if (counter in inputs) {
                            //console.log("input");
                            ctx.stroke();
                            ctx.beginPath();
                            ctx.ellipse(x, y, fmin, fmin);
                            ctx.fillStyle = Qt.rgba(1, 0, 0, 1);
                            ctx.fill();
                            ctx.closePath();
                            ctx.beginPath();
                        } else if (counter in expected) {
                            //console.log("expected");
                            ctx.stroke();
                            ctx.beginPath();
                            ctx.ellipse(x, y, fmin, fmin);
                            ctx.fillStyle = Qt.rgba(0, counter in neurons ? 0.8 : 0.2, counter in neurons ? 0.2 : 1, 1);
                            ctx.fill();
                            ctx.closePath();
                            ctx.beginPath();
                        }
                    }

                    ctx.stroke();
                }
            }
        }
    }

    RowLayout {
        anchors.left : parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 50

        Button {
            Layout.alignment: Qt.AlignCenter
            text: "Clear"
            onClicked: {
                cpp.clear();
                canvas.requestPaint();
            }
        }

        Button {
            Layout.alignment: Qt.AlignCenter
            text: "Clear Inputs"
            onClicked: {
                cpp.clearInputs();
                canvas.requestPaint();
            }
        }

        Button {
            id: iterate
            text: "Iterate"
            onClicked: {
                cpp.iterate(parseInt(iterNum.text));
            }
        }

        TextField {
            id:iterNum
            anchors.left: iterate.right
            anchors.leftMargin: 8
            text: "1"
            placeholderText: "1000"
        }

        Text {
            id: cliqueSizeL
            text: "Max lit"
        }

        TextField {
            id: cliqueSize
            anchors.left: cliqueSizeL.right
            anchors.leftMargin: 8
            text: "15"
            placeholderText: "..."
        }
    }

    property var neurons : ({})
    property var expected : ({})
    property var inputs : ({})
    property var noise : ({})
    property var connections : ({})
    property var positions: []
    property real fmin: 0
    property var info: ({})
    property int lastInfo: -1

    Component.onCompleted: {
        cpp.neuronsLit.connect(onNeuronsReceived);
        cpp.networkSet.connect(onNetworkChanged);
        cpp.iterate(1);
    }

    function getLinesColumn(n, width, height, spacing) {
        var ratio = width/height;
        var ncolumns = Math.max(1,Math.floor(Math.sqrt(ratio*n)));
        var nlines = Math.floor(n/ncolumns);

        while (ncolumns * nlines < n) {
            ncolumns++;
        }

        var clusterWidth = Math.floor(width/ncolumns - spacing);
        var clusterHeight = Math.floor(height/nlines - spacing);

        return {"lines": nlines, "columns": ncolumns, "height": clusterHeight, "width" : clusterWidth}
    }

    function onNeuronsReceived(n) {
        neurons = {}
        expected = {}
        inputs = {}
        noise = {}

        var ex = cpp.expected();
        var inp = cpp.inputs();
        var noi = cpp.noise();
        connections = cpp.connections();

        for (var i = 0; i < n.length; i++) {
            neurons[n[i]] = true;
        }

        for (i = 0; i < ex.length; i++) {
            expected[ex[i]] = true;
        }

        for (i = 0; i < inp.length; i++) {
            inputs[inp[i]] = true;
        }

        for (i = 0; i < noi.length; i++) {
            noise[noi[i]] = true;
        }

        canvas.requestPaint();
    }

    function onNetworkChanged(nclusters, nfanals) {
        neurons = {}
    }
}
