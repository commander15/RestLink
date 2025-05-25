import RestLink 2.0

Api {
    id: store

    property bool firstRun: false
    
    name: "Store"
    version: "1.0"
    url: "sqlite:database.sqlite"

    readonly property Request versionCheckRequest: Request {
        function initialize() {
            store.initializationRequest.run();
            store.fillingRequest.run();
        }

        method: Request.Post
        endpoint: "/query"
        body.data: "SELECT * FROM DatabaseVersions"
        api: store

        onFinished: {
            if (!response.success) {
                initialize();
                store.initialized();
                return;
            }

            var version;

            version = Qt.application.version.split('.');
            var appVersion = [ parseInt(version[0]), parseInt(version[1]), parseInt(version[2]) ];

            version = JSON.parse(response.body).data;
            var dbVersion = [ version.major, version.minor, version.patch ];

            var sameVersion = true;
            var olderVersion = false;
            for (var i = 0; i < 3; ++i) {
                if (appVersion[i] !== dbVersion[i])
                    sameVersion = false;
                else if (appVersion[i] < dbVersion[i] && !olderVersion)
                    olderVersion = true;
            }

            console.log(sameVersion ? "Same !" : "Not same !");

            store.initialized();
        }

        RequestParameter {
            name: "object"
            value: true
        }
    }

    readonly property Request initializationRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "structure.sql"
        api: store
        autoRun: false

        onFinished: console.log(response.success ? "Database created !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database creation !"))
    }

    readonly property Request fillingRequest: Request {
        method: Request.Post
        endpoint: "/query"
        body.file: "content.sql"
        api: store
        autoRun: false

        onFinished: console.log(response.success ? "Database filled !" : (response.hasHttpStatusCode ? response.body : "Something went wrong during database filling !"))
    }

    readonly property Request configurationRequest: Request {
        method: Request.Post
        endpoint: "/configuration"
        body.file: "configuration.json"
        api: store

        onFinished: console.log(response.success ? "API configured !" : "API configuration failed !")
    }

    signal initialized()
}
