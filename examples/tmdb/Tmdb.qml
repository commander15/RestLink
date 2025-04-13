import RestLink

Api {
    id: api

    property string key
    property bool adult: false
    property bool secure: false

    name: "TMDB"
    version: "3"
    url: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    ApiParameter {
        name: "api_key"
        value: api.key
        authentication: true
        enabled: api.key.length > 0
    }

    ApiParameter {
        name: "language"
        locale: true
    }

    ApiParameter {
        name: "adult"
        value: (api.adult ? "true" : "false")
    }
}
