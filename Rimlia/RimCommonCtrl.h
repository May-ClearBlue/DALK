#ifndef _RIM_COMMONCTRL_
#define _RIM_COMMONCTRL_

#include "MultiStdafx.h"
#include <commctrl.h>

class CCOMMONCTRLBase {
protected:
	HWND m_hWnd;
public:
	void SetHWnd(HWND hWnd)	{ m_hWnd = hWnd; }
	HWND GetHWnd()			{ return m_hWnd; }
};

class CListView : public CCOMMONCTRLBase {
public:
		BOOL	AddColumn		( int pos, LPCTSTR text,int width = -1 );
inline	BOOL	DeleteColumn	( int index )	{ return ListView_DeleteColumn(m_hWnd, index); }
inline	int		GetColumnWidth	( int index )	{ return ListView_GetColumnWidth(m_hWnd, index); }
inline	BOOL	SetColumnWidth	( int index, DWORD width = LVSCW_AUTOSIZE ) { return ListView_SetColumnWidth(m_hWnd, index, width); }
inline	int		GetColumnCount()				{ return Header_GetItemCount(ListView_GetHeader(m_hWnd)); }

		int		InsertItem		( int index, LPCTSTR text );
inline	BOOL	DeleteItem		( int index )	{ return ListView_DeleteItem(m_hWnd, index); }
inline	BOOL	DeleteAllItems()				{ return ListView_DeleteAllItems(m_hWnd); }

inline	int		GetItemCount()					{ return ListView_GetItemCount(m_hWnd); }

inline	void	SetSubItem		( int index, int subindex, LPCTSTR text )  { ListView_SetItemText(m_hWnd, index, subindex, const_cast<LPTSTR>(text) ); }
inline	UINT	GetStatus		( DWORD index, UINT state )	{ return ListView_GetItemState(m_hWnd, index, state); }
inline	BOOL	IsSelected		( DWORD index )				{ return (ListView_GetItemState(m_hWnd, index, LVIS_SELECTED) & LVIS_SELECTED); }

inline	void	GetItemText		( int index, int subindex, LPCTSTR buf, int size = 256 )	{ ListView_GetItemText(m_hWnd, index, subindex, const_cast<LPTSTR>(buf), size); }
inline	void	SetItemText		( int index, int subindex, LPCTSTR text )					{ ListView_SetItemText(m_hWnd, index, subindex, const_cast<LPTSTR>(text) ); }

};

#endif