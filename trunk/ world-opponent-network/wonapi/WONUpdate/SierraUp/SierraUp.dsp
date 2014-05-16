# Microsoft Developer Studio Project File - Name="SierraUp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SierraUp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SierraUp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SierraUp.mak" CFG="SierraUp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SierraUp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SierraUp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SierraUp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\.." /I ".." /I "..\..\.." /I "TukatiSDK\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib version.lib /nologo /subsystem:windows /machine:I386 /libpath:"../TukatiSDK/Lib"

!ELSEIF  "$(CFG)" == "SierraUp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\.." /I ".." /I "..\..\.." /I "..\TukatiSDK\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../TukatiSDK/Lib"

!ENDIF 

# Begin Target

# Name "SierraUp - Win32 Release"
# Name "SierraUp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SierraUp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\SierraUp.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SierraUp.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\SierraUpRes\_UpdateMaster.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\_UpdateResource.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\AboutCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\CommonCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\ConfigProxyCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\DownloadCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\LoginCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\MotdCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\OptionalPatchCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\PatchDetailsCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\SelectHostCtrl.cfg
# End Source File
# Begin Source File

SOURCE=..\Resources\SierraUp.ico
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\VersionCheckCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\WebLaunchCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\WelcomeCtrl.cfg
# End Source File
# End Group
# Begin Source File

SOURCE=.\psapi.dll
# End Source File
# Begin Source File

SOURCE=..\..\..\..\WONPatch\SierraUpRestarter\Release\SierraUpRestarter.exe
# End Source File
# Begin Source File

SOURCE=.\SierraUpRes\WONUpdate.wbf
# End Source File
# End Target
# End Project
