<#
  Captures the golden set into -Out: canvas captures (Ctrl+P), window captures and the -s report.
  The game runs in a fresh work folder holding the fixture config and save and the maps, without music.

    powershell -NoProfile -ExecutionPolicy Bypass -File tests/golden/screens.ps1 -Out bin/golden-check
    powershell -NoProfile -ExecutionPolicy Bypass -File .claude/skills/run-unicorn/driver.ps1 compare-set tests/golden bin/golden-check
#>
param([Parameter(Mandatory = $true)] [string]$Out)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$Driver = Join-Path $Root ".claude\skills\run-unicorn\driver.ps1"
$Fixtures = Join-Path $Root "tests\fixtures"
$Work = Join-Path $Root "bin\golden-run"

function Invoke-Driver {
  & powershell -NoProfile -ExecutionPolicy Bypass -File $Driver @args
  if ($LASTEXITCODE -ne 0) { throw "driver $($args -join ' ') failed" }
}

function Reset-WorkFolder {
  if (Test-Path $Work) { Remove-Item -Recurse -Force $Work }
  New-Item -ItemType Directory $Work | Out-Null
  Copy-Item (Join-Path $Fixtures "unicorn.cfg") $Work
  Copy-Item (Join-Path $Fixtures "Golden.sav") $Work
  Copy-Item (Join-Path $Root "maps") $Work -Recurse

  # The Load Game screen prints the save's date, so it is pinned
  (Get-Item (Join-Path $Work "Golden.sav")).LastWriteTime = Get-Date "2026-01-01 12:00"
}

function Save-Screen([string]$name) {
  Invoke-Driver capture (Join-Path $Out "$name.png")
}

$Out = (New-Item -ItemType Directory -Force $Out).FullName
New-Item -ItemType Directory -Force (Join-Path $Out "window") | Out-Null
$env:UNICORN_WORKDIR = $Work

# New-game path
Reset-WorkFolder
Invoke-Driver launch -nmu
Save-Screen "title-menu"                 # Gothic title, notice, "--> New Game"
Invoke-Driver keys down enter
Save-Screen "load-menu"                  # Load Game, one row: Golden, Fighter 1, Casual, 2026-01-01
Invoke-Driver keys esc up enter          # Esc leaves the title menu's cursor on Load Game
Save-Screen "legends"                    # first page after the title
Invoke-Driver keys enter
Save-Screen "intro"                      # "In ages past the unicorns roamed..."
Invoke-Driver keys enter
Save-Screen "new-game-options"           # New Game Options, Game mode Normal
Invoke-Driver quit

# Loaded-save path
Reset-WorkFolder
Invoke-Driver launch Golden -nmu
Save-Screen "title-cmdline"              # Gothic title, "Press <ENTER> to continue"
Invoke-Driver keys enter enter           # title -> Legends -> character sheet
Save-Screen "character"                  # Golden's character sheet, "Casual" at the bottom
Invoke-Driver keys enter
Save-Screen "overworld"                  # Radon Valley at 08:01, frame, side panel, small map, status icons
Invoke-Driver keys i
Save-Screen "inventory"
Invoke-Driver keys esc "?"
Save-Screen "upland-help"
Invoke-Driver keys enter H
Save-Screen "icons"                      # first of the icon legend's four pages
Invoke-Driver keys enter enter enter enter O
Save-Screen "game-options"               # in-game Game Options (preferences)
Invoke-Driver keys enter v 4
Save-Screen "look"                       # look-mode cursor on the city west of the player, "City"
Invoke-Driver keys esc
Invoke-Driver quit

# Window captures: what the window shows at fixed client sizes
Reset-WorkFolder
Invoke-Driver launch -nmu
foreach ($size in @(@(640, 480), @(1280, 960), @(1500, 1000))) {
  $name = "title-{0}x{1}" -f $size[0], $size[1]
  Invoke-Driver resize $size[0] $size[1]
  Invoke-Driver ss $name
  Move-Item -Force (Join-Path $Root "bin\screenshots\$name.png") (Join-Path $Out "window\$name.png")
}
Invoke-Driver quit

# The -s report, without the driver's own "exit" line
Reset-WorkFolder
$report = @(& powershell -NoProfile -ExecutionPolicy Bypass -File $Driver smoke)
$report | Select-Object -SkipLast 1 | Set-Content -Encoding ascii (Join-Path $Out "smoke.txt")

Remove-Item -Recurse -Force $Work
Write-Host "golden set written to $Out"
