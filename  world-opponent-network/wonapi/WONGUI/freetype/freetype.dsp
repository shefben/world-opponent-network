# Microsoft Developer Studio Project File - Name="freetype2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Xbox Static Library" 0x0b04

CFG=freetype2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freetype.mak" CFG="freetype2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "freetype2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "freetype2 - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "freetype2 - Xbox Release" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "freetype2 - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "freetype" /I "../../" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "freetype" /I "../../" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "freetype2___Xbox_Debug"
# PROP BASE Intermediate_Dir "freetype2___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /I "freetype" /I "../../" /D "WIN32" /D "_XBOX" /D "_DEBUG" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /I "freetype" /I "../../" /D "WIN32" /D "_XBOX" /D "_DEBUG" /FD /GZ /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "freetype2___Xbox_Release"
# PROP BASE Intermediate_Dir "freetype2___Xbox_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "." /I "freetype" /I "../../" /D "WIN32" /D "_XBOX" /D "NDEBUG" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /GR /GX /O2 /I "." /I "freetype" /I "../../" /D "WIN32" /D "_XBOX" /D "NDEBUG" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "freetype2 - Win32 Release"
# Name "freetype2 - Win32 Debug"
# Name "freetype2 - Xbox Debug"
# Name "freetype2 - Xbox Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\autohint.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bdf.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cff.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftbase.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftcache.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftdebug.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FTFont.cpp

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftglyph.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftinit.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftmm.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ftsystem.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pcf.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pfr.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psaux.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pshinter.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psmodule.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\raster.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sfnt.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\smooth.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\truetype.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\type1.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\type1cid.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\type42.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\winfnt.c

!IF  "$(CFG)" == "freetype2 - Win32 Release"

!ELSEIF  "$(CFG)" == "freetype2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Debug"

!ELSEIF  "$(CFG)" == "freetype2 - Xbox Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FTFont.h
# End Source File
# Begin Source File

SOURCE=.\freetype\config\ftoption.h
# End Source File
# End Group
# End Target
# End Project
