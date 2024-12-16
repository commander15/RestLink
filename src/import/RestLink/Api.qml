import RestLink

import QtQml

RestLinkApi {
    id: api

    default property list<QtObject> resources;

    Component.onCompleted: init()
}
