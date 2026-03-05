param(
    [string]$ThirdPartyDir = "third_party",
    [switch]$Force
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$raylibRef = if ($env:RAYLIB_REF) { $env:RAYLIB_REF } else { "5.5" }
$rayguiRef = if ($env:RAYGUI_REF) { $env:RAYGUI_REF } else { "1c2365a" }
$jsonRef = if ($env:NLOHMANN_JSON_REF) { $env:NLOHMANN_JSON_REF } else { "553c314fb" }
$nbnetRef = if ($env:NBNET_REF) { $env:NBNET_REF } else { "master" }
$ultralightRef = if ($env:ULTRALIGHT_REF) { $env:ULTRALIGHT_REF } else { "1b4b800" }

$deps = @(
    @{ Name = "raylib-master"; Url = "https://github.com/raysan5/raylib.git"; Ref = $raylibRef },
    @{ Name = "raygui-master"; Url = "https://github.com/raysan5/raygui.git"; Ref = $rayguiRef },
    @{ Name = "json-develop"; Url = "https://github.com/nlohmann/json.git"; Ref = $jsonRef },
    @{ Name = "nbnet"; Url = "https://github.com/nathhB/nbnet.git"; Ref = $nbnetRef },
    @{ Name = "ultralight"; Url = "https://github.com/ultralight-ux/Ultralight.git"; Ref = $ultralightRef }
)

function Sync-Repo {
    param(
        [hashtable]$Dep
    )

    $path = Join-Path $ThirdPartyDir $Dep.Name

    if (Test-Path $path) {
        if ($Force) {
            Write-Host "Removing existing $($Dep.Name) ..."
            Remove-Item -Recurse -Force $path
        } else {
            Write-Host "Skipping existing $($Dep.Name) (use -Force to refresh)."
            return
        }
    }

    Write-Host "Cloning $($Dep.Name) from $($Dep.Url) ..."
    git clone $Dep.Url $path
    git -C $path checkout $Dep.Ref
    git -C $path submodule update --init --recursive
    $sha = git -C $path rev-parse --short HEAD
    Write-Host "$($Dep.Name) checked out at $sha"
}

New-Item -ItemType Directory -Path $ThirdPartyDir -Force | Out-Null

foreach ($dep in $deps) {
    Sync-Repo -Dep $dep
}

$required = @(
    "raylib-master/src/raylib.h",
    "raygui-master/src/raygui.h",
    "json-develop/include/nlohmann/json.hpp",
    "nbnet/nbnet.h",
    "ultralight/include/Ultralight/Ultralight.h",
    "ultralight/lib/Ultralight.lib",
    "ultralight/bin/Ultralight.dll",
    "ultralight/resources/icudt67l.dat"
)

foreach ($relativePath in $required) {
    $fullPath = Join-Path $ThirdPartyDir $relativePath
    if (-not (Test-Path $fullPath)) {
        throw "Missing dependency file: $fullPath"
    }
}

Write-Host "third_party dependencies are ready."
