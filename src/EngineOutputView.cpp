/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "EngineOutputView.h"

#include <stdlib.h>
#include <vector>

#include "AutoLocker.h"
#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "FicsCodes.h"
#include "Messages.h"
#include "Tools.h"

using namespace std;

EngineOutputView::EngineOutputView(BackBoard * board)
    :
    BGroupView("engineoutput", B_VERTICAL, 2),
    fBoard(board),
    fClearTextView(false)
{
    BFont font;
    font.SetSize(11);
    font.SetFlags(Flags() | B_FORCE_ANTIALIASING);
    font.SetFace(B_CONDENSED_FACE /*| B_BOLD_FACE*/);
    SetFont(&font); //setting same font, for calculations

    rgb_color color = make_color(0, 220, 0, 255);

    fTextView = new BTextView("engine_analysis");
    fTextView->SetWordWrap(false);
    fTextView->MakeEditable(false);
   // fTextView->SetFontAndColor(&font, B_FONT_ALL, &color);
    fTextView->SetFontAndColor(&font);
    fTextView->SetText("");

    fVScrollBar = new BScrollBar("v_scrollbar",
                    fTextView, 0, 100, B_VERTICAL);
    fHScrollBar = new BScrollBar("h_scrollbar",
                    fTextView, 0, 100, B_HORIZONTAL);

    fEngineName = new BStringView("engine_name", " ");
    fEngineName->SetFont(&font);
    fEngineName->SetToolTip("Engine Name");
    font_height fontHeight;
    fEngineName->GetFontHeight(&fontHeight);
    float height = fontHeight.ascent + fontHeight.descent + fontHeight.leading;
    fEngineName->SetExplicitMinSize(BSize(StringWidth(" engine "), height + 4));
    fEngineNameBox = new BBox("engine_box");
    fEngineNameBox->SetBorder(B_PLAIN_BORDER);
    fEngineNameBox->AddChild(fEngineName);

    fBlinker = new BlinkerView();
    fBlinkerBox = new BBox("blinker_box");
    float boxHeight = height + 6;
    fBlinkerBox->SetExplicitMaxSize(BSize(boxHeight, boxHeight));
    fBlinkerBox->SetExplicitMinSize(BSize(boxHeight, boxHeight));
    fBlinkerBox->SetBorder(B_PLAIN_BORDER);
    fBlinkerBox->AddChild(fBlinker);
    fBlinkerBox->SetToolTip("Engine Activity");

    fGoButton = new BButton("Go", new BMessage(MENU_GAME_MOVENOW));
 //   fGoButton->SetFont(&font);

    fEvaluation = new BStringView("eval_sv", "");
    fEvaluation->SetFont(&font);
    fEvaluation->SetExplicitMinSize(BSize(StringWidth(" += (0.28) "), height + 4));
    fEvaluationBox = new BBox("evaluation_box");
    fEvaluationBox->SetBorder(B_PLAIN_BORDER);
    fEvaluationBox->AddChild(fEvaluation);
    fEvaluationBox->SetToolTip("Evaluation");

    fDepth = new BStringView("depth_sv", "");
    fDepth->SetFont(&font);
    fDepth->SetExplicitMinSize(BSize(StringWidth(" 16/24 "), height + 4));
    fDepthBox = new BBox("depth_box");
    fDepthBox->SetBorder(B_PLAIN_BORDER);
    fDepthBox->AddChild(fDepth);
    fDepthBox->SetToolTip("Depth");

    fCurrentMove = new BStringView("cmove_sv", "");
    fCurrentMove->SetFont(&font);
    fCurrentMove->SetExplicitMinSize(BSize(StringWidth(" 1.e2e4q+ "), height + 4));
    fCurrentMoveBox = new BBox("cmove_box");
    fCurrentMoveBox->SetBorder(B_PLAIN_BORDER);
    fCurrentMoveBox->AddChild(fCurrentMove);
    fCurrentMoveBox->SetToolTip("Current Analyzed Move");

    fNPS = new BStringView("nps_sv", "");
    fNPS->SetFont(&font);
    fNPS->SetExplicitMinSize(BSize(StringWidth(" 500 kN/s "), height + 4));
    fNPSBox = new BBox("nps_box");
    fNPSBox->SetBorder(B_PLAIN_BORDER);
    fNPSBox->AddChild(fNPS);
    fNPSBox->SetToolTip("Analyzed Knots per Second");

    BGroupLayout* textLayout;
    textLayout = BLayoutBuilder::Group<>(B_HORIZONTAL, 0)
        .AddGroup(B_VERTICAL, 0)
            .Add(fTextView)
            .Add(fHScrollBar)
        .End()
        .Add(fVScrollBar)
    ;

    textLayout->View()->SetViewColor(200, 200, 200, 255);

    fTextBox = new BBox("text_box");
    fTextBox->SetBorder(B_FANCY_BORDER);
    fTextBox->AddChild(textLayout->View());

    BLayoutBuilder::Group<>(this)
        .AddGroup(B_HORIZONTAL, 3, 1)
            .Add(fBlinkerBox)
            .Add(fEngineNameBox, 100)
            .Add(fGoButton)
            .AddGlue(0)
        .End()
        .AddGroup(B_HORIZONTAL, 2, 1)
            .Add(fEvaluationBox)
            .Add(fDepthBox)
            .Add(fCurrentMoveBox)
            .Add(fNPSBox)
            .AddGlue(0)
        .End()
        .Add(fTextBox)
    ;
}


