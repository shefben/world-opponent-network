# Microsoft Developer Studio Project File - Name="Directoryserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Directoryserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Directoryserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Directoryserver.mak" CFG="Directoryserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Directoryserver - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Directoryserver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Directoryserver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Directoryserver - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Directoryserver - Win32 Release"
# Name "Directoryserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DirMsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Socketmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\directorymessages.h
# End Source File
# Begin Source File

SOURCE=.\SocketMgr.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Won_server.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\msg\Comm\CommMsgs.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\common\DataObject.cpp
# End Source File
# Begin Source File

SOURCE=..\common\DataObject.h
# End Source File
# Begin Source File

SOURCE=..\common\EventLog.cpp
# End Source File
# Begin Source File

SOURCE=..\common\EventLog.h
# End Source File
# Begin Source File

SOURCE=..\common\OutputOperators.h
# End Source File
# Begin Source File

SOURCE=..\common\RegKey.cpp
# End Source File
# Begin Source File

SOURCE=..\common\RegKey.h
# End Source File
# Begin Source File

SOURCE=..\common\Threadbase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Threadbase.h
# End Source File
# Begin Source File

SOURCE=..\common\UTF8String.cpp
# End Source File
# Begin Source File

SOURCE=..\common\UTF8String.h
# End Source File
# Begin Source File

SOURCE=..\common\Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WON.h
# End Source File
# Begin Source File

SOURCE=..\common\WONDebug.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WONDebug.h
# End Source File
# Begin Source File

SOURCE=..\common\WONExceptCodes.h
# End Source File
# Begin Source File

SOURCE=..\common\WONException.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WONException.h
# End Source File
# End Group
# Begin Group "msg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\msg\BadMsgException.cpp
# End Source File
# Begin Source File

SOURCE=..\msg\BadMsgException.h
# End Source File
# Begin Source File

SOURCE=..\msg\HeaderTypes.h
# End Source File
# Begin Source File

SOURCE=..\msg\LServiceTypes.h
# End Source File
# Begin Source File

SOURCE=..\msg\MServiceTypes.h
# End Source File
# Begin Source File

SOURCE=..\msg\ServerStatus.h
# End Source File
# Begin Source File

SOURCE=..\msg\SServiceTypes.h
# End Source File
# Begin Source File

SOURCE=..\msg\tmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\msg\tmessage.h
# End Source File
# Begin Source File

SOURCE=..\msg\TServiceTypes.h
# End Source File
# End Group
# Begin Group "Socket"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Socket\IPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\IPSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\IPXSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\IPXSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\PXSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\PXSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\SPXSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\SPXSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\TCPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\TCPSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\TMsgSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\TMsgSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\UDPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\UDPSocket.h
# End Source File
# Begin Source File

SOURCE=..\Socket\WSSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Socket\WSSocket.h
# End Source File
# End Group
# Begin Group "SDKCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SDKCommon\Completion.cpp
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\Completion.h
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\Event.h
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\MemStream.cpp
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\MemStream.h
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\Stream.cpp
# End Source File
# Begin Source File

SOURCE=..\SDKCommon\Stream.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\bin\crypt.lib
# End Source File
# Begin Source File

SOURCE=..\..\bin\common.lib
# End Source File
# Begin Source File

SOURCE=..\..\bin\auth.lib
# End Source File
# End Target
# End Project
