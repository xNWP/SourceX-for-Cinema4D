@ECHO OFF
cls

rmdir /S /Q "X:\C4D\plugins\SourceX"
xcopy "res\*.*" /F /S /Y "X:\C4D\plugins\SourceX\res\"
copy "SourceX.cdl64" "X:\C4D\plugins\SourceX\"

echo Pushed.
pause > nul