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

const Char *word;
int guessRow = 0;
int guessCol = 0;
Char guess[6][5];

static Boolean MainFormHandleEvent(EventPtr e)
{
	int i, j;
	Boolean handled = false;
	RGBColorType rgb;
	RectangleType rect;
	rect.extent.x = 20;
	rect.extent.y = 20;

	switch (e->eType)
	{
	case keyDownEvent:
		if (e->data.keyDown.chr == chrBackspace)
		{
			if (guessCol > 0)
			{
				guessCol--;
			}
			rect.topLeft.x = guessCol * 24 + 20;
			rect.topLeft.y = guessRow * 24 + 18;
			WinEraseRectangle(&rect, 0);
			handled = true;
		}
		else if (e->data.keyDown.chr == chrLineFeed &&
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
			handled = true;
		}
		else if (((e->data.keyDown.chr >= chrCapital_A && e->data.keyDown.chr <= chrCapital_Z) ||
				  (e->data.keyDown.chr >= chrSmall_A && e->data.keyDown.chr <= chrSmall_Z)) &&
				 guessCol < 5)
		{
			if (e->data.keyDown.chr > chrCapital_Z)
			{
				guess[guessRow][guessCol] = e->data.keyDown.chr;
			}
			else
			{
				guess[guessRow][guessCol] = e->data.keyDown.chr + 0x20;
			}
			FntSetFont(largeFont);
			WinDrawChar(guess[guessRow][guessCol] - 0x20, guessCol * 24 + 27, guessRow * 24 + 20);
			guessCol++;
			handled = true;
		}
		break;
	default:
		//do nothing
	}
	return handled;
}

static Boolean ApplicationHandleEvent(EventPtr e)
{
	short formId;
	FormPtr frm;

	if (e->eType == frmLoadEvent)
	{
		formId = e->data.frmLoad.formID;
		frm = FrmInitForm(formId);
		FrmSetActiveForm(frm);

		if (formId == MainForm)
		{
			FrmSetEventHandler(frm, MainFormHandleEvent);
		}
		return true;
	}
	return false;
}

static void AppInit()
{
	UInt32 nowSeconds;
	UInt32 nowDays;
	UInt32 seedDays = 42904; // June 19 2021
	UInt32 wordIndex;

	FrmGotoForm(MainForm);
	nowSeconds = TimGetSeconds();
	nowDays = nowSeconds / 86400;
	wordIndex = nowDays - seedDays;
	word = wordOrder[wordIndex];
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	short err;
	EventType e;
	FormType *pfrm;
	int row, col;
	RectangleType rect;
	rect.extent.x = 20;
	rect.extent.y = 20;

	if (cmd == sysAppLaunchCmdNormalLaunch) // Make sure only react to NormalLaunch, not Reset, Beam, Find, GoTo...
	{
		AppInit();

		while (1)
		{
			EvtGetEvent(&e, 100);
			if (SysHandleEvent(&e))
				continue;
			if (MenuHandleEvent((void *)0, &e, &err))
				continue;
			if (ApplicationHandleEvent(&e))
			{
				continue;
			}

			switch (e.eType)
			{
			case ctlSelectEvent:
				switch (e.data.ctlSelect.controlID)
				{
				case OkButton:
					FrmGotoForm(MainForm);
					break;
				case QuitButton:
					goto _quit;
					break;
				}
				goto _default;
				break;

			case frmLoadEvent:
				FrmSetActiveForm(FrmInitForm(e.data.frmLoad.formID));
				break;

			case frmOpenEvent:
				pfrm = FrmGetActiveForm();
				FrmDrawForm(pfrm);
				if (e.data.frmOpen.formID == MainForm)
				{
					for (row = 0; row < 6; row++)
					{
						for (col = 0; col < 5; col++)
						{
							rect.topLeft.x = col * 24 + 20;
							rect.topLeft.y = row * 24 + 18;
							WinDrawRectangleFrame(simpleFrame, &rect);
						}
					}
				}
				break;

			case menuEvent:
				switch (e.data.menu.itemID)
				{
				case Quit:
					goto _quit;
					break;
				case About:
					FrmGotoForm(AboutForm);
					break;
				case HowTo:
					FrmGotoForm(HowToForm);
					break;
				}
				break;

			case appStopEvent:
				goto _quit;

			default:
			_default:
				if (FrmGetActiveForm())
					FrmDispatchEvent(&e);
			}
		}

	_quit:
		FrmCloseAllForms();
	}

	return 0;
}
