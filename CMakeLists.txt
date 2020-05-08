cmake_minimum_required(VERSION 3.16)
project(tomid)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)
include_directories(midi_processing)
include_directories(nall)

add_executable(tomid
        midi_processing/midi_container.cpp
        midi_processing/midi_container.h
        midi_processing/midi_processor.h
        midi_processing/midi_processor_gmf.cpp
        midi_processing/midi_processor_helpers.cpp
        midi_processing/midi_processor_hmi.cpp
        midi_processing/midi_processor_hmp.cpp
        midi_processing/midi_processor_lds.cpp
        midi_processing/midi_processor_mids.cpp
        midi_processing/midi_processor_mus.cpp
        midi_processing/midi_processor_riff_midi.cpp
        midi_processing/midi_processor_standard_midi.cpp
        midi_processing/midi_processor_syx.cpp
        midi_processing/midi_processor_xmi.cpp
        nall/utf8.hpp
        2mid.cpp
        8820.h
        json.hpp )
