add_executable(CreateObjectTest "CreateObjectTest.cpp")
target_link_libraries(CreateObjectTest PRIVATE ProducerQueueLib)
add_test(NAME CreateObjectTest COMMAND CreateObjectTest)

add_executable(DeleteObjectTest "DeleteObjectTest.cpp")
target_link_libraries(DeleteObjectTest PRIVATE ProducerQueueLib)
add_test(NAME DeleteObjectTest COMMAND DeleteObjectTest)

add_executable(SetObjectPropertyTest "SetObjectPropertyTest.cpp")
target_link_libraries(SetObjectPropertyTest PRIVATE ProducerQueueLib)
add_test(NAME SetObjectPropertyTest COMMAND SetObjectPropertyTest)

add_executable(GetCommandsTest "GetCommandsTest.cpp")
target_link_libraries(GetCommandsTest PRIVATE ProducerQueueLib)
add_test(NAME GetCommandsTest COMMAND GetCommandsTest)