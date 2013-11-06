/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSPlayersView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"

PlayersListView::PlayersListView(void)
    :
    BColumnListView("players_listview", 0)
{


}

ICSPlayersView::ICSPlayersView(void)
    :
    BGroupView("icstxtcontrol_view", B_VERTICAL, 0)
{
    fListView = new PlayersListView();
    fListView->AddColumn(new BTitledColumn("Name", 100, 0, 10000), 0);
    fListView->AddColumn(new BTitledColumn("Blitz", 50, 0, 10000), 1);
    fListView->AddColumn(new BTitledColumn("Bullet", 50, 0, 10000), 2);
    fListView->AddColumn(new BTitledColumn("Title", 50, 0, 10000), 3);
    fListView->AddColumn(new BTitledColumn("Status", 60, 0, 10000), 4);
    fListView->AddColumn(new BTitledColumn("Rank", 50, 0, 10000), 5);
    fListView->AddColumn(new BTitledColumn("Slow", 50, 0, 10000), 6);
    fListView->AddColumn(new BTitledColumn("Internet", 70, 0, 10000), 7);
    fListView->AddColumn(new BTitledColumn("Engine", 60, 0, 10000), 7);

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("Challenge", "star_green");
    toolBar->AddButton("Follow", "star_green");
    toolBar->AddButton("Picture", "star_green");
    toolBar->AddButton("Rating", "star_green");
    toolBar->AddButton("Ping", "star_green");

    BLayoutBuilder::Group<>(this)
        .Add(fListView)
        .Add(toolBar)
    ;
}


void
ICSPlayersView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSPlayersView::MessageReceived(BMessage* message)
{
    switch (message->what) {

        default:
            BGroupView::MessageReceived(message);
			break;
	}
}
