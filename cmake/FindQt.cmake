set(CMAKE_AUTOMOC ON)

if (Qt_FOUND)
    return()
endif()

set(Modules ${Qt_FIND_COMPONENTS})

if (RESTLINK_BUILD_QML_PLUGIN)
    list(APPEND Modules Qml)

    if (RESTLINK_BUILD_EXAMPLES)
        list(APPEND Modules Quick)
    endif()
endif()

set(Versions 5 6)

foreach (Version ${Versions})
    set(Qtx Qt${Version})
    find_package(${Qtx} CONFIG COMPONENTS ${Modules})

    if (${Qtx}_FOUND)
        message("Found ${Qtx}: ${${Qtx}_DIR} (found version \"${${Qtx}_VERSION}\")")

        foreach (Module ${Modules})
            set(QtModule Qt::${Module})
            set(QtxModule ${Qtx}::${Module})

            if (NOT TARGET ${QtModule} AND TARGET ${Qt5Module})
                add_library(${QtModule} ALIAS ${QtxModule})
            endif()
        endforeach()

        set(Qt_FOUND TRUE)
        break()
    endif()
endforeach()

if (Qt_FOUND)
    if (QT_VERSION VERSION_GREATER_EQUAL 6.6.1)
        qt_standard_project_setup()
    endif()

    if (QT_VERSION VERSION_GREATER_EQUAL 6 AND Qt6Qml_FOUND)
        qt_policy(SET QTP0001 NEW)
    endif()
else()
    # 404 ;)
endif()
