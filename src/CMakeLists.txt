configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../include/Version.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/../include/Version.hpp
        @ONLY)

set(HFM_GENERATED
    ${CMAKE_CURRENT_BINARY_DIR}/../include/Version.hpp)

set(HFM_INCLUDES 
    ../include/PriorityQueue.hpp)

set(HFM_SOURCES
    main.cpp
    PriorityQueue.cpp)

add_executable(huffman ${HFM_SOURCES} ${HFM_INCLUDES} ${HFM_GENERATED})
target_compile_features(huffman PUBLIC cxx_std_17)
set_target_properties(huffman PROPERTIES
    FOLDER "Binaries"
    CXX_EXTENSIONS OFF
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../binaries
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../binaries
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../binaries
    PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../binaries)

target_include_directories(huffman PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
    $<INSTALL_INTERFACE:include>
    PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_compile_definitions(huffman PRIVATE "$<$<CONFIG:DEBUG>:HFM_DEBUG>")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include" PREFIX "Header Files" FILES ${HFM_INCLUDES})

install(TARGETS huffman
    RUNTIME DESTINATION bin)