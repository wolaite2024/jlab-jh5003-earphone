rem @echo off
set flash_bank_path=%1
set flash_bank_num=%2
set ic_type=%3
set target=%4

::1>NUL copy ..\..\..\bin\bb_upperstack_bank%flash_bank_num%.lib 	..\..\..\bin\bb_upperstack.lib
cmd.exe /c copy ..\..\..\bin\%ic_type%\flash_map_config\%flash_bank_path%\flash_map.h ..\..\..\bin\%ic_type%\flash_map_config\flash_map.h
::cmd.exe /c copy ..\..\..\bin\%ic_type%\flash_map_config\%flash_bank_path%\flash_map.h ..\..\..\inc\%ic_type%\platform\flash_map.h

copy ..\..\..\bin\%ic_type%\ble_mgr\ble_mgr_%target%.lib ..\..\..\bin\%ic_type%\ble_mgr.lib