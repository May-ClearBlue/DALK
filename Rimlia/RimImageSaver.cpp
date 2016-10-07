#include "Multistdafx.h"

#include <algorithm>

#include "RimDIBSection.h"
#include "RimImageSaver.h"

int SaveBMP(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed) {
	CFile file;
	file.Create(Name,TRUE);

	int height = rInfo.Y;
	
	//上下反転処理
	if(isReversed && height > 0)
		height *= -1;

	BITMAPFILEHEADER bmfh;
	_ZEROINIT(bmfh);

	BMPINFO bmpi;
	SetBITMAPINFO((BITMAPINFO&)bmpi, rInfo.X, /*rInfo.Y*/height, rInfo.Depth,0);

	SetRGBQUAD(bmpi.bmiColors[0],0,0,1);
	SetRGBQUAD(bmpi.bmiColors[1],255,255,255);

	file.Write(sizeof(bmfh),&bmfh);
	file.Write(sizeof(bmpi.bmiHeader),&(bmpi.bmiHeader));

	DWORD Palette = 1;
	switch(rInfo.Depth){
	case 1:
		Palette = 2;
		break;
	case 4:
		Palette = 16;
		break;
	case 8:
		Palette = 256;
	}

	file.Write(Palette * 4,&bmpi.bmiColors);

	bmfh.bfOffBits = file.GetFileSize();

	int YSize = (rInfo.Y < 0 ? rInfo.Y * -1 : rInfo.Y);

	if(rInfo.Pitch == rInfo.X)
		file.Write( rInfo.X * YSize * rInfo.Depth / 8 , rInfo.pData);
	else{
		BYTE* pLine = (BYTE*)rInfo.pData;

		for(int a = 0;a < YSize;a++){
			file.Write(rInfo.X * rInfo.Depth / 8, pLine);
			pLine += rInfo.Pitch;		
		}
	}
	bmfh.bfSize = file.GetFileSize();
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfType = 0x4D42;

	file.SeekBegin(0);
	file.Write(sizeof(bmfh),&bmfh);

	//file.Close();

	return 0;
}

#if defined(_DALK_USE_DDS_) && defined(_DALK_WINDOWS_)

#include <ddraw.h>

int SaveDDS(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed) {
	CFile temp;

	int destsize = squish::GetStorageRequirements(rInfo.X, rInfo.Y, squish::kDxt5);
	void* buf;
	_VALLOC(buf, destsize);

	RGBQUAD* p = (RGBQUAD*)rInfo.pData;
	RGBQUAD q;
	for(int y = 0;y < rInfo.Y * rInfo.X; y++){
		q.rgbBlue = p->rgbRed;
		q.rgbGreen = p->rgbGreen;
		q.rgbRed = p->rgbBlue;
		q.rgbReserved = 255;
		*p = q;
		p++;
	}

	squish::CompressImage( (squish::u8*)rInfo.pData, rInfo.X, rInfo.Y, buf, squish::kDxt5 );

	DDSURFACEDESC2 ddsd = { 0 };
	ddsd.dwSize = 124;
	ddsd.dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_LINEARSIZE;
	ddsd.dwHeight = rInfo.Y; 
	ddsd.dwWidth = rInfo.X;
	ddsd.dwLinearSize = destsize;
	ddsd.dwDepth = 0;// ボリューム テクスチャの場合は、ボリュームの深度。 この場合は dwFlags に DDSD_DEPTH を含める。 
	ddsd.dwMipMapCount= 0;//ミップマップ レベルが定義されたミップマップの場合は、メイン イメージのミップマップ チェーンに含まれる総レベル数。この場合は dwFlags に DDSD_MIPMAPCOUNT を含める。 
//	ddsd.dwReserved1

	DWORD fcc = '5TXD';
	ddsd.ddpfPixelFormat.dwSize = 32;//ピクセル フォーマットの構造体を指定する 32 バイトの値。
	ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	ddsd.ddpfPixelFormat.dwFourCC = fcc;

	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;

	//	ddsd.dwReserved2 

	DWORD d = ' SDD';

	temp.Create(Name,TRUE);
	temp.Write(d);
	temp.Write(ddsd);
	temp.Write(destsize,buf);

	return destsize;
}

#endif

#ifdef _DALK_USE_PNG_

#ifdef _DEBUG
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng16.lib")
#else
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "zlib.lib")
#endif

int SavePNG(ISurfaceInfo& rInfo,int flag, LPCTSTR Name,BOOL isReversed) {
	FILE		*fp;
	png_structp	png_ptr;
	png_infop	info_ptr;
#if 0
	errno_t _err = 0;
#endif
    _FOPEN(fp, Name, _T("wb"));
    
	if(fp == NULL)
		return 1;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return 2;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		fclose(fp);
		return 3;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr,  &info_ptr);
		fclose(fp);
		return 4;
	}

	png_init_io(png_ptr, fp);
	png_set_write_status_fn(png_ptr, write_row_callback);
	png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);
	png_set_compression_level(png_ptr, 9);//Z_BEST_COMPRESSION);
	png_set_IHDR(png_ptr, info_ptr, rInfo.X, rInfo.Y, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_gAMA(png_ptr, info_ptr, 1.0);

	{
		time_t		gmt;		// G.M.T.
		png_time	mod_time;
		png_text	text_ptr[5];
		char		text_time[29] = { 0 };

		time(&gmt);
		png_convert_from_time_t(&mod_time, gmt);
		png_set_tIME(png_ptr, info_ptr, &mod_time);
		int _res = png_convert_to_rfc1123_buffer(text_time, &mod_time);

		text_ptr[0].key = "Title";
		text_ptr[0].text = "Test";
		text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
		text_ptr[1].key = "Author";
		text_ptr[1].text = "Yuzo KATO";
		text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
		text_ptr[2].key = "Description";
		text_ptr[2].text = "Test Pattern";
		text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
		text_ptr[3].key = "Creation Time";
		text_ptr[3].text = text_time;
		text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;
		text_ptr[4].key = "Software";
		text_ptr[4].text = "test.exe";
		text_ptr[4].compression = PNG_TEXT_COMPRESSION_NONE;
		png_set_text(png_ptr, info_ptr, text_ptr, 5);
	}
	
	png_byte* pBuf = new BYTE[rInfo.Pitch * rInfo.Y];
	memcpy(pBuf, rInfo.pData, rInfo.Pitch * rInfo.Y);

	vector<png_byte*> rows;
	png_byte* p = (png_byte*)pBuf;

	for(int a = 0;a < rInfo.Y;a++) {
			for(int b = 0; b < rInfo.X;b++){
				RGBQUAD* r1 = (RGBQUAD*)p;
				RGBQUAD r2 = { r1[b].rgbRed, r1[b].rgbGreen, r1[b].rgbBlue, r1[b].rgbReserved };
				r1[b] = r2;
			}
			rows.push_back(p);
			p += rInfo.Pitch;
		}
	if(flag)
		reverse(rows.begin(),rows.end());

	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);
	png_write_image(png_ptr, &rows[0] );
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);

	delete[] pBuf;

	return 0;
}

void write_row_callback(png_structp png_ptr, png_uint_32 row, int pass) {
//	printf("\r%3d%% saved", (row * 100) / png_ptr->height);
}

#endif
