/*
**
**	$Id: WndLogin.cpp,v 1.2 1998/04/19 15:27:58 bobak Exp bobak $
**	$Revision: 1.2 $
**	$Filename: WndLogin.cpp $
**	$Date: 1998/04/19 15:27:58 $
**
**	Author: Andreas F. Bobak (bobak@abstrakt.ch)
**  Modified by: Christopher J. Plymire (chrisjp@eudoramail.com)
**
**	Copyright (C) 1998 by Abstrakt SEC, Andreas F. Bobak.
**
**	This is free software; you can redistribute it and / or modify it 
**	under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
**
**	This software is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY;  without even the  implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
**	General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this software; if not, write to the Free Software
**	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
**	USA.
**
*/
#include <Message.h>
#include <Application.h>
#include <Message.h>
#include <stdio.h>
#include <MessageFilter.h>
#include <InterfaceKit.h>
// XXX: check
//#include <Box.h>
//#include <Button.h>
//#include <Screen.h>

#include <assert.h>

#include "OptionsDialog.h"
#include "Utility.h"
#include "App.h"
#include "WndLogin.h"

#define	WND_SPACE	10		// empty space between widgets and window frame

/*****************************************************************************
**	App
*/

/*
**	constants
*/
const char*	WndLogin::title		= "VNCviewer";	// window title
const float WndLogin::width		= 234;			// window width
const float WndLogin::height	= 105;			// window height

/****
**	@purpose	Constructs a new WndLogin instance.
**	@param		frame	Rectangle defining the initial size of the window.
*/
WndLogin::WndLogin( BRect frame )
	: BWindow(frame, title, B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
#if 0 // Use BMessage constructor
	/*
	**	create user interface elements
	*/
	static const float divider = 60;
	BRect rect;

	// background
	rect = Bounds();
	rect.InsetBy( -1, -1 );
	BBox* box = new BBox( rect );
	AddChild( box );

	// text field: Server
	rect.Set( WND_SPACE, WND_SPACE, width - WND_SPACE, 25 );
	myTxtCtrlHostname = AddTextCtrl( box, rect, "hostname", "Hostname:", msg_hostname, divider );
	myTxtCtrlHostname->MakeFocus();

	// text field: Password
	rect.OffsetBy( 0, 25 );
	myPassCtrlPassword = (PassControl*)AddTextCtrl( box, rect, "passwd", "Password:", msg_passwd, divider, true );

	// button: OK (=Connect)
	rect.Set( width - 70 - WND_SPACE, height - 25 - WND_SPACE, width - WND_SPACE, height - 5 - WND_SPACE );
	myBtnOK = AddButton( box, rect, "ok", "Connect", msg_ok );
	myBtnOK->MakeDefault( true );
	myBtnOK->SetEnabled( false );

	// button: Cancel
	rect.Set( width - 70*2 - WND_SPACE*2, height - 25 - WND_SPACE, width - 70 - WND_SPACE*2, height - 5 - WND_SPACE  );
	myBtnCancel = AddButton( box, rect, "cancel", "Cancel", msg_cancel );
	
	// 'open' window
	Show();
	#endif
}

/*
**	@purpose	Constructs a new WndLogin instance.
**	@param		frame	Rectangle defining the initial size of the window.
*/

WndLogin::WndLogin( BMessage* archive )
	: BWindow(archive)
{
	// Set the focus to our "hostname" box
	BView* textBox = FindView("serverText");
	if( textBox )
		textBox->MakeFocus(true);

	BRect	rectScreen = BScreen( B_MAIN_SCREEN_ID ).Frame();
 	BRect	rect;
 	
 	int width = (int)Frame().Width();
	int height = (int)Frame().Height();
	
	// center window
 	rect.SetLeftTop( BPoint( (rectScreen.Width() - width) / 2, (rectScreen.Height() - height) / 2 ) );
 	rect.SetRightBottom( BPoint( rect.left + width, rect.top + height ) );
	MoveTo( rect.left , rect.top );

	// 'open' window
	Show();	
}

/****
**	@purpose	Destroys an existing WndLogin instance.
*/
WndLogin::~WndLogin( void )
{
//	printf( "***destruct WndLogin\n" );
}

#define HOST_SELECT_MESSAGE 'hsel'

/****
**	@purpose	Creates and shows a WndLogin window.
**	@result		A pointer to the newly instantiated WndLogin window or NULL, if failed.
*/
WndLogin*
WndLogin::Create( void )
{
	BRect	rectScreen = BScreen( B_MAIN_SCREEN_ID ).Frame();
 	BRect	rect;


	BMessage archive;
	if( RehydrateWindow("ConnectionWindow" , &archive) )
	{
		WndLogin* pLogin = new WndLogin( &archive );
		if( pLogin )
		{
			pLogin->Lock();
			
			// Load our 'mru' items.
			int nIndex = 0;
			const char* ConnectionItem;
			BMenuField* pField = (BMenuField *)pLogin->FindView("hostList");
			
			while( (ConnectionItem = App::GetApp()->GetOptions()->GetConnectionItem(nIndex) ) )
			{
				BMessage* connectMessage = new BMessage(HOST_SELECT_MESSAGE);
				connectMessage->AddString("host" , ConnectionItem );
				
				BMenuItem* newItem = new BMenuItem( ConnectionItem , connectMessage );
				pField->Menu()->AddItem ( newItem );
				nIndex++;
			}
			
			if( nIndex > 0 )
			{					
				BTextControl *serverText = (BTextControl *)pLogin->FindView("serverText");
				serverText->SetText(pField->Menu()->ItemAt( pField->Menu()->CountItems() - 1)->Label() );
				serverText->TextView()->SelectAll();
			}
			
			pLogin->Unlock();
			return pLogin;
		}
	}
	
	// center window
// 	rect.SetLeftTop( BPoint( (rectScreen.Width() - width) / 2, (rectScreen.Height() - height) / 2 ) );
// 	rect.SetRightBottom( BPoint( rect.left + width, rect.top + height ) );

	return new WndLogin( rect );
}

