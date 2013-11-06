/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef PROMOTIONWINDOW_H
#define PROMOTIONWINDOW_H

#include <Window.h>

#include "ChessBoardView.h"
#include "Debug.h"

class PromotionWindow : public BWindow
{
public:
                    PromotionWindow(ChessBoardView* target);
                    ~PromotionWindow();
    virtual void    MessageReceived(BMessage* message);

private:
    ChessBoardView*     fTarget;
    Debug               out;
};


#endif
