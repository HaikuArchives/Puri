/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSGAMEWVIEW_H
#define ICSGAMEWVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include "ColumnListView.h"
#include "Debug.h"

class GameCategoryListView : public BColumnListView
{
public:
                    GameCategoryListView(void);

private:

};


class GamesListView : public BColumnListView
{
public:
                    GamesListView(void);

private:

};


class ICSGamesView : public BGroupView
{
public:
                    ICSGamesView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);
private:
    GameCategoryListView*   fCategoryListView;
    GamesListView*          fGamesListView;

    Debug           out;
};


#endif // ICSGAMEWVIEW_H
