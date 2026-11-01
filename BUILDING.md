# Building nwnexplorer

Use Visual Studio 2022 with the Desktop development with C++ workload,
C++ ATL support, and a Windows SDK. Open `NWNTools.sln` and build the
`nwnexplorer` project in Debug or Release for Win32. FMOD headers, its import
library, and the embedded runtime DLL are included in the repository.

From a Visual Studio Developer Command Prompt:

```
msbuild nwnexplorer\nwnexplorer.vcxproj /m /t:Rebuild /p:Configuration=Release /p:Platform=Win32
```

The executable is written to `nwnexplorer\Release\nwnexplorer.exe`.

GitHub Actions builds Debug and Release on Windows for pushes and pull requests.
Each successful build uploads an executable and debugging symbols as an artifact.
These checks verify compilation and linking; export dialogs, rendering, and audio
still need interactive testing on Windows.
