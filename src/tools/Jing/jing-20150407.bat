@echo off
java -jar %~dp0jing-20150407.jar -c https://www.wxwidgets.org/wxxrc %1
rem Offline version:
rem java -jar %~dp0jing-20150407.jar -c %~dp0xrc_schema.rnc %1
