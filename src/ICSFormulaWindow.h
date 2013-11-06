/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSFORMULAWINDOW_H
#define ICSFORMULAWINDOW_H

#include <InterfaceKit.h>
#include <GroupLayout.h>
#include <Window.h>

#include "Debug.h"

class ICSFormulaWindow : public BWindow
{
public:
                    ICSFormulaWindow(BWindow* target);
    virtual void    MessageReceived(BMessage* message);
    virtual bool    QuitRequested(void);
    
    enum{M_OK = 'iob1', M_HELP = 'ihb1', M_CANCEL = 'icb1'};
private:
            void    _Apply(void);

    BWindow*                fTarget;

    BCheckBox*              fUnrated;
    BCheckBox*              fRated;
    BCheckBox*              fActivate;
    BTextControl*           fEloMin;
    BTextControl*           fEloMax;
    BTextControl*           fTimeMin;
    BTextControl*           fTimeMax;
    BTextControl*           fIncMin;
    BTextControl*           fIncMax;

    Debug                   out;
};


#endif
