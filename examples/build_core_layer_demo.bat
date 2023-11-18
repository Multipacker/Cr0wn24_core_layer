@echo off

set ft_root=..\renderer\ext\freetype\

set disabled_warnings=-wd4201 -wd4100 -wd4152 -wd4189 -wd4101 -wd4456 -W4 -WX 
set common_defines=-D_CRT_SECURE_NO_WARNINGS
set common_compiler_flags=%common_defines% -Zi -FC -Od -nologo
set include_dirs=-I%ft_root%include\ -I%ft_root% -I..\..\

set ft_files=
set ft_files=%ft_files% %ft_root%\src\base\ftsystem.c
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

if not exist ..\build mkdir ..\build
pushd ..\build

set common_compiler_flags=%common_compiler_flags% %disabled_warnings% %include_dirs%

@REM cl -c %ft_files% -Zi -Od -DFT_CONFIG_OPTION_ERROR_STRINGS -DFT2_BUILD_LIBRARY %include_dirs%

@REM lib *.obj -OUT:freetype.lib

set shader_path=..\res\shaders
set shader_output_path=..\renderer\backends\d3d11
fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fh %shader_output_path%\d3d11_vshader.h /Vn d3d11_vshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv %shader_path%\shader.hlsl
fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fh %shader_output_path%\d3d11_pshader.h /Vn d3d11_pshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv %shader_path%\shader.hlsl

cl ..\examples\core_layer_demo.c %common_compiler_flags% -DEBUG:FULL -link -incremental:no freetype.lib gdi32.lib user32.lib kernel32.lib winmm.lib

del *.obj

popd