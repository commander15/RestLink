# RestLink

![RestLink](./docs/logo.png)

RestLink is a C++ library designed to facilitate easy integration with RESTful APIs, providing an abstraction layer for making HTTP requests and handling responses. It offers a convenient API to configure, send, and receive data from REST endpoints, with built-in support for handling parameters, headers, and error management.

## Features

- Simplified API integration for RESTful services.
- Support for setting API configurations (URL, name, version, etc.).
- Parameter management for API requests.
- Error handling.
- Integration with Qt's network stack for efficient communication.

## Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/commander15/RestLink.git
    ```

2. Add `RestLink` to your project by including the necessary headers in your CMake file.

    ```cmake
    include_directories(path_to_RestLink)
    ```

3. Link the `RestLink` library with your project.

    ```cmake
    target_link_libraries(YourProjectName RestLink::RestLink)
    ```

---

## C++ Example

Below is a simple C++ example demonstrating how to use RestLink to make an API request and handle the response.

### Example Code (C++)

[View Source](./examples/tmdb-desktop/main.cpp)

```cpp
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstandardpaths.h>

#include <RestLink/restlink.h>

using namespace RestLink;

void runRequest(const Request &request, Api *api)
{
    // We will save all responses on your Desktop folder, TMDB sub folder

    static QDir folder(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + '/' + api->name());
    if (!folder.exists())
        folder.mkpath(".");

    QString url = request.urlPath();
    if (url.count('/') > 1)
        folder.mkpath(url.section("/", 1, -2));

    QFile *file = new QFile(folder.path() + url + ".json", api);

    // Pretty assync handling alert !
    api->get(request, [file, api](Response *reply) {
        // Here application output will tell you all what is happening
        // (errors included)

        QTextStream out(stdout);
        out << reply->endpoint() << " => ";

        if (reply->isSuccess()) {
            const QByteArray data = reply->readBody();

            if (file->open(QIODevice::WriteOnly|QIODevice::Text)) {
                file->write(data);
                file->flush();
                file->close();
                file->deleteLater();
            }

            // Download completed !
            out << data.size() << " byte(s) downloaded" << Qt::endl;
        } else if (!reply->hasHttpStatusCode()) {
            // Logging this HTTP error
            out << "HTTP " << reply->httpStatusCode() << ' ' << reply->httpReasonPhrase() << Qt::endl;
        } else if (reply->hasNetworkError()) {
            // Logging this network error
            out << "Network error " << reply->networkError() << ": " << reply->networkErrorString() << Qt::endl;
        } else {
            // Logging this ... wait, we don't known what happened < 1% of time ;)
            out << "Oops ! - something weired happened" << Qt::endl;
        }

        // Let's count before quit !
        // 3, 2, 1, ... bye bye !
        int count = api->property("count").toInt() - 1;
        if (count == 0)
            qApp->quit();
        else
            api->setProperty("count", count);
    });
}

void run(Api *api)
{
    // We prepare some requests in a list

    QList<Request> requests;
    requests.append("/configuration"); // geeky things
    requests.append("/discover/movie"); // movie discovery
    requests.append("/discover/tv"); // tv discovery

    {
        Request request("/search/company");

        // We are seaching Marvel Studios, let's go from Marvel to have more results
        request.addQueryParameter("query", "Marvel");

        requests.append(request);
    }

    {
        Request request("/movie/{movie_id}");

        // Do you like Marvel Venom movie ?
        request.setPathParameter("movie_id", 335983);

        requests.append(request);
    }

    // We run them all
    for (const Request &request : std::as_const(requests))
        runRequest(request, api);

    api->setProperty("count", requests.size());
}

