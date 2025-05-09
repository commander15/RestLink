import RestLink 2.0

// This file just showcase a clean way to create a QML component on top of a RestLink one

Request {
    id: request

    enum Type { Movie = 0, TV = 1, Multi = 2 }

    property string query
    property int type: TmdbSearchRequest.Type.Multi

    function typeString(type: int): string {
        switch (type) {
        case TmdbSearchRequest.Type.Movie:
            return "movie";

        case TmdbSearchRequest.Type.TV:
            return "tv";

        default:
            return "multi";
        }
    }
    
    method: Request.Get
    endpoint: "/search/" + typeString(type)
    
    RequestParameter {
        name: "query"
        value: request.query
    }
}
