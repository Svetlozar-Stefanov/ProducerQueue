add_library(ProducerQueueLib "src/ProducerQueue.cpp")
target_include_directories(ProducerQueueLib PUBLIC "include")

add_subdirectory(test)