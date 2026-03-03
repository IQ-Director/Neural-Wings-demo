@echo off
set PYTHON_EXE=D:\Program\Anaconda\python.exe
set PACKAGER_PATH=D:\lib\emsdk\upstream\emscripten\tools\file_packager.py
set OUT_DIR=build_web
"%PYTHON_EXE%" "%PACKAGER_PATH%" "%OUT_DIR%\Neural_Wings-demo.data" --js-output="%OUT_DIR%\assets_metadata.js" --preload assets@assets
pause