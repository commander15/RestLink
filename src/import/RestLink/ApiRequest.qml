import RestLink

import QtQml

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

    default property list<QtObject> data;

    Component.onCompleted: run()
}
