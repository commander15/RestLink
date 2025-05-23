import RestLink 2.0

Api {
    id: store

    property bool firstRun: true
    
    name: "Store"
    version: "1.0"
    url: "sqlite:database.sqlite"

    readonly property Request initializationRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "structure.sql"
        api: store
        autoRun: store.firstRun

        onFinished: console.log(response.success ? "Database created !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database creation !"))
    }

    readonly property Request fillingRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "content.sql"
        api: store
        autoRun: store.firstRun

        onFinished: console.log(response.success ? "Database filled !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database filling !"))
    }

    readonly property Request configurationRequest: Request {
        method: Request.Post
        endpoint: "/configuration"
        body.file: "configuration.json"
        api: store

        onFinished: console.log(response.success ? "API configured !" : "API configuration failed !")
    }
}
