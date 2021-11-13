# fpga-bounce
Example fpga bounce with Yosys simulation.

Forked from the example https://projectf.io/posts/verilog-sim-verilator-sdl

If you're looking for how to do a simple display driver in SystemVerilog, that's the place to go.

This for swaps out yosys's write_cxxrtl to generate C++ code for the simulator and tests.
This is a lot simpler than the Verilator setup and at the moment seems to have similar performance, at least for this example.
