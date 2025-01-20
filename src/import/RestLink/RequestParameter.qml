import RestLink

import QtQml

RequestParameterBase {
    id: parameter

    enum Type {
        UrlPath = 0,
        UrlQuery = 1,
        Header = 2
    }

    property bool authentication: false
    property bool secret: false
}
