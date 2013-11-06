/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef INTROWINDOW_H
#define INTROWINDOW_H

#include <GroupLayout.h>
#include <CardLayout.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <Window.h>

#include "Debug.h"

class IntroWindow : public BWindow
{
public:
                        IntroWindow(void);
    virtual void        MessageReceived(BMessage* message);
	enum{M_ONLINE = 'iob1', M_EXIT = 'ixb1', M_OFFLINE = 'iob2'};
private:
    BCardLayout*            fCardLayout;
    BGroupLayout*           fLoginLayout;

    BTextControl*           fServerTC;
    BTextControl*           fPortTC;
    BTextControl*           fUsernameTC;
    BTextControl*           fPasswordTC;

    BCheckBox*              fRemember;

    Debug                   out;
};


#endif
