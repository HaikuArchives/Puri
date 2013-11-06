/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSINBOXVIEW_H
#define ICSINBOXVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "ColumnListView.h"
#include "Debug.h"

class InboxListView : public BColumnListView
{
public:
                    InboxListView(void);

private:

};

class ICSInboxView : public BGroupView
{
public:
                    ICSInboxView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:
    InboxListView*      fListView;
    BTextView*          fTextView;

    Debug               out;
};


#endif // ICSINBOXVIEW_H
