/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSPLAYERSVIEW_H
#define ICSPLAYERSVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "ColumnListView.h"
#include "Debug.h"

class PlayersListView : public BColumnListView
{
public:
                    PlayersListView(void);

private:

};

class ICSPlayersView : public BGroupView
{
public:
                    ICSPlayersView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);

private:
    PlayersListView*    fListView;

    Debug               out;
};


#endif // ICSPLAYERSVIEW_H
