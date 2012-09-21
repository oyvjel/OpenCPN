/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
 * Author:   Pavel Saviankou
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Pavel Saviankou                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 */
#include <iostream>
using namespace std;

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/artprov.h"
#include "wx/stdpaths.h"
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
// #include <wx/version.h> //Gunther
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/brush.h>
#include <wx/colour.h>


#if wxCHECK_VERSION(2, 9, 0)
#include <wx/dialog.h>
#else
//  #include "scrollingdialog.h"
#endif

#include "dychart.h"

#ifdef __WXMSW__
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <psapi.h>
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#include "routeprintout.h"
#include "printtable.h"

#define PRINT_WP_NAME 0
#define PRINT_WP_POSITION 1
#define PRINT_WP_COURSE 2
#define PRINT_WP_DISTANCE 3
#define PRINT_WP_DESCRIPTION 4

// Global print data, to remember settings during the session
extern wxPrintData               *g_printData;
// Global page setup data
extern wxPageSetupData           *g_pageSetupData;
MyRoutePrintout::MyRoutePrintout(std::vector<bool> _toPrintOut, 
				  Route * route, 
				  const wxChar *title
				):MyPrintout(title), 
				  myRoute(route), 
				  toPrintOut(_toPrintOut)
{
    // Let's have at least some device units margin
  marginX = 5;
  marginY = 5;
 
  
      table.StartFillHeader();
    // setup widths for columns
      if (toPrintOut[PRINT_WP_NAME])
      {
	table << "Name";
      }
      if (toPrintOut[PRINT_WP_POSITION])
      {
	table << "Pos";
      }
      if (toPrintOut[PRINT_WP_COURSE])
      {
	table << "Course";
      }
      if (toPrintOut[PRINT_WP_DISTANCE])
      {
	table << "Dist";
      }
      if (toPrintOut[PRINT_WP_DESCRIPTION])
      {
	table << "Description";
      }
  
      table.StartFillWidths();
  // setup widths for columns
      if (toPrintOut[PRINT_WP_NAME])
      {
	table << 23;
      }
      if (toPrintOut[PRINT_WP_POSITION])
      {
	table << 40;
      }
      if (toPrintOut[PRINT_WP_COURSE])
      {
	table << 30;
      }
      if (toPrintOut[PRINT_WP_DISTANCE])
      {
	table << 38;
      }
      if (toPrintOut[PRINT_WP_DESCRIPTION])
      {
	table << 100;
      }
  
      table.StartFillData();
  
  
    for (int n=1; n<= myRoute->GetnPoints(); n++)
    {
      RoutePoint * point = myRoute->GetPoint( n );

      if (toPrintOut[PRINT_WP_NAME])
      {
// 	dc->DrawText(point->GetName(), marginX , marginY + 10*n ); 
	string cell(point->GetName().fn_str());
	table << cell;
      }
      if (toPrintOut[PRINT_WP_POSITION])
      {
// 	dc->DrawText(_("POS"), marginX + 50, marginY + 10*n   ); 
	wxString point_position = toSDMM( 1, point->m_lat, point->m_bIsInTrack ) + _T("\n") + toSDMM( 2, point->m_lon, point->m_bIsInTrack );
// 	point_position.Printf(_T("%03.0f Deg. T \n %03.0f Deg. T"), point->GetLatitude(), point->GetLongitude());;

	string cell(point_position.fn_str());
	table << cell;
      }
      if (toPrintOut[PRINT_WP_COURSE])
      {
	wxString point_course; 
	point_course.Printf(_T("%03.0f Deg"), point->GetCourse());
	string cell(point_course.fn_str());
	table << cell;
      }
      if (toPrintOut[PRINT_WP_DISTANCE])
      {
	wxString point_distance; 
	point_distance.Printf(_T("%6.2f NM"), point->GetDistance());
	string cell(point_distance.fn_str());
	table << cell;

      }
      if (toPrintOut[PRINT_WP_DESCRIPTION])
      {
// 	dc->DrawText(point->GetDescription(), marginX + 200, marginY + 10*n   );
	string cell(point->GetDescription().fn_str());
	table << cell;
      }
      table << "\n";
    }
    

    
}


