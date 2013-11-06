/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "NavigationView.h"

#include <LayoutBuilder.h>
#include <TranslationUtils.h>
#include <TranslatorFormats.h>

#include "ImageButton.h"
#include "Messages.h"
#include "Tools.h"

NavigationView::NavigationView(void)
	:
    BGroupView("navigation_view", B_VERTICAL, 0),
    fAutohiding(false)
{
	BLayoutBuilder::Group<>(this)
		.AddStrut(0)
		.AddGroup(B_HORIZONTAL, 0).GetLayout( &fMainGroup )
			.AddStrut(0)
			.AddGlue(0)
			.Add(new ImageButton("first", new BMessage(MSG_MOVEFIRST), 0.3))
			.Add(new ImageButton("back" , new BMessage(MSG_MOVEBACK ), 0.3))
			.Add(new ImageButton("next" , new BMessage(MSG_MOVENEXT ), 0.3))
			.Add(new ImageButton("last" , new BMessage(MSG_MOVELAST ), 0.3))
			.AddGlue(0)
			.AddStrut(0)
		.End()
		.AddStrut(0)
	;
}


void
NavigationView::MouseMoved(BPoint where, uint32 code,
									                const BMessage* dragMessage)
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
NavigationView::MakeFocus(bool focusState)
{
	if (fAutohiding) {
		if (focusState == false)
			fMainGroup->SetVisible(false);
	}

	BGroupView::MakeFocus(focusState);
}


void
NavigationView::SetAutohiding(bool autohiding)
{
	fAutohiding = autohiding;

	if (fAutohiding)
		fMainGroup->SetVisible(false);
	else
		fMainGroup->SetVisible(true);
}
