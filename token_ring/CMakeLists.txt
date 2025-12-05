file(GLOB SOURCES CONFIGURE_DEPENDS "impl/*.cpp")

add_library(libtokenring ${SOURCES})
target_include_directories(libtokenring PUBLIC include)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(test_token_ring ${TEST_SOURCES})
target_link_libraries(test_token_ring PRIVATE libtokenring)
