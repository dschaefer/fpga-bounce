execute_process(
    COMMAND yosys-config --datdir
    OUTPUT_VARIABLE YOSYS_DIR
)

string(STRIP ${YOSYS_DIR} YOSYS_DIR)

function(hdl_bit NAME)
    set(singleValueArgs TOP)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(HDL "" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    list(TRANSFORM HDL_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

    add_custom_target(${NAME}_bit ALL
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bit"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bit"
        COMMAND yosys 
            -p "synth_ecp5 -top ${HDL_TOP} -json ${NAME}.json"
            ${HDL_SOURCES}
            > yosys.out
    	COMMAND nextpnr-ecp5
            --85k --timing-allow-fail
            --package CABGA381
            --json ${NAME}.json
            --lpf ${CMAKE_CURRENT_SOURCE_DIR}/ulx3s_v20.lpf
    		--textcfg ${NAME}.config
            2> nextpnr.out
    	COMMAND ecppack
            --compress
            --freq 62.0
            --input ${NAME}.config
            --bit ${NAME}.bit
        DEPENDS ${HDL_SOURCES}
    )
endfunction(hdl_bit)

function(hdl_sim NAME)
    set(multiValueArgs SOURCES SIM_SOURCES)
    cmake_parse_arguments(HDL "" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    list(TRANSFORM HDL_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

    add_custom_target(${NAME}_cpp
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp"
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp"
        COMMAND yosys 
            -p "write_cxxrtl -header ${NAME}.cpp"
            ${HDL_SOURCES}
            > yosys.out
        DEPENDS ${HDL_SOURCES}
    )

    add_executable(${NAME} ${HDL_SIM_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp")
    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${YOSYS_DIR}/include)
    add_dependencies(${NAME} ${NAME}_cpp)
endfunction(hdl_sim)

function(hdl_test NAME)
    set(multiValueArgs SOURCES TEST_SOURCES)
    cmake_parse_arguments(HDL "" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    hdl_sim(${NAME} SOURCES ${HDL_SOURCES} SIM_SOURCES ${HDL_TEST_SOURCES})
    target_link_libraries(${NAME} PUBLIC gtest_main)

    add_test(
        NAME ${NAME}
        COMMAND ${NAME}
    )
endfunction(hdl_test)
