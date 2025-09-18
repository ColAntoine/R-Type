$ErrorActionPreference = "Stop"

# Get the directory where this script is located
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
# Get the project root (parent of scripts directory)
$ProjectRoot = Split-Path -Parent $ScriptDir

Write-Host "Project root: $ProjectRoot"
Set-Location $ProjectRoot

if (-Not (Test-Path "external")) {
    New-Item -ItemType Directory -Path "external"
}

if (-Not (Test-Path "external\vcpkg")) {
    Write-Host "Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git external\vcpkg
}

if (-Not (Test-Path "external\vcpkg\vcpkg.exe")) {
    Write-Host "Bootstrapping vcpkg..."
    & .\external\vcpkg\bootstrap-vcpkg.bat
}

Write-Host "Installing vcpkg packages..."
& .\external\vcpkg\vcpkg.exe install

Write-Host "Build script completed successfully!"