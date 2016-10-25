#ifndef _PNGLOADER_
#define _PNGLOADER_

#include "RimCGLoader.h"
#include "libpng/png.h"

class CPNGLoader : public ICGLoaderBase {
protected:
	BOOL		m_isDIBmode;		//TRUEの時はDIBと同じく上下反転でロードする（libpngのデフォルトもこれ。死ねばいいのに）初期値はFALSE
	png_structp	png_ptr;
	png_infop	info_ptr;
	int			color_type;
public:
static	void	pngread		(png_struct *png,png_bytep buf,png_size_t size);
		CPNGLoader() { m_ext = _T("png"); m_isDIBmode = FALSE; }
		int		LoadData	(ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method = 0);
};

#endif
