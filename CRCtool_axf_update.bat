@echo off
REM 1. CRC Tool'u çalıştır (Yeni dosya oluşturacak: DEA801_G03_V01.20.00_crc.axf)
"%~dp0crctool_2025_08.exe" -nw "DEA801_G03_V01.20.00.axf"

REM 2. Hata kontrolü (Opsiyonel ama önerilir)
IF %ERRORLEVEL% NEQ 0 (
    echo CRC Tool hatasi!
    exit /b 1
)

REM 3. Orijinal dosyanın yedeğini al (İsteğe bağlı, debug için gerekmez ama güvenlidir)
copy "DEA801_G03_V01.20.00" "DEA801_G03_V01.20.00.bak" >nul

REM 4. CRC'li dosyayı orijinal dosya ismine kopyala (Üzerine yaz)
move /Y "DEA801_G03_V01.20.00_crc.axf" "DEA801_G03_V01.20.00.axf"

echo CRC islemi tamamlandi ve dosya guncellendi.