/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSEventCalendarView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"

EventListView::EventListView(void)
    :
    BColumnListView("event_listview", 0)
{


}


ICSEventCalendarView::ICSEventCalendarView(void)
    :
    BGroupView("icseventcalendarview", B_VERTICAL, 0)
{
    fListView = new EventListView();
    fListView->AddColumn(new BTitledColumn("Event", 200, 0, 10000), 0);
    fListView->AddColumn(new BTitledColumn("Room", 100, 0, 10000), 1);
    fListView->AddColumn(new BTitledColumn("Type", 80, 0, 10000), 2);
    fListView->AddColumn(new BTitledColumn("Date", 100, 0, 10000), 3);
    fListView->AddColumn(new BTitledColumn("Time", 50, 0, 10000), 4);



    ToolBar* toolBar = new ToolBar;
    toolBar->AddButton("Visit Event", "move_now");

    BLayoutBuilder::Group<>(this)
        .Add(fListView)
        .Add(toolBar)
    ;
}


void
ICSEventCalendarView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSEventCalendarView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
