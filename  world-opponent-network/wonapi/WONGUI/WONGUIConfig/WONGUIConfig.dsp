# Microsoft Developer Studio Project File - Name="WONGUIConfig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WONGUIConfig - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONGUIConfig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONGUIConfig.mak" CFG="WONGUIConfig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONGUIConfig - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONGUIConfig - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONGUIConfig - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby/EmpireLobby2", KGMCAAAA"
# PROP Scc_LocalPath "..\..\wonlobby\empirelobby2"
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WONGUIConfig - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\WONGUIConfig_r.lib"

!ELSEIF  "$(CFG)" == "WONGUIConfig - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "../.." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Lib\WONGUIConfig_d.lib"

!ELSEIF  "$(CFG)" == "WONGUIConfig - Win32 Dinkumware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONGUIConfig___Win32_Dinkumware_Release"
# PROP BASE Intermediate_Dir "WONGUIConfig___Win32_Dinkumware_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DinkumwareRelease"
# PROP Intermediate_Dir "DinkumwareRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /I "../.." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /Oityb1 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\Lib\WONGUIConfig_r.lib"
# ADD LIB32 /nologo /out:"..\..\Lib\WONGUIConfig_dink_r.lib"

!ENDIF 

# Begin Target

# Name "WONGUIConfig - Win32 Release"
# Name "WONGUIConfig - Win32 Debug"
# Name "WONGUIConfig - Win32 Dinkumware Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BrowserComponentConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ComboBoxConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ComponentConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ContainerConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DialogConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FontConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GUIConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GUIConfigBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageButtonConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ImageConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\InputBoxConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LabelConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ListBoxConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MenuConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MSComponentConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MultiListBoxConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ResourceConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ScrollAreaConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ScrollbarConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SimpleComponentConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SplitterConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TabCtrlConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TextBoxConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WinSkin.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONControlsConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONGUIConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BrowserComponentConfig.h
# End Source File
# Begin Source File

SOURCE=.\ButtonConfig.h
# End Source File
# Begin Source File

SOURCE=.\ComboBoxConfig.h
# End Source File
# Begin Source File

SOURCE=.\ComponentConfig.h
# End Source File
# Begin Source File

SOURCE=.\ContainerConfig.h
# End Source File
# Begin Source File

SOURCE=.\DialogConfig.h
# End Source File
# Begin Source File

SOURCE=.\FontConfig.h
# End Source File
# Begin Source File

SOURCE=..\GUIConfig.h
# End Source File
# Begin Source File

SOURCE=.\ImageButtonConfig.h
# End Source File
# Begin Source File

SOURCE=.\ImageConfig.h
# End Source File
# Begin Source File

SOURCE=.\InputBoxConfig.h
# End Source File
# Begin Source File

SOURCE=.\LabelConfig.h
# End Source File
# Begin Source File

SOURCE=.\ListBoxConfig.h
# End Source File
# Begin Source File

SOURCE=.\MenuConfig.h
# End Source File
# Begin Source File

SOURCE=.\MSComponentConfig.h
# End Source File
# Begin Source File

SOURCE=.\MultiListBoxConfig.h
# End Source File
# Begin Source File

SOURCE=.\ResourceConfig.h
# End Source File
# Begin Source File

SOURCE=.\ScrollAreaConfig.h
# End Source File
# Begin Source File

SOURCE=.\ScrollbarConfig.h
# End Source File
# Begin Source File

SOURCE=.\SimpleComponentConfig.h
# End Source File
# Begin Source File

SOURCE=.\SplitterConfig.h
# End Source File
# Begin Source File

SOURCE=.\TabCtrlConfig.h
# End Source File
# Begin Source File

SOURCE=.\TextBoxConfig.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlConfig.h
# End Source File
# Begin Source File

SOURCE=.\WinSkin.h
# End Source File
# Begin Source File

SOURCE=.\WONControlsConfig.h
# End Source File
# Begin Source File

SOURCE=.\WONGUIConfig.h
# End Source File
# End Group
# End Target
# End Project
