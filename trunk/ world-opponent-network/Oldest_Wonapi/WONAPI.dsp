# Microsoft Developer Studio Project File - Name="WONAPI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104
# TARGTYPE "Xbox Static Library" 0x0b04

CFG=WONAPI - Win32 ServerWatson
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WONAPI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WONAPI.mak" CFG="WONAPI - Win32 ServerWatson"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WONAPI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 Debug SingleThreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 Release SingleThreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "WONAPI - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "WONAPI - Win32 Watson" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 ServerDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 ServerRelease" (based on "Win32 (x86) Static Library")
!MESSAGE "WONAPI - Win32 ServerWatson" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Titan/WONAPI", TQWBAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "WONAPI - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

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
# ADD CPP /nologo /MTd /Gm /GR /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONAPI___Win32_Debug_SingleThreaded"
# PROP BASE Intermediate_Dir "WONAPI___Win32_Debug_SingleThreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_SingleThreaded"
# PROP Intermediate_Dir "Debug_SingleThreaded"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /D "__WON_SINGLETHREADED__" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONAPI___Win32_Release_SingleThreaded"
# PROP BASE Intermediate_Dir "WONAPI___Win32_Release_SingleThreaded"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_SingleThreaded"
# PROP Intermediate_Dir "Release_SingleThreaded"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /D "__WON_SINGLETHREADED__" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONAPI___Xbox_Debug"
# PROP BASE Intermediate_Dir "WONAPI___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WONAPI___Xbox_Debug"
# PROP Intermediate_Dir "WONAPI___Xbox_Debug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "." /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONAPI___Xbox_Release"
# PROP BASE Intermediate_Dir "WONAPI___Xbox_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WONAPI___Xbox_Release"
# PROP Intermediate_Dir "WONAPI___Xbox_Release"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GR /GX /O2 /I "." /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "." /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONAPI___Win32_Watson"
# PROP BASE Intermediate_Dir "WONAPI___Win32_Watson"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WONAPI___Win32_Watson"
# PROP Intermediate_Dir "WONAPI___Win32_Watson"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONAPI___Win32_ServerDebug"
# PROP BASE Intermediate_Dir "WONAPI___Win32_ServerDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ServerDebug"
# PROP Intermediate_Dir "ServerDebug"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "../stlport/stlport" /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WONAPI___Win32_ServerRelease"
# PROP BASE Intermediate_Dir "WONAPI___Win32_ServerRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ServerRelease"
# PROP Intermediate_Dir "ServerRelease"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GR /GX /O2 /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "../stlport/stlport" /I "." /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /c
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WONAPI___Win32_ServerWatson"
# PROP BASE Intermediate_Dir "WONAPI___Win32_ServerWatson"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ServerWatson"
# PROP Intermediate_Dir "ServerWatson"
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Z7 /Od /I "../stlport/stlport" /I "." /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WON_MASTER_CPP_BUILD__" /YX /FD /GZ /c
RSC=rc.exe
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

# Name "WONAPI - Win32 Release"
# Name "WONAPI - Win32 Debug"
# Name "WONAPI - Win32 Debug SingleThreaded"
# Name "WONAPI - Win32 Release SingleThreaded"
# Name "WONAPI - Xbox Debug"
# Name "WONAPI - Xbox Release"
# Name "WONAPI - Win32 Watson"
# Name "WONAPI - Win32 ServerDebug"
# Name "WONAPI - Win32 ServerRelease"
# Name "WONAPI - Win32 ServerWatson"
# Begin Group "WONAuth"

# PROP Default_Filter ""
# Begin Group "WONAuth Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONAuth\Auth.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\Auth2Certificate.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthBase.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthCertificate.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthContext.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthPeerData.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthPubKeyBlock.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthSession.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\CDKey.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\GetCertOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthClient.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthServer.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthServerOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONAuth Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONAuth\Auth2Certificate.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthBase.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthCertificate.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthCertificateBase.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthContext.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthPeerData.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthPubKeyBlock.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\AuthSession.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\CDKey.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\CryptTransform.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\GetCertOp.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthClient.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthOp.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthServer.h
# End Source File
# Begin Source File

SOURCE=.\WONAuth\PeerAuthServerOp.h
# End Source File
# End Group
# End Group
# Begin Group "WONCommon"

