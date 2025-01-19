@echo off
for /f %%a in ('git rev-parse HEAD') do (
	echo #define VERSION "v.0.1-git%%a" > buildconfig.h
)
echo #define NAME "MibiCraft" >> buildconfig.h
