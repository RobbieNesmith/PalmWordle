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
				if (e.data.ctlSelect.controlID == Ok)
					goto _quit;
				goto _default;
				break;

			case frmLoadEvent:
				FrmSetActiveForm(FrmInitForm(e.data.frmLoad.formID));
				break;

			case frmOpenEvent:
				pfrm = FrmGetActiveForm();
				FrmDrawForm(pfrm);
				WinDrawChars(word, 5, 50, 50);
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
