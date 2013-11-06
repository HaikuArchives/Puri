/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSEVENTCALENDARVIEW_H
#define ICSEVENTCALENDARVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "ColumnListView.h"
#include "Debug.h"

class EventListView : public BColumnListView
{
public:
                    EventListView(void);

private:

};

class ICSEventCalendarView : public BGroupView
{
public:
                    ICSEventCalendarView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:

    EventListView*      fListView;

    Debug               out;
};


#endif // ICSEVENTCALENDARVIEW_H
