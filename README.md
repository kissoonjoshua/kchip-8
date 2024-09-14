# Chip-8 Emulator Written in C++
## Build
- Run `cmake -S . -B build -DCMAKE_C_COMPILER="clang.exe" -DCMAKE_CXX_COMPILER="clang++.exe" -DCMAKE_CXX_FLAGS="-O2 -march=native"`
- Run `cmake --build build`
- (Optional, Windows) `windeployqt6 build/kchip-8.exe`