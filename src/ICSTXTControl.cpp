/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSTXTControl.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "AutoLocker.h"
#include "Messages.h"
#include "Tools.h"

ICSTXTControl::ICSTXTControl(void)
    :
    BGroupView("icstxtcontrol_view", B_VERTICAL, 0),
    fIsFocus(false)
{
    fTxtView = new BTextView("ics_output");
    fTxtView->MakeEditable(false);
    fTxtView->MakeSelectable(true);
    fTxtControl = new BTextControl("", "", new BMessage(M_SEND_BUTTON));
    fSendButton = new BButton("send", new BMessage(M_SEND_BUTTON));
    fScrollBar = new BScrollBar("scrollbar", fTxtView, 0, 100, B_VERTICAL);

    BLayoutBuilder::Group<>(this)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(fTxtView)
            .Add(fScrollBar)
        .End()
        .AddStrut(5)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(fTxtControl)
            .Add(fSendButton)
        .End()
    ;
}


void
ICSTXTControl::AttachedToWindow(void)
{
    fSendButton->SetTarget(this);
    fTxtControl->SetTarget(this);
    BGroupView::AttachedToWindow();
}


void
ICSTXTControl::AddTxt(BString const& str)
{
    float min, max;
    AutoLocker<BLocker> autolocker(fLocker);
    fScrollBar->GetRange(&min, &max);

    if (fScrollBar->Value() == max) {
        fTxtView->Insert(fTxtView->TextLength(), str.String(), str.Length());
        fScrollBar->GetRange(&min, &max);
        fScrollBar->SetValue(max);
    }
    else {
        fTxtView->Insert(fTxtView->TextLength(), str.String(), str.Length());
    }
}


void
ICSTXTControl::MessageReceived(BMessage* message)
{
    switch (message->what) {
		case M_SEND_BUTTON:
        {
            AutoLocker<BLocker> autolocker(fLocker);

            BString str(fTxtControl->Text());

            if (str.Length() < 1)
                return;

            BString command = str;
            SendICS(command.Prepend("1000 "), Window());

            str.Prepend("\n").Append("\n");
            fTxtView->Insert(fTxtView->TextLength(), str, str.Length());
            float min, max;
            fScrollBar->GetRange(&min, &max);
            fScrollBar->SetValue(max);
            fTxtControl->SetText("");
            fTxtControl->MakeFocus();
			break;
        }

        case ICS_USER_CMD_RESPONSE:
        {
            BString str = "";
            message->FindString("info", &str);
            AddTxt(str);
            break;
        }

		default:
            BGroupView::MessageReceived(message);
			break;
	}
}

