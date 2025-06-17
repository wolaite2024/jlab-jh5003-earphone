@echo off

set ic_type=%1
set target=%2

1>NUL copy .\objects\btm.lib    ..\..\..\..\bin\rtl87x%ic_type%\framework\

