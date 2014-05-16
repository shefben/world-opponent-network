# Microsoft Developer Studio Project File - Name="pnglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Xbox Static Library" 0x0b04

CFG=pnglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak" CFG="pnglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pnglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pnglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pnglib - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pnglib - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "pnglib - Xbox Release" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby/EmpireLobby2", KGMCAAAA"
# PROP Scc_LocalPath "..\..\wonlobby\empirelobby2"

!IF  "$(CFG)" == "pnglib - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "../zlib" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\pnglib_r.lib"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "../zlib" /I "../.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\pnglib_d.lib"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pnglib___Win32_Dinkumware_Release"
# PROP BASE Intermediate_Dir "pnglib___Win32_Dinkumware_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DinkumwareRelease"
# PROP Intermediate_Dir "DinkumwareRelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "../zlib" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "../zlib" /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\pnglib_r.lib"
# ADD LIB32 /nologo /out:"..\..\Lib\pnglib_dink_r.lib"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "pnglib___Xbox_Debug"
# PROP BASE Intermediate_Dir "pnglib___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "../zlib" /I "../.." /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "../zlib" /I "../.." /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /GZ /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\pnglib_d.lib"
# ADD LIB32 /nologo /out:"Xbox_Debug\pnglib_d.lib"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "pnglib___Xbox_Release"
# PROP BASE Intermediate_Dir "pnglib___Xbox_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "../zlib" /I "../.." /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "../zlib" /I "../.." /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\pnglib_r.lib"
# ADD LIB32 /nologo /out:"Xbox_Release\pnglib_r.lib"

!ENDIF 

# Begin Target

# Name "pnglib - Win32 Release"
# Name "pnglib - Win32 Debug"
# Name "pnglib - Win32 Dinkumware Release"
# Name "pnglib - Xbox Debug"
# Name "pnglib - Xbox Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\png.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNGDecoder.cpp

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PNGDecoder1.0.5.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\pngerror.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngget.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngmem.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngpread.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngread.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngrio.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngrtran.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngrutil.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngset.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngtrans.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngwio.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngwrite.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngwtran.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pngwutil.c

!IF  "$(CFG)" == "pnglib - Win32 Release"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

!ELSEIF  "$(CFG)" == "pnglib - Win32 Dinkumware Release"

!ELSEIF  "$(CFG)" == "pnglib - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pnglib - Xbox Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\png.h
# End Source File
# Begin Source File

SOURCE=.\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\PNGDecoder.h
# End Source File
# Begin Source File

SOURCE=.\wonpng.h
# End Source File
# End Group
# End Target
# End Project
