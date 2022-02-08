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

#define thickFrame 2
#define PALETTE_OFFSET 127
#define CORRECT_COLOR PALETTE_OFFSET
#define INWORD_COLOR PALETTE_OFFSET + 1
#define WRONG_COLOR PALETTE_OFFSET + 2

const Char *word;
int guessRow = 0;
int guessCol = 0;
Char guess[6][5];

static Boolean CheckAndRenderGridSquare(Char c, int col, int x, int y)
{
	Boolean correct = true;
	Boolean inWord = true;
	IndexedColorType color;
	RectangleType rect;
	int i;
	Boolean enableColor;
	FrameType frame = thickFrame;
	rect.topLeft.x = x;
	rect.topLeft.y = y;
	rect.extent.x = 20;
	rect.extent.y = 20;

	WinPushDrawState();
	WinScreenMode(winScreenModeGet, NULL, NULL, NULL, &enableColor);

	if (c != word[col])
	{
		correct = false;
		inWord = false;
		for (i = 0; i < 5; i++)
		{
			if (word[i] == c)
			{
				inWord = true;
			}
		}
	}

	if (correct)
	{
		color = CORRECT_COLOR;
	}
	else if (inWord)
	{
		color = INWORD_COLOR;
		if (!enableColor)
		{
			frame = roundFrame;
		}
	}
	else
	{
		color = WRONG_COLOR;
	}

	if (enableColor)
	{
		WinSetForeColor(color);
		WinDrawRectangleFrame(frame, &rect);
	}
	else
	{
		if (!correct)
		{
			WinEraseRectangleFrame(simpleFrame, &rect);
		}
		if (correct || inWord)
		{
			WinDrawRectangleFrame(frame, &rect);
		}
	}
	WinPopDrawState();
	WinDrawChar(c - 0x20, x + 7, y + 2);
	return correct;
}

static void RenderBoard()
{
	int row, col;
	RectangleType rect;
	rect.extent.x = 20;
	rect.extent.y = 20;

	for (row = 0; row < 6; row++)
	{
		for (col = 0; col < 5; col++)
		{
			rect.topLeft.x = col * 24 + 20;
			rect.topLeft.y = row * 24 + 18;

			if (row < guessRow)
			{
				CheckAndRenderGridSquare(guess[row][col], col, col * 24 + 20, row * 24 + 18);
			}
			else
			{
				WinDrawRectangleFrame(simpleFrame, &rect);
				if (row == guessRow && col < guessCol)
				{
					WinDrawChar(guess[row][col] - 0x20, col * 24 + 27, row * 24 + 20);
				}
			}
		}
	}
}

static void RenderCorrectWord()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		CheckAndRenderGridSquare(word[i], i, i * 24 + 20, 70);
	}
}

static void RenderFinalWord()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		CheckAndRenderGridSquare(guess[guessRow][i], i, i * 24 + 20, 110);
	}
}

static Boolean WinFormHandleEvent(EventPtr e)
{
	if (e->eType == frmOpenEvent)
	{
		RenderCorrectWord();
		return true;
	}
	return false;
}

static Boolean LoseFormHandleEvent(EventPtr e)
{
	if (e->eType == frmOpenEvent)
	{
		RenderCorrectWord();
		RenderFinalWord();
		return true;
	}
	return false;
}

static Boolean MainFormHandleEvent(EventPtr e)
{
	int i;
	Boolean handled = false;
	RectangleType rect;
	Boolean won;
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
			won = true;
			for (i = 0; i < 5; i++)
			{
				won &= CheckAndRenderGridSquare(guess[guessRow][i], i, i * 24 + 20, guessRow * 24 + 18);
			}

			if (won)
			{
				FrmGotoForm(WinForm);
				return true;
			}

			if (guessRow < 5 && !won)
			{
				guessRow++;
				guessCol = 0;
			}
			else
			{
				FrmGotoForm(LoseForm);
				return true;
			}
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
	case frmOpenEvent:
		RenderBoard();
		handled = true;
		break;
	default:
		// do nothing
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

		switch (formId)
		{
		case MainForm:
			FrmSetEventHandler(frm, MainFormHandleEvent);
			break;
		case WinForm:
			FrmSetEventHandler(frm, WinFormHandleEvent);
			break;
		case LoseForm:
			FrmSetEventHandler(frm, LoseFormHandleEvent);
			break;
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
	RGBColorType table[3];

	table[0].r = 0;
	table[0].g = 127;
	table[0].b = 0;

	table[1].r = 255;
	table[1].g = 255;
	table[1].b = 0;

	table[2].r = 200;
	table[2].g = 200;
	table[2].b = 200;

	WinPalette(winPaletteSet, PALETTE_OFFSET, 3, table);

	FrmGotoForm(MainForm);
	nowSeconds = TimGetSeconds();
	nowDays = nowSeconds / 86400;
	wordIndex = nowDays - seedDays;
	wordIndex %= NUM_WORDS;
	if (wordIndex < 0)
	{
		wordIndex += NUM_WORDS;
	}
	word = wordOrder[wordIndex];
}

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	short err;
	EventType e;
	FormType *pfrm;

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
				goto _default;
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
