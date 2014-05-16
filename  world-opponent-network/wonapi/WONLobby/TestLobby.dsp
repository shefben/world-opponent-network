# Microsoft Developer Studio Project File - Name="TestLobby" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=TestLobby - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TestLobby.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestLobby.mak" CFG="TestLobby - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestLobby - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "TestLobby - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby", XHDCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestLobby - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TestLobby___Win32_Release"
# PROP BASE Intermediate_Dir "TestLobby___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /I "EmpireLobby2" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "TestLobby - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TestLobby___Win32_Debug"
# PROP BASE Intermediate_Dir "TestLobby___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I ".." /I "EmpireLobby2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "TestLobby - Win32 Release"
# Name "TestLobby - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\WONGUI\MSSound.cpp
# End Source File
# Begin Source File

SOURCE=.\TestLobby.cpp
# End Source File
# Begin Source File

SOURCE=.\WONLobby.cfg
# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resources\LobbyDefault\_GameResource.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\_LobbyMaster.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\_LobbyResource.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\_LobbySkin.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\_WONStatus.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\AddFriendDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\AdminActionDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\BadUserDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\ChatCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\ChooseNetworkAdapterDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\ColorScheme.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\CommonOptions.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\CreateAccountCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\CreateGameDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\CreateRoomDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\CreditsDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\DirectConnectCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\DirectConnectDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\FriendsListCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GameBrowserCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GameOptionsCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GameStagingCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GameStagingTabs.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GameUserListCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GenericDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\GettingStartedCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\InitFailedCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\InternetGameStagingCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\InternetOptions.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\InternetQuickPlay.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\InternetScreen.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LANNameDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LanOptions.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LANPortDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LanScreen.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LoginCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LoginHelpCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LoginScreen.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LoginStatusCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LostPasswordCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\LostUserNameCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\MainInternetTab.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\MainLanTab.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\MOTDCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\OkCancelDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\PasswordDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\PlayerOptionsCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\RoomCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\RoomDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\RoomLabelCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\RootScreen.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\ServerListCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\ServerListCustomHeader.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\StatusDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\TOUDialog.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\UpdateAccountCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\UserListCtrl.cfg
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\UserListTabCtrl.cfg
# End Source File
# End Group
# Begin Group "Help"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Chat.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\CommunityTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\DirectTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\EntryRestrictions.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Friends.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\GameBrowser.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\GamePlayHelp.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\GamePorts.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\GameTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\HelpRoot.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\HelpTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\HostGame.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Icons.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Ignore.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\LanTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\LobbyTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\loginhelp.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\OptionsTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Overview.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\QuickPlayTab.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Security.html
# End Source File
# Begin Source File

SOURCE=.\Resources\LobbyDefault\Help\Troubleshooting.html
# End Source File
# End Group
# End Target
# End Project
