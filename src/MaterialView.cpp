/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "MaterialView.h"

#include <algorithm>

#include <LayoutBuilder.h>

#include "Tools.h"

MaterialView::MaterialView(void)
	:
    BGroupView("material_view", B_VERTICAL, 0)
{
    fBMV = new BasicMaterialView();

    BLayoutBuilder::Group<>(this)
		.AddGroup(B_HORIZONTAL, 0)
			.Add(fBMV)
		.End()
    .End()
	;
}


BasicMaterialView::BasicMaterialView(void)
    :
    BView(BRect(), "basic_material_view", B_FOLLOW_ALL,
            B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE | B_FRAME_EVENTS ),
    fIR(BRect()),
    fMergedMode(false),
    fColumns(0)
{
    // init the array with NULL pointers;
    std::fill(&fBM[0], &fBM[12], static_cast< BBitmap* >(NULL));

    //TODO: Direct assignment should work, bug of gnu
    BString imageName[12] = {"wK", "wQ", "wR", "wB", "wN", "wP", "bK", "bQ",
                                                        "bR", "bB", "bN", "bP"};
    std::copy(&imageName[0], &imageName[12], &fImageName[0]);

    SetViewColor(B_TRANSPARENT_32_BIT);
    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    Invalidate();
}


void
BasicMaterialView::Clear(void)
{
    fWPieces.clear();
    fBPieces.clear();
    Invalidate();
}


void
BasicMaterialView::Draw(BRect updateRect)
{
    SetHighColor(255, 255, 255, 255);
    FillRect(Bounds());

    if (fMergedMode == false) {
        fIR.OffsetTo(-fIR.Width(), 0);

        for (uint i = 0; i < fWPieces.size(); ++i)
            DrawBitmapAsync(fBM[fWPieces[i]],
                                   fIR.OffsetBySelf(fIR.Width() , 0).LeftTop());

        fIR.OffsetTo(-fIR.Width(), Frame().bottom - fIR.Height());

        for (uint i = 0; i < fBPieces.size(); ++i)
            DrawBitmapAsync(fBM[ fBPieces[i]],
                                    fIR.OffsetBySelf(fIR.Width() ,0).LeftTop());
    } else {
        int counter = 0;

        fIR.OffsetTo( -fIR.Width(), 0);
        for (uint i = 0; i < fWPieces.size(); ++i) {
            DrawBitmapAsync(fBM[fWPieces[i]],
                                    fIR.OffsetBySelf(fIR.Width() ,0).LeftTop());
            ++counter;

            if (counter == fColumns)
                fIR.OffsetTo(-fIR.Width(), Frame().bottom - fIR.Height());
        }

        ++counter;
        fIR.OffsetBySelf(fIR.Width() ,0);

        for (uint i = 0; i < fBPieces.size(); ++i) {
            DrawBitmapAsync(fBM[fBPieces[i]],
                                    fIR.OffsetBySelf(fIR.Width() ,0).LeftTop());
            ++counter;

            if (counter == fColumns)
                fIR.OffsetTo(-fIR.Width(), Frame().bottom - fIR.Height());
        }
    }

    SetHighColor(0,  0,  0, 255);
    StrokeRect(Bounds());
    Sync();
    BView::Draw(updateRect);
}


void
BasicMaterialView::_LoadImages(void)
{
    for (int i = 0; i < 12; ++i) {
        delete fBM[i];
        fBM[i] = LoadBitmap(fImageName[i], fIR.Width());
    }
}


void
BasicMaterialView::FrameResized(float newWidth, float newHeight)
{
    if (fWPieces.size() == 0 && fBPieces.size() == 0)
        return;

    float max = std::max( fWPieces.size(), fBPieces.size() );
    fMergedMode = false;

    if (max < 3 || abs(fWPieces.size()-fBPieces.size()) < 2
            || newHeight/2 < newWidth/max)
        fColumns = max;
    else {
        fColumns = (fWPieces.size() + fBPieces.size())/2 + 1;

        if (static_cast<uint>(fColumns) != fWPieces.size())
            fMergedMode = true;
    }



    float width = newWidth / fColumns;
    width = std::min(width, newHeight/2);
    fIR = BRect(0, 0, width, width);
    _LoadImages();
}


void
BasicMaterialView::SetMaterial(BString const& str)
{
	Clear();

	for (int i = 0; i < str.Length(); ++i) {
        switch (str[i]) {
            case 'K':   fWPieces.push_back( wK );    break;
            case 'Q':   fWPieces.push_back( wQ );    break;
            case 'R':   fWPieces.push_back( wR );    break;
            case 'B':   fWPieces.push_back( wB );    break;
            case 'N':   fWPieces.push_back( wN );    break;
            case 'P':   fWPieces.push_back( wP );    break;

            case 'k':   fBPieces.push_back( bK );    break;
            case 'q':   fBPieces.push_back( bQ );    break;
            case 'r':   fBPieces.push_back( bR );    break;
            case 'b':   fBPieces.push_back( bB );    break;
            case 'n':   fBPieces.push_back( bN );    break;
            case 'p':   fBPieces.push_back( bP );    break;

            default: break;
        }
    }

    FrameResized(Bounds().Width(), Bounds().Height());
    Invalidate();
}

