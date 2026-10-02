# init.ps1
# 用法：在 PowerShell 中执行  . .\init.ps1   （注意前面有个点和空格）

# 切到脚本所在目录，保证从任意位置调用都正确
Set-Location -LiteralPath $PSScriptRoot

$target = Join-Path $PSScriptRoot 'datasource\py_impl'
$activate = Join-Path $target 'venv\Scripts\Activate.ps1'

if (-not (Test-Path -LiteralPath $activate)) {
    Write-Host "[ERROR] 未找到 $activate" -ForegroundColor Red
    return
}

Set-Location -LiteralPath $target
Write-Host "[INFO] 当前目录: $(Get-Location)" -ForegroundColor Green

# 关键：用点号 source，把激活效果留在当前会话
. $activate

Write-Host "[INFO] venv 已激活: $env:VIRTUAL_ENV" -ForegroundColor Green