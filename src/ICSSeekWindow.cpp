/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSSeekWindow.h"

#include <stdlib.h>

#include <Application.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#include "FicsCodes.h"
#include "Messages.h"
#include "Settings.h"


ICSSeekWindow::ICSSeekWindow(BWindow* target)
	:
    BWindow(BRect(), "Seek", B_TITLED_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS
            | B_AUTO_UPDATE_SIZE_LIMITS | B_OUTLINE_RESIZE
            /*| B_WILL_ACCEPT_FIRST_CLICK*/),
    fTarget(target)
{
    BStringView* timeSV = new BStringView("time", "time");
    timeSV->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));
    BStringView* incSV = new BStringView("inc", "Gain per move");
    incSV->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));


    BGroupLayout* clocksLayout = BLayoutBuilder::Group<>(B_VERTICAL, 5)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .AddGroup(B_VERTICAL, 5, 1)
                .Add(timeSV)
                .Add(incSV)
            .End()
            .AddStrut(5)
            .AddGroup(B_VERTICAL, 5, 1)
                .AddGroup(B_HORIZONTAL, 0, 1)
                    .Add(fTimeMin = new BTextControl("", "5", NULL))
                    .Add(new BStringView("...", " ... "))
                    .Add(fTimeMax = new BTextControl("", "5", NULL))
                .End()
                .AddGroup(B_HORIZONTAL, 0, 1)
                    .Add(fIncMin = new BTextControl("", "3", NULL))
                    .Add(new BStringView("...", " ... "))
                    .Add(fIncMax = new BTextControl("", "3", NULL))
                .End()
            .End()
        .End()
        .AddStrut(5)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(new BButton("Bullet", new BMessage(M_BULLET)))
            .Add(new BButton("Fast", new BMessage(M_FAST)))
            .Add(new BButton("Normal", new BMessage(M_NORMAL)))
            .Add(new BButton("Slow", new BMessage(M_SLOW)))
        .End()
    ;

    BBox* clocksBox = new BBox("clocks_box");
    clocksBox->SetLabel("Clocks");
    clocksBox->SetBorder(B_FANCY_BORDER);
    clocksBox->AddChild(clocksLayout->View());

    fRated = new BCheckBox("Rated", new BMessage(M_RATED));
    fRated->SetValue(1);
    fWithClock = new BCheckBox("With Clock");
    fWithClock->SetValue(1);
    fWithClock->SetEnabled(false);

    BGroupLayout* conditionsLayout = BLayoutBuilder::Group<>(B_VERTICAL, 5)
        .SetInsets(5, 5, 5, 5)
        .Add(fRated)
        .Add(fWithClock)
        .AddGlue(0)
    ;

    BBox* conditionsBox = new BBox("conditions_box");
    conditionsBox->SetLabel("Conditions");
    conditionsBox->SetBorder(B_FANCY_BORDER);
    conditionsBox->AddChild(conditionsLayout->View());

    fColorAutomatic = new BRadioButton("Automatic", NULL);
    fColorAutomatic->SetValue(1);
    fColorAutomatic->SetEnabled(false);
    fColorWhite = new BRadioButton("White", NULL);
    fColorWhite->SetEnabled(false);
    fColorBlack = new BRadioButton("Black", NULL);
    fColorBlack->SetEnabled(false);

    BGroupLayout* colorLayout = BLayoutBuilder::Group<>(B_VERTICAL, 0)
        .SetInsets(5, 5, 5, 5)
        .Add(fColorAutomatic)
        .Add(fColorWhite)
        .Add(fColorBlack)
    ;

    BBox* colorBox = new BBox("color_box");
    colorBox->SetLabel("Color");
    colorBox->SetBorder(B_FANCY_BORDER);
    colorBox->AddChild(colorLayout->View());

    BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
        .SetInsets(10, 10, 10, 10)
        .Add(clocksBox)
        .AddGroup(B_HORIZONTAL, 20, 1)
            .Add(conditionsBox)
            .Add(colorBox)
        .End()
        .AddGlue(0)
        .AddGroup(B_HORIZONTAL, 5, 1)
            .AddGlue(0)
            .Add(new BButton("OK", new BMessage(M_OK)))
            .Add(new BButton("Cancel", new BMessage(M_CANCEL)))
            .Add(new BButton("Help", new BMessage(M_HELP)))
        .End()
    ;

    _LoadSettings();
    CenterIn(fTarget->Frame());
    Show();
}


