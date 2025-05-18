qt_add_executable(Store
    main.cpp
)

qt_add_qml_module(Store
    URI Store
    VERSION 1.0
    QML_FILES App.qml
    QML_FILES StoreApi.qml
    QML_FILES ProductListPage.qml
    QML_FILES ProductItemDelegate.qml
    QML_FILES CategoryListPage.qml
    RESOURCES resources.qrc
    QML_FILES ConfigurationPage.qml
    QML_FILES CategoryItemDelegate.qml
)

target_link_libraries(Store PRIVATE Qt::Quick Qt::Sql)
