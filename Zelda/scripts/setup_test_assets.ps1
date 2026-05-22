# Copia fuentes y sonidos de Windows para probar el juego (sin descargar nada).
$root = Split-Path -Parent $PSScriptRoot
$assets = Join-Path $root "assets"
$fonts = Join-Path $assets "fonts"
$audio = Join-Path $assets "audio"

New-Item -ItemType Directory -Force -Path $fonts, $audio | Out-Null

$win = $env:WINDIR
$fontCandidates = @(
    "$win\Fonts\arial.ttf",
    "$win\Fonts\segoeui.ttf"
)
foreach ($f in $fontCandidates) {
    if (Test-Path $f) {
        Copy-Item $f (Join-Path $fonts "arial.ttf") -Force
        Write-Host "Font: $f -> assets/fonts/arial.ttf"
        break
    }
}

$sounds = @{
    "pickup.wav"      = "$win\Media\Windows Notify.wav"
    "attack.wav"      = "$win\Media\Windows Hardware Insert.wav"
    "hurt.wav"        = "$win\Media\Windows Error.wav"
    "enemy_death.wav" = "$win\Media\Windows Recycle.wav"
}
foreach ($kv in $sounds.GetEnumerator()) {
    if (Test-Path $kv.Value) {
        Copy-Item $kv.Value (Join-Path $audio $kv.Key) -Force
        Write-Host "Sound: $($kv.Key)"
    }
}

Write-Host ""
Write-Host "Listo. Recompila y copia assets al build (cmake --build .) o ejecuta desde build/Debug."
