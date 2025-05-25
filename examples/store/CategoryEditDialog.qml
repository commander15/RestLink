import QtQuick 2.15
import QtQuick.Controls 2.15

import RestLink 2.0

Dialog {
    id: dialog
    
    property var category
    property alias api: saveRequest.api
    
    function change(which) {
        category = which;
        open();
    }

    signal success()

    footer: DialogButtonBox {
        standardButtons: DialogButtonBox.Save | DialogButtonBox.Cancel
        onAccepted: dialog.accept()
        onRejected: dialog.reject()
    }
    
    onAccepted: function() {
        var cat = Object();
        cat.name = nameInput.text;
        cat.description = category.description;
        saveRequest.body.data = JSON.stringify(cat);
        saveRequest.run();
        console.log(saveRequest.body.data);
    }
    
    TextField {
        id: nameInput
        text: dialog.category.name
        placeholderText: "New name"
        anchors.fill: parent
    }
    
    Request {
        id: saveRequest
        method: Request.Put
        endpoint: '/categories/{id}'
        body.json: true
        autoRun: false
        
        onFinished: function() {
            if (response.success) {
                console.log("Category updated successfully.");
                dialog.success();
            } else {
                console.log("HTTP " + response.httpStatusCode);
                console.log(response.body);
            }
        }
        
        RequestParameter {
            name: "id"
            value: dialog.category.id
            type: RequestParameter.UrlPath
        }
    }
}
