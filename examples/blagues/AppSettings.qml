import QtCore
import QtQml 2.14
import QtQml.Models 2.14
import QtQuick 2.14

Settings {
    id: settings
    
    property bool devEnabled: true
    property string jokeModel: "[]"
    property bool firstRun: true

    Component.onDestruction: function() {
        if (firstRun)
            firstRun = false;
    }
}
