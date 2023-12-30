import RestLink

import QtQml

RestLinkApiRequestParameter {
    id: parameter

    enum Scope {
        UrlPath = 0,
        UrlQuery = 1,
        Header = 2
    }

    default property list<QtObject> data;
}
