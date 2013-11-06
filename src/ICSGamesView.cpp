/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSGamesView.h"

#include <Button.h>
#include <LayoutBuilder.h>
#include <Window.h>

#include "ColumnTypes.h"
#include "Messages.h"
#include "ToolBar.h"


GameCategoryListView::GameCategoryListView(void)
    :
    BColumnListView("gamecategory_listview", 0)
{


}


GamesListView::GamesListView(void)
    :
    BColumnListView("games_listview", 0)
{


}


ICSGamesView::ICSGamesView(void)
    :
    BGroupView("icsgamesview", B_VERTICAL, 0)
{
    fCategoryListView = new GameCategoryListView();
    fCategoryListView->AddColumn(new BTitledColumn("Tournament", 100, 0, 1000), 0);
    fCategoryListView->AddColumn(new BTitledColumn("Round", 60, 0, 1000), 1);
    fCategoryListView->AddColumn(new BTitledColumn("Games", 60, 0, 1000), 2);
    fCategoryListView->AddColumn(new BTitledColumn("In progress", 70, 0, 1000), 3);
    fCategoryListView->AddColumn(new BTitledColumn("Kibitzers", 70, 0, 1000), 4);
    fCategoryListView->AddColumn(new BTitledColumn("Result", 55, 0, 1000), 5);
    fCategoryListView->AddColumn(new BTitledColumn("Chairman", 70, 0, 1000), 6);

    fGamesListView = new GamesListView();
    fGamesListView->AddColumn(new BTitledColumn("White", 80, 0, 1000), 0);
    fGamesListView->AddColumn(new BTitledColumn("Elo", 35, 0, 1000), 1);
    fGamesListView->AddColumn(new BTitledColumn("Black", 80, 0, 1000), 2);
    fGamesListView->AddColumn(new BTitledColumn("Elo", 35, 0, 1000), 3);
    fGamesListView->AddColumn(new BTitledColumn("Result", 55, 0, 1000), 4);
    fGamesListView->AddColumn(new BTitledColumn("Kibitzers", 60, 0, 1000), 5);
    fGamesListView->AddColumn(new BTitledColumn("Tournament", 80, 0, 1000), 6);
    fGamesListView->AddColumn(new BTitledColumn("Time control", 60, 0, 1000), 7);
    fGamesListView->AddColumn(new BTitledColumn("Start time", 70, 0, 1000), 8);
    fGamesListView->AddColumn(new BTitledColumn("Type", 50, 0, 1000), 9);
    fGamesListView->AddColumn(new BTitledColumn("Round", 50, 0, 1000), 10);
    fGamesListView->AddColumn(new BTitledColumn("Moves", 50, 0, 1000), 10);

    ToolBar* toolBar = new ToolBar();
    toolBar->AddButton("Watch", "star_green");

    BLayoutBuilder::Group<>(this)
        .AddSplit(B_VERTICAL, 0)
            .Add(fCategoryListView, 1)
            .Add(fGamesListView, 2)
        .End()
        .Add(toolBar)
    ;
}


void
ICSGamesView::AttachedToWindow(void)
{
	BGroupView::AttachedToWindow();
}


void
ICSGamesView::MessageReceived(BMessage* message)
{
    switch (message->what) {

		default:
            BGroupView::MessageReceived(message);
			break;
	}
}
