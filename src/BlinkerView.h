
/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef BLINKERVIEW_H
#define BLINKERVIEW_H

#include <GradientLinear.h>
#include <Rect.h>
#include <View.h>

#include "Debug.h"

class BlinkerView : public BView
{
public:
                    BlinkerView(void);
    virtual void    Draw(BRect updateRect);
            void    SetOn(bool on = true);
private:
    BGradientLinear         fGradientOn;
    BGradientLinear         fGradientOff;
    bool                    fIsOn;
    Debug                   out;
};

#endif
