@echo off
rem 1>NUL del  ..\..\..\bin\leaudio.lib
set ic_type=%1
set target=%2

if "%ic_type%" == "3d" (
	if "%target%" == "dongle" (
		1>NUL copy .\objects\leaudio.lib    ..\..\..\bin\rtl87x%ic_type%\leaudio\leaudio_%target%.lib
	)else (
		1>NUL copy .\objects\leaudio.lib    ..\..\..\bin\rtl87x%ic_type%\leaudio\leaudio.lib
	)
) else (
	1>NUL copy .\objects\leaudio.lib ..\..\..\bin\rtl87x%ic_type%\
	if "%target%" == "" (
		1>NUL copy .\objects\leaudio.lib    ..\..\..\bin\rtl87x%ic_type%\leaudio\leaudio.lib
	)else (
		1>NUL copy .\objects\leaudio.lib    ..\..\..\bin\rtl87x%ic_type%\leaudio\leaudio_%target%.lib
	)
)

