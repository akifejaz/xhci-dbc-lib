@ECHO OFF
CALL ./edksetup.bat rebuild
CMD /C build -a X64 -t VS2017 -p RuntimeDriverPkg/RuntimeDriverPkg.dsc
COPY ".\Build\RuntimeDriverPkg\DEBUG_VS2017\X64\RuntimeDriver.efi" .
PAUSE
