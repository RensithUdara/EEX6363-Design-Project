@echo off
REM Build script for Design Project - Windows Batch Version
REM This script builds the complete compiler without requiring make

echo ==========================================
echo Building Design Project Complete Compiler
echo ==========================================

REM Change to project root directory
cd /d "%~dp0..\.."

REM Check if build directory exists, create if not
if not exist "build" mkdir build

REM Check for required tools
where gcc >nul 2>&1
if errorlevel 1 (
    echo Error: GCC compiler not found. Please install one of:
    echo   1. MinGW-w64 ^(recommended^): https://www.mingw-w64.org/
    echo   2. TDM-GCC: https://jmeubank.github.io/tdm-gcc/
    echo   3. MSYS2: https://www.msys2.org/
    echo   4. Visual Studio Build Tools with C++ support
    pause
    exit /b 1
)

where flex >nul 2>&1
if errorlevel 1 (
    echo Warning: Flex not found. Using pre-generated lexer if available.
    echo For full functionality, install flex from:
    echo   https://github.com/lexxmark/winflexbison/releases
)

echo Checking TMA source files...
if not exist "..\TMA03\ast.c" (
    echo Error: TMA03 source files not found
    echo Please ensure TMA01, TMA02, and TMA03 directories exist
    pause
    exit /b 1
)

echo.
echo Step 1: Generating lexical analyzer...
flex -o build\lex.yy.c ..\TMA03\scanner.l
if errorlevel 1 (
    echo Warning: Flex failed, trying to continue with existing file...
)

echo.
echo Step 2: Compiling source files...

echo   Compiling lexical analyzer...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c build\lex.yy.c -o build\lex.yy.o
if errorlevel 1 goto :error

echo   Compiling AST module...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c ..\TMA03\ast.c -o build\ast.o
if errorlevel 1 goto :error

echo   Compiling semantic analyzer...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c ..\TMA03\semantic.c -o build\semantic.o
if errorlevel 1 goto :error

echo   Compiling error handler...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c ..\TMA03\error_handler.c -o build\error_handler.o
if errorlevel 1 goto :error

echo   Compiling code generator...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c codegen.c -o build\codegen.o
if errorlevel 1 goto :error

echo   Compiling register allocator...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c register_allocator.c -o build\register_allocator.o
if errorlevel 1 goto :error

echo   Compiling main compiler...
gcc -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT -I..\TMA03 -I..\TMA02 -I..\TMA01\question1b -c compiler.c -o build\compiler.o
if errorlevel 1 goto :error

echo.
echo Step 3: Linking executable...
gcc build\compiler.o build\codegen.o build\register_allocator.o build\lex.yy.o build\ast.o build\semantic.o build\error_handler.o -o build\compiler.exe -lm
if errorlevel 1 goto :error

echo.
echo ==========================================
echo Build completed successfully!
echo Executable: build\compiler.exe
echo ==========================================
echo.
echo Usage examples:
echo   build\compiler.exe program.src -o program.asm
echo   build\compiler.exe -v -r program.src
echo   build\compiler.exe --help
echo.
goto :end

:error
echo.
echo ==========================================
echo Build failed!
echo ==========================================
echo Please check the error messages above.
echo Make sure you have:
echo   1. GCC compiler installed and in PATH
echo   2. All TMA source files present
echo   3. Write permissions in the build directory
echo.
pause
exit /b 1

:end
echo Press any key to continue...
pause >nul