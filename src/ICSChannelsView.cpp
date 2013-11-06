/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSChannelsView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"

ChannelsListView::ChannelsListView(void)
    :
    BColumnListView("channels_listview", 0)
{


}

ICSChannelsView::ICSChannelsView(void)
    :
    BGroupView("ics_channels_view", B_VERTICAL, 0)
{
    fListView = new ChannelsListView();
    fListView->AddColumn(new BTitledColumn("Name", 100, 0, 10000), 0);
    fListView->AddColumn(new BTitledColumn("Status", 70, 0, 10000), 1);
    fListView->AddColumn(new BTitledColumn("N People", 100, 0, 10000), 2);

    ToolBar* toolBar = new ToolBar;
    toolBar->AddButton("Join", "join");
    toolBar->AddButton("Leave", "leave");
    toolBar->AddButton("Talk", "speaker");
    toolBar->AddButton("Refresh", "swap");
    toolBar->AddButton("Who?", "find");
    toolBar->AddButton("New", "new");

    BLayoutBuilder::Group<>(this)
        .Add(fListView)
        .Add(toolBar)
    ;
}


void
ICSChannelsView::AttachedToWindow(void)
{

}


void
ICSChannelsView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