/****
**	@purpose	Adds a BTextControl child to a given view.
**	@param		parent	A pointer to the BView to which to add the control. 
**						If <parent> is NULL, the control is added directly to the window.
**	@param		rect	Passed to the control constructor. See the BeBook.
**	@param		id		Passed to the control constructor. See the BeBook.
**	@param		label	Passed to the control constructor. See the BeBook.
**	@param		mesg	Passed to the control constructor. See the BeBook.
**	@param		divider	Passed to the control constructor. See the BeBook.
**	@result		A pointer to the newly instantiated BTextControl or NULL, if failed.
*/
BTextControl*
WndLogin::AddTextCtrl( BView* parent, BRect& rect, char* id, char* label, msg mesg, float divider, bool passwd )
{
	BTextControl* textCtrl;
	if (passwd)
		textCtrl = new PassControl( rect, id, label, "", new BMessage(mesg) );
	else
		textCtrl = new BTextControl( rect, id, label, "", new BMessage(mesg) );

	assert( textCtrl != NULL );
	textCtrl->SetDivider( divider );
	textCtrl->SetTarget( NULL, this );

	if (parent != NULL)
		parent->AddChild( textCtrl );
	else
	{
		Lock();
		AddChild( textCtrl );
		Unlock();
	}
	return textCtrl;
}

/****
**	@purpose	Adds a BButton child to a given view.
**	@param		parent	A pointer to the BView to which to add the control. 
**						If <parent> is NULL, the control is added directly to the window.
**	@param		rect	Passed to the control constructor. See the BeBook.
**	@param		id		Passed to the control constructor. See the BeBook.
**	@param		label	Passed to the control constructor. See the BeBook.
**	@param		mesg	Passed to the control constructor. See the BeBook.
**	@result		A pointer to the newly instantiated BButton or NULL, if failed.
*/
BButton*
WndLogin::AddButton( BView* parent, BRect& rect, char* id, char* label, msg mesg )
{
	BButton* btn = new BButton(rect, id, label, new BMessage(mesg));
	assert( btn != NULL );

	if (parent != NULL)
		parent->AddChild( btn );
	else
	{
		Lock();
		AddChild( btn );
		Unlock();
	}
	return btn;
}

bool WndLogin::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

/****
**	@purpose	See the BeBook.
**	@param		msg		A pointer to the received message.
*/
void
WndLogin::MessageReceived( BMessage* msg )
{
	BTextView*	tv;

	switch (msg->what)
	{
		case HOST_SELECT_MESSAGE:
		{
			BMenuItem* pItem;
			if( msg->FindPointer("source" , (void **)&pItem) != B_OK)
				return; 
				
				BTextControl *serverText = (BTextControl *)FindView("serverText");
				serverText->SetText(pItem->Label() );
				serverText->TextView()->SelectAll();
				
				break;
		}
		
		case msg_ok:
		{
			BMessage* msg = new BMessage(App::msg_connect);

			tv = (BTextView *)FindView("serverText")->ChildAt( 0 );

			msg->AddString( "hostname", tv->Text() );	
//		msg->AddString( "password", myPassCtrlPassword->actualText() );
	
			be_app->PostMessage( msg, NULL );
			//Quit();
			break;
		}
	
		case msg_options:
		{
			BMessage archive;
			if( RehydrateWindow( "OptionsWindow" , &archive) )
			{
				App* pApp = (App *)be_app;
					
				OptionsDialog* pWindow = new OptionsDialog(&archive , pApp->GetOptions());
				pWindow->Show();
			}
		}
		

#if 0
	case msg_cancel:
		be_app->PostMessage( B_QUIT_REQUESTED );
		Quit();
		break;
#endif

#if 0
	case msg_hostname:
	case msg_passwd:
	{
		bool		s/*, l, p*/;

		tv = (BTextView *)myTxtCtrlHostname->ChildAt( 0 );
		s  = tv->TextLength() > 0 ? true : false;

		/*
		tv = (BTextView *)myTxtCtrlLogin->ChildAt( 0 );
		l  = tv->TextLength() > 0 ? true : false;

		tv = (BTextView *)myTxtCtrlPassword->ChildAt( 0 );
		p  = tv->TextLength() > 0 ? true : false;
		*/

		if (s /*&& l && p*/ && !myBtnOK->IsEnabled())
			myBtnOK->SetEnabled( true );

		break;
	}
#endif

	default:
		BWindow::MessageReceived( msg );
		break;
	}
}
