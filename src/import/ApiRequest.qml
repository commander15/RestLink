import RestLink 1.0

import QtQml 2.14

RestLinkApiRequest {
    id: request

    enum RequestType {
        GetRequest,
        PostRequest,
        PutRequest,
        PatchRequest,
        DeleteRequest
    }

    property int verb: ApiRequest.GetRequest

    readonly property bool finished: !running

    default property list<QtObject> data;

    Component.onCompleted: run()
}
