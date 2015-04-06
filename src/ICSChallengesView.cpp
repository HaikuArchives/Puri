/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSChallengesView.h"

#include <stdlib.h>
#include <vector>

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "AutoLocker.h"
#include "ColumnTypes.h"
#include "FicsCodes.h"
#include "ICSFormulaWindow.h"
#include "ICSSeekWindow.h"
#include "Messages.h"
#include "Tools.h"

using namespace std;

ChallengesCLV::ChallengesCLV(void)
    :
    BColumnListView("challenges_columnlistview", 0)
{

}


void
ChallengesCLV::ItemInvoked(void)
{
    !out << "Invoked" << endl;
    BIntegerField* fieldID = (BIntegerField*)CurrentSelection()->GetField(11);
    DeselectAll();

    if (fieldID == NULL)
        return;

    BString str;
    str << BLK_PLAY << " play " << fieldID->Value();
    SendICS(str, Window());

    BColumnListView::ItemInvoked();
}


ICSChallengesView::ICSChallengesView(void)
    :
    BGroupView("icschallengesview", B_VERTICAL, 0),
    fPulseRate(3),
    fPulses(0),
    fIsSeeking(false)
{
    fColumnList = new ChallengesCLV;
    fColumnList->AddColumn(new BIntegerColumn("idx", 40, 0, 1000), 11);
    fColumnList->AddColumn(new BStringColumn("Name", 140, 0, 1000, true), 10);
    fColumnList->AddColumn(new BStringColumn("Title", 40, 0, 1000, true), 9);
    fColumnList->AddColumn(new BIntegerColumn("Elo", 60, 0, 1000), 8);
    fColumnList->AddColumn(new BIntegerColumn("Time", 50, 0, 1000), 7);
    fColumnList->AddColumn(new BIntegerColumn("Inc", 50, 0, 1000), 6);
    fColumnList->AddColumn(new BStringColumn("Rated", 55, 0, 1000, true), 5);
    fColumnList->AddColumn(new BStringColumn("Type", 75, 0, 1000, true), 4);
    fColumnList->AddColumn(new BStringColumn("Color", 50, 0, 1000, true), 3);
    fColumnList->AddColumn(new BStringColumn("Range", 90, 0, 1000, true), 2);
    fColumnList->AddColumn(new BIntegerColumn("automatic", 35, 0, 1000), 1);
    fColumnList->AddColumn(new BIntegerColumn("formula", 35, 0, 1000), 0);

    fToolBar = new ToolBar();
    fToolBar->AddButton("Accept", "confirm", new BMessage(M_ACCEPT));
    fToolBar->AddButton("Delete", "quit", new BMessage(M_DELETE));
    fToolBar->AddButton("Modify", "star_green");
    fToolBar->AddButton("Seek", "find", new BMessage(M_SEEK));
    fToolBar->AddButton("Formula", "calculator", new BMessage(M_FORMULA));

    BLayoutBuilder::Group<>(this)
        .Add(fColumnList)
        .Add(fToolBar)
    ;
}


void
ICSChallengesView::AttachedToWindow(void)
{
    SetFlags(Flags() | B_PULSE_NEEDED);
    Window()->SetPulseRate(3000000);
        // 3 seconds
    fIsSeeking = true;
    fToolBar->SetTarget(this);

    BGroupView::AttachedToWindow();
}


