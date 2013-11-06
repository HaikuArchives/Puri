/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSTABLSTOOLBAR_H
#define ICSTABLSTOOLBAR_H

#include <vector>

#include <GroupView.h>
#include <TabView.h>

class ICSTabsToolBar : public BGroupView
{
public:
                    ICSTabsToolBar(void);
    virtual void 	MouseMoved(BPoint where, uint32 code,
                                const BMessage* dragMessage);
    virtual void    AttachedToWindow(void);
    virtual	void	MakeFocus(bool focusState = true);
   /* virtual	void	SetLowColor(rgb_color a_color)
   {
        BGroupView::SetLowColor(200,0,0,255);

    }*/
            void	SetAutohiding(bool autohiding = true);
            bool	IsAutohiding(void)	{ return fAutohiding; }

private:
    BTabView*               fTabView;
    std::vector<BTab*>      fTabsVec;

    BGroupLayout*	fMainGroup;
    bool			fAutohiding;
};

#endif // ICSTABLSTOOLBAR_H