# PROP Default_Filter ""
# Begin Group "WONCommon Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONCommon\AsyncOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\AsyncOpContainer.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\AsyncOpTracker.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\ByteBuffer.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Common.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\CommonSmall.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\CRC.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Linux.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Mac.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Windows.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\FileReader.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Md5.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Linux.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Mac.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Windows.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\ReadBuffer.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\RegKey.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\RegKey_Windows.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\StringUtil.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\Thread.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\TimerThread.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\WONConsole.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCommon\WriteBuffer.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONCommon Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONCommon\AsyncOp.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\AsyncOpContainer.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\AsyncOpTracker.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\BiMap.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\ByteBuffer.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Completion.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CRC.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CriticalSection_Linux.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CriticalSection_Mac.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\CriticalSection_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Linux.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Mac.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Event_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\FileReader.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\LittleEndian.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Md5.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Linux.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Mac.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Platform_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\ReadBuffer.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\RegKey.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\RegKey_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\StringUtil.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Thread.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Thread_Linux.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Thread_Mac.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\Thread_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\TimerThread.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\WONConsole.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\WONTypes.h
# End Source File
# Begin Source File

SOURCE=.\WONCommon\WriteBuffer.h
# End Source File
# End Group
# End Group
# Begin Group "WONCrypt"

# PROP Default_Filter ""
# Begin Group "WONCrypt Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONCrypt\BigInteger.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCrypt\Blowfish.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCrypt\Crypt.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCrypt\ElGamal.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCrypt\MD5Digest.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONCrypt\Random.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONCrypt Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONCrypt\BigInteger.h
# End Source File
# Begin Source File

SOURCE=.\WONCrypt\Blowfish.h
# End Source File
# Begin Source File

SOURCE=.\WONCrypt\ElGamal.h
# End Source File
# Begin Source File

SOURCE=.\WONCrypt\MD5Digest.h
# End Source File
# Begin Source File

SOURCE=.\WONCrypt\Random.h
# End Source File
# End Group
# End Group
# Begin Group "WONDB"

# PROP Default_Filter ""
# Begin Group "WONDB Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONDB\ContestCallOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\CreateAccountOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\DB.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\DBProxyOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\EventReportOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\GetAccountOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\GetProfileOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\ProfileData.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\RequestAccountInfoEmailOp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\WONDB\RequestPasswordEmailOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\SetProfileOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDB\UpdateAccountOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONDB Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONDB\ContestCallOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\CreateAccountOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\DBProxyOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\EventReportOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\GetAccountOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\GetProfileOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\ProfileData.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\RequestAccountInfoEmailOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\RequestPasswordEmailOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\SetProfileOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDB\UpdateAccountOp.h
# End Source File
# End Group
# End Group
# Begin Group "WONDir"

# PROP Default_Filter ""
# Begin Group "WONDir Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONDir\AddEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\Dir.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\DirEntity.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\DirEntityReplyParser.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\EntityClearDataObjectsOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\EntityModifyDataObjectsOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\EntitySetDataObjectsOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\FindEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\GetDirOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\GetEntityRequest.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\GetMultiDirOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\GetServiceOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\ModifyEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\RemoveEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\RenameEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONDir\RenewEntityOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONDir Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONDir\AddEntityOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\DirEntity.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\DirEntityReplyParser.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\DirTypes.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\EntityClearDataObjectsOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\EntityModifyDataObjectsOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\EntitySetDataObjectsOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\FindEntityOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\GetDirOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\GetEntityRequest.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\GetMultiDirOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\GetServiceOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\ModifyEntityOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\RemoveEntityOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\RenameEntityOp.h
# End Source File
# Begin Source File

SOURCE=.\WONDir\RenewEntityOp.h
# End Source File
# End Group
# End Group
# Begin Group "WONMisc"

# PROP Default_Filter ""
# Begin Group "WONMisc Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONMisc\DetectFirewallOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\FindPatchOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\FTPGetOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\GameSpySupport.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\GetMOTDOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\GetVersionOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\gqueryreporting.c

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\HTTPGetOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\MediaMetrix.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\Misc.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONMisc\nonport.c

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# End Group
# Begin Group "WONMisc Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONMisc\DetectFirewallOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\FindPatchOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\FTPGetOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\GameSpySupport.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\GetMOTDOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\GetVersionOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\gqueryreporting.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\HTTPGetOp.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\MediaMetrix.h
# End Source File
# Begin Source File

SOURCE=.\WONMisc\nonport.h
# End Source File
# End Group
# End Group
# Begin Group "WONRouting"

# PROP Default_Filter ""
# Begin Group "RS Administrative"

# PROP Default_Filter ""
# Begin Group "RS Administrative Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingAbortServShutdownOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingBanClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingBecomeModeratorOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetBadUserListOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingMuteClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendServerAlertOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServerAlertOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServShutdownAbortOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServShutdownStartOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingStartServShutdownOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingYouWereBannedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingYouWereMutedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "RS Administrative Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingAbortServShutdownOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingBanClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingBecomeModeratorOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetBadUserListOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingMuteClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendServerAlertOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServerAlertOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServShutdownAbortOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingServShutdownStartOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingStartServShutdownOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingYouWereBannedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingYouWereMutedOp.h
# End Source File
# End Group
# End Group
# Begin Group "RS Client"

