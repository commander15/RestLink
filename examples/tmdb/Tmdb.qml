import RestLink

Api {
    id: api

    property string key
    property string token
    property bool adult: false
    property bool secure: false

    name: "TMDB"
    version: "3"
    url: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    RequestParameter {
        name: "Authorization"
        value: "Bearer " + api.token
        type: RequestParameter.Header
        authentication: true
        enabled: api.token.length > 0
    }

    RequestParameter {
        name: "api_key"
        value: api.key
        authentication: true
        enabled: api.key.length > 0
    }

    RequestParameter {
        name: "language"
        locale: true
    }

    RequestParameter {
        name: "adult"
        value: (api.adult ? "true" : "false")
    }
}
