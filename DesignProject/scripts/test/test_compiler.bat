@echo off
echo Testing Design Project Compiler
echo ================================
echo.

set COMPILER=build\compiler.exe
set TESTFILE=test_simple.txt

echo Test 1: Basic compilation
%COMPILER% -o output.s %TESTFILE%
echo.

echo Test 2: Verbose output with statistics
%COMPILER% -v --show-stats %TESTFILE%
echo.

echo Test 3: Show generated assembly code
%COMPILER% --show-code -o verbose_output.s %TESTFILE%
echo.

echo Test 4: Different register allocation strategies
echo   - Naive allocation:
%COMPILER% -r naive -o naive.s %TESTFILE%
echo   - LRU allocation:
%COMPILER% -r lru -o lru.s %TESTFILE%
echo   - Optimal allocation:
%COMPILER% -r optimal -o optimal.s %TESTFILE%
echo   - Graph coloring allocation:
%COMPILER% -r graph -o graph.s %TESTFILE%
echo.

echo Test 5: Debug mode with AST display
%COMPILER% -d --show-ast %TESTFILE%
echo.

echo All tests completed!
echo Generated files:
dir *.s