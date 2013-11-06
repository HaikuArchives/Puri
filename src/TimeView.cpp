/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "TimeView.h"

#include <iostream>
#include <math.h>
#include <sstream>

#include <Font.h>
#include <LayoutBuilder.h>
#include <String.h>

TimeView::TimeView(int32 const& replyCode)
    :
    BGroupView("time_view", B_VERTICAL, 0),
    fIsRunning(false),
    fIsTurnWhite(true),
    fTimeWhite(0),
    fTimeBlack(0),
    fPulseRate(2),
    fPulses(0),
    fReplyCode(replyCode)
{
    BFont font;
    font.SetSize(25);
    font.SetFlags(Flags() | B_FORCE_ANTIALIASING);
    font.SetFace(B_CONDENSED_FACE | B_BOLD_FACE);

	SetFlags(Flags() | B_PULSE_NEEDED);
	fWhiteClock = new BStringView("clock_white", "00:00:00");
    fWhiteClock->SetFont(&font);
	fBlackClock = new BStringView("clock_black", "00:00:00");
    fBlackClock->SetFont(&font);

    BGroupLayout* layout;
    layout = BLayoutBuilder::Group<>(B_HORIZONTAL, 0)
        .SetInsets(0, 0, 0, 0)
        .AddGlue()
		.Add(fWhiteClock)
        .AddStrut(10)
		.Add(fBlackClock)
		.AddGlue()
    ;

    layout->View()->SetViewColor(140,140,140);


    fBox = new BBox("box");
    fBox->SetBorder(B_FANCY_BORDER);
    fBox->AddChild(layout->View());

	BLayoutBuilder::Group<>(this)
        .Add(fBox)
	;
}


void
TimeView::AllAttached(void)
{
    fBox->SetViewColor(140, 140, 140);
    fWhiteClock->SetViewColor(140, 140, 140);
	fBlackClock->SetViewColor(140, 140, 140);
    Init(0);
    BGroupView::AllAttached();
}


void
TimeView::_Init(void)
{
    fIsTurnWhite = true;
    fPulses = 0;
    fWhiteClock->SetHighColor(200, 200, 0);
	fBlackClock->SetHighColor(200, 200, 0);
    fWhiteClock->Invalidate();
    fBlackClock->Invalidate();
}


void
TimeView::Start(void)
{
    fIsRunning = true;

    if (fIsTurnWhite) {
        fBlackClock->SetHighColor(200, 200, 200);

        if (fTimeWhite > 0)
            fWhiteClock->SetHighColor(255, 255, 0);
        else
            fWhiteClock->SetHighColor(255, 0, 0);

        fBlackClock->Invalidate();
        fWhiteClock->Invalidate();
    } else {
        fWhiteClock->SetHighColor(200, 200, 200);

        if (fTimeBlack > 0)
            fBlackClock->SetHighColor(255, 255, 0);
        else
            fBlackClock->SetHighColor(255, 0, 0);

        fWhiteClock->Invalidate();
        fBlackClock->Invalidate();
    }
}


void
TimeView::Stop(void)
{
    fIsRunning = false;
    fWhiteClock->SetHighColor(200, 200, 0);
    fWhiteClock->Invalidate();
    fBlackClock->SetHighColor(200, 200, 0);
    fBlackClock->Invalidate();
}


void
TimeView::Pulse(void)
{
	if (fIsRunning == false)
        return;

    fPulses++;

    if (fPulses < fPulseRate)
        return;

    fPulses = 0;

    if (fIsTurnWhite) {
        fTimeWhite--;
        _DisplayTimeWhite();

        if (fTimeWhite == 0)
            _NotifyTime();
    } else {
        fTimeBlack--;
        _DisplayTimeBlack();

        if (fTimeBlack == 0)
            _NotifyTime();
    }
}


void
TimeView::_NotifyTime(void)
{
    BMessage msg(fReplyCode);
    msg.AddInt32("time", fIsTurnWhite);
    Window()->PostMessage(&msg);
}


