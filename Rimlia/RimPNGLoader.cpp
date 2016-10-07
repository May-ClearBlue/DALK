#include "MultiStdAfx.h"

#include "RimPNGLoader.h"

#ifdef _DEBUG
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "libpng16.lib")
#else
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "zlib.lib")
#endif

void CPNGLoader::pngread ( png_struct *png, png_bytep buf, png_size_t size ) {
	//memcpy(buf, png_get_io_ptr(png) , size);
	( (IDataStream*)( png_get_io_ptr(png) ) )->Read( size, buf, false );
}

int CPNGLoader::LoadData( ISurfaceInfo& rPixel, IDataStream* pData, int malloc_method ) {
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);

    if(info_ptr != NULL) {
		int bit_depth,num_palette = 0;;

		png_set_read_fn( png_ptr, pData, pngread );
		png_read_info( png_ptr, info_ptr );
		png_get_IHDR( png_ptr, info_ptr, (png_uint_32*)&rPixel.X, (png_uint_32*)&rPixel.Y, &bit_depth, &color_type, NULL,NULL,NULL);

		switch(color_type){
		case 0://(グレースケール / 1,2,4,8,16) 
			rPixel.Depth = 8;
			png_set_packing(png_ptr);//1,2,4bit->8bit
			break;
		case 2://(Trueカラー / 8,16)
			rPixel.Depth = 24;
			//png_set_strip_16(png_ptr);//8bitｶﾗｰに落とす
			break;
		case 3://(インデックスカラー / 1,2,4,8) 
			rPixel.Depth = 8;
			png_set_packing(png_ptr);//1,2,4bit->8bit

		    png_colorp Palette;

			if( png_get_PLTE( png_ptr, info_ptr, &Palette,&num_palette ) ){
	            num_palette = (num_palette > 255) ? 256 : num_palette;
				for(int a = 0; a < num_palette; a ++){
					rPixel.DIBPalette[a].rgbRed   = Palette[a].red;
					rPixel.DIBPalette[a].rgbGreen = Palette[a].green;
					rPixel.DIBPalette[a].rgbBlue  = Palette[a].blue;
			    }
			}
			break;
		case 4://4(αチャネルのあるグレースケール / 8,16) 
			break;
		case 6://6(αチャネルのある Trueカラー / 8,16) 
			//αチャンネルは別途処理すること
			//png_set_strip_16(png_ptr);//8bitｶﾗｰに落とす
			rPixel.Depth = 32;
		}

		//透過色チェック
		//png_bytep Trans;
		//int TransCount;
		//png_color_16p Trans16;
		//png_get_tRNS(Png,Info,&Trans,&TransCount,&Trans16);

		//取り出しﾀｲﾌﾟを設定
		//png_read_update_info(png_ptr, info_ptr);

		//WinはRGB逆転
		if(rPixel.Depth >= 24)
			png_set_bgr(png_ptr);

		if( rPixel.Depth == 24 ) {
			png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
			rPixel.Depth = 32;
			//png_set_filler(png_ptr, 0XFF, PNG_FILLER_BEFORE);
		}

		//読み込み(ｲﾝﾀｰﾚｰｽには未対応)
		_VALLOC(rPixel.pData, rPixel.Y * rPixel.X * rPixel.Depth / 8 );
		rPixel.Pitch = ( rPixel.X * rPixel.Depth / 8 );

		png_bytep* row_pointers = new png_bytep[rPixel.Y];

		BYTE* pRow = (BYTE*)rPixel.pData;

	//※上下反転になってる（DIBと同じ）なので
		if(!m_isDIBmode) {
			pRow += rPixel.Pitch * (rPixel.Y - 1);

			for(int a = 0; a < rPixel.Y; a++) {
				row_pointers[a] = pRow;
				pRow -= rPixel.Pitch;//( rCGInfo.X * rCGInfo.Depth / 8 );
			}
		}
		else {
			for(int a = 0; a < rPixel.Y; a++) {
				row_pointers[a] = pRow;
				pRow += rPixel.Pitch;//( rCGInfo.X * rCGInfo.Depth / 8 );
			}
		}

		png_read_image( png_ptr, row_pointers );

		png_read_end( png_ptr, info_ptr );

		delete[] row_pointers;

	    png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)NULL);
	}

	return 0;
}
