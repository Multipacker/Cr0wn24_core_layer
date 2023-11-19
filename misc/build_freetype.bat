@echo off

set core_layer_root=%1

set ft_root=%core_layer_root%\external\freetype\

set ft_files=%ft_root%\src\base\ftsystem.c
set ft_files=%ft_files% %ft_root%\src\base\ftinit.c
set ft_files=%ft_files% %ft_root%\src\base\ftdebug.c
set ft_files=%ft_files% %ft_root%\src\base\ftbase.c
set ft_files=%ft_files% %ft_root%\src\base\ftbitmap.c
set ft_files=%ft_files% %ft_root%\src\base\ftbbox.c
set ft_files=%ft_files% %ft_root%\src\base\ftglyph.c

set ft_files=%ft_files% %ft_root%\src\truetype\truetype.c
set ft_files=%ft_files% %ft_root%\src\sfnt\sfnt.c
set ft_files=%ft_files% %ft_root%\src\raster\raster.c
set ft_files=%ft_files% %ft_root%\src\psnames\psnames.c
set ft_files=%ft_files% %ft_root%\src\gzip\ftgzip.c
set ft_files=%ft_files% %ft_root%\src\smooth\smooth.c

if not exist ..\build\ mkdir ..\build\
pushd ..\build\

cl -c -nologo -Zi -Od %ft_files% /Fo -I%ft_root% -I%ft_root%\include -DFT_CONFIG_OPTION_ERROR_STRINGS -DFT2_BUILD_LIBRARY

lib *.obj -OUT:freetype.lib

del *.obj > NUL

popd