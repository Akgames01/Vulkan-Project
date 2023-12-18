@echo off 

call "H:\Visual studio 2019\VC\Auxiliary\Build\vcvars64.bat"

SET includes = /Isrc /I%VULKAN_SDK%/Include
SET links = /link /LIBPATH:%VULKAN_SDK%/Lib vulkan-1.lib
SET defines = /D DEBUG

echo "Building Main..."

cl /EHsc %includes% %defines% Project1/Main.cpp %links%