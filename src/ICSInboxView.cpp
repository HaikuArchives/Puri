/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSInboxView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"

InboxListView::InboxListView(void)
    :
    BColumnListView("inbox_listview", 0)
{


}


ICSInboxView::ICSInboxView(void)
    :
    BGroupView("ics_inbox_view", B_VERTICAL, 0)
{
    fListView = new InboxListView();
    fListView->AddColumn(new BTitledColumn("Subject", 100, 0, 10000), 0);
    fListView->AddColumn(new BTitledColumn("From", 70, 0, 10000), 1);
    fListView->AddColumn(new BTitledColumn("To", 70, 0, 10000), 2);
    fListView->AddColumn(new BTitledColumn("Time", 50, 0, 10000), 3);
    fListView->AddColumn(new BTitledColumn("Size", 50, 0, 10000), 4);

    fTextView = new BTextView("inbox_textview");

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("New", "new");
    toolBar->AddButton("Delete", "quit");
    toolBar->AddButton("Reply", "mail_reply");

    BLayoutBuilder::Group<>(this)
        .AddSplit(B_VERTICAL, 0, 3)
            .Add(fListView)
            .Add(fTextView)
        .End()
        .Add(toolBar)
    ;
}


void
ICSInboxView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSInboxView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
