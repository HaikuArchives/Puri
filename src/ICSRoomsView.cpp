/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSRoomsView.h"

#include <Box.h>
#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "Messages.h"
#include "ToolBar.h"


RoomItem::RoomItem(const char* name)
	:
    BListItem()
{
	fName = strdup(name);
}


RoomItem::~RoomItem()
{
	delete fName;
}


void
RoomItem::DrawItem(BView *owner, BRect itemRect, bool complete)
{
	rgb_color kBlack = { 0,0,0,0 };
	rgb_color kHighlight = { 156,154,156,0 };

	if (IsSelected() || complete) {
		rgb_color color;
		if (IsSelected())
			color = kHighlight;
		else
			color = owner->ViewColor();

		owner->SetHighColor(color);
		owner->SetLowColor(color);
		owner->FillRect(itemRect);
		owner->SetHighColor(kBlack);

	} else {
		owner->SetLowColor(owner->ViewColor());
	}

	BFont font = be_plain_font;
	font_height	finfo;
	font.GetHeight(&finfo);

	BPoint point = BPoint(itemRect.left + 5, itemRect.bottom - finfo.descent + 1);

	owner->SetHighColor(kBlack);
	owner->SetFont(be_plain_font);
	owner->MovePenTo(point);

	owner->MovePenTo(point);
	owner->DrawString(fName);
}


RoomsListView::RoomsListView(void)
    :
    BOutlineListView("rooms_listview")
{

}

#include <List.h>
ICSRoomsView::ICSRoomsView(void)
    :
    BGroupView("icsroomsview", B_VERTICAL, 0)
{
    RoomItem* item1 = new RoomItem("Play and Watch");
   // item1->SetOutlineLevel(1);
    RoomItem* item2 = new RoomItem("Main Playing Hall");
    //item2->SetOutlineLevel(0);
    RoomItem* item3 = new RoomItem("Beginners");
    //item2->SetOutlineLevel(0);
    RoomItem* item4 = new RoomItem("Tournaments");
    RoomItem* item5 = new RoomItem("Computer Chess");


    fRoomsListView = new RoomsListView();
    fRoomsListView->AddItem(item1);
    fRoomsListView->AddUnder(item3,  item1);
    fRoomsListView->AddUnder(item2, item1);
    fRoomsListView->AddItem(item4);
    fRoomsListView->AddItem(item5);

    fScrollBar = new BScrollBar("rooms_scrollbar", fRoomsListView, 0, 100,
        B_VERTICAL);

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("Refresh", "swap");
    
    BGroupLayout* layout;
    layout = BLayoutBuilder::Group<>(B_VERTICAL, 0)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(fRoomsListView)
            .Add(fScrollBar)
        .End()
        .Add(toolBar)
    ;
    
    BBox* boxFrame = new BBox("box_frame");
    boxFrame->SetBorder(B_FANCY_BORDER);
    boxFrame->AddChild(layout->View());

    BLayoutBuilder::Group<>(this)
        .Add(boxFrame)
    ;
}


void
ICSRoomsView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSRoomsView::MessageReceived(BMessage* message)
{
    switch (message->what) {


		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
