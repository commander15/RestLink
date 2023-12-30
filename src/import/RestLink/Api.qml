import RestLink

import QtQml

RestLinkApi {
    id: api

    property alias name: api.apiName
    property alias version: api.apiVersion
    property alias url: api.apiUrl

    default property list<QtObject> data;

    Component.onCompleted: init()
}
