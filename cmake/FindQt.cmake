set(Modules ${Qt_FIND_COMPONENTS})

if (RESTLINK_BUILD_QML_PLUGIN)
    list(APPEND Modules Qml)

    if (RESTLINK_BUILD_EXAMPLES)
        list(APPEND Modules Quick)
    endif()
endif()

include(CMakeFindDependencyMacro)
find_dependency(Qt6 CONFIG REQUIRED COMPONENTS ${Modules})

if (Qt6Qml_FOUND)
    qt_policy(SET QTP0001 NEW)
endif()
