/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "IntroWindow.h"

#include <Alert.h>
#include <Button.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <MenuItem.h>
#include <Application.h>
#include <Screen.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <SplitLayoutBuilder.h>
#include <SplitView.h>
#include <StringView.h>
#include <View.h>

#include "BoardWindow.h"
#include "Settings.h"
#include "ICS.h"
#include "ICSMainWindow.h"
#include "Messages.h"

IntroWindow::IntroWindow(void)
    :
    BWindow(BRect(), "Plese Choose",
            B_NO_BORDER_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
            B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS)
{
    fServerTC   = new BTextControl("Server:           ", "freechess.org", NULL);
    fPortTC     = new BTextControl("Port:", "5000", NULL);
    fUsernameTC = new BTextControl("", "", NULL);
    fPasswordTC = new BTextControl("", "", NULL);
    fPasswordTC->TextView()->HideTyping(true);
    fRemember = new BCheckBox("Remember");
    BButton* loginButton = new BButton("Play Online", new BMessage(M_ONLINE));
    loginButton->SetFontSize(20);

    BButton* offlineButton = new BButton("Play Offline", new BMessage(M_OFFLINE));
    offlineButton->SetFontSize(20);
    BButton* exitButton = new BButton("Exit", new BMessage(M_EXIT));
    exitButton->SetFontSize(20);

    fLoginLayout = BGroupLayoutBuilder(B_VERTICAL, 0)
        .SetInsets(10, 10, 10, 10)
    //    .AddGlue(0)
        .AddGroup(B_HORIZONTAL, 0, 1)
            .Add(fServerTC, 5)
            .AddStrut(10)
            .Add(fPortTC, 1)
        .End()
        .AddStrut(5)
        .Add(new BSeparatorView(B_HORIZONTAL), 0)
        .AddStrut(5)
        .AddGroup(B_HORIZONTAL, 0, 4)
            .AddGroup(B_VERTICAL, 5, 1)
                .Add(new BStringView("username_sv", "Username "))
                .Add(new BStringView("password_sv", "Password "))
            .End()
            .AddStrut(23)
            .AddGroup(B_VERTICAL, 5, 4)
                .Add(fUsernameTC)
                .Add(fPasswordTC)
            .End()
        .End()
        .Add(fRemember)
        .AddStrut(5)
        .Add(new BSeparatorView(B_HORIZONTAL), 0)
        .AddStrut(30)
        .AddGroup(B_HORIZONTAL, 0, 4)
            .Add(loginButton)
           // .AddStrut(10)
       // .Add(new BSeparatorView(B_HORIZONTAL), 20)
           // .AddStrut(10)
            .Add(offlineButton)
            .Add(exitButton)
        .End()
        .AddGlue(0)
      //  .RootLayout()
	;

    fCardLayout = new BCardLayout();

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .Add(fCardLayout)
    ;

    fCardLayout->AddItem(fLoginLayout);
    fCardLayout->SetVisibleItem(fLoginLayout);


    BString user = "", password = "";
    int32   value = 0;
    Settings settings("IntroWnd");
    settings << "user" >> user << "pass" >> password << "remember" >> value;
    fUsernameTC->SetText(user);
    fPasswordTC->SetText(password);
    fRemember->SetValue(value);
    if (BString(fUsernameTC->Text()).Length() == 0)
        fUsernameTC->MakeFocus();
    else if (BString(fPasswordTC->Text()).Length() == 0)
        fPasswordTC->MakeFocus();
    else
        loginButton->MakeFocus();

    SetDefaultButton(loginButton);
    CenterOnScreen();
    Show();
}


void IntroWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
        case M_ONLINE:
        {
            if (BString(fServerTC->Text()).IsEmpty()
                | BString(fServerTC->Text()).IsEmpty()
                | BString(fUsernameTC->Text()).IsEmpty()
                | BString(fPasswordTC->Text()).IsEmpty())
                    return;
            ICSInfo info;
            info.server = fServerTC->Text();
            info.port   = atoi(fPortTC->Text());
            info.user   = fUsernameTC->Text();
            info.pass   = fPasswordTC->Text();

            Hide();
            new ICSMainWindow(info);
            if (fRemember->Value() == 0)
                info.pass = "";

            Settings settings("IntroWnd");
            settings << "user" << info.user << "pass" << info.pass
                << "remember" << fRemember->Value() << std::endl;
            Close();
            break;
        }

        case M_EXIT:
            be_app->PostMessage(B_QUIT_REQUESTED);
            break;

        case M_OFFLINE:
            Hide();
            new BoardWindow();
            Close();
            break;

		default:
			BWindow::MessageReceived(message);
			break;
	}
}
