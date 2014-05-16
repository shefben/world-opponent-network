# Microsoft Developer Studio Project File - Name="jpeglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Xbox Static Library" 0x0b04

CFG=jpeglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak" CFG="jpeglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeglib - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeglib - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "jpeglib - Xbox Release" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby/EmpireLobby2", KGMCAAAA"
# PROP Scc_LocalPath "..\..\wonlobby\empirelobby2"

!IF  "$(CFG)" == "jpeglib - Win32 Release"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\jpeglib_r.lib"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\jpeglib_d.lib"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "jpeglib___Win32_Dinkumware_Release"
# PROP BASE Intermediate_Dir "jpeglib___Win32_Dinkumware_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DinkumwareRelease"
# PROP Intermediate_Dir "DinkumwareRelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\jpeglib_r.lib"
# ADD LIB32 /nologo /out:"..\..\Lib\jpeglib_dink_r.lib"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "jpeglib___Xbox_Debug"
# PROP BASE Intermediate_Dir "jpeglib___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../.." /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../.." /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /GZ /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\jpeglib_d.lib"
# ADD LIB32 /nologo /out:"Xbox_Debug\jpeglib_d.lib"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "jpeglib___Xbox_Release"
# PROP BASE Intermediate_Dir "jpeglib___Xbox_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../.." /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../.." /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\jpeglib_r.lib"
# ADD LIB32 /nologo /out:"Xbox_Release\jpeglib_r.lib"

!ENDIF 

# Begin Target

# Name "jpeglib - Win32 Release"
# Name "jpeglib - Win32 Debug"
# Name "jpeglib - Win32 Dinkumware Release"
# Name "jpeglib - Xbox Debug"
# Name "jpeglib - Xbox Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\jcapimin.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcapistd.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jccoefct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jccolor.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcdctmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jchuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcinit.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcmainct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcmarker.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcmaster.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcomapi.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcparam.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcphuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcprepct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jcsample.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jctrans.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdapimin.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdapistd.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdatadst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdatasrc.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdcoefct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdcolor.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jddctmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdhuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdinput.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdmainct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdmarker.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdmaster.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdmerge.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdphuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdpostct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdsample.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jdtrans.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jerror.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jfdctflt.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jfdctfst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jfdctint.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jidctflt.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jidctfst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jidctint.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jidctred.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jmemansi.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jmemmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JPEGDecoder6a.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\JPGDecoder.cpp

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jquant1.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jquant2.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\jutils.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "jpeglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jdct.h
# End Source File
# Begin Source File

SOURCE=.\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\jerror.h
# End Source File
# Begin Source File

SOURCE=.\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\JPGDecoder.h
# End Source File
# Begin Source File

SOURCE=.\jversion.h
# End Source File
# End Group
# End Target
# End Project
