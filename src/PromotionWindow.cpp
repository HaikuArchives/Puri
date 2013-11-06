/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "PromotionWindow.h"

#include <LayoutBuilder.h>

#include "ImageButton.h"
#include "Messages.h"

PromotionWindow::PromotionWindow(ChessBoardView* target)
	:
    BWindow(target->Frame(), "Please Choose", B_TITLED_WINDOW_LOOK,
                B_MODAL_APP_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS
                    | B_NOT_CLOSABLE
                    | B_AUTO_UPDATE_SIZE_LIMITS | B_OUTLINE_RESIZE
                    | B_NOT_RESIZABLE | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE ),
    fTarget(target)
{
    BRect frame = BRect(0, 0, fTarget->fIR.Width()*8, fTarget->fIR.Width()*8);

    frame.OffsetBySelf(fTarget->Window()->Frame().LeftTop());
    frame.OffsetBySelf(fTarget->Frame().LeftTop());
    frame.OffsetBySelf(fTarget->fBoard2dOffset);

    ImageButton* (button)[4];
    BString nameW[4] = {"wQ", "wR", "wB", "wN"};
    BString nameB[4] = {"bQ", "bR", "bB", "bN"};

    int     valW[4] = {Q_W, R_W, B_W, N_W};
    int     valB[4] = {Q_B, R_B, B_B, N_B};

    for (int i = 0; i < 4; ++i) {
        if (fTarget->IsWhiteTurn() == false)
            button[i] = new ImageButton(nameW[i], new BMessage(valW[i]), 0.1);
        else
            button[i] = new ImageButton(nameB[i], new BMessage(valB[i]), 0.1);

        button[i]->SetExplicitMinSize(BSize(frame.Width()/5, frame.Width()/5));
        button[i]->SetExplicitMaxSize(BSize(frame.Width()/5, frame.Width()/5));
    }
    //if (isWhite) name = ;

	BLayoutBuilder::Group<>(this, B_HORIZONTAL, 0)
		.Add(button[0]).Add(button[1]).Add(button[2]).Add(button[3])
	;

    CenterIn(frame);
    Show();
}


PromotionWindow::~PromotionWindow()
{

}


void PromotionWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
        case Q_W: case R_W: case B_W: case N_W:
        case Q_B: case R_B: case B_B: case N_B:
        {
            BMessage msg(MSG_PROMOTE);
            msg.AddInt32("val", message->what);
            fTarget->Window()->PostMessage(&msg, fTarget);
            Close();
            break;
        }

		default:
        {
            BMessage msg(MSG_PROMOTE);

            if (fTarget->IsWhiteTurn() == false)
                msg.AddInt32("val", Q_W);
            else
                msg.AddInt32("val", Q_B);

            fTarget->Window()->PostMessage(&msg, fTarget);
            Close();
            BWindow::MessageReceived(message);
            break;
        }
	}
}