class Interceptor : public RequestInterceptor
{
public:
    Request intercept(RequestHandler::Method method, const Request &request, const Body &body) override
    {
        // Just a showcase
        return request;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QLoggingCategory::setFilterRules("restlink.info=true");

    Api api;
    api.setName("TMDB"); // Optional
    api.setVersion(QVersionNumber(3)); // Optional
    api.setUrl(QUrl("http://api.themoviedb.org/3")); // Mandatory
    api.setBearerToken(TMDB_BEARER_TOKEN); // Mandatory if you want to Avoid HTTP - Forbidden :)

    // Just to showcase the thing
    Interceptor interceptor;
    api.addRequestInterceptor(&interceptor);

    // Let's go !
    run(&api);

    return app.exec();
}
```

### In this example:

- We create and configure an `Api` instance with base URL and bearer token.
- Multiple `Request` objects are prepared and customized with parameters or path variables.
- Each request is executed using `api->get(...)` with a lambda callback handling success and error cases.
- JSON responses are saved into files under the Desktop directory.

##### This example can be found on: examples/tmdb-desktop

---

## QML Example

[View source](./examples/tmdb/Main.qml)

RestLink can also be integrated into QML-based applications, making it easy to use API functionality in the user interface. Below is a simple QML example.

### Example Code (QML)

```qml
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

import RestLink 2.0

// Let's test RestLink in QML using TMDB API

ApplicationWindow {
    id: win

    // If okay, TMDB V3 is our window title
    title: tmdb.name + " V" + tmdb.version

    // Mobile like !
    width: 320
    height: 640

    // Be visible :)
    visible: true

    header: TextField {
        id: query

        text: "Halo"
        placeholderText: "Query..."

        // Run request with the new query on return
        onAccepted: request.run()
    }

    ListView {
        id: view

        delegate: MediaDelegate {
            readonly property var media: model

            // Yeah, i'm lazy, just followed TDMB API documentation ;)
            poster: (modelData.poster_path ? modelData.poster_path : "")
            title: (modelData.title ? modelData.title : modelData.name)
            overview: (modelData.overview ? modelData.overview : "")
            popularity: (modelData.popularity ? modelData.popularity : 0.0)
            year: 2025 // Something is wrong here

            accentColor: "pink"
            width: ListView.view.width
            height: 96
        }

        spacing: 5

        anchors.fill: parent
        anchors.margins: 9
    }

    Request {
        id: request

        // Just to mention, it's already the default
        method: Request.Get // Http method

        // Documentation on: https://developer.themoviedb.org
        endpoint: "/search/multi"

        // Don't forget it !
        api: tmdb

        RequestParameter {
            name: "query"
            value: query.text

            // Just to mention, it's already the default
            type: RequestParameter.UrlQuery
        }

        // We just log the full request Url
        // We do it here cause response object got created just before this signal has been emitted
        onStarted: console.log(response.url)

        // Let's understand what server said !
        onFinished: function() {
            if (response.success) {
                // Parsing JSON
                var json = JSON.parse(response.body);

                // Filling view with JSON response
                view.model = json.results;

                // Checking compression header, have you built RestLink with ZLIB ?
                // gzip and defalte supported without manual intervention
                console.log("Compression: " + response.header('Content-Encoding'));
            } else if (response.hasHttpStatusCode) {
                // Logging HTTP error
                console.log("HTTP " + response.httpStatusCode + ' ' + response.httpReasonPhrase);
            } else if (response.hasNetworkError) {
                // Logging network error
                console.log("Network Error " + response.networkError + ' ' + response.networkErrorString);
            } else {
                // Logging unknown error
                console.log("Unknown error occured");
            }
        }
    }

    Api {
        id: tmdb

        name: "TMDB" // Optional
        version: "3" // Optional, yeah it's a string
        url: "https://api.themoviedb.org/3" // Mandatory

        // This will automatically add the Authorization header for you,
        // just put your Bearer token without 'Bearer' prefix
        // You can set the token in CMake cache variable of the same name so no need to
        // change something here directly, that will avoid some problems with versioning tools
        bearerToken: TMDB_BEARER_TOKEN

        // By default, system locale used, but we switch to US English okay ?
        locale: Qt.locale("en_US")

        // As a lazy man, i don't want to add these parameters on each requests
        // so i add them here for reuse.

        ApiParameter {
            name: "language"

            // value is used as format, this will use the locale Api property to generate language code
            // format examples: '.' for en, '-' for en-US and '_' for en_US
            value: "." // Here en will be generated is locale is en_US, TMDB API requirement

            // Correspond to the RestLink::Parameter::Locale flag
            locale: true // Makes the value of this property as language code taken from Api locale property
        }

        ApiParameter {
            name: "adult"
            value: false // i'm a child :)
        }

        ApiParameter {
            name: "Authorization"
            value: "Bearer ..."

            // By default, the parameter is a url query parameter, we don't want this
            type: ApiParameter.Header // Yes, it's a header !

            // We don't need it so we disable without removing, nice euh ?
            enabled: false // finally we don't need this since Api already manages it
        }
    }
}

// Written with ♥️ by Amadou Wanie Benjamain !
```

#### In this example:

- An `Api` object is configured with TMDB base URL and bearer token.
- A `Request` object is created and bound to the `Api`, with a `query` parameter dynamically updated from a `TextField`.
- On pressing "Enter", the request is sent using `request.run()`.
- On success, the JSON response is parsed and displayed in a `ListView` using a `MediaDelegate`.
- Errors are logged to the console, including HTTP errors, network errors, and unknown cases.

##### This example can be found on: examples/tmdb

## Documentation

The full RestLink documentation is available here: https://commander15.github.io/RestLink-Docs

---

#### Made with ♥️ by Amadou Wanie Benjamain !
