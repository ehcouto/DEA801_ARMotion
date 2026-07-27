@echo off
REM 1. CRC Tool'u çalıştır (Yeni dosya oluşturacak: DEA801_G03_V01.20.00_crc.axf)
"%~dp0crctool_2026_07.exe" -nw "DEA802_ARMotion_Release.axf"

REM 2. Hata kontrolü (Opsiyonel ama önerilir)
IF %ERRORLEVEL% NEQ 0 (
    echo CRC Tool hatasi!
    exit /b 1
)

REM 3. Orijinal dosyanın yedeğini al (İsteğe bağlı, debug için gerekmez ama güvenlidir)
copy "DEA802_ARMotion_Release" "DEA802_ARMotion_Release.bak" >nul

REM 4. CRC'li dosyayı orijinal dosya ismine kopyala (Üzerine yaz)
move /Y "DEA802_ARMotion_Release_crc.axf" "DEA802_ARMotion_Release.axf"

echo CRC islemi tamamlandi ve dosya guncellendi.