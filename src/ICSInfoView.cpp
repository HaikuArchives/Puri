/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSInfoView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "Messages.h"
#include "ToolBar.h"

ICSInfoView::ICSInfoView(void)
    :
    BGroupView("icsinfoview", B_VERTICAL, 0)
{
    fTextView = new BTextView("info_textview");

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("Back", "seek_backward");
    toolBar->AddButton("Forward", "seek_forward");
    toolBar->AddButton("Stop", "playback_stop");
    toolBar->AddButton("Refresh", "swap");
    toolBar->AddButton("Home", "home");

    BLayoutBuilder::Group<>(this)
        .Add(fTextView)
        .Add(toolBar)
    ;
}


void
ICSInfoView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSInfoView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
