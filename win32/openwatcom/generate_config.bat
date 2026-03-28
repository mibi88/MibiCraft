@echo off
where /Q git
if %errorlevel% neq 0 echo #define VERSION "v.0.1-unknown" > buildconfig.h
for /f %%a in ('git rev-parse HEAD') do (
	echo #define VERSION "v.0.1-git%%a" > buildconfig.h
)
echo #define NAME "MibiCraft" >> buildconfig.h
