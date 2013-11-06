/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSCHATVIEW_H
#define ICSCHATVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Locker.h>
#include <Message.h>
#include <OptionPopUp.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "Debug.h"


class ICSChatView : public BGroupView
{
public:
                    ICSChatView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

            void    AddText(BString str);
            void    AddTellTarget(BString str);

    enum{   M_SEND_BUTTON = 'isb1'  };

private:
    BTextView*      fTextView;
    BTextControl*   fTextControl;
    BOptionPopUp*   fChannel;
    BButton*        fSendButton;

    BScrollBar*     fScrollBar;

    BLocker         fLocker;

    Debug           out;
};


#endif // ICSCHATVIEW_H
