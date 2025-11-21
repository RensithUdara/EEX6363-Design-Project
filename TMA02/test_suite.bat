@echo off
rem Comprehensive test script for TMA03 Semantic Analyzer (Windows)

echo ===================================
echo TMA03 Semantic Analyzer Test Suite
echo ===================================
echo.

rem Check if build directory exists, if not create it
if not exist "build" mkdir build

rem Build the project first
echo Building semantic analyzer...
echo ===============================

call build.bat
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build successful!
echo.

rem Check if executable exists
if not exist "build\semantic_analyzer.exe" (
    echo Executable not found after build
    pause
    exit /b 1
)

echo Using executable: build\semantic_analyzer.exe
echo.

rem Run tests
echo Running Test Suite
echo ==================
echo.

rem Function to run semantic analyzer and show results
:run_test
set test_file=%1
set description=%2

echo ----------------------------------------
echo Testing: %description%
echo File: %test_file%
echo ----------------------------------------

if not exist "%test_file%" (
    echo Test file not found: %test_file%
    goto :eof
)

echo Running semantic analyzer...
build\semantic_analyzer.exe "%test_file%"

echo.
echo Exit code: %ERRORLEVEL%

rem Show generated files
echo Generated files:
if exist "%test_file%.derivation" (
    echo   ✓ %test_file%.derivation
    echo     Derivation ^(first 10 lines^):
    for /f "skip=0 tokens=* delims=" %%a in ('type "%test_file%.derivation"') do (
        echo       %%a
        set /a count+=1
        if !count! geq 10 goto :done_derivation
    )
    :done_derivation
    set count=0
)

if exist "%test_file%.symboltable" (
    echo   ✓ %test_file%.symboltable
    echo     Symbol table:
    for /f "skip=0 tokens=* delims=" %%a in ('type "%test_file%.symboltable"') do (
        echo       %%a
        set /a count+=1
        if !count! geq 20 goto :done_symbols
    )
    :done_symbols
    set count=0
)

if exist "%test_file%.semanticerrors" (
    echo   ✓ %test_file%.semanticerrors
    echo     Semantic errors:
    type "%test_file%.semanticerrors" | findstr "^" > nul && (
        for /f "tokens=*" %%a in ('type "%test_file%.semanticerrors"') do echo       %%a
    )
)

echo.
goto :eof

rem Test valid programs
if exist "test_valid_semantic.txt" (
    call :run_test "test_valid_semantic.txt" "Valid Program with Classes and Inheritance"
)

if exist "valid1.txt" (
    call :run_test "valid1.txt" "Valid Program 1 (from TMA02)"
)

if exist "valid2.txt" (
    call :run_test "valid2.txt" "Valid Program 2 (from TMA02)"
)

rem Test error programs  
if exist "test_error_semantic.txt" (
    call :run_test "test_error_semantic.txt" "Program with Semantic Errors"
)

if exist "error1.txt" (
    call :run_test "error1.txt" "Error Program 1 (from TMA02)"
)

if exist "error2.txt" (
    call :run_test "error2.txt" "Error Program 2 (from TMA02)"
)

rem Create additional test programs
echo Creating additional test programs...

rem Create a simple valid test
(
echo func main^(^) =^> integer {
echo     local x : integer;
echo     local y : float;
echo     
echo     x := 10;
echo     y := 3.14;
echo     
echo     if ^(x ^> 5^) then {
echo         write^(y^);
echo     };
echo     
echo     return^(x^);
echo }
) > simple_valid.txt

rem Create a simple error test
(
echo func test^(^) =^> integer {
echo     local x : integer;
echo     local y : float;
echo     
echo     x := 3.14;  // Type error
echo     undefinedVar := 5;  // Undefined variable
echo     return^(y^);  // Type mismatch
echo }
) > simple_error.txt

call :run_test "simple_valid.txt" "Simple Valid Program"
call :run_test "simple_error.txt" "Simple Error Program"

rem Summary
echo =========================================
echo Test Suite Complete!
echo =========================================
echo.
echo Files generated during testing:
dir *.derivation *.symboltable *.semanticerrors 2>nul || echo No output files found

echo.
echo To run individual tests:
echo   build\semantic_analyzer.exe program.txt
echo.
echo Output files:
echo   program.txt.derivation    - Parse derivation
echo   program.txt.symboltable   - Symbol table dump
echo   program.txt.semanticerrors - Semantic error report
echo.

pause