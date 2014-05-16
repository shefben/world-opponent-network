# Microsoft Developer Studio Project File - Name="WONGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Xbox Static Library" 0x0b04

CFG=WONGUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONGUI.mak" CFG="WONGUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONGUI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONGUI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONGUI - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "WONGUI - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "WONGUI - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby/WONGUI", XSECAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONGUI___Win32_Release"
# PROP BASE Intermediate_Dir "WONGUI___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /D "NDEBUG" /D "WIN32" /D "_LIB" /D _WIN32_WINNT=0x0400 /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONGUI_r.lib"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONGUI___Win32_Debug"
# PROP BASE Intermediate_Dir "WONGUI___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I ".." /D "_DEBUG" /D "WIN32" /D "_LIB" /D _WIN32_WINNT=0x0400 /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONGUI_d.lib"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONGUI___Xbox_Release"
# PROP BASE Intermediate_Dir "WONGUI___Xbox_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Xbox_Release"
# PROP Intermediate_Dir "Xbox_Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I ".." /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONGUI___Xbox_Debug1"
# PROP BASE Intermediate_Dir "WONGUI___Xbox_Debug1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xbox_Debug"
# PROP Intermediate_Dir "Xbox_Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I ".." /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D _WIN32_WINNT=0x0400 /YX /FD /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONGUI___Win32_Dinkumware_Release"
# PROP BASE Intermediate_Dir "WONGUI___Win32_Dinkumware_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DinkumwareRelease"
# PROP Intermediate_Dir "DinkumwareRelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /D "NDEBUG" /D "WIN32" /D "_LIB" /D _WIN32_WINNT=0x0400 /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /I ".." /D "NDEBUG" /D "WIN32" /D "_LIB" /D _WIN32_WINNT=0x0400 /YX /FD /Oityb1 /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\Lib\WONGUI_r.lib"
# ADD LIB32 /nologo /out:"..\Lib\WONGUI_dink_r.lib"

!ENDIF 

# Begin Target

# Name "WONGUI - Win32 Release"
# Name "WONGUI - Win32 Debug"
# Name "WONGUI - Xbox Release"
# Name "WONGUI - Xbox Debug"
# Name "WONGUI - Win32 Dinkumware Release"
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Group "Kernel Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Clipboard.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Cursor.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Font.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Graphics.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GUICompat.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GUIString.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GUITypes.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Image.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KernelBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NativeImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RawImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sound.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Window.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WindowEvent.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WindowManager.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Kernel Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\ColorScheme.h
# End Source File
# Begin Source File

SOURCE=.\Cursor.h
# End Source File
# Begin Source File

SOURCE=.\DisplayContext.h
# End Source File
# Begin Source File

SOURCE=.\EventTypes.h
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\Graphics.h
# End Source File
# Begin Source File

SOURCE=.\GUICompat.h
# End Source File
# Begin Source File

SOURCE=.\GUICompletion.h
# End Source File
# Begin Source File

SOURCE=..\WONLobby\EmpireLobby2\res\GUICompletion.h
# End Source File
# Begin Source File

SOURCE=.\GUIString.h
# End Source File
# Begin Source File

SOURCE=.\GUISystem.h
# End Source File
# Begin Source File

SOURCE=.\GUITypes.h
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\Keyboard.h
# End Source File
# Begin Source File

SOURCE=.\NativeImage.h
# End Source File
# Begin Source File

SOURCE=.\RawImage.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# Begin Source File

SOURCE=.\WindowEvent.h
# End Source File
# Begin Source File

SOURCE=.\WindowManager.h
# End Source File
# Begin Source File

SOURCE=.\WONGUI.h
# End Source File
# End Group
# End Group
# Begin Group "Component"

# PROP Default_Filter ""
# Begin Group "Component Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BrowserComponent.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Button.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ComboBox.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Component.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ComponentBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Container.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dialog.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImageButton.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InputBox.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Label.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ListBox.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MenuBar.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSControls.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MultiListBox.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RootContainer.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScrollArea.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Scrollbar.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SimpleComponent.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SpecialPurposeControls.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Spinner.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Splitter.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TabCtrl.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Table.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TextBox.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TreeCtrl.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONControls.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Component Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BrowserComponent.h
# End Source File
# Begin Source File

