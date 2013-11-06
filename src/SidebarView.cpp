/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "SidebarView.h"

#include <LayoutBuilder.h>

#include "ImageButton.h"
#include "Messages.h"

SidebarView::SidebarView(void)
	:
    BGroupView("sidebar_view", B_HORIZONTAL, 0),
    fAutohiding(false)
{
    ImageButton* movenowButton = new ImageButton("move_now",
        new BMessage(MENU_GAME_MOVENOW), 0.3);
    movenowButton->SetToolTip("Move Now");
    ImageButton* newgameButton = new ImageButton("new",
        new BMessage(MENU_GAME_NEWGAME), 0.3);
    newgameButton->SetToolTip("New Game");
    ImageButton* flipboardButton = new ImageButton("refresh",
        new BMessage(MENU_VIEW_FLIPBOARD), 0.3);
    flipboardButton->SetToolTip("Flip Board");
    ImageButton* fullscreenButton = new ImageButton("fullscreen",
                new BMessage(MENU_VIEW_FULLSCREEN), 0.3);
    fullscreenButton->SetToolTip("Fullscreen");
    ImageButton* view3DButton = new ImageButton("view_3d",
                new BMessage(MENU_VIEW_3D), 0.3);
    view3DButton->SetToolTip("3D View");
    ImageButton* hidemenuButton = new ImageButton("hide_menu",
                new BMessage(MENU_VIEW_SHOWMENU), 0.3);
    hidemenuButton->SetToolTip("Hide Menu");
    ImageButton* quitButton = new ImageButton("exit",
                new BMessage(MENU_APP_QUIT), 0.3);
    quitButton->SetToolTip("Quit");

    BLayoutBuilder::Group<>(this)
    	.AddStrut(0)
        .AddGroup(B_VERTICAL, 0).GetLayout( &fMainGroup )
        	.AddStrut(0)
            .AddGlue(0)
            .Add(movenowButton)
            .Add(newgameButton)
            .Add(fullscreenButton)
            .Add(view3DButton)
            .Add(flipboardButton)
            .Add(hidemenuButton)
            .Add(quitButton)
            .AddGlue(0)
            .AddStrut(0)
        .End()
        .AddStrut(0)
    ;
}


void
SidebarView::MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage)
{
	switch (code) {
		case B_EXITED_VIEW:
		//	fMainGroup->SetVisible(false);
			break;

		case B_ENTERED_VIEW:
			if (fAutohiding) {
				fMainGroup->SetVisible(true);
				MakeFocus(true);
			}
			break;

		default:
			break;
	}

}


void
SidebarView::MakeFocus(bool focusState)
{
	if (fAutohiding) {
		if (focusState == false)
			fMainGroup->SetVisible(false);
	}

	BGroupView::MakeFocus(focusState);
}


void
SidebarView::SetAutohiding(bool autohiding)
{
	fAutohiding = autohiding;

	if (fAutohiding)
		fMainGroup->SetVisible(false);
	else
		fMainGroup->SetVisible(true);
}
