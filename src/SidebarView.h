/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

#include <GroupView.h>

class SidebarView : public BGroupView
{
public:
                    SidebarView(void);
    virtual void 	MouseMoved(BPoint where, uint32 code,
                                const BMessage* dragMessage);
    virtual	void	MakeFocus(bool focusState = true);
            bool	IsAutohiding(void)	{ return fAutohiding; }
            void	SetAutohiding(bool autohiding = true);

private:
    BGroupLayout*	fMainGroup;
    bool			fAutohiding;
};

#endif // SIDEBARVIEW_H
