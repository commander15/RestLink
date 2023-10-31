import QtQml 2.14

import RestLink 1.0

QtObject {
    id: joke

    property string jokeString
    property string jokeAnswer
    property string jokeType

    property bool allowDev: false
    property alias loading: request.running

    property ApiRequest request: ApiRequest {
        id: request

        endpoint: "/random"

        api: Api {
            id: api
            apiConfigurationUrl: "https://commander-systems.000webhostapp.com/RestLink/APIs/Blagues.json"
        }

        onFinished: function() {
            var json = JSON.parse(response);
            joke.jokeString = json.joke;
            joke.jokeAnswer = json.answer;
            joke.jokeType = json.type;
        }

        ApiRequestParameter {
            name: "disallow"
            value: "dev"
            scope: ApiRequestParameter.UrlQuery
            enabled: !view.allowDev
        }
    }
}
