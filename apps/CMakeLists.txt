add_executable(sensel-test sensel-test.cpp)
target_compile_features(sensel-test PRIVATE cxx_std_17)
target_include_directories(sensel-test PUBLIC "${SENSEL_INCLUDE_DIRS}")
target_link_libraries(sensel-test ${SENSEL_LIBRARIES})

########## morph-class test
add_executable(mySensel-test mySensel-test.cpp)
target_compile_features(mySensel-test PRIVATE cxx_std_17)
target_include_directories(mySensel-test PUBLIC "${SENSEL_INCLUDE_DIRS}")
target_link_libraries(mySensel-test ${SENSEL_LIBRARIES})

target_include_directories(mySensel-test PUBLIC ../include)
target_link_libraries(mySensel-test sensosc)




########## senselosc
add_executable(senselosc senselosc.cpp)
target_compile_features(senselosc PRIVATE cxx_std_17)
target_include_directories(senselosc PUBLIC "${SENSEL_INCLUDE_DIRS}")
target_link_libraries(senselosc PRIVATE 
	${SENSEL_LIBRARIES}
	oscpack
	sensosc
)
