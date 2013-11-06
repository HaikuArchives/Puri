/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSChatView.h"

#include "AutoLocker.h"
#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "FicsCodes.h"
#include "Messages.h"
#include "Tools.h"


ICSChatView::ICSChatView(void)
    :
    BGroupView("ics_chatview", B_VERTICAL, 0)
{
    BFont font;
    font.SetSize(12);
    font.SetFlags(Flags() | B_FORCE_ANTIALIASING);
    font.SetFace(B_CONDENSED_FACE | B_BOLD_FACE);

    rgb_color color = make_color(0, 220, 0, 255);

    fTextView = new BTextView("chat_output");
    fTextView->MakeEditable(false);
    fTextView->SetViewColor(make_color(0, 0, 0, 255));
    fTextView->SetFontAndColor(&font, B_FONT_ALL, &color);
    fTextView->SetText("");

    fScrollBar = new BScrollBar("chat_scrollbar", fTextView, 0, 100, B_VERTICAL);
    fTextControl = new BTextControl("chat_input", "", "", new BMessage(M_SEND_BUTTON));
    fSendButton = new BButton("send", new BMessage(M_SEND_BUTTON));
    fChannel = new BOptionPopUp("chat_channel", "", NULL);
    fChannel->AddOptionAt("Main Channel", 0, 0);
    fChannel->SetValue(0);

    BLayoutBuilder::Group<>(this)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(fTextView)
            .Add(fScrollBar)
        .End()
        .AddGroup(B_HORIZONTAL, 5, 1)
            .Add(fChannel)
            .Add(fSendButton)
        .End()
        .Add(fTextControl)
    ;

}


void
ICSChatView::AttachedToWindow(void)
{
    fSendButton->SetTarget(this);
    fTextControl->SetTarget(this);

    BGroupView::AttachedToWindow();
}


void
ICSChatView::AddText(BString str)
{
    if (str == "")
        return;

    str.Append("\n\n");
    float min, max;
    AutoLocker<BLocker> autolock(fLocker);

    fScrollBar->GetRange(&min, &max);

    if (fScrollBar->Value() == max) {
        fTextView->Insert(fTextView->TextLength(), str, str.Length());
        fScrollBar->GetRange(&min, &max);
        fScrollBar->SetValue(max);
    }
    else {
        fTextView->Insert(fTextView->TextLength(), str, str.Length());
    }

}


void
ICSChatView::AddTellTarget(BString str)
{
    int n = fChannel->CountOptions();

    fChannel->AddOptionAt(str, 1000 + n, n);

    fChannel->SetValue(1000 + n);
}


//     AutoLocker< BLocker > lock(fLocker);
//     locking could be needed somewhere
void
ICSChatView::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case ICS_OFFERED_INFO:
        {
            BString str = " ";
			message->FindString("info", &str);
            str.RemoveSet("\n\r");
            AddText(str);
            break;
        }

        case ICS_SHOUT:
        {
            BString str = " ";
			message->FindString("info", &str);
            str.RemoveSet("\n\r");
            AddText(str);
            break;
        }

        case ICS_TELL:
        {
            BString str = " ";
			message->FindString("info", &str);
            str.RemoveSet("\n\r");
            AddText(str);
            break;
        }

        case M_SEND_BUTTON:
        {
            AutoLocker<BLocker> autolock(fLocker);
            BString str(fTextControl->Text());

            if (str.Length() < 1)
                return;

            AddText(str.Prepend("You: "));

            int32   value = -1;
            const char* name = " ";

            fChannel->SelectedOption(&name, &value);

            if (value == -1)
                return;

            BString nameStr(name);

            BString command;

            if (value >= 1000)
                command << BLK_TELL << " tell " << nameStr << " " << str;
            else
                command << BLK_SHOUT << " shout " << str;

            SendICS(command, Window());
            fTextControl->SetText("");
            float min, max;
            fScrollBar->GetRange(&min, &max);
            fScrollBar->SetValue(max);
            break;
        }


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
