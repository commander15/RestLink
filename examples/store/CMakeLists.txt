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
    QML_FILES ProductDetailsPage.qml
    QML_FILES ResponseViewer.qml
    QML_FILES AppBar.qml
    QML_FILES CategoryEditDialog.qml
)

target_link_libraries(Store PRIVATE Qt::Quick Qt::Sql)

target_compile_definitions(Store PRIVATE TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/testdata/store")
