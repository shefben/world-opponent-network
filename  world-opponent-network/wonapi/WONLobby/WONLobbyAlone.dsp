# Microsoft Developer Studio Project File - Name="WONLobbyAlone" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WONLobbyAlone - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONLobbyAlone.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONLobbyAlone.mak" CFG="WONLobbyAlone - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONLobbyAlone - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONLobbyAlone - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONLobbyAlone - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONLobbyAlone___Win32_Release"
# PROP BASE Intermediate_Dir "WONLobbyAlone___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /Zm165 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobbyAlone_r.lib"

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONLobbyAlone___Win32_Debug"
# PROP BASE Intermediate_Dir "WONLobbyAlone___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /Zm165 /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobbyAlone_d.lib"

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

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
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /Oityb1 /Zm165 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobbyAlone_dink_r.lib"

!ENDIF 

# Begin Target

# Name "WONLobbyAlone - Win32 Release"
# Name "WONLobbyAlone - Win32 Debug"
# Name "WONLobbyAlone - Win32 Dinkumware Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AccountLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AddFriendDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AdminActionDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BadUserDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChatCommandParser.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChatCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ChooseNetworkAdapterDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClientOptionsPopup.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CreateAccountCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CreateGameDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CreateRoomDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CreditsDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DialogLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectConnectCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectConnectDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FriendOptionsPopup.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FriendsListCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GameBrowserCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GameOptionsPopup.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GameStagingCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GameUserListCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InitFailedCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InitLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LanLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LanNameDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LanPortDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyClient.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyCompletion.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyConfig.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyContainer.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyEvent.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyFriend.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyGame.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyGroup.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyMisc.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyOptionsCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyPersistentData.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyPlayer.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyScreen.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyServer.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyStaging.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LobbyStagingPrv.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LoginCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LoginHelpCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LoginScreen.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LoginStatusCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LostPasswordCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LostUserNameCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MOTDCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OkCancelDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PasswordDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PingLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PingPainter.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\QuickPlayCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoomCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoomDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoomLabelCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RootScreen.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoutingLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ServerListCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SpecialControls.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StagingLogic.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StatusDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TOUDialog.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UpdateAccountCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UserListCtrl.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONLobby.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONLobbyPrv.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONStatusString.cpp

!IF  "$(CFG)" == "WONLobbyAlone - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobbyAlone - Win32 Dinkumware Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AccountLogic.h
# End Source File
# Begin Source File

SOURCE=.\AddFriendDialog.h
# End Source File
# Begin Source File

SOURCE=.\AdminActionDialog.h
# End Source File
# Begin Source File

SOURCE=.\BadUserDialog.h
# End Source File
# Begin Source File

SOURCE=.\ChatCommandParser.h
# End Source File
# Begin Source File

SOURCE=.\ChatCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ChooseNetworkAdapterDialog.h
# End Source File
# Begin Source File

SOURCE=.\ClientOptionsPopup.h
# End Source File
# Begin Source File

SOURCE=.\CreateAccountCtrl.h
# End Source File
# Begin Source File

SOURCE=.\CreateGameDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreateRoomDialog.h
# End Source File
# Begin Source File

SOURCE=.\CreditsDialog.h
# End Source File
# Begin Source File

SOURCE=.\DialogLogic.h
# End Source File
# Begin Source File

SOURCE=.\DirectConnectCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DirectConnectDialog.h
# End Source File
# Begin Source File

SOURCE=.\FriendOptionsPopup.h
# End Source File
# Begin Source File

SOURCE=.\FriendsListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GameBrowserCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GameOptionsPopup.h
# End Source File
# Begin Source File

SOURCE=.\GameStagingCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GameUserListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\InitFailedCtrl.h
# End Source File
# Begin Source File

SOURCE=.\InitLogic.h
# End Source File
# Begin Source File

SOURCE=.\LanLogic.h
# End Source File
# Begin Source File

