/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSFormulaWindow.h"

#include <stdlib.h>

#include <Application.h>
#include <Box.h>
#include <Button.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#include "FicsCodes.h"
#include "Messages.h"
#include "Settings.h"

ICSFormulaWindow::ICSFormulaWindow(BWindow* target)
	:
    BWindow(BRect(), "Formula", B_TITLED_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS
            | B_AUTO_UPDATE_SIZE_LIMITS | B_OUTLINE_RESIZE
           /* | B_WILL_ACCEPT_FIRST_CLICK*/),
    fTarget(target)
{
    fUnrated = new BCheckBox("Unrated");
    fUnrated->SetValue(1);
    fRated = new BCheckBox("Rated");
    fRated->SetValue(1);

    BGroupLayout* layout;
    layout = BLayoutBuilder::Group<>(B_VERTICAL, 0)
        .SetInsets(5, 5, 5, 5)
        .Add(fUnrated)
        .Add(fRated)
    ;

    BBox* box1 = new BBox("box1");
    box1->SetBorder(B_FANCY_BORDER);
    box1->AddChild(layout->View());

    fActivate = new BCheckBox("Activate");
    fActivate->SetValue(0);

    BStringView* eloSV = new BStringView("elo", "elo");
    eloSV->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));
    BStringView* timeSV = new BStringView("time", "time");
    timeSV->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));
    BStringView* incSV = new BStringView("inc", "Gain per move");
    incSV->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_MIDDLE));

    BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
        .SetInsets(10, 10, 10, 10)
        .Add(box1)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .AddGroup(B_VERTICAL, 5, 1)
                .Add(eloSV)
                .Add(timeSV)
                .Add(incSV)
            .End()
            .AddStrut(5)
            .AddGroup(B_VERTICAL, 5, 1)
                .AddGroup(B_HORIZONTAL, 0, 1)
                    .Add(fEloMin = new BTextControl("", "0", NULL))
                    .Add(new BStringView("-", " - "))
                    .Add(fEloMax = new BTextControl("", "3000", NULL))
                .End()
                .AddGroup(B_HORIZONTAL, 0, 1)
                    .Add(fTimeMin = new BTextControl("", "0", NULL))
                    .Add(new BStringView("-", " - "))
                    .Add(fTimeMax = new BTextControl("", "20", NULL))
                .End()
                .AddGroup(B_HORIZONTAL, 0, 1)
                    .Add(fIncMin = new BTextControl("", "0", NULL))
                    .Add(new BStringView("-", " - "))
                    .Add(fIncMax = new BTextControl("", "15", NULL))
                .End()
            .End()
        .End()
        .Add(new BSeparatorView(B_HORIZONTAL), 0)
        .Add(fActivate)
        .AddGlue(0)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(new BButton("OK", new BMessage(M_OK)))
            .Add(new BButton("Help", new BMessage(M_HELP)))
            .Add(new BButton("Cancel", new BMessage(M_CANCEL)))
        .End()
    ;

    BString e1, e2, t1, t2, i1, i2;
    Settings settings("FormulaWnd");

    settings << "EloMin" >> e1 << "EloMax" >> e2 << "TimeMin" >> t1 << "TimeMax"
        >> t2 << "IncMin" >> i1 << "IncMax" >> i2;

    if (e1 != "" && e2 != "" && t1 != "" && t2 != "" && i1 != "" && i2 != "") {
        fEloMin->SetText(e1);   fEloMax->SetText(e2);
        fTimeMin->SetText(t1);  fTimeMax->SetText(t2);
        fIncMin->SetText(i1);   fIncMax->SetText(i2);
    }

    int32 rated = 1, unrated = 1, activated = 0;

    settings << "rated" >> rated << "unrated" >> unrated
        << "activated" >> activated;

    fRated->SetValue(rated);
    fUnrated->SetValue(unrated);
    fActivate->SetValue(activated);

    CenterIn(fTarget->Frame());
    Show();
}


bool
ICSFormulaWindow::QuitRequested(void)
{
    return true;
}


void
ICSFormulaWindow::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case M_OK:
        {
            _Apply();
            Close();
            break;
        }

        case M_HELP:
            break;

        case M_CANCEL:
            Close();
            break;

		default:
            BWindow::MessageReceived(message);
            break;
	}
}


void
ICSFormulaWindow::_Apply(void)
{
    BString str;
    str << BLK_SET << " set formula";

    if (fActivate->Value() == 1) {
        if (fRated->Value() == 1)
            str << " (rated ||";
        else
            str << " (!rated &&";


        if (fUnrated->Value() == 1)
            str << " unrated)";
        else
            str << " !unrated)";

        // the converstion to int is done to be sure that the user-input_iterator
        // is indeed a number. If the text is not a valid number, atoi will
        // return 0.
        str << " && rating >= " << atoi(fEloMin->Text());
        str << " && rating <= " << atoi(fEloMax->Text());

        str << " && time >= " << atoi(fTimeMin->Text());
        str << " && time <= " << atoi(fTimeMax->Text());

        str << " && inc >= " << atoi(fIncMin->Text());
        str << " && inc <= " << atoi(fIncMax->Text());

        str << " &&";
    }

    str << " !atomic && !wild && !suicide && !crazyhouse && !bughouse"
        << " && !nonstandard";

    SendICS(str, fTarget);

    Settings settings("FormulaWnd");
    settings
        << "EloMin" << fEloMin->Text()  << "EloMax"  << fEloMax->Text()
        << "TimeMin"<< fTimeMin->Text() << "TimeMax" << fTimeMax->Text()
        << "IncMin" << fIncMin->Text()  << "IncMax"  << fIncMax->Text()
        << "rated" << fRated->Value()   << "unrated" << fUnrated->Value()
        << "activated" << fActivate->Value() << std::endl;
}
