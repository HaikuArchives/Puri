/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSTabsToolBar.h"

#include <Button.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>

#include "ImageButton.h"
#include "Messages.h"

ICSTabsToolBar::ICSTabsToolBar(void)
	:
    BGroupView("icstabstoolbar", B_HORIZONTAL, 0),
    fAutohiding(false)
{
    // Home Tab
    BGroupLayout* homeLayout = BGroupLayoutBuilder(B_VERTICAL, 0)
        .AddGlue(0)
//        .Add(new ImageButton("star_green", NULL, 0.3))
        .AddGlue(0)
        .RootLayout()
    ;

    // Account Tab
    BGroupLayout* accoutLayout = BGroupLayoutBuilder(B_VERTICAL, 0)
        .AddGlue(0)
  //      .Add(new ImageButton("star_green", NULL, 0.3))
        .AddGlue(0)
        .RootLayout()
    ;

    // Home Tab
    BGroupLayout* communityLayout = BGroupLayoutBuilder(B_VERTICAL, 0)
        .AddGlue(0)
    //    .Add(new ImageButton("star_green", NULL, 0.3))
        .AddGlue(0)
        .RootLayout()
    ;

    // Home Tab
    BGroupLayout* viewLayout = BGroupLayoutBuilder(B_VERTICAL, 0)
        .AddGlue(0)
     //   .Add(new ImageButton("star_green", NULL, 0.3))
        .AddGlue(0)
        .RootLayout()
    ;

    fTabView = new BTabView("tab_view");
    fTabView->SetTabWidth(B_WIDTH_FROM_LABEL);
    fTabsVec.push_back(new BTab());
	fTabView->AddTab(homeLayout->View(), fTabsVec.back());
	fTabsVec.back()->SetLabel("Home");
    fTabsVec.push_back(new BTab());
	fTabView->AddTab(accoutLayout->View(), fTabsVec.back());
	fTabsVec.back()->SetLabel("Account");
    fTabsVec.push_back(new BTab());
	fTabView->AddTab(communityLayout->View(), fTabsVec.back());
	fTabsVec.back()->SetLabel("Community");
    fTabsVec.push_back(new BTab());
	fTabView->AddTab(viewLayout->View(), fTabsVec.back());
	fTabsVec.back()->SetLabel("View");

    BLayoutBuilder::Group<>(this)
        .Add(fTabView)
    ;
}


void
ICSTabsToolBar::AttachedToWindow(void)
{
   int size = fTabView->CountTabs();

   for (int i = 0; i < size; ++i)
    fTabView->ViewForTab(i)->SetViewColor(220,220,255,255);

    BGroupView::AttachedToWindow();
}

void
ICSTabsToolBar::MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage)
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
ICSTabsToolBar::MakeFocus(bool focusState)
{
	if (fAutohiding) {
		if (focusState == false)
			fMainGroup->SetVisible(false);
	}

	BGroupView::MakeFocus(focusState);
}


void
ICSTabsToolBar::SetAutohiding(bool autohiding)
{
	fAutohiding = autohiding;

	if (fAutohiding)
		fMainGroup->SetVisible(false);
	else
		fMainGroup->SetVisible(true);
}
