param (
    [string]$programName
)

# Verifica se o nome do programa foi fornecido
if (-not $programName) {
    Write-Host "Uso: timesaver <nome_do_programa>"
    exit
}

# Define os caminhos para o compilador e bibliotecas
$includePath = "C:\MinGW\include"
$libPath = "C:\MinGW\lib"

# Compila o programa
Write-Host "Compilando $programName.c..."
gcc -c -o "$programName.o" "$programName.c" -I"$includePath"
if ($LASTEXITCODE -ne 0) {
    Write-Host "Erro durante a compilação."
    exit
}

# Linka o programa
Write-Host "Linkando $programName.o..."
gcc -o "$programName.exe" "$programName.o" -L"$libPath" -lfreeglut -lopengl32 -lglu32
if ($LASTEXITCODE -ne 0) {
    Write-Host "Erro durante o linking."
    exit
}

# Executa o programa
Write-Host "Executando $programName.exe..."
Invoke-Expression ".\$programName.exe"