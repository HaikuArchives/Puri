/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ToolBar.h"


#include "Messages.h"
#include "Tools.h"

ToolBar::ToolBar(void)
    :
    BView(BRect(), "toolbar", B_FOLLOW_NONE, B_WILL_DRAW |
        B_FULL_UPDATE_ON_RESIZE),
    fSpacing(0),
    fIconSpace(2),
    fButtonHighlight(-1),
    fButtonPressed(false),
    fFontSize(14)
{
    fTargetHandler = NULL;
    fTargetLooper = NULL;

    SetViewColor(B_TRANSPARENT_32_BIT);
    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);

	SetFontSize(fFontSize);    
}


void
ToolBar::AttachedToWindow(void)
{
    if (fTargetLooper == NULL)
        fTargetLooper = (BLooper*)Window();

	BView::AttachedToWindow();
}


void
ToolBar::AllAttached(void)
{
	BView::AllAttached();
}


void
ToolBar::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BView::MessageReceived(message);
			break;
	}
}


void
ToolBar::FrameResized(float newWidth, float newHeight)
{
    BView::FrameResized(newWidth, newHeight);
}


void
ToolBar::AddButton(BString const& label, BString const& iconName, BMessage* msg)
{
    fButton.push_back(label);
    fButtonIconName.push_back(iconName);
    fButtonIcon.push_back(LoadBitmap(iconName, fIconWidth));
    fButtonMsg.push_back(msg);
    BRect rect = BRect(0, 0, StringWidth(label) + fButtonMargin.left +
        fButtonMargin.right, fFontHeight.ascent + 2 * fFontHeight.descent +
            fButtonMargin.top + fButtonMargin.bottom);

    if (fButtonIcon.back() != NULL)
        rect.right += fIconWidth;

    if (fButtonBounds.empty() == false)
        rect.OffsetBy(fButtonBounds.back().RightTop() +
            BPoint(fSpacing, 0));
    else
        rect.OffsetBy(fMargin.LeftTop());

    fButtonBounds.push_back(rect);

    fIconOffset.push_back(rect.LeftTop() +
        BPoint(fButtonMargin.left, (rect.Height() - fIconWidth) / 2));

    if (fButtonIcon.back() != NULL)
        fTxtOffset.push_back(rect.LeftBottom() + BPoint(fButtonMargin.left +
            fIconWidth + fIconSpace, -fButtonMargin.bottom - fFontHeight.descent));
    else
        fTxtOffset.push_back(rect.LeftBottom() + BPoint(fButtonMargin.left,
            -fButtonMargin.bottom - fFontHeight.descent));

}


void
ToolBar::MouseUp(BPoint where)
{
    if (fButtonPressed == false)
        return;

    BPoint mousePos;
    uint32 mouseButtons;

    GetMouse(&mousePos, &mouseButtons);

    if (mouseButtons == B_PRIMARY_MOUSE_BUTTON)
        return;

    fButtonPressed = false;
    fButtonHighlight = -1;
    MouseMoved(where, 0, NULL);
    BView::MouseUp(where);
}


void
ToolBar::MouseDown(BPoint where)
{
    BPoint mousePos;
    uint32 mouseButtons;

    GetMouse(&mousePos, &mouseButtons);

    if (mouseButtons != B_PRIMARY_MOUSE_BUTTON)
        return;

    for (int i = 0; i < static_cast<int>(fButtonBounds.size()); ++i)
        if (fButtonBounds[i].Contains(where)) {
            fTargetLooper->PostMessage(fButtonMsg[i], fTargetHandler);
            fButtonPressed = true;
            fButtonHighlight = -1;
            MouseMoved(where, 0, NULL);
            break;
        }

    BView::MouseDown(where);
}


