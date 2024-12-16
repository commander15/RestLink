pragma ComponentBehavior: Bound

import RestLink

import QtQml

RestLinkApiRequest {
    id: request

    enum Operation {
        Get = 0,
        Post = 1,
        Put = 2,
        Patch = 3,
        Delete = 4
    }

    property bool autoRun: true

    default property list<QtObject> resources;

    readonly property Connections _apiConnections: Connections {
        target: request.api

        function onReady() {
            if (request.autoRun)
                request.run();
        }
    }
}
