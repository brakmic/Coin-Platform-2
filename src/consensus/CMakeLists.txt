# Add library target for consensus
file(GLOB CONSENSUS_SOURCES *.cpp)
file(GLOB CONSENSUS_HEADERS *.hpp)

add_library(consensus ${CONSENSUS_SOURCES} ${CONSENSUS_HEADERS})

target_include_directories(consensus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/src/common)
target_link_libraries(consensus
    time_chain
    value_chain
    storage
    networking
    cryptography
    common
)
