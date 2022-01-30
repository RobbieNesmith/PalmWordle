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
#include <System/SystemPublic.h>
#include <UI/UIPublic.h>
#include "wordorder.h"

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	short err;
	EventType e;
	FormType *pfrm;

	if (cmd == sysAppLaunchCmdNormalLaunch) // Make sure only react to NormalLaunch, not Reset, Beam, Find, GoTo...
	{
		FrmGotoForm(Form1);

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
				WinDrawChars(wordOrder[400], 5, 50, 50);
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