# PROP Default_Filter ""
# Begin Group "RS Client Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingClientFlagsChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientJoinedServerOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientLeftServerOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDetectFirewallOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDisconnectClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetClientListOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingPeerChatOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingPeerDataOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingRegisterClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendChatOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendComplaintOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendDataOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetClientFlagsOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "RS Client Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingClientFlagsChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientJoinedServerOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientLeftServerOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDetectFirewallOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDisconnectClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetClientListOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingPeerChatOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingPeerDataOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingRegisterClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendChatOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendComplaintOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSendDataOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetClientFlagsOp.h
# End Source File
# End Group
# End Group
# Begin Group "RS DataObject"

# PROP Default_Filter ""
# Begin Group "RS DataObject Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingCreateDataObjectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectCreatedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectDeletedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectModifiedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDeleteDataObjectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingModifyDataObjectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingReadDataObjectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingUnsubDataObjectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "RS DataObject Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingCreateDataObjectOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectCreatedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectDeletedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDataObjectModifiedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDeleteDataObjectOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingModifyDataObjectOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingReadDataObjectOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingUnsubDataObjectOp.h
# End Source File
# End Group
# End Group
# Begin Group "RS Group"

# PROP Default_Filter ""
# Begin Group "RS Group Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingAcceptClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingCancelJoinGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientJoinedGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientLeftGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingCreateGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetGroupListOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetGroupListReqOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetMembersOfGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupCaptChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupCreatedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupDeletedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupFlagsChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupInvitationOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupJoinAttemptOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupMPlyrsChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupNameChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupObserverCountOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingInviteClientOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingJoinGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingLeaveGroupOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingMemberFlagsChangedOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingRelinquishCaptainOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupFlagsOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupMaxPlayersOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupNameOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupPasswordOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "RS Group Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONRouting\RoutingAcceptClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingCancelJoinGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientJoinedGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingClientLeftGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingCreateGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetGroupListOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetGroupListReqOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGetMembersOfGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupCaptChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupCreatedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupDeletedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupFlagsChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupInvitationOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupJoinAttemptOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupMPlyrsChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupNameChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingGroupObserverCountOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingInviteClientOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingJoinGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingLeaveGroupOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingMemberFlagsChangedOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingRelinquishCaptainOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupFlagsOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupMaxPlayersOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupNameOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingSetGroupPasswordOp.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\WONRouting\AllRoutingOps.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\Routing.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingConnection.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingConnection.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDerivedServerOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingDerivedServerOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingOp.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\RoutingTypes.h
# End Source File
# Begin Source File

SOURCE=.\WONRouting\SmartRoutingConnection.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONRouting\SmartRoutingConnection.h
# End Source File
# End Group
# Begin Group "WONServer"

# PROP Default_Filter ""
# Begin Group "WONServer Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONServer\MultiPingOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\PingOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\Server.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerConnection.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerContext.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerRequestOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONServer Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONServer\MultiPingOp.h
# End Source File
# Begin Source File

SOURCE=.\WONServer\PingOp.h
# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerContext.h
# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerOp.h
# End Source File
# Begin Source File

SOURCE=.\WONServer\ServerRequestOp.h
# End Source File
# End Group
# End Group
# Begin Group "WONSocket"

# PROP Default_Filter ""
# Begin Group "WONSocket Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONSocket\AcceptOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\AsyncSocket.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\BlockingSocket.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\CloseOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\ConnectOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\IPAddr.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\NetStats.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\QueueSocket.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvBytesFromOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvBytesOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvCRMsgOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvMsgOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendBytesOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendBytesToOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendMsgOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\Socket.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketOp.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThread.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx_Linux.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx_Windows.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadSimple.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadWinsock2.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "WONSocket Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WONSocket\AcceptOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\AsyncSocket.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\BlockingSocket.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\CloseOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\ConnectOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\IPAddr.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\NetStats.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\QueueSocket.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvBytesFromOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvBytesOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvCRMsgOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\RecvMsgOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendBytesOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendBytesToOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SendMsgOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketOp.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThread.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx_Linux.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadEx_Windows.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadSimple.h
# End Source File
# Begin Source File

SOURCE=.\WONSocket\SocketThreadWinsock2.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\WONAPI.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONAPI.h
# End Source File
# Begin Source File

SOURCE=.\WONShared.h
# End Source File
# Begin Source File

SOURCE=.\WONStatus.cpp

!IF  "$(CFG)" == "WONAPI - Win32 Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Debug SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Release SingleThreaded"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Debug"

!ELSEIF  "$(CFG)" == "WONAPI - Xbox Release"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 Watson"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerDebug"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerRelease"

!ELSEIF  "$(CFG)" == "WONAPI - Win32 ServerWatson"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WONStatus.h
# End Source File
# End Target
# End Project
