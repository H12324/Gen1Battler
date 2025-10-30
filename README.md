#### Gen 1 Battle Simulator.

Could be a fun project. If not well it won't last long.

Goal:
- Be able to simulate the gen 1 ai and most moves/mons.
- (Optional) Make it modular and expandable
- (Optional) Basic multiplayer
- (Optional) Graphical Component

---

This repository now contains a minimal C++ project skeleton for a Gen1 battle simulator. Files added include a top-level `CMakeLists.txt`, `src/` with core/engine/data/ai/ui modules, `tests/` using Catch2, and a small `cmake/FetchCatch2.cmake` helper to fetch the test framework.

## Build and generate Visual Studio project (Windows / PowerShell)

Recommended steps to generate a Visual Studio solution from CMake and build it:

1) Create the build directory and generate the solution. Replace the generator with the Visual Studio version you have installed:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
```

If you have Visual Studio 2019 installed, use `"Visual Studio 16 2019"` instead.

2) Open the generated solution file `build/Gen1Battler.sln` in Visual Studio and build, or build from the command line:

```powershell
cmake --build build --config Release
```

3) Run tests (Debug recommended for test build):

```powershell
cmake --build build --config Debug --target tests
ctest --test-dir build -C Debug --verbose
```

Notes:
- The test helper `cmake/FetchCatch2.cmake` uses CMake's FetchContent to pull Catch2 v2.13.10.
- The data loader in `src/data/loader.*` is a stub. For JSON parsing add a library like `nlohmann/json` and update CMake accordingly.
#### Gen 1 Battle Simulator.

Could be a fun project. If not well it won't last long.

Goal: 
- Be able to simulate the gen 1 ai and most moves/mons.
- (Optional) Make it modular and expandable
- (Optional) Basic multiplayer
- (Optional) Graphical Component
