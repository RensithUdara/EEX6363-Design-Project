@echo off
REM Build script for TMA03 Semantic Analyzer on Windows

echo Building TMA03 Semantic Analyzer...
echo.

REM Create build directory
if not exist build mkdir build

REM Generate parser files
echo Generating parser files...
bison -d -v -o parser.tab.c parser.y
if errorlevel 1 (
    echo Error: Failed to generate parser files
    goto :error
)

REM Generate lexer files  
echo Generating lexer files...
flex -o lex.yy.c scanner.l
if errorlevel 1 (
    echo Error: Failed to generate lexer files
    goto :error
)

REM Compile object files
echo Compiling source files...

gcc -Wall -Wextra -g -std=c99 -c parser.tab.c -o build/parser.tab.o
if errorlevel 1 (
    echo Error: Failed to compile parser
    goto :error
)

gcc -Wall -Wextra -g -std=c99 -c lex.yy.c -o build/lex.yy.o
if errorlevel 1 (
    echo Error: Failed to compile lexer  
    goto :error
)

gcc -Wall -Wextra -g -std=c99 -c ast.c -o build/ast.o
if errorlevel 1 (
    echo Error: Failed to compile AST module
    goto :error
)

gcc -Wall -Wextra -g -std=c99 -c semantic.c -o build/semantic.o
if errorlevel 1 (
    echo Error: Failed to compile semantic analyzer
    goto :error
)

REM Link executable
echo Linking executable...
gcc -Wall -Wextra -g -o build/semantic_analyzer.exe build/parser.tab.o build/lex.yy.o build/ast.o build/semantic.o
if errorlevel 1 (
    echo Error: Failed to link executable
    goto :error
)

echo.
echo ✓ Build completed successfully!
echo Executable: build/semantic_analyzer.exe
echo.
echo Usage: build/semantic_analyzer.exe input_file.txt
echo.
goto :end

:error
echo.
echo ✗ Build failed!
echo Please check the error messages above.
pause
exit /b 1

:end
pause