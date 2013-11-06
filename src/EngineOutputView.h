/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ENGINEOUTPUTVIEW_H
#define ENGINEOUTPUTVIEW_H

#include <Button.h>
#include <Box.h>
#include <GroupView.h>
#include <Locker.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <StringView.h>
#include <TextView.h>

#include "BackBoard.h"
#include "BlinkerView.h"
#include "Debug.h"


class EngineOutputView : public BGroupView
{
public:
                    EngineOutputView(BackBoard * board);
    virtual void    AllAttached(void);
    virtual void    MessageReceived(BMessage* message);

            void    AddText(BString str);
            void    SetOn(bool on = true)
            {
                fBlinker->SetOn(on);
            }

private:
            void    _ParseEngineOutput(BString & str);
            void    _ParseDepth(const char*& p);
            void    _ParseCurrentMove(const char*& p);
            void    _ParseNodes(const char*& p);

    BBox*           fEngineNameBox;
    BBox*           fBlinkerBox;
    BBox*           fEvaluationBox;
    BBox*           fDepthBox;
    BBox*           fCurrentMoveBox;
    BBox*           fNPSBox;
    BBox*           fTextBox;

    BStringView*    fEngineName;
    BStringView*    fEvaluation;
    BStringView*    fDepth;
    BStringView*    fCurrentMove;
    BStringView*    fNPS;

    BTextView*      fTextView;
    BScrollBar*     fVScrollBar;
    BScrollBar*     fHScrollBar;
    BButton*        fGoButton;
    BlinkerView*    fBlinker;

    BackBoard*      fBoard;
    BLocker         fLocker;
    BString         fRestString;

    bool            fClearTextView;
    Debug           out;
};


#endif // ENGINEOUTPUTVIEW_H
