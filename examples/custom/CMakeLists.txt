qt_add_executable(Custom
    main.cpp
    usercontroller.h usercontroller.cpp
    worker.h worker.cpp
)

target_link_libraries(Custom PRIVATE Qt::Sql)
