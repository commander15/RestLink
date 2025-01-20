import RestLink

import QtQml

ApiBase {
    id: api

    default property list<QtObject> resources;

    Component.onCompleted: init()
}
