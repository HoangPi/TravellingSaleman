@echo off
set IMAGE_NAME=travellingsaleman-traveling_salesman_problem


REM Set VcXsrv path
set VCXSERV_PATH="C:\Program Files\VcXsrv\vcxsrv.exe"

REM Check if VcXsrv is installed
IF EXIST %VCXSERV_PATH% (
    echo ✅ VcXsrv is installed.
) ELSE (
    echo ❌ VcXsrv is NOT installed!
    echo 🔥 PANIC MODE: GUI apps will not work without VcXsrv.
    echo Please install VcXsrv from https://sourceforge.net/projects/vcxsrv/
    exit /b 1
)

REM Check if VcXsrv is already running
tasklist /FI "IMAGENAME eq vcxsrv.exe" | find /I "vcxsrv.exe" >nul
IF %ERRORLEVEL% EQU 0 (
    echo ✅ VcXsrv is already running. Skipping launch.
) ELSE (
    echo 🚀 Launching VcXsrv...
    start "" %VCXSERV_PATH% :0 -multiwindow -clipboard -wgl -ac
    timeout /t 2 >nul
)

REM Check if Docker image exists
docker image inspect %IMAGE_NAME% >nul 2>&1
IF %ERRORLEVEL% EQU 0 (
    echo ✅ Docker image '%IMAGE_NAME%' already exists. Skipping build.
) ELSE (
    REM Check if Dockerfile exists
    IF NOT EXIST Dockerfile (
        echo ❌ Dockerfile not found in current directory.
        exit /b 1
    )

    REM Build the Docker image
    echo 🚧 Building Docker image: %IMAGE_NAME%...
    docker build -t %IMAGE_NAME% .

    IF %ERRORLEVEL% NEQ 0 (
        echo ❌ Docker build failed.
        exit /b 1
    )
)

REM Build and run Docker Compose
echo 🧱 Building and launching container via Docker Compose...
docker compose up --build
