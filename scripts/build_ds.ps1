<#
.SYNOPSIS
    批量编译 DataStructure/算法实现 下的所有 .cpp（408 数据结构算法实现脚手架）。

.DESCRIPTION
    每个 .cpp 都是独立可编译的单文件程序，main 里带验收断言：
      返回值 0  = 该文件全部 CHECK 通过
      返回值非 0 = 还有 TODO 没做 / 断言不过

.EXAMPLE
    pwsh scripts/build_ds.ps1                    # 只编译
    pwsh scripts/build_ds.ps1 -Run               # 编译后逐个运行
    pwsh scripts/build_ds.ps1 -Run -NoTrace      # 运行时关闭算法过程打印
    pwsh scripts/build_ds.ps1 -Name kmp -Run     # 只处理名字含 kmp 的文件
#>
[CmdletBinding()]
param(
    [string]$Name,
    [switch]$Run,
    [switch]$NoTrace
)

$ErrorActionPreference = 'Stop'

# g++ 的诊断信息里含中文路径，统一切到 UTF-8 避免乱码
try {
    $prevCP = [Console]::OutputEncoding
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding $false
} catch { }

$repoRoot = Split-Path -Parent $PSScriptRoot
$srcRoot  = Join-Path $repoRoot 'DataStructure\算法实现'
$outDir   = Join-Path $srcRoot  'build'

if (-not (Test-Path $srcRoot)) { throw "source root not found: $srcRoot" }

$gxx = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $gxx) { throw 'g++ not found in PATH (install MinGW-w64 and add it to PATH).' }

New-Item -ItemType Directory -Force -Path $outDir | Out-Null

# -Wno-unused-parameter：TODO 练习位的形参暂时用不到，不必刷屏
$flags = @('-std=c++17', '-O0', '-Wall', '-Wextra', '-Wno-unused-parameter')
if ($NoTrace) { $flags += '-DDS_TRACE=0' }

$sources = Get-ChildItem -Path $srcRoot -Recurse -Filter *.cpp -File |
    Where-Object { $_.FullName -notmatch '\\build\\' -and $_.FullName -notmatch '\\_archive\\' } |
    Sort-Object FullName
if ($Name) { $sources = $sources | Where-Object { $_.BaseName -like "*$Name*" } }
if (-not $sources) { Write-Host 'no .cpp file found.'; exit 0 }

$buildFail = New-Object System.Collections.Generic.List[string]
$testFail  = New-Object System.Collections.Generic.List[string]
$allPass   = New-Object System.Collections.Generic.List[string]

foreach ($src in $sources) {
    $rel = $src.FullName.Substring($srcRoot.Length + 1)
    $exe = Join-Path $outDir ($src.BaseName + '.exe')

    Write-Host ("[BUILD] {0}" -f $rel) -ForegroundColor Cyan
    $log = & $gxx @flags $src.FullName -o $exe 2>&1
    if ($LASTEXITCODE -ne 0) {
        $log | ForEach-Object { Write-Host "        $_" -ForegroundColor Red }
        $buildFail.Add($rel)
        continue
    }
    if ($log) { $log | ForEach-Object { Write-Host "        $_" -ForegroundColor Yellow } }

    if ($Run) {
        Write-Host ("[ RUN ] {0}" -f $src.BaseName) -ForegroundColor DarkCyan
        & $exe
        if ($LASTEXITCODE -ne 0) {
            Write-Host ("[ FAIL] {0} returned {1} (TODO left or check failed)" -f $src.BaseName, $LASTEXITCODE) -ForegroundColor Red
            $testFail.Add($rel)
        } else {
            $allPass.Add($rel)
        }
    }
}

Write-Host ''
Write-Host '================ summary ================'
Write-Host ("compiled : {0}" -f $sources.Count)
if ($Run) {
    Write-Host ("all-pass : {0}" -f $allPass.Count) -ForegroundColor Green
    Write-Host ("todo/fail: {0}" -f $testFail.Count) -ForegroundColor Yellow
}
Write-Host ("build-err: {0}" -f $buildFail.Count) -ForegroundColor Red
foreach ($f in $buildFail) { Write-Host ("  ! build  {0}" -f $f) -ForegroundColor Red }
foreach ($f in $testFail)  { Write-Host ("  - todo   {0}" -f $f) -ForegroundColor Yellow }
if ($Run)  { Write-Host ("exe dir  : {0}" -f $outDir) }

if ($buildFail.Count -gt 0) { exit 1 }
exit 0