void MyRoutePrintout::GetPageInfo( int *minPage, int *maxPage, int *selPageFrom, int *selPageTo )
{
    *minPage = 1;
    *maxPage = numberOfPages;
    *selPageFrom = 1;
    *selPageTo = numberOfPages;
}


void MyRoutePrintout::OnPreparePrinting()
{
  pageToPrint = 1;
  wxDC *dc = GetDC();
  wxFont routePrintFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  dc->SetFont(routePrintFont);
    
  table.AdjustCells(dc, marginX, marginY);
  numberOfPages = table.GetNumberPages();
}


bool MyRoutePrintout::OnPrintPage( int page )
{
  wxDC *dc = GetDC();
  pageToPrint = page;
  DrawPage( dc );
  return true;
}

void MyRoutePrintout::DrawPage( wxDC *dc )
{


  
    // Get the Size of the Chart Canvas
//     int sx, sy;
//     dc->GetClientSize( &sx, &sy );                       // of the canvas

//     float maxX = sx;
//     float maxY = sy;



    // Add the margin to the graphic size
//     maxX += ( 2 * marginX );
//     maxY += ( 2 * marginY );

    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize( &w, &h );
    
    int maxX = w;
    int maxY = h;
    
    // Calculate a suitable scaling factor
    float scaleX = (float) ( w / maxX );
    float scaleY = (float) ( h / maxY );

    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);

    // Calculate the position on the DC for centring the graphic
    float posX = (float) ( ( w - ( maxX * actualScale ) ) / 2.0 );
    float posY = (float) ( ( h - ( maxY * actualScale ) ) / 2.0 );

    posX = wxMax(posX, marginX);
    posY = wxMax(posY, marginY);

    // Set the scale and origin
    dc->SetUserScale( actualScale, actualScale );
    dc->SetDeviceOrigin( (long) posX, (long) posY );