void
TimeView::_DisplayTimeWhite(void)
{
    if (fTimeWhite > 0)
        fWhiteClock->SetHighColor(255, 255, 0);
    else
        fWhiteClock->SetHighColor(255, 0, 0);

    fWhiteClock->SetText(_TimeToString(fTimeWhite));
    fWhiteClock->Invalidate();
}


void
TimeView::_DisplayTimeBlack(void)
{
    if (fTimeBlack > 0)
        fBlackClock->SetHighColor(255, 255, 0);
    else
        fBlackClock->SetHighColor(255, 0, 0);

    fBlackClock->SetText(_TimeToString(fTimeBlack));
    fBlackClock->Invalidate();
}


void
TimeView::Switch(void)
{
    fIsTurnWhite = !fIsTurnWhite;

    if (fIsTurnWhite) {
        fBlackClock->SetHighColor(200, 200, 200);
        fBlackClock->Invalidate();

        if (fTimeWhite > 0)
            fWhiteClock->SetHighColor(255, 255, 0);
        else
            fWhiteClock->SetHighColor(255, 0, 0);

        fWhiteClock->Invalidate();
    } else {
        fWhiteClock->SetHighColor(200, 200, 200);
        fWhiteClock->Invalidate();

        if (fTimeBlack > 0)
            fBlackClock->SetHighColor(255, 255, 0);
        else
            fBlackClock->SetHighColor(255, 0, 0);

        fBlackClock->Invalidate();
    }

    if (fIsRunning == false)
        fIsRunning = true;
}


void
TimeView::SetTimeMS(int time, bool white)
{
    if (white == true)
        fTimeWhite = time / 1000;
    else
        fTimeBlack= time / 1000;

    fPulses = 0;
    Invalidate();
}


void
TimeView::SetTimeWhite(int const& time)
{
    fTimeWhite = time;
    fPulses = 0;
    _DisplayTimeWhite();
}


void
TimeView::SetTimeWhite(BString time)
{
    if (time == "")
        return;

    fTimeWhite = _StringToTime(time);
    fPulses = 0;
    fWhiteClock->SetText(_TimeToString(fTimeWhite));
    fWhiteClock->Invalidate();
}


void
TimeView::SetTimeBlack(int const& time)
{
    fTimeBlack = time;
    fPulses = 0;
    _DisplayTimeBlack();
}


void
TimeView::SetTimeBlack(BString time)
{
    if (time == "")
        return;

    fTimeBlack = _StringToTime(time);
    fPulses = 0;
    fBlackClock->SetText(_TimeToString(fTimeBlack));
    fBlackClock->Invalidate();
}


void
TimeView::Init(int time)
{
	Stop();
	SetTime(time);
	_Init();
}


void
TimeView::SetTime(int const& time)
{
    SetTimeWhite(time);
    SetTimeBlack(time);
}


int
TimeView::_StringToTime(BString str)
{
    int time = 0;
    int idx1 = str.Length();
    int idx2 = str.Length();

    int factor = 1;

    while (idx1 > 0) {
        idx1 = str.FindLast(":", idx2) + 1;

        if (idx1 == B_ERROR)
            idx1 = 0;

        BString tempStr = "";
        str.CopyInto(tempStr, idx1, idx2 - idx1);
        std::stringstream ss(tempStr.String());
        int tempTime;
        ss >> tempTime;
        time = time + tempTime * factor;

        factor = factor * 60;
        idx2 = idx1 - 1;
    }

    return time;
}

BString
TimeView::_TimeToString(int time)
{
    BString  str;

    str << " ";

    if (time < 0) {
        str << "-";
        time= fabs(time);
	}

    int sec= time%60;
	time/=60;
	int min=time%60;
	time/=60;
	int hour=time;

    if (fTimeWhite >= 3600 || fTimeBlack >= 3600)
        str << hour << ":";

    if (min < 10)
        str << "0";

    str << min << ":";

    if (sec < 10)
        str << "0";

    str << sec << " ";

    return str;
}
