<#
  Makes a self-signed code-signing certificate for this PC's Debug builds and points the build at it.
  The certificate lives in the current user's personal store, and DebugSigning.user.props names it by thumbprint.
  Run it again to reuse a valid certificate, or to make a new one once it expires. Delete that file to build unsigned.

    powershell -NoProfile -ExecutionPolicy Bypass -File build/debug-cert.ps1
#>
$ErrorActionPreference = "Stop"
$Subject = "CN=Quest for the Unicorn (local debug)"
$Settings = Join-Path $PSScriptRoot "DebugSigning.user.props"

$certificate = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert |
  Where-Object { $_.Subject -eq $Subject -and $_.HasPrivateKey -and $_.NotAfter -gt (Get-Date) } |
  Sort-Object NotAfter -Descending |
  Select-Object -First 1

if (-not $certificate) {
  $certificate = New-SelfSignedCertificate -Type CodeSigningCert -Subject $Subject `
    -CertStoreLocation Cert:\CurrentUser\My -KeyAlgorithm RSA -KeyLength 3072 -HashAlgorithm SHA256 `
    -KeyExportPolicy NonExportable -NotAfter (Get-Date).AddYears(5)
  "Created $Subject"
}

@"
<?xml version="1.0" encoding="utf-8"?>
<!-- Written by debug-cert.ps1: the certificate that signs this PC's Debug builds -->
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <DebugSigningThumbprint>$($certificate.Thumbprint)</DebugSigningThumbprint>
  </PropertyGroup>
</Project>
"@ | Set-Content -Encoding UTF8 $Settings

"Debug builds sign with $($certificate.Thumbprint), valid until $($certificate.NotAfter.ToString('yyyy-MM-dd'))"
