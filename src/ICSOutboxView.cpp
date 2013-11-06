/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSOutboxView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"

OutboxListView::OutboxListView(void)
    :
    BColumnListView("outbox_listview", 0)
{


}

ICSOutboxView::ICSOutboxView(void)
    :
    BGroupView("ics_outbox_view", B_VERTICAL, 0)
{
    fListView = new OutboxListView();
    fListView->AddColumn(new BTitledColumn("Subject", 100, 0, 10000), 0);
    fListView->AddColumn(new BTitledColumn("From", 70, 0, 10000), 1);
    fListView->AddColumn(new BTitledColumn("To", 70, 0, 10000), 2);
    fListView->AddColumn(new BTitledColumn("Time", 50, 0, 10000), 3);
    fListView->AddColumn(new BTitledColumn("Size", 50, 0, 10000), 4);

    fTextView = new BTextView("inbox_textview");

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("New", "new");
    toolBar->AddButton("Delete", "quit");

    BLayoutBuilder::Group<>(this)
        .AddSplit(B_VERTICAL, 0, 3)
            .Add(fListView)
            .Add(fTextView)
        .End()
        .Add(toolBar)
    ;
}


void
ICSOutboxView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSOutboxView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
