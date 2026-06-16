# PowerShell build script for Student Management System in C
Write-Host "Compiling Student Management System..." -ForegroundColor Cyan

# Create data directory if it doesn't exist
if (-not (Test-Path -Path "data")) {
    New-Item -ItemType Directory -Path "data" | Out-Null
    Write-Host "Created 'data' directory." -ForegroundColor Green
}

# Locate GCC compiler
$gccPath = "gcc"
if (-not (Get-Command "gcc" -ErrorAction SilentlyContinue)) {
    if (Test-Path "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe") {
        $gccPath = "C:\Program Files\CodeBlocks\MinGW\bin\gcc.exe"
        Write-Host "Using compiler from: $gccPath" -ForegroundColor DarkCyan
    } else {
        Write-Host "Error: GCC compiler not found. Please install MinGW and add gcc to PATH." -ForegroundColor Red
        exit 1
    }
}

# Compile command
& $gccPath -Wall -Wextra -std=c99 -g src/main.c src/student.c src/auth.c src/file_ops.c src/activity_log.c src/utils.c -o sms.exe


if ($LASTEXITCODE -eq 0) {
    Write-Host "Build Successful! Executable: sms.exe" -ForegroundColor Green
    Write-Host "You can run the system using: .\sms.exe" -ForegroundColor Yellow
} else {
    Write-Host "Build Failed. Please check the compiler errors above." -ForegroundColor Red
}
