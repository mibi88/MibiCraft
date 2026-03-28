@echo off
for %%f in (..\..\img\*.png) do (
    echo --- Converting %%~nf.png... ---
	imgconv "..\..\img\%%~nf.png" "%%~nf.h" "%%~nf.c"
)