SOURCE=.\Button.h
# End Source File
# Begin Source File

SOURCE=.\ComboBox.h
# End Source File
# Begin Source File

SOURCE=.\Component.h
# End Source File
# Begin Source File

SOURCE=.\Container.h
# End Source File
# Begin Source File

SOURCE=.\Dialog.h
# End Source File
# Begin Source File

SOURCE=.\ImageButton.h
# End Source File
# Begin Source File

SOURCE=.\InputBox.h
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\ListBox.h
# End Source File
# Begin Source File

SOURCE=.\MenuBar.h
# End Source File
# Begin Source File

SOURCE=.\MSControls.h
# End Source File
# Begin Source File

SOURCE=.\MultiListBox.h
# End Source File
# Begin Source File

SOURCE=.\RootContainer.h
# End Source File
# Begin Source File

SOURCE=.\ScrollArea.h
# End Source File
# Begin Source File

SOURCE=.\Scrollbar.h
# End Source File
# Begin Source File

SOURCE=.\SimpleComponent.h
# End Source File
# Begin Source File

SOURCE=.\SpecialPurposeControls.h
# End Source File
# Begin Source File

SOURCE=.\Spinner.h
# End Source File
# Begin Source File

SOURCE=.\Splitter.h
# End Source File
# Begin Source File

SOURCE=.\TabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Table.h
# End Source File
# Begin Source File

SOURCE=.\TextBox.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\WONControls.h
# End Source File
# End Group
# End Group
# Begin Group "MS"

# PROP Default_Filter ""
# Begin Group "MS Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSBMPDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSClipboard.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSDisplayContext.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSFont.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSGraphics.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSNativeImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSRawImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSSound.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MSWindow.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MS Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MSBMPDecoder.h
# End Source File
# Begin Source File

SOURCE=.\MSClipboard.h
# End Source File
# Begin Source File

SOURCE=.\MSCursor.h
# End Source File
# Begin Source File

SOURCE=.\MSDisplayContext.h
# End Source File
# Begin Source File

SOURCE=.\MSFont.h
# End Source File
# Begin Source File

SOURCE=.\MSGraphics.h
# End Source File
# Begin Source File

SOURCE=.\MSKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\MSNativeImage.h
# End Source File
# Begin Source File

SOURCE=.\MSRawImage.h
# End Source File
# Begin Source File

SOURCE=.\MSSound.h
# End Source File
# Begin Source File

SOURCE=.\MSWindow.h
# End Source File
# End Group
# End Group
# Begin Group "Surface"

# PROP Default_Filter ""
# Begin Group "Surface Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SDisplayContext.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SFont.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SGraphics.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SNativeImage.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SRawImage.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Surface.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SurfaceImage.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SWindow.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Surface Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SDisplayContext.h
# End Source File
# Begin Source File

SOURCE=.\SFont.h
# End Source File
# Begin Source File

SOURCE=.\SGraphics.h
# End Source File
# Begin Source File

SOURCE=.\SNativeImage.h
# End Source File
# Begin Source File

SOURCE=.\SRawImage.h
# End Source File
# Begin Source File

SOURCE=.\Surface.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceImage.h
# End Source File
# Begin Source File

SOURCE=.\SWindow.h
# End Source File
# End Group
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Group "Util Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Animation.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Background.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BackgroundGrabber.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BMPDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChildLayoutManager.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChildLayouts.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ComponentListener.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DelayLoadImage.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GIFDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLDisplayGen.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLDocument.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLDocumentGen.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLFromText.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLParser.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HTMLTag.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImageDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImageFilter.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LayoutHelper.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Rectangle.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResourceCollection.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SelectionColor.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SoundDecoder.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StretchImage.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StringLocalize.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TGADecoder.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UtilBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WIMDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSound.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WrappedText.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Util Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Align.h
# End Source File
# Begin Source File

