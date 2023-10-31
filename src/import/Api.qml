import QtQml 2.14

import RestLink 1.0

RestLinkApi {
    id: api

    default property var children

    Component.onCompleted: registerParameters()
}
