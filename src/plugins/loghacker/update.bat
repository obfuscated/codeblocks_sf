@echo off
copy /Y loghacker.dll ..\..\devel\share\CodeBlocks\plugins > nul 2>&1
zip -j9 ..\..\devel\share\CodeBlocks\loghacker.zip manifest.xml