void
ICSChallengesView::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case M_ACCEPT:
        {
            if (fColumnList->FocusRow() == NULL)
                return;

            BIntegerField* field = (BIntegerField*) fColumnList->FocusRow()
                ->GetField(11);

            BString str;
            str << BLK_PLAY << " play " << field->Value();
            SendICS(str, Window());
            break;
        }

        case M_DELETE:
        {
            BString str;
            str << BLK_UNSEEK << " unseek";
            SendICS(str, Window());
            break;
        }

        case M_MODIFY:

            break;

        case M_SEEK:
            new ICSSeekWindow(Window());
            break;

        case M_FORMULA:
            new ICSFormulaWindow(Window());
            break;

        // to be deleted
        case ICS_OFFERED_GAMES:
        {
            BString str = "";
			message->FindString("info", &str);
           // !out << str << std::endl;
            break;
        }

        // to be deleted
        case ICS_OFFERED_SEEK:
        {
            BString str = "";
			message->FindString("info", &str);
            //!out << str << std::endl;
            break;

        }

        case ICS_SEEK_ADD:
        {
            BString str = "";
			message->FindString("info", &str);
            str.ReplaceAll("  ", " ");
            str.ReplaceAll("<s>", "");
            str.ReplaceAll("\r", "");

            std::vector<BString> vec = Tools::Split(str, '\n');

            for (uint i = 0; i < vec.size(); ++i)
                AddSeek(vec[i]);

            break;
        }

        case ICS_SEEK_REMOVE:
        {
            BString str = "";
			message->FindString("info", &str);

            int n1 = 0;
            int n2 = str.Length();

            while ((n1 = str.FindLast(" ", n2)) != B_ERROR) {
                char temp[n2 - n1];
                ++n1;
                str.CopyInto(temp, n1, n2 - n1);
                temp[n2 - n1] = '\0';
                RemoveSeek(atoi(temp));
                n2 = --n1;
            }

            break;
        }

        case ICS_SEEK_CLEAR:
        {
            fColumnList->Clear();
            break;
        }

		default:
            BGroupView::MessageReceived(message);
			break;
	}
}


void
ICSChallengesView::AddSeek(BString const& str)
{
    if (str.Length() == 0)
        return;

    BRow*	row = new BRow();

    int n1 = 0;
    int n2 = str.Length();

    for (int i = 0; (n1 = str.FindLast(" ", n2)) != B_ERROR; ++i) {
        char temp[n2 - n1];
        ++n1;
        str.CopyInto(temp, n1, n2 - n1);
        temp[n2 - n1] = '\0';

        switch (i) {
            case 0: case 1:
                if (temp[2] == 't')
                    row->SetField(new BIntegerField(1), i);
                else
                    row->SetField(new BIntegerField(0), i);
                break;

            case 2:
                row->SetField(new BStringField(temp + 3), i);
                break;

            case 3:
                row->SetField(new BStringField(temp + 2), i);
                break;

            case 4:
                row->SetField(new BStringField(temp + 3), i);
                break;

            case 5:
                row->SetField(new BStringField(temp + 2), i);
                break;

            case 6:
                row->SetField(new BIntegerField(atoi(temp + 2)), i);
                break;

            case 7:
                row->SetField(new BIntegerField(atoi(temp + 2)), i);
                break;

            case 8:
            {
                BString str(temp + 3);
                str.ReplaceSet("EP ", "");
                row->SetField(new BIntegerField(atoi(str)), i);
                break;
            }

            case 9:
                row->SetField(new BStringField(temp + 3), i);
                break;

            case 10:
                row->SetField(new BStringField(temp + 2), i);
                break;

            case 11:
                row->SetField(new BIntegerField(atoi(temp)), i);
                break;

            default:
                row->SetField(new BStringField(temp), i);
                break;
        }

        n2 = --n1;
    }

    AutoLocker<BLocker> autolocker(fLocker);
    fColumnList->AddRow(row);
}


void
ICSChallengesView::RemoveSeek(int index)
{
    BIntegerField* field;

    AutoLocker<BLocker> autolocker(fLocker);

    for (int i = 0; i < fColumnList->CountRows(); ++i) {
        field = (BIntegerField*)fColumnList->RowAt(i)->GetField(11);
        if (field->Value() == index) {
            fColumnList->RemoveRow(fColumnList->RowAt(i));
            return;
        }
    }
}


void
ICSChallengesView::Pulse(void)
{
    if (fIsSeeking == false)
        return;

    if (fPulses < fPulseRate) {
        ++fPulses;
        return;
    }

    fPulses = 0;
    //Window()->PostMessage(MSG_ICS_SHOW_OFFERED_GAMES);
}
