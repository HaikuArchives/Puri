/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "Tools.h"

#include <Application.h>
#include <IconUtils.h>
#include <Path.h>
#include <Resources.h>
#include <Roster.h>
#include <Window.h>

#include "BackPiece.h"
#include "ICS.h"
#include "Messages.h"


BString
GetAppPath(void)
{
    app_info	info;
    be_app->GetAppInfo(&info);

    BPath path = BPath(&info.ref);
    path.GetParent(&path);

    return path.Path();
}


BString
GetAppName(void)
{
    app_info	info;
    be_app->GetAppInfo(&info);

    BPath path = BPath(&info.ref);
    path.GetParent(&path);

    return path.Leaf();
}


std::vector<BString>
Split(BString str, const char token, BString start, BString stop)
{
    std::vector<BString> vec;

    int n1 = 0;
    int n2 = str.Length();

    if (start != "")
        n1 = str.FindFirst(start, 0);

    if (stop != "")
        n2 = str.FindFirst(stop, n1 + 1);

    if (n1 == B_ERROR || n2 == B_ERROR)
        return vec;

    n1 = n1 + start.Length();

    int n = n1;

    for (; n < n2; ++n)
        if (str[n] == token) {
            if (n > 0 && str[n-1] != token) {
                char temp[n - n1 + 1];
                str.CopyInto(temp, n1, n - n1);
                temp[n - n1] = '\0';
                vec.push_back(temp);
                n1 = n + 1;
            } else {
                n1 = n + 1;
            }
        }

    char temp[n - n1 + 1] ;
    str.CopyInto(temp, n1, n - n1);
    temp[n - n1] = '\0';
    vec.push_back(temp);
    return vec;
}


std::vector<BString>
Split(unsigned int length, BString str,
    const char token, BString start, BString stop)
{
    std::vector<BString> vec;

    int n1 = 0;
    int n2 = str.Length();

    if (start != "")
        n1 = str.FindFirst(start, 0);

    if (stop != "")
        n2 = str.FindFirst(stop, n1 + 1);

    if (n1 == B_ERROR || n2 == B_ERROR) {
        vec.resize(length, "");
        return vec;
    }

    n1 = n1 + start.Length();

    int n = n1;

    for (; n < n2; ++n)
        if (str[n] == token) {
            if (n > 0 && str[n-1] != token) {
                char temp[n - n1 + 1];
                str.CopyInto(temp, n1, n - n1);
                temp[n - n1] = '\0';
                vec.push_back(temp);
                if (vec.size() == length)
                    return vec;

                n1 = n + 1;
            } else {
                n1 = n + 1;
            }
        }

    char temp[n - n1 + 1] ;
    str.CopyInto(temp, n1, n - n1);
    temp[n - n1] = '\0';
    vec.push_back(temp);
    vec.resize(length, "");

    return vec;
}


BString
Find(BString str, BString searchStr)
{
   BString result = "";

  if (str.Length() == 0 || searchStr.Length() == 0)
    return result;

    int n1 = str.FindFirst(searchStr) + searchStr.Length();
    int n2 = str.FindFirst(" ", n1);

    if (n2 == B_ERROR)
        n2 = str.Length();

    str.CopyInto(result, n1, n2 - n1);

    return result;
}


BBitmap*
LoadBitmap(BString const& imageName, int size)
{
    BResources* res = be_app->AppResources();

	if (res == NULL)
		return NULL;

	size_t nbytes = 0;
	color_space cspace = B_RGBA32;

	const void* data = res->LoadResource('HVIF', imageName.String(), &nbytes);

   // size--;

	BBitmap* bitmap = new BBitmap(BRect(0, 0, size, size), cspace);

	if (bitmap->InitCheck() != B_OK) {
        delete bitmap;
        bitmap = NULL;
    } else if (BIconUtils::GetVectorIcon((const uint8*)data, nbytes, bitmap)
                 != B_OK) {
        delete bitmap;
        bitmap = NULL;
    }

    res->RemoveResource(data);
	return bitmap;
}


BString
PieceToStr(int piece)
{
    BString str = "";

    switch (piece) {
        case K_W:   str = "K";  break;
        case Q_W:   str = "Q";  break;
        case R_W:   str = "R";  break;
        case B_W:   str = "B";  break;
        case N_W:   str = "N";  break;
        case P_W:   str = "P";  break;
        case K_B:   str = "k";  break;
        case Q_B:   str = "q";  break;
        case R_B:   str = "r";  break;
        case B_B:   str = "b";  break;
        case N_B:   str = "n";  break;
        case P_B:   str = "p";  break;
        default:                break;
    }

    return str;
}


void
SendICS(BString str, BLooper* looper)
{
    BMessage msg(MSG_SEND_ICS_CMD);
    msg.AddString("info", str);
    looper->PostMessage(&msg);
}