bool
ICSSeekWindow::QuitRequested(void)
{
    return true;
}


void
ICSSeekWindow::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case M_OK:
            _SaveSettings();
            Close();
            break;

        case M_CANCEL:
            Close();
            break;

        case M_HELP:
            break;

        case M_BULLET:
            fTimeMin->SetText("1");
            fTimeMax->SetText("1");
            fIncMin->SetText("0");
            fIncMax->SetText("0");
            break;

        case M_FAST:
            fTimeMin->SetText("3");
            fTimeMax->SetText("4");
            fIncMin->SetText("0");
            fIncMax->SetText("0");
            break;

        case M_NORMAL:
            fTimeMin->SetText("3");
            fTimeMax->SetText("5");
            fIncMin->SetText("1");
            fIncMax->SetText("2");
            break;

        case M_SLOW:
            fTimeMax->SetText("15");
            fTimeMin->SetText("25");
            fIncMax->SetText("1");
            fIncMin->SetText("10");
            break;

        case M_RATED:
            if (fRated->Value() == 1) {
                fWithClock->SetValue(1);
                fWithClock->SetEnabled(false);
                fColorAutomatic->SetValue(1);
                fColorAutomatic->SetEnabled(false);
                fColorWhite->SetValue(0);
                fColorWhite->SetEnabled(false);
                fColorBlack->SetValue(0);
                fColorBlack->SetEnabled(false);
            } else {
                fWithClock->SetEnabled(true);
                fColorAutomatic->SetEnabled(true);
                fColorWhite->SetEnabled(true);
                fColorBlack->SetEnabled(true);
            }

            break;

		default:
            BWindow::MessageReceived(message);
            break;
	}
}


void
ICSSeekWindow::_SaveSettings(void)
{
    Settings settings("seek_wnd");
    settings
        << "TimeMin" << fTimeMin->Text() << "TimeMax" << fTimeMax->Text()
        << "IncMin" << fIncMin->Text() << "IncMax" << fIncMax->Text()
        << "Rated" << fRated->Value() << "WithClock" << fWithClock->Value()
        << "Automatic" << fColorAutomatic->Value()
        << "White" << fColorWhite->Value() << "Black" << fColorBlack->Value()
        << std::endl;

    BString command;
  /*  command
        << BLK_SET << " set time " << fTimeMin->Text() << "\n\r "
        << BLK_SET << " set inc "  << fIncMin->Text() << "\n\r ";
*/
    command << BLK_SEEK << " seek "
        << atoi(fTimeMin->Text()) << " " << atoi(fIncMin->Text());

    if (fRated->Value() == 0)
        command << " unrated";
    else
        command << " rated";

    if (fColorAutomatic->Value() == 0) {
        if (fColorWhite->Value() == 1)
            command << " white";
        else
            command << " black";
    }

    command << " a f";
    SendICS(command, fTarget);
}


void
ICSSeekWindow::_LoadSettings(void)
{
    Settings settings("seek_wnd");
    BString str;
    str = "5"; settings << "TimeMin" >> str; fTimeMin->SetText(str);
    str = "5"; settings << "TimeMax" >> str; fTimeMax->SetText(str);
    str = "3"; settings << "IncMin"  >> str;  fIncMin->SetText(str);
    str = "3"; settings << "IncMax"  >> str;  fIncMax->SetText(str);

    int32 value;
    value = 1; settings << "WithClock" >> value; fWithClock->SetValue(value);
    value = 1; settings << "Automatic" >> value; fColorAutomatic->SetValue(value);
    value = 1; settings << "White" >> value; fColorWhite->SetValue(value);
    value = 1; settings << "Black" >> value; fColorBlack->SetValue(value);
    value = 1; settings << "Rated" >> value; fRated->SetValue(value);

    PostMessage(M_RATED);
}
