set(TARGET "${PROJECT_NAME}-patterns")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    src/Singleton.cpp
    src/StrategizedLocking.cpp
    src/ThreadSafeInterface.cpp
    src/ActiveObject.cpp
    src/MonitorObject.cpp
    $<$<TARGET_EXISTS:LibEvent::Core>:src/Reactor.cpp>
    $<$<TARGET_EXISTS:LibEvent::Core>:src/ReactorTest.cpp>
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    $<$<TARGET_EXISTS:LibEvent::Core>:LibEvent::Core>
    $<$<TARGET_EXISTS:LibEvent::Core>:LibEvent::Threads>
    GTest::gtest_main
    GTest::gmock_main
)

if (NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(${TARGET})
endif()
