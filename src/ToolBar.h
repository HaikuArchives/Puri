/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <vector>

#include <GradientLinear.h>
#include <Handler.h>
#include <Looper.h>
#include <Message.h>
#include <String.h>
#include <View.h>

#include "Debug.h"

class ToolBar : public BView
{
public:
                    ToolBar(void);
    virtual void    AttachedToWindow(void);
    virtual void    AllAttached(void);
    virtual void    MessageReceived(BMessage* message);
    virtual void    Draw(BRect updateRect);
    virtual void    FrameResized(float newWidth, float newHeight);
    virtual void    MouseMoved(BPoint where, uint32 code,
                        const BMessage* dragMessage);
    virtual void    MouseDown(BPoint where);
    virtual void    MouseUp(BPoint where);

            void    SetTarget(BHandler* handler, BLooper* looper = NULL);

            void    AddButton(BString const& label, BString const& iconName = "",
                        BMessage* msg = NULL);
                        
        	int		FontSize(void);
        	void	SetFontSize(int const& fontSize);

private:

    BLooper*                    fTargetLooper;
    BHandler*                   fTargetHandler;

    BGradientLinear             fGradient;
    BGradientLinear             fGradientHighlight;
    BGradientLinear             fGradientPressed;

    std::vector<BString>        fButton;
    std::vector<BRect>          fButtonBounds;
    std::vector<BMessage*>      fButtonMsg;
    std::vector<BBitmap*>       fButtonIcon;
    std::vector<BString>        fButtonIconName;
    std::vector<BPoint>         fTxtOffset;
    std::vector<BPoint>         fIconOffset;

    BRect                       fMargin;
    BRect                       fButtonMargin;
    font_height                 fFontHeight;

    float                       fIconWidth;
    float                       fSpacing;
    float                       fIconSpace;

    int                         fButtonHighlight;
    bool                        fButtonPressed;
    int							fFontSize;

    Debug                       out;
};


#endif // TOOLBAR_H
