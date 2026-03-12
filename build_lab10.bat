@echo off
echo Starting DOSBox for linking...
"C:\Program Files (x86)\DOSBox-0.74-3\DOSBox.exe" -noconsole -c "mount d D:\develops\git\github\as\AssemblyLab" -c "d:" -c "cd Lab10" -c "..\masm\ml /c lab10_1.asm" -c "..\masm\link lab10_1.obj" -c "exit"
echo Done! Check Lab10 folder for .exe file
pause
