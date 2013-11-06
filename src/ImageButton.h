/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <Button.h>
#include <Bitmap.h>
#include <Mime.h>
#include <Rect.h>
#include <Resources.h>
#include <String.h>

#include "Debug.h"

class ImageButton : public BButton
{
public:
                    ImageButton(BString imageName, BMessage *message = NULL,
                                    float marginProportion = 0,
                                    const char* name = "im_button",
                                    uint32 flags = B_FRAME_EVENTS | B_WILL_DRAW
                                        | B_NAVIGABLE |B_FULL_UPDATE_ON_RESIZE);

    virtual void    Draw(BRect updateRect);
    virtual	void    FrameResized(float newWidth, float newHeight);

private:
    BBitmap*        _RescaleBitmap(const BBitmap* src, int32 width,
                                                                  int32 height);
    BString         fImageName;
    BBitmap*        fBitmap;
    BRect           fImageRect;
    float           fMarginProportion;

    Debug           out;
};

#endif
