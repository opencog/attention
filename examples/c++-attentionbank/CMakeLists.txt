INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(crudexample CRUDExample.cc)
TARGET_LINK_LIBRARIES(crudexample atomspace attentionbank)

ADD_EXECUTABLE(aseventsexample AtomSpaceEventSubscribeExample.cc)
TARGET_LINK_LIBRARIES(aseventsexample atomspace attentionbank)