void
ToolBar::MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage)
{
    for (int i = 0; i < static_cast<int>(fButtonBounds.size()); ++i) {
        if (fButtonBounds[i].Contains(where)) {
            if (i == fButtonHighlight)
                return;

            if (fButtonHighlight >= 0)
                fButtonPressed = false;

            fButtonHighlight = i;
            FillRect(Bounds(), fGradient);
            SetHighColor(make_color(180, 180, 180, 255));
            StrokeRect(Bounds());

            SetHighColor(make_color(100, 100, 100, 255));

            if (fButtonPressed) {
                FillRoundRect(fButtonBounds[i], 4, 4, fGradientPressed);
                StrokeRoundRect(fButtonBounds[i].InsetByCopy(0, -0.1), 4, 4);
                StrokeRoundRect(fButtonBounds[i], 4, 4);
                SetHighColor(make_color(0, 0, 0, 255));

                for (int j = 0; j < static_cast<int>(fButtonBounds.size()); ++j) {
                    if (j == i) {
                        DrawBitmapAsync(fButtonIcon[j], fIconOffset[j] + BPoint(1, 1) );
                        DrawString(fButton[j], fTxtOffset[j] + BPoint(1, 1));
                    } else {
                        DrawBitmapAsync(fButtonIcon[j], fIconOffset[j]);
                        DrawString(fButton[j], fTxtOffset[j]);
                    }
                }
            } else {
                FillRoundRect(fButtonBounds[i], 4, 4, fGradientHighlight);
                StrokeRoundRect(fButtonBounds[i], 4, 4);
                SetHighColor(make_color(0, 0, 0, 255));

                for (int j = 0; j < static_cast<int>(fButtonBounds.size()); ++j) {
                    DrawBitmapAsync(fButtonIcon[j], fIconOffset[j]);
                    DrawString(fButton[j], fTxtOffset[j]);
                }
            }

            return;
        }
    }

    if (fButtonHighlight != -1)
        Invalidate();

    fButtonHighlight = -1;
    fButtonPressed = false;

    BView::MouseMoved(where, code, dragMessage);
}


void
ToolBar::Draw(BRect updateRect)
{
    BView::Draw(updateRect);
    BRect rect = Bounds().InsetBySelf(0, 0);
    FillRect(rect, fGradient);

    for (uint i = 0; i < fButtonBounds.size(); ++i) {
        DrawBitmapAsync(fButtonIcon[i], fIconOffset[i]);
        SetHighColor(make_color(0, 0, 0, 255));
        DrawString(fButton[i], fTxtOffset[i]);
    }

    SetHighColor(make_color(180, 180, 180, 255));
    StrokeRect(rect);
}


void
ToolBar::SetTarget(BHandler* handler, BLooper* looper)
{
    if (looper == NULL && handler != NULL)
        looper = (BLooper*)Window();
    else
        fTargetLooper = looper;

    fTargetHandler = handler;
}


int
ToolBar::FontSize(void)
{
	return fFontSize;	
}


void
ToolBar::SetFontSize(int const& fontSize)
{
	fFontSize = fontSize;
	
	BFont font;
    GetFont(&font);
    font.SetSize(fFontSize);
    font.SetFlags(Flags() | B_FORCE_ANTIALIASING);
    font.SetFace(B_CONDENSED_FACE /*| B_BOLD_FACE*/);
    SetFont(&font);
    GetFontHeight(&fFontHeight);

    fMargin.Set(5, 3, 5, 0);
    fButtonMargin.Set(5, 0, 5, 2);

    float height = ceilf(fFontHeight.ascent + 2 * fFontHeight.descent +
        fMargin.top + fMargin.bottom + fButtonMargin.top + fButtonMargin.bottom);

    fIconWidth = height - fMargin.top - fMargin.bottom - fButtonMargin.top -
        fButtonMargin.bottom - fFontHeight.descent - 2;

    SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, height));
    SetExplicitMinSize(BSize(0, height));

    fGradient.SetStart(0, 0);
    fGradient.SetEnd(0, height);
    fGradient.AddColor(make_color(230, 230, 255, 255), 0);
    fGradient.AddColor(make_color(180, 180, 255, 255), 255);

    fGradientHighlight.SetStart(0, 0);
    fGradientHighlight.SetEnd(0, height);
    fGradientHighlight.AddColor(make_color(210, 210, 255, 255), 0);
    fGradientHighlight.AddColor(make_color(150, 150, 255, 255), 255);

    fGradientPressed.SetStart(0, 0);
    fGradientPressed.SetEnd(0, height);
    fGradientPressed.AddColor(make_color(180, 180, 255, 255), 0);
    fGradientPressed.AddColor(make_color(130, 130, 255, 255), 255);

    Invalidate();
}
