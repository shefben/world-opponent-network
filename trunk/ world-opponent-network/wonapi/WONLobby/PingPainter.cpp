#include "PingPainter.h"

#include "LobbyResource.h"
#include "LobbyTypes.h"
#include "LobbyConfig.h"

#include "WONGUI/Graphics.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::LobbyPaintPingBars(Graphics &g, unsigned short thePing, int theHeight, HorzAlign theHorzAlign, int theWidth)
{
	if(thePing==LobbyPing_None)
		return;

	static unsigned int gPingCeiling = 5000;
	if (gPingCeiling == 5000)
	{
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		if (aConfig)
			gPingCeiling = aConfig->mPingCeiling;
	}

	GUIString aStr;
	ImagePtr anImage;
	int aNumBars = 0;

	if(thePing==LobbyPing_Pinging)
		anImage = LobbyGlobal_Searching_Image; //aStr = ServerListCtrl_Pinging_String;
	else if(thePing==LobbyPing_PingingDetails)
		anImage = anImage = LobbyGlobal_Searching_Image; //aStr = ServerListCtrl_Querying_String;
	else if(thePing==LobbyPing_Failed)
		anImage = ServerListCtrl_PingFailed_Image;
	else
	{
		aNumBars = 10 - (thePing * 10) / gPingCeiling; // Calc inverted 1 to 10 bars
		if(aNumBars<1)
			aNumBars = 1;
		if(aNumBars>10)
			aNumBars = 10;
	}


	int dx = 0;
	if(theHorzAlign!=HorzAlign_Left)
	{
		int aWidth = 0;
		if(!aStr.empty())
			aWidth = g.GetFont()->GetStringWidth(aStr);
		else if(anImage.get()!=NULL)
			aWidth = anImage->GetWidth();
		else
			aWidth = aNumBars * 3;

		if(theHorzAlign==HorzAlign_Center)
			dx = (theWidth - aWidth)/2;
		else 
			dx = theWidth - aWidth;

		if(dx<0)
			dx = 0;
	}

	if(!aStr.empty())
		g.DrawString(aStr,dx,0);
	else if(anImage.get()!=NULL)
	{
		int dy = (theHeight-anImage->GetHeight())/2;
		if(dy<0)
			dy = 0;

		g.DrawImage(anImage,dx,dy);
	}
	else
	{
		g.SetColor(0x10D810);
		int x = dx;
		int aHeight = theHeight-2;
		for(int i=0; i<aNumBars; i++)
		{
			g.FillRect(x,1,2,aHeight);
			x+=3;
		}	
	}
}
