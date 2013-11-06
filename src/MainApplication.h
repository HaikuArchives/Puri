/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MAINAPPLICATION_H
#define MAINAPPLICATION_H


#include <Application.h>

#include "IntroWindow.h"

class MainApplication : public BApplication
{
public:
    MainApplication(void)
        :
        BApplication("application/x-vnd.cipri.Puri")
    {

    }

    virtual void ReadyToRun(void)
    {
        fIntroWindow = new IntroWindow();
    }

private:
    IntroWindow*		fIntroWindow;
};

#endif
