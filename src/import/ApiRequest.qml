import QtQml 2.14

import RestLink 1.0

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

    readonly property alias downloadedBytes: internal.downloadedBytes
    readonly property alias totalBytes: internal.totalBytes
    readonly property bool finished: !running

    property list<ApiRequestParameter> parameters
    default property alias children: request.parameters

    readonly property QtObject _internal : QtObject {
        id: internal

        property real downloadedBytes: 0
        property real totalBytes: 0

        property Connections c0: Connections {
            target: request.apiReply

            function onDownloadProgress(downloaded, total) {
                internal.downloadedBytes = downloaded;
                internal.totalBytes = total;
            }
        }
    }

    Component.onCompleted: run()
}
