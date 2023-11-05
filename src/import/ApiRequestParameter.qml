import RestLink 1.0

import QtQml 2.14

RestLinkApiRequestParameter {
    id: parameter

    enum Scope {
        UrlPath = 0,
        UrlQuery = 1,
        Header = 2
    }

    default property list<QtObject> data;
}
