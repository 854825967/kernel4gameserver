rem TortoiseProc.exe /command:update /path:".\" /closeonend:1
protogen -i:demo.proto -o:demo.cs -p:detectMissing
rem move /y .\*.cs 
pause.