//     wxFont routePrintFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
//     dc->SetFont(routePrintFont);
    
        
    wxFont routePrintFont_bold(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    dc->SetFont(routePrintFont_bold);

    
    int currentX = marginX;
    int currentY = marginY;
    vector< PrintCell >& header_content = table.GetHeader();
    for (size_t j=0; j< header_content.size(); j++)
    {
      PrintCell& cell = header_content[j];
      dc->DrawRectangle(currentX, currentY, cell.GetWidth(), cell.GetHeight());
      dc->DrawText(cell.GetText(),  currentX, currentY);
      currentX += cell.GetWidth();
    }


    
    wxFont routePrintFont_normal(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    dc->SetFont(routePrintFont_normal);    
    wxBrush brush( wxNullColour,  wxTRANSPARENT );
    dc->SetBrush(brush);
//     dc->DrawRectangle(marginX, marginY, maxX-4*marginX, maxY-4*marginY);
    
    vector< vector < PrintCell > > & cells = table.GetContent();
    currentY = marginY + table.GetHeaderHeight();
    int currentHeight = 0;
    for (size_t i=0; i< cells.size(); i++)
    {
      vector< PrintCell >& content_row = cells [i]; 
      currentX = marginX;
      for (size_t j=0; j< content_row.size(); j++)
      {
	PrintCell& cell = content_row[j];
	if (cell.GetPage() == pageToPrint)
	{
	  dc->DrawRectangle(currentX, currentY, cell.GetWidth(), cell.GetHeight());
	  dc->DrawText(cell.GetText(),  currentX, currentY);
	  currentX += cell.GetWidth();
	  currentHeight = cell.GetHeight();

	}
      }
      currentY += currentHeight;
    }
    
    
    

}

// WP # , Position , Course , Remarks





// ---------- RoutePrintSelection dialof implementation

/*!
 * RoutePrintSelection type definition
 */

/* global instance of RoutePrintSelection
 */
RoutePrintSelection * pRoutePrintSelection;


IMPLEMENT_DYNAMIC_CLASS( RoutePrintSelection, wxDialog )
/*!
 * RouteProp event table definition
 */

BEGIN_EVENT_TABLE( RoutePrintSelection, wxDialog )
    EVT_BUTTON( ID_ROUTEPRINT_SELECTION_CANCEL, RoutePrintSelection::OnRoutepropCancelClick )
    EVT_BUTTON( ID_ROUTEPRINT_SELECTION_OK, RoutePrintSelection::OnRoutepropOkClick )
END_EVENT_TABLE()

/*!
 * RouteProp constructors
 */

RoutePrintSelection::RoutePrintSelection()
{
}

RoutePrintSelection::RoutePrintSelection( wxWindow* parent, Route * _route,  wxWindowID id, const wxString& caption, const wxPoint& pos,
        const wxSize& size, long style )
{
       route = _route;  


    long wstyle = style;
// #ifdef __WXOSX__
//     wstyle |= wxSTAY_ON_TOP;
// #endif

    Create( parent, id, caption, pos, size, wstyle );
//     GetSizer()->SetSizeHints( this );
    Centre();
}

RoutePrintSelection::~RoutePrintSelection()
{}

/*!
 * RouteProp creator
 */

bool RoutePrintSelection::Create( wxWindow* parent, wxWindowID id, const wxString& caption,
        const wxPoint& pos, const wxSize& size, long style )
{

    SetExtraStyle( GetExtraStyle() | wxWS_EX_BLOCK_EVENTS );
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();

    return TRUE;
}

/*!
 * Control creation for RouteProp
 */

void RoutePrintSelection::CreateControls()
{
    ////@begin RouteProp content construction
    
    
    
    RoutePrintSelection* itemDialog1 = this;
    
     wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox( itemDialog1, wxID_ANY,
            _("Elements to print...") );
    
    
    wxStaticBoxSizer* itemBoxSizer1 = new wxStaticBoxSizer(itemStaticBoxSizer3Static,  wxVERTICAL );
    itemDialog1->SetSizer( itemBoxSizer1 );
    
    
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer( wxVERTICAL );
    
    
    

    
    wxBoxSizer* bSizer00;
    bSizer00 = new wxBoxSizer( wxHORIZONTAL );        
    m_checkBoxWPName = new wxCheckBox( itemDialog1, wxID_ANY, _("WP Name"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPName->SetValue(true);
    bSizer00->Add( m_checkBoxWPName, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    
    wxStaticText* label1 = new  wxStaticText(itemDialog1, wxID_ANY, _("With selecting this element name of the WP will be present int the route printout."), wxDefaultPosition, wxDefaultSize);
    bSizer00->Add( label1, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    bSizer2->Add(bSizer00, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 )  ; 
    
        
    wxBoxSizer* bSizer10;
    bSizer10 = new wxBoxSizer( wxHORIZONTAL );   
    m_checkBoxWPPosition = new wxCheckBox( itemDialog1, wxID_ANY, _("Position"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPPosition->SetValue(true);
    bSizer10->Add( m_checkBoxWPPosition, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label2 = new  wxStaticText(itemDialog1, wxID_ANY, _("GPS Position of the WP is printed as well."), wxDefaultPosition, wxDefaultSize);
    bSizer10->Add( label2, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    bSizer2->Add(bSizer10, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 )  ; 
        

    
    wxBoxSizer* bSizer20;
    bSizer20 = new wxBoxSizer( wxHORIZONTAL );    
    m_checkBoxWPCourse = new wxCheckBox( itemDialog1, wxID_ANY, _("Course"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPCourse->SetValue(true);
    bSizer20->Add( m_checkBoxWPCourse, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label3 = new  wxStaticText(itemDialog1, wxID_ANY, _("Indicates the course from the WP tp the next one. "), wxDefaultPosition, wxDefaultSize);
    bSizer20->Add( label3, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    bSizer2->Add(bSizer20, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 )  ; 
        
    
    
    wxBoxSizer* bSizer30;
    bSizer30 = new wxBoxSizer( wxHORIZONTAL );    
    m_checkBoxWPDistanceToNext = new wxCheckBox( itemDialog1, wxID_ANY, _("Distance to next WP"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPDistanceToNext->SetValue(true);
    bSizer30->Add( m_checkBoxWPDistanceToNext, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label4 = new  wxStaticText(itemDialog1, wxID_ANY, _("Distance from the current WP to the next WP."), wxDefaultPosition, wxDefaultSize);    
    bSizer30->Add( label4, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    bSizer2->Add(bSizer30, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 )  ; 
        
    
    wxBoxSizer* bSizer40;
    bSizer40 = new wxBoxSizer( wxHORIZONTAL );    
    m_checkBoxWPDescription = new wxCheckBox( itemDialog1, wxID_ANY, _("Description"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    m_checkBoxWPDescription->SetValue(true);
    bSizer40->Add( m_checkBoxWPDescription, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    wxStaticText* label5 = new  wxStaticText(itemDialog1, wxID_ANY, _("Include the description of the WP for print out. It is probably most valuable field."), wxDefaultPosition, wxDefaultSize);       
    bSizer40->Add( label5, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );
    bSizer2->Add(bSizer40, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5 )  ; 
                
    itemBoxSizer1->Add( bSizer2, 5, wxEXPAND, 5 );

    
    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer( wxHORIZONTAL );
    itemBoxSizer1->Add( itemBoxSizer16, 0, wxALIGN_RIGHT | wxALL, 5 );
    
    m_CancelButton = new wxButton( itemDialog1, ID_ROUTEPRINT_SELECTION_CANCEL, _("Cancel"), wxDefaultPosition,
            wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_CancelButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5 );

    m_OKButton = new wxButton( itemDialog1, ID_ROUTEPRINT_SELECTION_OK, _("OK"), wxDefaultPosition,
            wxDefaultSize, 0 );
    itemBoxSizer16->Add( m_OKButton, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 5 );
    m_OKButton->SetDefault();

    SetColorScheme( (ColorScheme) 0 );

}


void RoutePrintSelection::SetColorScheme( ColorScheme cs )
{
    DimeControl( this );
}

/*
 * Should we show tooltips?
 */

bool RoutePrintSelection::ShowToolTips()
{
    return TRUE;
}

void RoutePrintSelection::SetDialogTitle( wxString title )
{
    SetTitle( title );
}


void RoutePrintSelection::OnRoutepropCancelClick( wxCommandEvent& event )
{
    Hide();
    event.Skip();
}

void RoutePrintSelection::OnRoutepropOkClick( wxCommandEvent& event )
{
  std::vector<bool> toPrintOut;
  toPrintOut.push_back(m_checkBoxWPName->GetValue()); 
  toPrintOut.push_back(m_checkBoxWPPosition->GetValue()); 
  toPrintOut.push_back(m_checkBoxWPCourse->GetValue()); 
  toPrintOut.push_back(m_checkBoxWPDistanceToNext->GetValue()); 
  toPrintOut.push_back(m_checkBoxWPDescription->GetValue());   
  
    if( NULL == g_printData ) 
    {
        g_printData = new wxPrintData;
        g_printData->SetOrientation( wxLANDSCAPE );
        g_pageSetupData = new wxPageSetupDialogData;
    }
  
  MyRoutePrintout * myrouteprintout1 = new MyRoutePrintout(toPrintOut, route,  _("Route Print"));
  
  wxPrintDialogData printDialogData( *g_printData );
  printDialogData.EnablePageNumbers( true );

  wxPrinter printer( &printDialogData );
  if( !printer.Print( this, myrouteprintout1, true ) ) 
  {
    if( wxPrinter::GetLastError() == wxPRINTER_ERROR ) OCPNMessageBox(
	    _("There was a problem printing.\nPerhaps your current printer is not set correctly?"),
	    _T("OpenCPN"), wxOK );
//        else
//            OCPNMessageBox(_T("Print Cancelled"), _T("OpenCPN"), wxOK);
    } 

  
/*
    MyRoutePrintout * myrouteprintout1 = new MyRoutePrintout(toPrintOut, route,  _("Route Print"));
    MyRoutePrintout * myrouteprintout2 = new MyRoutePrintout(toPrintOut, route,  _("Route Print"));
    wxPrintPreview *preview = new wxPrintPreview(myrouteprintout1, myrouteprintout2);
    
    
    
    wxPreviewFrame *frame = new wxPreviewFrame(preview, this,
                                               _T("Route Print Preview"),
                                               wxPoint(100, 100),
                                               wxSize(600, 650));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
 
    */
    
    
    
    
//     cc1->Refresh( false );
    Hide();
    event.Skip();
}


