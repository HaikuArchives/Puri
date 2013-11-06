/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSOUTBOXVIEW_H
#define ICSOUTBOXVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "ColumnListView.h"
#include "Debug.h"

class OutboxListView : public BColumnListView
{
public:
                    OutboxListView(void);

private:

};

class ICSOutboxView : public BGroupView
{
public:
                    ICSOutboxView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:
    OutboxListView*      fListView;
    BTextView*          fTextView;

    Debug           out;
};


#endif // ICSOUTBOXVIEW_H
