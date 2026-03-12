@echo off
echo Building all ASM files with MASM6.x in DOSBox...
echo.

setlocal enabledelayedexpansion

for /r %%D in (Lab* Checkpoints Project*) do (
    if exist "%%D\*.asm" (
        echo Processing %%D...
        "C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" -noconsole -c "mount d D:\develops\git\github\as\AssemblyLab" -c "d:" -c "cd %%D" -c "..\masm\ml /c *.asm" -c "..\masm\link *.obj" -c "exit" 2>nul
    )
)

echo.
echo Build complete!
echo Check Lab*, Checkpoints, Project* folders for .obj and .exe files
pause
