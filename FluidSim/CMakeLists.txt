add_library(FluidSim STATIC)
target_sources(FluidSim PRIVATE
    SimSource.cpp
    headers/SimSource.h
    SimState.cpp
    headers/SimState.h
    SimTimer.cpp
    headers/SimTimer.h
)
target_include_directories(FluidSim PUBLIC "headers/")
