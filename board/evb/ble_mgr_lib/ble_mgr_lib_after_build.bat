rem 1>NUL del ..\..\..\bin\ble_mgr.lib
set ic_type=%1
set target=%2

1>NUL copy .\objects\ble_mgr.lib ..\..\..\bin\rtl87x%ic_type%\

if "%target%" == "" (
	echo %target%
) else (
	1>NUL copy .\objects\ble_mgr.lib    ..\..\..\bin\rtl87x%ic_type%\ble_mgr\ble_mgr_%target%.lib
)

