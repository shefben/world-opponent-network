# Microsoft Developer Studio Project File - Name="mp3lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mp3lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp3lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp3lib.mak" CFG="mp3lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp3lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mp3lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mp3lib - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3lib - Win32 Release"

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
# ADD CPP /nologo /MT /GR /GX /O2 /I "../.." /D "NDEBUG" /D "FPM_INTEL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\mp3lib_r.lib"

!ELSEIF  "$(CFG)" == "mp3lib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /Gm /GR /GX /ZI /Od /I "../.." /D "FPM_DEFAULT" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\mp3lib_d.lib"

!ELSEIF  "$(CFG)" == "mp3lib - Win32 Dinkumware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DinkumwareRelease"
# PROP BASE Intermediate_Dir "DinkumwareRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DinkumwareRelease"
# PROP Intermediate_Dir "DinkumwareRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /GR /GX /O2 /I "../.." /D "NDEBUG" /D "FPM_INTEL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /YX /FD /c
# ADD CPP /nologo /MT /GR /GX /O2 /I "../.." /D "NDEBUG" /D "FPM_INTEL" /D "WIN32" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /D "ASO_ZEROCHECK" /YX /FD /Oityb1 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\mp3lib_dink_r.lib"

!ENDIF 

# Begin Target

# Name "mp3lib - Win32 Release"
# Name "mp3lib - Win32 Debug"
# Name "mp3lib - Win32 Dinkumware Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bit.c
# End Source File
# Begin Source File

SOURCE=.\decoder.c
# End Source File
# Begin Source File

SOURCE=.\fixed.c
# End Source File
# Begin Source File

SOURCE=.\frame.c
# End Source File
# Begin Source File

SOURCE=.\huffman.c
# End Source File
# Begin Source File

SOURCE=.\layer12.c
# End Source File
# Begin Source File

SOURCE=.\layer3.c
# End Source File
# Begin Source File

SOURCE=.\MP3Decoder.cpp
# End Source File
# Begin Source File

SOURCE=.\stream.c
# End Source File
# Begin Source File

SOURCE=.\synth.c
# End Source File
# Begin Source File

SOURCE=.\timer.c
# End Source File
# Begin Source File

SOURCE=.\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\acconfig.h
# End Source File
# Begin Source File

SOURCE=.\bit.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\decoder.h
# End Source File
# Begin Source File

SOURCE=.\fixed.h
# End Source File
# Begin Source File

SOURCE=.\frame.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\huffman.h
# End Source File
# Begin Source File

SOURCE=.\layer12.h
# End Source File
# Begin Source File

SOURCE=.\layer3.h
# End Source File
# Begin Source File

SOURCE=.\mad.h
# End Source File
# Begin Source File

SOURCE=.\MP3Decoder.h
# End Source File
# Begin Source File

SOURCE=.\stream.h
# End Source File
# Begin Source File

SOURCE=.\synth.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# End Target
# End Project
