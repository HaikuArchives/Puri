
/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MATERIALVIEW_H
#define MATERIALVIEW_H

#include <vector>

#include <GroupView.h>
#include <String.h>

#include "Debug.h"

class BasicMaterialView : public BView
{
public:
                    BasicMaterialView(void);

    void            Clear(void);
    void            SetMaterial( BString const& str );

    virtual void    Draw(BRect updateRect);
    virtual	void    FrameResized(float newWidth, float newHeight);

    friend class MaterialView;

    enum {wK = 0, wQ, wR, wB, wN, wP, bK, bQ, bR, bB, bN, bP};

private:
    void	            _LoadImages(void);

    BRect               fIR;
    BBitmap*            fBM[12];
    BString             fImageName[12];
    std::vector<int>    fWPieces;
    std::vector<int>    fBPieces;
    bool                fMergedMode;
    int                 fColumns;

    Debug               out;
};


class MaterialView : public BGroupView
{
public:
            MaterialView(void);

    void    Clear(void)
    {
        fBMV->Clear();
    }

    void    SetMaterial(BString const& str)
    {
        fBMV->SetMaterial(str);
    }

private:
    BasicMaterialView*  fBMV;
};

#endif
