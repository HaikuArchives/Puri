
#ifndef TOOLS_H
#define	TOOLS_H

#include <vector>

#include <Bitmap.h>
#include <Looper.h>
#include <String.h>

#include "Messages.h"

BBitmap*                LoadBitmap(BString const& imageName, int size);
BString                 GetAppPath(void);
BString                 GetAppName(void);
BString                 PieceToStr(int piece);
std::vector<BString>    Split(BString str, char token = ' ', BString start = "",
                            BString stop = "");
std::vector<BString>    Split(unsigned int length, BString str, char token = ' ',
                            BString start = "", BString stop = "");
BString                 Find(BString str, BString searchStr);

void                    SendICS(BString str, BLooper* looper);

#endif
