@echo off
rem Copyright (c) Microsoft Corporation.
rem Licensed under the MIT License.

setlocal
set error=0

set FXCOPTS=/nologo /WX /Ges /Zi /Zpc /Qstrip_reflect /Qstrip_debug

if %1.==xbox. goto continuexbox
if %1.==. goto continuepc
echo usage: CompileShaders [xbox]
exit /b

:continuexbox
set XBOXOPTS=/D__XBOX_DISABLE_SHADER_NAME_EMPLACEMENT
if NOT %2.==noprecompile. goto skipnoprecompile
set XBOXOPTS=%XBOXOPTS% /D__XBOX_DISABLE_PRECOMPILE=1
:skipnoprecompile

set XBOXFXC="%XboxOneXDKLatest%\xdk\FXC\amd64\FXC.exe"
if exist %XBOXFXC% goto continue
set XBOXFXC="%XboxOneXDKLatest%xdk\FXC\amd64\FXC.exe"
if exist %XBOXFXC% goto continue
set XBOXFXC="%XboxOneXDKBuild%xdk\FXC\amd64\FXC.exe"
if exist %XBOXFXC% goto continue
set XBOXFXC="%DurangoXDK%xdk\FXC\amd64\FXC.exe"
if not exist %XBOXFXC% goto needxdk
goto continue

:continuepc

set PCFXC="%WindowsSdkVerBinPath%x86\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkBinPath%%WindowsSDKVersion%\x86\fxc.exe"
if exist %PCFXC% goto continue
set PCFXC="%WindowsSdkDir%bin\%WindowsSDKVersion%\x86\fxc.exe"
if exist %PCFXC% goto continue

set PCFXC=fxc.exe

:continue
if not defined CompileShadersOutput set CompileShadersOutput=Compiled
set StrTrim=%CompileShadersOutput%##
set StrTrim=%StrTrim: ##=%
set CompileShadersOutput=%StrTrim:##=%
@if not exist "%CompileShadersOutput%" mkdir "%CompileShadersOutput%"

call :CompileShader%1 SpriteEffect vs SpriteVertexShader
call :CompileShader%1 SpriteEffect ps SpritePixelShader


if NOT %1.==xbox. goto skipxboxonly

call :CompileShaderSM4xbox ToneMap ps PSHDR10_Saturate
call :CompileShaderSM4xbox ToneMap ps PSHDR10_Reinhard
call :CompileShaderSM4xbox ToneMap ps PSHDR10_ACESFilmic
call :CompileShaderSM4xbox ToneMap ps PSHDR10_Saturate_SRGB
call :CompileShaderSM4xbox ToneMap ps PSHDR10_Reinhard_SRGB
call :CompileShaderSM4xbox ToneMap ps PSHDR10_ACESFilmic_SRGB

:skipxboxonly

echo.

if %error% == 0 (
    echo Shaders compiled ok
) else (
    echo There were shader compilation errors!
    exit /b 1
)

endlocal
exit /b 0

:CompileShader
set fxc=%PCFXC% "%1.fx" %FXCOPTS% /T%2_4_0_level_9_1 /E%3 "/Fh%CompileShadersOutput%\%1_%3.inc" "/Fd%CompileShadersOutput%\%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderSM4
set fxc=%PCFXC% "%1.fx" %FXCOPTS% /T%2_4_0 /E%3 "/Fh%CompileShadersOutput%\%1_%3.inc" "/Fd%CompileShadersOutput%\%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSL
set fxc=%PCFXC% "%1.hlsl" %FXCOPTS% /T%2_4_0_level_9_1 /E%3 "/Fh%CompileShadersOutput%\%1_%3.inc" "/Fd%CompileShadersOutput%\%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderxbox
:CompileShaderSM4xbox
set fxc=%XBOXFXC% "%1.fx" %FXCOPTS% /T%2_5_0 %XBOXOPTS% /E%3 "/Fh%CompileShadersOutput%\XboxOne%1_%3.inc" "/Fd%CompileShadersOutput%\XboxOne%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:CompileShaderHLSLxbox
set fxc=%XBOXFXC% "%1.hlsl" %FXCOPTS% /T%2_5_0 %XBOXOPTS% /E%3 "/Fh%CompileShadersOutput%\XboxOne%1_%3.inc" "/Fd%CompileShadersOutput%\XboxOne%1_%3.pdb" /Vn%1_%3
echo.
echo %fxc%
%fxc% || set error=1
exit /b

:needxdk
echo ERROR: CompileShaders xbox requires the Microsoft Xbox One XDK
echo        (try re-running from the XDK Command Prompt)
