@echo off

for /r src\include %%G in (*.cpp *.h) do (
    echo "%%G"
    src\build_tools\clang-format\clang-format.exe -i "%%G"
)

for /r src\sdk %%G in (*.cpp *.h) do (
    echo "%%G"
    src\build_tools\clang-format\clang-format.exe -i "%%G"
)

for /r src\src %%G in (*.cpp *.h) do (
    echo "%%G"
    src\build_tools\clang-format\clang-format.exe -i "%%G"
)