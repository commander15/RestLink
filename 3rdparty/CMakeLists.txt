# Bcrypt
add_library(Bcrypt STATIC Bcrypt.cpp/src/bcrypt.cpp Bcrypt.cpp/src/blowfish.cpp)
target_include_directories(Bcrypt PRIVATE Bcrypt.cpp/src/)
target_include_directories(Bcrypt PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Bcrypt.cpp/include/>)
install(TARGETS Bcrypt
    EXPORT RestLinkTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