void
EngineOutputView::AllAttached(void)
{
    fEngineNameBox->SetViewColor(200, 200, 200);
    fEngineName->SetViewColor(200, 200, 200);
    fBlinkerBox->SetViewColor(200, 200, 200);
    fBlinker->SetViewColor(200, 200, 200);
    fEvaluationBox->SetViewColor(200, 200, 200);
    fEvaluation->SetViewColor(200, 200, 200);
    fDepthBox->SetViewColor(200, 200, 200);
    fDepth->SetViewColor(200, 200, 200);
    fCurrentMoveBox->SetViewColor(200, 200, 200);
    fCurrentMove->SetViewColor(200, 200, 200);
    fNPSBox->SetViewColor(200, 200, 200);
    fNPS->SetViewColor(200, 200, 200);
    fTextView->SetViewColor(200, 200, 200);
    fTextBox->SetViewColor(200, 200, 200);
    fGoButton->SetExplicitMinSize(BSize(0, 0));

    BGroupView::AllAttached();
}


void
EngineOutputView::AddText(BString str)
{
    if (str == "")
        return;

    if (fClearTextView) {
        fClearTextView = false;
        fTextView->SetText("");
    }

    str.Append("\n\n");
    float min, max;
    AutoLocker<BLocker> autolock(fLocker);

    fVScrollBar->GetRange(&min, &max);

    if (fVScrollBar->Value() == max) {
        fTextView->Insert(fTextView->TextLength(), str, str.Length());
        fVScrollBar->GetRange(&min, &max);
        fVScrollBar->SetValue(max);
    }
    else {
        fTextView->Insert(fTextView->TextLength(), str, str.Length());
    }

}


void
EngineOutputView::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case REPLYCODE_ENGINE:
        {
            BString str;
            message->FindString("info", &str);
            _ParseEngineOutput(str);
            break;
        }

		default:
            BGroupView::MessageReceived(message);
			break;
	}
}


void
EngineOutputView::_ParseEngineOutput(BString & str)
{
    vector<BString> v = Split(str, '\n');

    if (fRestString.Length() != 0) {
        v[0].Prepend(fRestString);
        fRestString = "";
    }

    if (str[str.Length() - 1] != '\n') {
        fRestString = v.back();
        v.pop_back();
    }

    const char* p;
    vector<BString>::iterator line = v.begin();
    for ( ;line != v.end(); ++line) {
      //  !out<< *line << endl;

        p = line->LockBuffer(65356);

        if (strncmp(p, "info ", 5) == 0) {
            SetOn();
            p += 5;
            if (strncmp(p, "depth ", 6) == 0) {
                p += 6;
                _ParseDepth(p);
            } else if (strncmp(p, "currmove ", 9) == 0) {
                p += 9;
                _ParseCurrentMove(p);
            } else if (strncmp(p, "nodes ", 6) == 0) {
                p += 6;
                _ParseNodes(p);
            }
        } else if (strncmp(p, "id name ", 8) == 0) {
            p += 8;
            fEngineName->SetText(BString(p).Prepend(" ").Append(" "));
        } else if (strncmp(p, "bestmove ", 9) == 0) {
            p += 9;
            fClearTextView = true;
            vector<BString> v = Split(p);
            BMessage msg(MSG_ENGINEMOVE);
            msg.AddString("info", v[0]);
            Window()->PostMessage(&msg);
            SetOn(false);
        } else {

        }

        line->UnlockBuffer();
    }
}


void
EngineOutputView::_ParseDepth(const char*& p)
{
    vector<BString> v = Split(14, p);

    if (v[1] == "seldepth") {
        BString depth;
        depth << " " << v[0] << "/" << v[2] << " ";
        fDepth->SetText(depth);

        BString nps;
        nps << atoi(v[11]) / 1000 << " kN/s";
        fNPS->SetText(nps);

        BString evaluation;

        if (v[6] == "cp"){
            int cp = atoi(v[7]);
            if (fBoard->IsWhiteTurn() == false)
                cp  = -1 * cp;

            evaluation << " (" << ((float)cp) / 100 << ") ";
            fEvaluation->SetText(evaluation);
        } else if (v[6] == "mate") {
            int mateMoves = atoi(v[7]);
            if (fBoard->IsWhiteTurn() == false)
                mateMoves = -1 * mateMoves;

            evaluation << " # " << mateMoves << " ";
            fEvaluation->SetText(evaluation);
        }

        if (v[3] == "multipv") {
            const char* pv;

            if ((pv = strstr(p, " pv ")) != NULL) {
                pv += 3;
                AddText(pv);
            }
        }

    } else {
        fDepth->SetText(v[0].Prepend(" ").Append(" "));
    }

}


void
EngineOutputView::_ParseCurrentMove(const char*& p)
{
    vector<BString> v = Split(p);
    BString move;
    move << v[2] << ". " << v[0];
    fCurrentMove->SetText(move);
}


void
EngineOutputView::_ParseNodes(const char*& p)
{
    vector<BString> v = Split(p);
    BString nps;
    nps << atoi(v[2]) / 1000 << " kN/s";
    fNPS->SetText(nps);
}
