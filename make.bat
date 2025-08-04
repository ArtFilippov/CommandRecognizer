set workspaceDirectory="C:\Users\filippar\sharedVbox"

if not exist %workspaceDirectory%\common\ (
    CALL git clone ssh://git@gitea:2222/filippar/libcomport.git  %workspaceDirectory%/libcomport
    CALL cmd /c "%workspaceDirectory%/libcomport/make.bat" %workspaceDirectory%
)


if exist %workspaceDirectory%\commandRecognizer\ (
    RD /S "%workspaceDirectory%\commandRecognizer"
)

if exist %~dp0build\ (
    RD /S "%~dp0build\"
)

cmake -S %~dp0 -B %~dp0build -G "MinGW Makefiles" -DOS=arm -DCMAKE_PREFIX_PATH="%workspaceDirectory%"  -DCMAKE_BUILD_TYPE=Debug
cmake --build %~dp0build
cmake --install %~dp0build --prefix %workspaceDirectory%