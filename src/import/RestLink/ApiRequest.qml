import RestLink

import QtQml

RestLinkApiRequest {
    id: request

    enum RequestType {
        GetRequest = 0,
        PostRequest = 1,
        PutRequest = 2,
        PatchRequest = 3,
        DeleteRequest = 4
    }

    property alias verb: request.type

    default property list<QtObject> resources;

    Component.onCompleted: run()
}
