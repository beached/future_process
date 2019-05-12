cmake_minimum_required( VERSION 2.8.12 )

project( future_process )

enable_testing( )

include( "${CMAKE_SOURCE_DIR}/dependent_projects/CMakeListsCompiler.txt" )

include_directories( "./include" )

set( DEP_ROOT_DIR_BASE "${CMAKE_SOURCE_DIR}/dependent_projects/build" )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/debug" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
elseif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
	set( DEP_ROOT_DIR "${DEP_ROOT_DIR_BASE}/release" )
endif( )

message( "Building for: ${CMAKE_BUILD_TYPE}" )
if( EXISTS "${DEP_ROOT_DIR}" )
	message( "${DEP_ROOT_DIR}" )
	message( "using dependency folder" )
	add_custom_target( dependency_stub )
	include_directories( SYSTEM "${DEP_ROOT_DIR}/install/include" )
	link_directories( "${DEP_ROOT_DIR}/install/lib" )
else( )
	message( "downloading and building deps" )
	include( "${CMAKE_SOURCE_DIR}/dependent_projects/dependencies.txt" )
	add_library( dependency_stub STATIC "${CMAKE_SOURCE_DIR}/dependent_projects/stub.cpp" )
	add_dependencies( dependency_stub ${DEP_PROJECT_DEPS} )
endif( )

set( HEADER_FOLDER "include" )
set( TEST_FOLDER "tests" )
set( SOURCE_FOLDER "src" )

include_directories( ${HEADER_FOLDER} )

set( HEADER_FILES
	${HEADER_FOLDER}/daw/daw_channel.h
	${HEADER_FOLDER}/daw/daw_future_process.h
	${HEADER_FOLDER}/daw/daw_process.h
	${HEADER_FOLDER}/daw/daw_shared_memory.h
	${HEADER_FOLDER}/daw/daw_semaphore.h
	${HEADER_FOLDER}/daw/daw_string_channel.h
)

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )
add_custom_target( full )

#add_executable( future_process_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/future_process_test.cpp )
add_executable( future_process_test_bin ${HEADER_FILES} ${TEST_FOLDER}/future_process_test.cpp )
add_dependencies( future_process_test_bin dependency_stub )
target_link_libraries( future_process_test_bin uv )
add_test( future_process_test future_process_test_bin )
add_dependencies( check future_process_test_bin )
add_dependencies( full future_process_test_bin )

#add_executable( process_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/process_test.cpp )
add_executable( process_test_bin ${HEADER_FILES} ${TEST_FOLDER}/process_test.cpp )
add_dependencies( process_test_bin dependency_stub )
target_link_libraries( process_test_bin uv )
add_test( process_test process_test_bin )
add_dependencies( check process_test_bin )
add_dependencies( full process_test_bin )

#add_executable( semaphore_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/semaphore_test.cpp )
add_executable( semaphore_test_bin ${HEADER_FILES} ${TEST_FOLDER}/semaphore_test.cpp )
add_dependencies( semaphore_test_bin dependency_stub )
target_link_libraries( semaphore_test_bin uv )
add_test( semaphore_test semaphore_test_bin )
add_dependencies( check semaphore_test_bin )
add_dependencies( full semaphore_test_bin )

#add_executable( channel_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/channel_test.cpp )
add_executable( channel_test_bin ${HEADER_FILES} ${TEST_FOLDER}/channel_test.cpp )
add_dependencies( channel_test_bin dependency_stub )
target_link_libraries( channel_test_bin uv )
add_test( channel_test channel_test_bin )
add_dependencies( check channel_test_bin )
add_dependencies( full channel_test_bin )

#add_executable( string_channel_test_bin EXCLUDE_FROM_ALL ${HEADER_FILES} ${TEST_FOLDER}/string_channel_test.cpp )
add_executable( string_channel_test_bin ${HEADER_FILES} ${TEST_FOLDER}/string_channel_test.cpp )
add_dependencies( string_channel_test_bin dependency_stub )
target_link_libraries( string_channel_test_bin uv )
add_test( string_channel_test string_channel_test_bin )
add_dependencies( check string_channel_test_bin )
add_dependencies( full string_channel_test_bin )

install( DIRECTORY ${HEADER_FOLDER}/daw/ DESTINATION include/daw/ )

