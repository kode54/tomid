@copy /y json_header + 8820.json + json_footer 8820json.h
@cl /O2 /MT /EHsc /MP /D_USING_V110_SDK_71_ /Dstrcasecmp=_stricmp /Dstrncasecmp=_strnicmp  2mid.cpp midi_processing\*.cpp /link /subsystem:console,5.01
