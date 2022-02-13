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
Boolean enableColor;
IndexedColorType letterStatuses[26];

static void DrawKeyboardLetter(Char c, IndexedColorType color, int x, int y)
{
	RectangleType rect;
	FrameType frame = simpleFrame;

	rect.topLeft.x = x;
	rect.topLeft.y = y;
	rect.extent.x = 10;
	rect.extent.y = 10;

	WinDrawChar(c, x + 1, y - 1);
	if (enableColor)
	{
		WinSetForeColor(color);
		WinDrawRectangleFrame(frame, &rect);
	}
	else
	{
		if (color == CORRECT_COLOR)
		{
			frame = thickFrame;
			WinDrawRectangleFrame(frame, &rect);
		}
		else if (color == INWORD_COLOR)
		{
			frame = roundFrame;
			WinDrawRectangleFrame(frame, &rect);
		}
		else if (color == WRONG_COLOR)
		{
			WinDrawRectangle(&rect, 0);
			WinEraseChars(&c, 1, x + 1, y - 1);
		}
	}
}

static void DrawKeyboardUsedChars(int x, int y)
{
	const char *topRow = "qwertyuiop";
	const char *midRow = "asdfghjkl";
	const char *botRow = "zxcvbnm";
	int guessNum, colNum, i, letterIndex, letterStatus;

	WinPushDrawState();

	for (guessNum = 0; guessNum < guessRow; guessNum++)
	{
		for (colNum = 0; colNum < 5; colNum++)
		{
			letterIndex = guess[guessNum][colNum] - 'a';
			if (guess[guessNum][colNum] == word[colNum])
			{
				letterStatuses[letterIndex] = CORRECT_COLOR;
			}
			else if (letterStatuses[letterIndex] != CORRECT_COLOR)
			{
				letterStatuses[letterIndex] = WRONG_COLOR;
				for (i = 0; i < 5; i++)
				{
					if (guess[guessNum][colNum] == word[i])
					{
						letterStatuses[letterIndex] = INWORD_COLOR;
					}
				}
			}
		}
	}

	FntSetFont(stdFont);

	for (i = 0; i < StrLen(topRow); i++)
	{
		letterStatus = letterStatuses[topRow[i] - 'a'];
		DrawKeyboardLetter(topRow[i], letterStatus, x + i * 14, y);
	}

	for (i = 0; i < StrLen(midRow); i++)
	{
		letterStatus = letterStatuses[midRow[i] - 'a'];
		DrawKeyboardLetter(midRow[i], letterStatus, x + 7 + i * 14, y + 14);
	}

	for (i = 0; i < StrLen(botRow); i++)
	{
		letterStatus = letterStatuses[botRow[i] - 'a'];
		DrawKeyboardLetter(botRow[i], letterStatus, x + 21 + i * 14, y + 28);
	}

	WinPopDrawState();
}

static Boolean CheckAndRenderWord(const Char c[5], int x, int y)
{
	Boolean won = true;
	int i, j, k;
	int wordInstances, guessInstances;
	int timesBeaten;
	IndexedColorType colors[5] = {
		CORRECT_COLOR,
		CORRECT_COLOR,
		CORRECT_COLOR,
		CORRECT_COLOR,
		CORRECT_COLOR};
	RectangleType rect;
	rect.topLeft.x = x;
	rect.topLeft.y = y;
	rect.extent.x = 20;
	rect.extent.y = 20;

	WinPushDrawState();

	FntSetFont(largeFont);

	for (i = 0; i < 5; i++)
	{
		if (word[i] != c[i])
		{
			colors[i] = WRONG_COLOR;
			won = false;
			for (j = 0; j < 5; j++)
			{
				if (word[j] == c[i])
				{
					wordInstances = 0;
					guessInstances = 0;
					for (k = 0; k < 5; k++)
					{
						if (word[k] == c[i])
						{
							wordInstances++;
						}
						if (c[k] == c[i])
						{
							guessInstances++;
						}
					}
					colors[i] = INWORD_COLOR;
					if (wordInstances < guessInstances)
					{
						timesBeaten = 0;
						for (k = 0; k < 5; k++)
						{
							if (k < i)
							{
								if (c[k] == c[i] && colors[k] < WRONG_COLOR)
								{
									timesBeaten++;
								}
							}
							else if (k > i)
							{
								if (c[k] == c[i] && c[k] == word[k])
								{
									timesBeaten++;
								}
							}
						}
						if (timesBeaten >= wordInstances)
						{
							colors[i] = WRONG_COLOR;
						}
					}
				}
			}
		}

		rect.topLeft.x = x + i * 24;

		if (enableColor)
		{
			WinSetForeColor(colors[i]);
			WinDrawRectangleFrame(thickFrame, &rect);
		}
		else
		{
			if (colors[i] != CORRECT_COLOR)
			{
				WinEraseRectangleFrame(simpleFrame, &rect);
			}
			if (colors[i] == CORRECT_COLOR)
			{
				WinDrawRectangleFrame(thickFrame, &rect);
			}
			else if (colors[i] == INWORD_COLOR)
			{
				WinDrawRectangleFrame(roundFrame, &rect);
			}
		}
		WinDrawChar(c[i] - 0x20, x + i * 24 + 7, y + 2);
	}

	WinPopDrawState();
	return won;
}

static void RenderBoard()
{
	int row, col;
	RectangleType rect;
	rect.extent.x = 20;
	rect.extent.y = 20;

	for (row = 0; row < 6; row++)
	{
		if (row < guessRow)
		{
			CheckAndRenderWord(guess[row], 20, row * 24 + 18);
		}
		else
		{
			for (col = 0; col < 5; col++)
			{
				rect.topLeft.x = col * 24 + 20;
				rect.topLeft.y = row * 24 + 18;
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
	CheckAndRenderWord(word, 20, 70);
}

static void RenderFinalWord()
{
	CheckAndRenderWord(guess[guessRow], 20, 110);
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
			won = CheckAndRenderWord(guess[guessRow], 20, guessRow * 24 + 18);

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

static Boolean KeyboardFormHandleEvent(EventPtr e)
{
	Boolean handled = false;
	if (e->eType == frmOpenEvent)
	{
		DrawKeyboardUsedChars(10, 20);
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
		case KeyboardForm:
			FrmSetEventHandler(frm, KeyboardFormHandleEvent);
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
	WinScreenMode(winScreenModeGet, NULL, NULL, NULL, &enableColor);

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
				case Keyboard:
					FrmGotoForm(KeyboardForm);
					break;
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
