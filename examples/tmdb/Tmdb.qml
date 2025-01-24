import RestLink

Api {
    id: api

    property string token
    property string key
    property bool adult: false
    property bool secure: false

    name: "TMDB"
    version: "3"
    url: "http" + (secure ? 's' : '') + "://api.themoviedb.org/" + version

    ApiParameter {
        name: "Authorization"
        value: "Bearer " + api.token
        type: ApiParameter.Header
        authentication: true
        enabled: api.token.length > 0
    }

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
