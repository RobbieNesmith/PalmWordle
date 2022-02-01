/*****************************************************************************\
*                                                                             *
* Project       : Mobile development tutorial                                 *
* Application   : Mobile version of "Hello World"                             *
* Component     : Main                                                        *
* Author        : Eric Poncet (mobile.eric-poncet.com)                        *
* Creation      : 06/18/2000                                                  *
*                                                                             *
*******************************************************************************
*                                                                             *
* History       :                                                             *
*                                                                             *
\*****************************************************************************/

#include "PalmWordle.h"
#include <PalmOS.h>
#include "wordorder.h"

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	short err;
	EventType e;
	FormType *pfrm;
	const Char *word;
	UInt32 nowSeconds;
	UInt32 nowDays;
	UInt32 seedDays = 42904; // June 19 2021
	UInt32 wordIndex;
	int row, col, i, j;
	int guessRow = 0;
	int guessCol = 0;
	Char guess[6][5];
	RGBColorType rgb;
	RectangleType rect;
	rect.topLeft.x = 10;
	rect.topLeft.y = 10;
	rect.extent.x = 20;
	rect.extent.y = 20;

	if (cmd == sysAppLaunchCmdNormalLaunch) // Make sure only react to NormalLaunch, not Reset, Beam, Find, GoTo...
	{
		FrmGotoForm(Form1);
		nowSeconds = TimGetSeconds();
		nowDays = nowSeconds / 86400;
		wordIndex = nowDays - seedDays;
		word = wordOrder[wordIndex];

		while (1)
		{
			EvtGetEvent(&e, 100);
			if (SysHandleEvent(&e))
				continue;
			if (MenuHandleEvent((void *)0, &e, &err))
				continue;

			switch (e.eType)
			{
			case ctlSelectEvent:
				goto _default;
				break;

			case frmLoadEvent:
				FrmSetActiveForm(FrmInitForm(e.data.frmLoad.formID));
				break;

			case frmOpenEvent:
				pfrm = FrmGetActiveForm();
				FrmDrawForm(pfrm);
				for (row = 0; row < 6; row++)
				{
					for (col = 0; col < 5; col++)
					{
						rect.topLeft.x = col * 24 + 20;
						rect.topLeft.y = row * 24 + 18;
						WinDrawRectangleFrame(simpleFrame, &rect);
					}
				}
				break;

			case keyDownEvent:
				if (e.data.keyDown.chr == chrBackspace)
				{
					if (guessCol > 0)
					{
						guessCol--;
					}
					rect.topLeft.x = guessCol * 24 + 20;
					rect.topLeft.y = guessRow * 24 + 18;
					WinEraseRectangle(&rect, 0);
				}
				else if (e.data.keyDown.chr == chrLineFeed &&
						 guessCol == 5)
				{
					for (i = 0; i < 5; i++)
					{
						if (guess[guessRow][i] == word[i])
						{
							rgb.r = 0;
							rgb.g = 127;
							rgb.b = 0;
						}
						else
						{
							rgb.r = 200;
							rgb.g = 200;
							rgb.b = 200;
							for (j = 0; j < 5; j++)
							{
								if (word[j] == guess[guessRow][i])
								{
									rgb.r = 255;
									rgb.g = 255;
									rgb.b = 0;
								}
							}
						}
						rect.topLeft.x = i * 24 + 20;
						rect.topLeft.y = guessRow * 24 + 18;
						WinSetForeColorRGB(&rgb, NULL);
						WinDrawRectangleFrame(0x0002, &rect);
					}
					if (guessRow < 6)
					{
						guessRow++;
						guessCol = 0;
					}
					rgb.r = 0;
					rgb.g = 0;
					rgb.b = 0;
					WinSetForeColorRGB(&rgb, NULL);
				}
				else if (((e.data.keyDown.chr >= chrCapital_A && e.data.keyDown.chr <= chrCapital_Z) ||
						  (e.data.keyDown.chr >= chrSmall_A && e.data.keyDown.chr <= chrSmall_Z)) &&
						 guessCol < 5)
				{
					if (e.data.keyDown.chr > chrCapital_Z)
					{
						guess[guessRow][guessCol] = e.data.keyDown.chr;
					}
					else
					{
						guess[guessRow][guessCol] = e.data.keyDown.chr + 0x20;
					}
					FntSetFont(largeFont);
					WinDrawChar(guess[guessRow][guessCol] - 0x20, guessCol * 24 + 27, guessRow * 24 + 20);
					guessCol++;
				}
				break;

			case menuEvent:
				break;

			case appStopEvent:
				goto _quit;

			default:
			_default:
				if (FrmGetActiveForm())
					FrmHandleEvent(FrmGetActiveForm(), &e);
			}
		}

	_quit:
		FrmCloseAllForms();
	}

	return 0;
}
