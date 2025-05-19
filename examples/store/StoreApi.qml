import RestLink 2.0

Api {
    id: store

    property bool firstRun: false
    
    name: "Store"
    version: "1.0"
    url: "sqlite:///home/commander/Projects/RestLink/build/Desktop_Qt_6_9_0-Debug/testdata/store/database.sqlite"

    readonly property Request initializationRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "../testdata/store/structure.sql"
        api: store
        autoRun: store.firstRun

        onFinished: console.log(response.success ? "Database created !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database creation !"))
    }

    readonly property Request fillingRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "../testdata/store/data.sql"
        api: store
        autoRun: store.firstRun

        onFinished: console.log(response.success ? "Database filled !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database filling !"))
    }

    readonly property Request configurationRequest: Request {
        method: Request.Post
        endpoint: "/configuration"
        body.file: "../testdata/store/configuration.json"
        api: store

        onFinished: console.log(response.success ? "API configured !" : "API configuration failed !")
    }
}
