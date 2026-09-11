@echo off
REM 1. CRC Tool'u çalıştır (Yeni dosya oluşturacak: DEA802_ARMotion_Dbg_crc.axf)
"%~dp0crctool_2025_08.exe" -nw "DEA802_ARMotion_Dbg.axf"

REM 2. Hata kontrolü (Opsiyonel ama önerilir)
IF %ERRORLEVEL% NEQ 0 (
    echo CRC Tool hatasi!
    exit /b 1
)

REM 3. Orijinal dosyanın yedeğini al (İsteğe bağlı, debug için gerekmez ama güvenlidir)
copy "DEA802_ARMotion_Dbg.axf" "DEA802_ARMotion_Dbg.bak" >nul

REM 4. CRC'li dosyayı orijinal dosya ismine kopyala (Üzerine yaz)
move /Y "DEA802_ARMotion_Dbg_crc.axf" "DEA802_ARMotion_Dbg.axf"

echo CRC islemi tamamlandi ve dosya guncellendi.