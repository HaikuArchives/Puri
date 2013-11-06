/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSROOMSVIEW_H
#define ICSROOMSVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <ListItem.h>
#include <Message.h>
#include <OutlineListView.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "Debug.h"

class RoomItem : public BListItem
{
public:
    RoomItem(const char* name);
    ~RoomItem();
    virtual void DrawItem(BView* owner, BRect itemRect, bool = false);
private:
    char* fName;
};


class RoomsListView : public BOutlineListView
{
public:
                    RoomsListView(void);

private:

};


class ICSRoomsView : public BGroupView
{
public:
                    ICSRoomsView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:
    RoomsListView*      fRoomsListView;
    BScrollBar*         fScrollBar;

    Debug               out;
};


#endif // ICSROOMSVIEW_H
