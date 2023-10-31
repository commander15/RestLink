set(Modules ${Qt5_FIND_COMPONENTS})

if (RESTLINK_BUILD_QML_PLUGIN)
    list(APPEND Modules Qml)

    if (RESTLINK_BUILD_EXAMPLES)
        list(APPEND Modules Quick)
    endif()
endif()

find_package(Qt5 CONFIG REQUIRED COMPONENTS ${Modules})
