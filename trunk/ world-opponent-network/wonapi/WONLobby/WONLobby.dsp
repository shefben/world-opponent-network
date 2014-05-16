# Microsoft Developer Studio Project File - Name="WONLobby" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=WONLobby - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONLobby.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONLobby.mak" CFG="WONLobby - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONLobby - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONLobby - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONLobby - Win32 Dinkumware Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI/WONLobby", XHDCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WONLobby - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONLobby___Win32_Release"
# PROP BASE Intermediate_Dir "WONLobby___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /I "../WONGUI/zlib" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobby_r.lib"

!ELSEIF  "$(CFG)" == "WONLobby - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONLobby___Win32_Debug"
# PROP BASE Intermediate_Dir "WONLobby___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I ".." /I "../WONGUI/zlib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobby_d.lib"

!ELSEIF  "$(CFG)" == "WONLobby - Win32 Dinkumware Release"

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
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I ".." /I "EmpireLobby2" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /I ".." /I "../WONGUI/zlib" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /Oityb1 /c
# SUBTRACT CPP /O<none>
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\WONLobby_dink_r.lib"

!ENDIF 

# Begin Target

# Name "WONLobby - Win32 Release"
# Name "WONLobby - Win32 Debug"
# Name "WONLobby - Win32 Dinkumware Release"
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AccountLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AddFriendDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AdminActionDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\BadUserDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ChatCommandParser.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ChatCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ChooseNetworkAdapterDialog.cpp

!IF  "$(CFG)" == "WONLobby - Win32 Release"

!ELSEIF  "$(CFG)" == "WONLobby - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONLobby - Win32 Dinkumware Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClientOptionsPopup.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CreateAccountCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CreateGameDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CreateRoomDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CreditsDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DialogLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DirectConnectCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\DirectConnectDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FriendOptionsPopup.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\FriendsListCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GameBrowserCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GameOptionsPopup.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GameStagingCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GameUserListCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\InitFailedCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\InitLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LanLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LanNameDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LanPortDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyClient.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyCompletion.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyConfig.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyContainer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyEvent.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyFriend.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyGame.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyGroup.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyMisc.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyOptionsCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyPersistentData.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyPlayer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyScreen.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyServer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyStaging.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LobbyStagingPrv.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LoginCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LoginHelpCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LoginScreen.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LoginStatusCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LostPasswordCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LostUserNameCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MOTDCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\OkCancelDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PasswordDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PingLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PingPainter.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\QuickPlayCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RoomCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RoomDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RoomLabelCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RootScreen.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\RoutingLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ServerListCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SpecialControls.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StagingLogic.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\StatusDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\TOUDialog.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UpdateAccountCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UserListCtrl.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONLobby.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONLobbyPrv.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONStatusString.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
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
# Begin Group "WONAPI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONAuth\Auth.cpp
# End Source File
# Begin Source File

SOURCE=..\WONCommon\Common.cpp
# End Source File
# Begin Source File

SOURCE=..\WONConfig\Config.cpp
# End Source File
# Begin Source File

SOURCE=..\WONCrypt\Crypt.cpp
# End Source File
# Begin Source File

SOURCE=..\WONDB\DB.cpp
# End Source File
# Begin Source File

SOURCE=..\WONDir\Dir.cpp
# End Source File
# Begin Source File

SOURCE=..\WONMisc\gqueryreporting.c
# End Source File
# Begin Source File

SOURCE=..\WONMisc\Misc.cpp
# End Source File
# Begin Source File

SOURCE=..\WONMisc\nonport.c
# End Source File
# Begin Source File

SOURCE=..\WONRouting\Routing.cpp
# End Source File
# Begin Source File

SOURCE=..\WONServer\Server.cpp
# End Source File
# Begin Source File

SOURCE=..\WONSocket\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\WONAPI.cpp
# End Source File
# Begin Source File

SOURCE=..\WONStatus.cpp
# End Source File
# End Group
# Begin Group "WONGUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONGUI\BrowserComponent.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\WONGUIConfig\BrowserComponentConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\ComponentBuild.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\WONGUIConfig\GUIConfigBuild.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\KernelBuild.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\MSBuild.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\UtilBuild.cpp
# End Source File
# End Group
# Begin Group "pnglib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONGUI\pnglib\png.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\PNGDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngget.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngread.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngset.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\pnglib\pngwutil.c
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONGUI\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\zlib\zutil.c
# End Source File
# End Group
# Begin Group "jpeglib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcapimin.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcapistd.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jccoefct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jccolor.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jchuff.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcinit.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcmainct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcmarker.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcmaster.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcparam.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcphuff.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcprepct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jcsample.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jctrans.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdatadst.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jerror.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jmemansi.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\JPGDecoder.cpp
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\WONGUI\jpeglib\jutils.c
# End Source File
# End Group
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
