#include "MultiStdafx.h"


#include "RimCommonCtrl.h"

BOOL CListView::AddColumn ( int pos, LPCTSTR text, int width ) {
	LVCOLUMN lvcol;
	lvcol.mask = LVCF_TEXT;
	
	if(width > 0)
		lvcol.mask |= LVCF_WIDTH;

	lvcol.pszText = const_cast<LPTSTR>(text);
	lvcol.cx = width < 0 ? 100 : width;
	lvcol.iSubItem = pos;

	return ListView_InsertColumn(m_hWnd, pos, &lvcol);
}

int CListView::InsertItem( int index, LPCTSTR text ) {
	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = index;
	item.iSubItem = 0;
	item.pszText = const_cast<LPTSTR>(text);
	item.lParam = 0;
	return ListView_InsertItem(m_hWnd, &item);
}
