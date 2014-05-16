# Microsoft Developer Studio Project File - Name="WONInternalAPI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WONInternalAPI - Win32 Watson
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONInternalAPI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONInternalAPI.mak" CFG="WONInternalAPI - Win32 Watson"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONInternalAPI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONInternalAPI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONInternalAPI - Win32 Watson" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby/EmpireLobby2", KGMCAAAA"
# PROP Scc_LocalPath ".\wonlobby\empirelobby2"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "../lib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONInternalAPI___Win32_Debug"
# PROP BASE Intermediate_Dir "WONInternalAPI___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "../lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONInternalAPI___Win32_Watson"
# PROP BASE Intermediate_Dir "WONInternalAPI___Win32_Watson"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Watson"
# PROP Intermediate_Dir "Watson"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "../lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "." /I "../lib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "WONInternalAPI - Win32 Release"
# Name "WONInternalAPI - Win32 Debug"
# Name "WONInternalAPI - Win32 Watson"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONInternal\ActivatePatchOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\AddPatchOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\AddVersionOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\BannedKeyOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\BaseMsgRequestOp.cpp
# End Source File
# Begin Source File

SOURCE=.\WONInternal\FilePushOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetAccountFromCDKeyExOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetAccountFromCDKeyOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetPatchServerModuleList.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetVersionListOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\Internal.cpp
# End Source File
# Begin Source File

SOURCE=.\WONInternal\KeySubscribeOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\MasterKeyOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\PatchCleanupOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RegAuth.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RehupOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RemovePatchOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\RemoveVersionOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONInternal\TitleConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONInternal\UpdateVersionOp.cpp

!IF  "$(CFG)" == "WONInternalAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONInternalAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONInternal\ActivatePatchOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\AddPatchOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\AddVersionOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\BannedKeyOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\BaseMsgRequestOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\FilePushOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetAccountFromCDKeyExOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetAccountFromCDKeyOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetPatchServerModuleListOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\GetVersionListOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\KeySubscribeOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\MasterKeyOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\PatchCleanupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RegAuth.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RehupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RemovePatchOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\RemoveVersionOp.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\TitleConfig.h
# End Source File
# Begin Source File

SOURCE=.\WONInternal\UpdateVersionOp.h
# End Source File
# Begin Source File

SOURCE=.\WONTypes.h
# End Source File
# End Group
# End Target
# End Project
