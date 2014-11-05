import QtQuick 2.2
import QtQuick.Window 2.1

Window {
    visible: true
    width: 1000
    height: 700

    MouseArea {
        anchors.fill: parent
        onClicked: {
            cpp.iterate(1);
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
            var fmin = fwidth < fheight ? fwidth : fheight;

            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            ctx.strokeStyle = Qt.rgba(0.28, 0.28, 0.28, 1);
            ctx.lineWidth = 1;

            var positions = [];
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

    property var neurons : ({})
    property var expected : ({})
    property var inputs : ({})
    property var noise : ({})
    property var connections : ({})

    Component.onCompleted: {
        cpp.neuronsLit.connect(onNeuronsReceived);
        cpp.networkSet.connect(onNetworkChanged);
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
