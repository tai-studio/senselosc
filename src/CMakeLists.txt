# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${senselosc_SOURCE_DIR}/include/modern/*.hpp")
set(MODERN_HEADER_LIST "${senselosc_SOURCE_DIR}/include/modern/lib.hpp")


# sensosc

file(GLOB SENSOSC_HEADER_LIST CONFIGURE_DEPENDS "${senselosc_SOURCE_DIR}/include/sensosc/*.hpp")
# set(SENSOSC_HEADER_LIST "${senselosc_SOURCE_DIR}/include/sensosc/Morph.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(sensosc Morph.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(sensosc PUBLIC ../include)
target_include_directories(sensosc PRIVATE "${SENSEL_INCLUDE_DIRS}")
# This depends on sensel
target_link_libraries(sensosc ${SENSEL_LIBRARIES})

# All users of this library will need at least C++11
target_compile_features(sensosc PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files sensosc"
  FILES ${SENSEOSC_HEADER_LIST})


