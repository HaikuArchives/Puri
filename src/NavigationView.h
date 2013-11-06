/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef NAVIGATIONVIEW_H
#define NAVIGATIONVIEW_H

#include <GroupView.h>
#include <ListView.h>

class NavigationView : public BGroupView
{
public:
                    NavigationView(void);
    virtual void    MouseMoved(BPoint where, uint32 code,
                                const BMessage* dragMessage);
    virtual	void	MakeFocus(bool focusState = true);
            void	SetAutohiding(bool autohiding = true);
            bool	IsAutohiding(void)
            {
                return fAutohiding;
            }

private:
    BGroupLayout*	    fMainGroup;
    bool				fAutohiding;
};

#endif
