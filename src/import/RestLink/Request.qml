#pragma ComponentBehavior: Bound

import RestLink

import QtQml

RequestBase {
    id: request

    enum Operation {
        Head = 0,
        Get = 1,
        Post = 2,
        Put = 3,
        Patch = 4,
        Delete = 5
    }

    property bool autoRun: true

    default property list<QtObject> resources;

    readonly property Connections _apiConnections: Connections {
        target: request.api
        enabled: request.autoRun

        function onReady() {
            request.run();
        }
    }

    //onApiChanged: _firstRun()
    Component.onCompleted: _firstRun()

    function _firstRun() {
        if (autoRun && (!running && !finished) && api && api.ready) {
            run();
        }
    }
}
