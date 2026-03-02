@echo off
set EMSDK_PYTHON=D:\lib\emsdk\upstream\emscripten\tools\file_packager.py
set OUT_DIR=build_web

%EMSDK_PYTHON% %OUT_DIR%/Neural_Wings-demo.data --js-output=%OUT_DIR%/assets_metadata.js --preload ../assets@assets --no-compile
pause