SOURCE=.\LanNameDialog.h
# End Source File
# Begin Source File

SOURCE=.\LanPortDialog.h
# End Source File
# Begin Source File

SOURCE=.\LanTypes.h
# End Source File
# Begin Source File

SOURCE=.\LobbyClient.h
# End Source File
# Begin Source File

SOURCE=.\LobbyCompletion.h
# End Source File
# Begin Source File

SOURCE=.\LobbyConfig.h
# End Source File
# Begin Source File

SOURCE=.\LobbyContainer.h
# End Source File
# Begin Source File

SOURCE=.\LobbyDialog.h
# End Source File
# Begin Source File

SOURCE=.\LobbyEvent.h
# End Source File
# Begin Source File

SOURCE=.\LobbyFriend.h
# End Source File
# Begin Source File

SOURCE=.\LobbyGame.h
# End Source File
# Begin Source File

SOURCE=.\LobbyGameMsg.h
# End Source File
# Begin Source File

SOURCE=.\LobbyGroup.h
# End Source File
# Begin Source File

SOURCE=.\LobbyLogic.h
# End Source File
# Begin Source File

SOURCE=.\LobbyMisc.h
# End Source File
# Begin Source File

SOURCE=.\LobbyOptionsCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LobbyPersistentData.h
# End Source File
# Begin Source File

SOURCE=.\LobbyPlayer.h
# End Source File
# Begin Source File

SOURCE=.\LobbyResource.h
# End Source File
# Begin Source File

SOURCE=.\LobbyScreen.h
# End Source File
# Begin Source File

SOURCE=.\LobbyServer.h
# End Source File
# Begin Source File

SOURCE=.\LobbyStaging.h
# End Source File
# Begin Source File

SOURCE=.\LobbyStagingPrv.h
# End Source File
# Begin Source File

SOURCE=.\LobbyStatus.h
# End Source File
# Begin Source File

SOURCE=.\LobbyTypes.h
# End Source File
# Begin Source File

SOURCE=.\LoginCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LoginHelpCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LoginScreen.h
# End Source File
# Begin Source File

SOURCE=.\LoginStatusCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LostPasswordCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LostUserNameCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MOTDCtrl.h
# End Source File
# Begin Source File

SOURCE=.\NetLogic.h
# End Source File
# Begin Source File

SOURCE=.\OkCancelDialog.h
# End Source File
# Begin Source File

SOURCE=.\PasswordDialog.h
# End Source File
# Begin Source File

SOURCE=.\PingLogic.h
# End Source File
# Begin Source File

SOURCE=.\PingPainter.h
# End Source File
# Begin Source File

SOURCE=.\QuickPlayCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RoomCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RoomDialog.h
# End Source File
# Begin Source File

SOURCE=.\RoomLabelCtrl.h
# End Source File
# Begin Source File

SOURCE=.\RootScreen.h
# End Source File
# Begin Source File

SOURCE=.\RoutingLogic.h
# End Source File
# Begin Source File

SOURCE=.\ServerListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SoundDecoder.h
# End Source File
# Begin Source File

SOURCE=.\SpecialControls.h
# End Source File
# Begin Source File

SOURCE=.\StagingLogic.h
# End Source File
# Begin Source File

SOURCE=.\StatusDialog.h
# End Source File
# Begin Source File

SOURCE=.\TOUDialog.h
# End Source File
# Begin Source File

SOURCE=.\UpdateAccountCtrl.h
# End Source File
# Begin Source File

SOURCE=.\UserListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\WONLobby.h
# End Source File
# Begin Source File

SOURCE=.\WONLobbyPrv.h
# End Source File
# Begin Source File

SOURCE=.\WONStatusString.h
# End Source File
# End Group
# Begin Group "Build"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONGUI\BrowserComponent.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\WONGUIConfig\BrowserComponentConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\DataBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\LobbyResource.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicBuild.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscBuild.cpp
# End Source File
# End Group
# End Target
# End Project
