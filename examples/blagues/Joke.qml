import QtQml 2.14

import RestLink 1.0

QtObject {
    id: joke

    property string jokeString
    property string jokeAnswer
    property string jokeType

    property bool allowDev: false
    property alias loading: request.running

    property Api api: Api {
        configurationUrl: "https://commander-systems.000webhostapp.com/RestLink?app_id=47cb8b8c-7efc-11ee-905d-836d044cadfc&app_version=2.0"
    }

    signal jokeChanged()

    property ApiRequest request: ApiRequest {
        id: request

        endpoint: (!joke.allowDev ? "/random" : "/type/dev/random")

        api: joke.api

        onCompleted: function() {
            var json = JSON.parse(response.data);
            joke.jokeString = json.joke;
            joke.jokeAnswer = json.answer;
            joke.jokeType = json.type;
            joke.jokeChanged();
        }

        onError: function() {
            console.log("Error !");
        }

        ApiRequestParameter {
            name: "disallow"
            value: "dev"
            scope: ApiRequestParameter.UrlQuery
            enabled: !joke.allowDev
        }
    }

    property ApiRequest apiReq: ApiRequest {
        endpoint: "/discover/movie"
        api: joke.api

        onCompleted: console.log(JSON.parse(response.data).total_pages)
    }
}
