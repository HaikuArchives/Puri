/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSSEEKWINDOW_H
#define ICSSEEKWINDOW_H

#include <InterfaceKit.h>
#include <GroupLayout.h>
#include <Window.h>

#include "Debug.h"

class ICSSeekWindow : public BWindow
{
public:
                    ICSSeekWindow(BWindow* target);
    virtual void    MessageReceived(BMessage* message);
    virtual bool    QuitRequested(void);

    enum{	M_OK = 'iob1', M_CANCEL = 'icb1', M_HELP = 'ihb1',
         	M_BULLET = 'ibb1', M_FAST = 'ifb1', M_NORMAL = 'inb1',
         	M_SLOW = 'isb1', M_RATED = 'irb1'};
private:
            void    _LoadSettings(void);
            void    _SaveSettings(void);

    BWindow*                fTarget;

    BTextControl*           fTimeMin;
    BTextControl*           fTimeMax;
    BTextControl*           fIncMin;
    BTextControl*           fIncMax;

    BRadioButton*           fColorAutomatic;
    BRadioButton*           fColorWhite;
    BRadioButton*           fColorBlack;

    BCheckBox*              fRated;
    BCheckBox*              fWithClock;


    Debug                   out;
};


#endif
