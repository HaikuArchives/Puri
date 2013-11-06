/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ImageButton.h"

#include <algorithm>

#include <Application.h>
#include <Point.h>
#include <Screen.h>
#include <Size.h>

#include "Tools.h"

ImageButton::ImageButton(BString imageName,BMessage *message,
                        float marginProportion, const char* name,  uint32 flags)
    :
    BButton(name, "", message, flags),
    fImageName(imageName),
    fBitmap(NULL),
    fMarginProportion(1 - marginProportion)
{
    SetExplicitMinSize(BSize(10, 10));
    SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNLIMITED));
   // SetExplicitPreferredSize(BSize(50, 30));

    SetViewColor(B_TRANSPARENT_32_BIT);
    SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
/*
    BScreen screen;

    monitor_info info;
    screen.GetMonitorInfo(&info);

    !out << "Width: " << info.width << "  Height " << info.height << std::endl;
    */
}


void
ImageButton::Draw(BRect updateRect)
{
    BButton::Draw(updateRect);
    DrawBitmapAsync(fBitmap, fImageRect.LeftTop());
    Sync();
}


void
ImageButton::FrameResized(float newWidth, float newHeight)
{
    BButton::FrameResized(newWidth, newHeight);

    int width = std::min(newWidth, newHeight) * fMarginProportion;
    fImageRect = BRect(0, 0, width, width);

    fImageRect.OffsetTo((newWidth - width)/2, (newHeight - width)/2);

    delete fBitmap;

    fBitmap = LoadBitmap(fImageName, fImageRect.Width());
    Invalidate();
}


BBitmap*
ImageButton::_RescaleBitmap(const BBitmap* src, int32 width, int32 height)
{
	if (!src || !src->IsValid())
		return NULL;

	BRect srcSize = src->Bounds();

	if (height < 0) {
		float srcProp = srcSize.Height()/srcSize.Width();
		height = (int32)(width * ceil(srcProp));
	}

	BBitmap* res = new BBitmap(BRect(0, 0, (float)width, (float)height),
                                                             src->ColorSpace());

	float dx = (srcSize.Width() + 1)/(float)(width + 1);
	float dy = (srcSize.Height() + 1)/(float)(height + 1);
	uint8 bpp = (uint8)(src->BytesPerRow()/ceil(srcSize.Width()));

	int srcYOff = src->BytesPerRow();
	int dstYOff = res->BytesPerRow();

	void* dstData = res->Bits();
	void* srcData = src->Bits();

	for (int32 y = 0; y <= height; y++) {
		void* dstRow = (void*)((uint32)dstData + (uint32)(y * dstYOff));
		void* srcRow = (void*)((uint32)srcData + ((uint32)(y * dy) * srcYOff));

		for (int32 x = 0; x <= width; x++)
			memcpy((void*)((uint32)dstRow + (x * bpp)), (void*)((uint32)srcRow
                                              + ((uint32)(x * dx) * bpp)), bpp);
	}

	return res;
}
