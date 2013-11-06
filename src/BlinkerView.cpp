/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "BlinkerView.h"

BlinkerView::BlinkerView(void)
    :
    BView(BRect(), "blinker_view", B_FOLLOW_ALL,
            B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
    fIsOn(false)
{
    fGradientOff.SetStart(BPoint(0, 0));
    fGradientOff.SetEnd(BPoint(0, 14));
    fGradientOff.AddColor(make_color(255, 255, 255, 255), 0);
    fGradientOff.AddColor(make_color(200, 200, 200, 255), 120);
    fGradientOff.AddColor(make_color(50, 50, 50, 255), 255);

    fGradientOn.SetStart(BPoint(0, 0));
    fGradientOn.SetEnd(BPoint(0, 14));
    fGradientOn.AddColor(make_color(255, 255, 0, 255), 0);
    fGradientOn.AddColor(make_color(220, 220, 0, 255), 120);
    fGradientOn.AddColor(make_color(140, 140, 0, 255), 255);

}


void
BlinkerView::Draw(BRect updateRect)
{

    SetHighColor(ViewColor());
    FillRect(Bounds());

    BRect rect = Bounds();
    float radius = rect.Height()* 0.5;

    if (fIsOn)
        FillEllipse(BPoint(radius, radius),
            radius * 0.7, radius * 0.7, fGradientOn);
    else
        FillEllipse(BPoint(radius, radius),
            radius * 0.7, radius * 0.7, fGradientOff);
}


void
BlinkerView::SetOn(bool on)
{
    if (fIsOn == on)
        return;

    fIsOn = on;
    Invalidate();
}