SOURCE=.\Animation.h
# End Source File
# Begin Source File

SOURCE=.\Background.h
# End Source File
# Begin Source File

SOURCE=.\BackgroundGrabber.h
# End Source File
# Begin Source File

SOURCE=.\BMPDecoder.h
# End Source File
# Begin Source File

SOURCE=.\ChildLayoutManager.h
# End Source File
# Begin Source File

SOURCE=.\ChildLayouts.h
# End Source File
# Begin Source File

SOURCE=.\ComponentListener.h
# End Source File
# Begin Source File

SOURCE=.\DelayLoadImage.h
# End Source File
# Begin Source File

SOURCE=.\GIFDecoder.h
# End Source File
# Begin Source File

SOURCE=.\HTMLDisplayGen.h
# End Source File
# Begin Source File

SOURCE=.\HTMLDocument.h
# End Source File
# Begin Source File

SOURCE=.\HTMLDocumentGen.h
# End Source File
# Begin Source File

SOURCE=.\HTMLFromText.h
# End Source File
# Begin Source File

SOURCE=.\HTMLParser.h
# End Source File
# Begin Source File

SOURCE=.\HTMLParser3.h
# End Source File
# Begin Source File

SOURCE=.\HTMLTag.h
# End Source File
# Begin Source File

SOURCE=.\ImageDecoder.h
# End Source File
# Begin Source File

SOURCE=.\ImageFilter.h
# End Source File
# Begin Source File

SOURCE=.\KeyboardListener.h
# End Source File
# Begin Source File

SOURCE=.\LayoutHelper.h
# End Source File
# Begin Source File

SOURCE=.\Rectangle.h
# End Source File
# Begin Source File

SOURCE=.\ResourceCollection.h
# End Source File
# Begin Source File

SOURCE=.\SelectionColor.h
# End Source File
# Begin Source File

SOURCE=.\SoundDecoder.h
# End Source File
# Begin Source File

SOURCE=.\StretchImage.h
# End Source File
# Begin Source File

SOURCE=.\StringLocalize.h
# End Source File
# Begin Source File

SOURCE=.\TGADecoder.h
# End Source File
# Begin Source File

SOURCE=.\WIMDecoder.h
# End Source File
# Begin Source File

SOURCE=.\WONSound.h
# End Source File
# Begin Source File

SOURCE=.\WrappedText.h
# End Source File
# End Group
# End Group
# Begin Group "X"

# PROP Default_Filter ""
# Begin Group "X Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XDisplayContext.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XFont.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XGraphics.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XNativeImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XRawImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XWindow.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "X Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XCursor.h
# End Source File
# Begin Source File

SOURCE=.\XDisplayContext.h
# End Source File
# Begin Source File

SOURCE=.\XFont.h
# End Source File
# Begin Source File

SOURCE=.\XGraphics.h
# End Source File
# Begin Source File

SOURCE=.\XKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\XNativeImage.h
# End Source File
# Begin Source File

SOURCE=.\XRawImage.h
# End Source File
# Begin Source File

SOURCE=.\XWindow.h
# End Source File
# End Group
# End Group
# Begin Group "XB"

# PROP Default_Filter ""
# Begin Group "XB Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBBitmapDecoder.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBBuild.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBFont.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBGraphics.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBInput.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBNativeImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBRawImage.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XBWindow.cpp

!IF  "$(CFG)" == "WONGUI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONGUI - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "XB Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBDisplayContext.h
# End Source File
# Begin Source File

SOURCE=.\XBFont.h
# End Source File
# Begin Source File

SOURCE=.\XBGraphics.h
# End Source File
# Begin Source File

SOURCE=.\XBInput.h
# End Source File
# Begin Source File

SOURCE=.\XBNativeImage.h
# End Source File
# Begin Source File

SOURCE=.\XBRawImage.h
# End Source File
# Begin Source File

SOURCE=.\XBWindow.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\hand.cur
# End Source File
# End Target
# End Project
