include(FetchContent)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS CACHE STRING "")

# Compression
if (RESTLINK_SUPPORT_COMPRESSION)
    include(${CMAKE_CURRENT_LIST_DIR}/CompressionDependencies.cmake)
endif()

# Excel
if (RESTLINK_SUPPORT_EXCEL)
    include(${CMAKE_CURRENT_LIST_DIR}/ExcelDependencies.cmake)
endif()
