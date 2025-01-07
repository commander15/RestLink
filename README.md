# RestLink

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

## C++ Example

Below is a simple C++ example demonstrating how to use RestLink to make an API request and handle the response.

### Example Code (C++)

```cpp
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>

#include <RestLink/api.h>
#include <RestLink/apirequest.h>
#include <RestLink/apireply.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Initialize RestLink API
    RestLink::Api api;

    // Set the API name, version and url
    api.setApiName("ExampleAPI");
    api.setApiVersion(1);
    api.setApiUrl(QUrl("https://api.example.com"));

    // Set User-Agent
    api.setUserAgent("MyApp/1.0");

    // Add API parameters
    RestLink::ApiRequestParameter param("key", "value");
    api.addParameter(param);

    // Create a sample API request
    RestLink::ApiRequest request;
    request.setEndpoint("/data");

    // Send the request and process the reply assynchronously when finished
    api.get(request, [](RestLink::ApiReply *reply) {
        if (reply->isSuccess()) {
            auto data = reply->readJsonObject();
            // Processing...
        } else if (reply->hasNetworkError()) {
            // Handle network error
        } else if (!reply->isHttpStatusSuccess()) {
            // Handle HTTP error
        }
    });

    return a.exec();
}
```

In this example:

We initialize the Api object and configure it with an endpoint URL, name, version, and user-agent.
We add a request parameter (key, value).
We create an ApiRequest object to set the request endpoint and HTTP method.
Finally, the request is sent, and the response is handled asynchronously using a signal.In this example:

We initialize the Api object and configure it with a base URL, name, version, and user-agent.
We add a request parameter (key, value).
We create an ApiRequest object to set the request endpoint and HTTP method.
Finally, the request is sent, and the response is handled asynchronously using a callback.

## QML Example

RestLink can also be integrated into QML-based applications, making it easy to use API functionality in the user interface. Below is a simple QML example.

### Example Code (QML)

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import RestLink 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "RestLink QML Example"

    Api {
        id: myApi
        name: "ExampleAPI"
        version: 1
        url: "https://api.example.com"
        userAgent: "MyApp/1.0"
        
        // Adding parameter that must be present on each request
        ApiRequestParameter {
            name: "key"
            value: "value"
            scope: ApiRequestParameter.UrlQuery
        }
    }
    
    ApiRequest {
        id: request
    
        endpoint: "/data"
        api: myApi
        autoRun: false // if true, request run automaticaly after object construction
        
        onFinished: function() {
            if (response.success) {
                var data = JSON.parse(response.readBody());
                console.log(data);
            } else if (response.networkError !== 0) {
                // Handle network error
                console.log(response.networkErrorString)
            } else if (response.httpStatusCode != 200) {
                // Handle HTTP error
                console.log(response.httpReasonPhrase);
            }
        }
    }

    Button {
        text: "Send API Request"
        anchors.centerIn: parent
        onClicked: request.run()
    }
}
```

### Key Points in the QML Example:

The Api object is configured with the necessary properties such as apiUrl, apiName, and apiVersion.
The ApiRequest object is configured with the endpoint, api object and autoRun disabled to make the request run only after the button press.
The onFinished signal handles the API response. If the request is successful, it logs the response data; otherwise, it logs the error message.
The Button allows triggering the API request.

## Documentation

The full RestLink documentation is available here: https://commander15.github.io/RestLink-Docs

## Contact

For more informations or questions, please reach out to amadoubenjamain@gmail.com.
