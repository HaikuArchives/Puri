/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSINFOVIEW_H
#define ICSINFOVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "Debug.h"

class ICSInfoView : public BGroupView
{
public:
                    ICSInfoView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:
    BTextView*      fTextView;

    Debug           out;
};


#endif // ICSINFOVIEW_H
