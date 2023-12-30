set(Modules ${Qt6_FIND_COMPONENTS})

if (RESTLINK_BUILD_QML_PLUGIN)
    list(APPEND Modules Qml)

    if (RESTLINK_BUILD_EXAMPLES)
        list(APPEND Modules Quick)
    endif()
endif()

find_package(Qt6 CONFIG REQUIRED COMPONENTS ${Modules})
