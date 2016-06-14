﻿
//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TLG5/6 decoder
//---------------------------------------------------------------------------
// 2004/04/01 W.Dee          First release
//---------------------------------------------------------------------------

#include "Multistdafx.h"
#include "RimTLGLoader.h"
#include "memory.h"

	//TVPInitTVPGL();
	//TVPCPUType = TVPCheckCPU();
	//TVPGL_IA32_Init();

//---------------------------------------------------------------------------
// TJSAlignedAlloc : aligned memory allocator
//---------------------------------------------------------------------------
// template classes to determine an integer type which have the same size as void *.
struct tTJSPointerSizeEnum { enum tTJSPointerSize { size = sizeof(void*) }; };
template <tjs_int size> struct tTJSPointerSizedIntegerBase { typedef long type; };
template <> struct tTJSPointerSizedIntegerBase<8> { typedef tjs_uint64 type; };
template <> struct tTJSPointerSizedIntegerBase<4> { typedef tjs_uint32 type; };
struct tTJSPointerSizedInteger : public tTJSPointerSizedIntegerBase< tTJSPointerSizeEnum::size > {};
//---------------------------------------------------------------------------
void * TJSAlignedAlloc(tjs_uint bytes, tjs_uint align_bits){
	// aligned memory allocation is to be used to gain performance on some processors.
	tjs_int align = 1 << align_bits;
	void *ptr = (void *)(new tjs_uint8[bytes + align + sizeof(void*)]);
	void *org_ptr = ptr;
	tTJSPointerSizedInteger::type *iptr =
		reinterpret_cast<tTJSPointerSizedInteger::type *>(&ptr);
	*iptr += align + sizeof(void*);
	*iptr &= ~(tTJSPointerSizedInteger::type)(align - 1);
	(reinterpret_cast<void**>(ptr))[-1] = org_ptr;
	return ptr;
}

//---------------------------------------------------------------------------
void TJSAlignedDealloc(void *ptr){
	delete [] (tjs_uint8*)((reinterpret_cast<void**>(ptr))[-1]);
}
//---------------------------------------------------------------------------

/*
	TLG5:
		Lossless graphics compression method designed for very fast decoding
		speed.

	TLG6:
		Lossless/near-lossless graphics compression method which is designed
		for high compression ratio and faster decoding. Decoding speed is
		somewhat slower than TLG5 because the algorithm is much more complex
		than TLG5. Though, the decoding speed (using SSE enabled code) is
		about 20 times faster than JPEG2000 lossless mode (using JasPer
		library) while the compression ratio can beat or compete with it.
		Summary of compression algorithm is described in
		environ/win32/krdevui/tpc/tlg6/TLG6Saver.cpp
		(in Japanese).
*/

//---------------------------------------------------------------------------
// TLG5 loading handler
//---------------------------------------------------------------------------
int TVPLoadTLG5( ISurfaceInfo& rPixel, IDataStream* pData ) {
	// load TLG v5.0 lossless compressed graphic

	unsigned char mark[12];
	tjs_int width, height, colors, blockheight;
	pData->Read(1,mark,false);

	colors = mark[0];
	
	pData->Read(width);
	pData->Read(height);
	pData->Read(blockheight);

	//24bit or 32bitのみのサポート
	if(colors != 3 && colors != 4)
		return 1;

	rPixel.Depth = colors * 8;
	rPixel.X = width;
	rPixel.Y = height;

	if(rPixel.pData == NULL)
		_VALLOC(rPixel.pData, width * height * colors / 8);

	int blockcount = (int)((height - 1) / blockheight) + 1;

	// skip block size section
	pData->Seek(pData->GetOffset() + blockcount * sizeof(tjs_uint32),FILE_BEGIN);

	tjs_uint8 *inbuf = NULL;
	tjs_uint8 *outbuf[4];
	tjs_uint8 *text = NULL;
	tjs_int r = 0;
	for(int i = 0; i < colors; i++) outbuf[i] = NULL;

	try{
		text = (tjs_uint8*)TJSAlignedAlloc(4096, 4);
		memset(text, 0, 4096);

		inbuf = (tjs_uint8*)TJSAlignedAlloc(blockheight * width + 10, 4);

		for(tjs_int i = 0; i < colors; i++)
			outbuf[i] = (tjs_uint8*)TJSAlignedAlloc(4 * blockheight * width + 10, 4);

		tjs_uint8 *prevline = NULL;
		for(tjs_int y_blk = 0; y_blk < height; y_blk += blockheight){
			// read file and decompress
			for(tjs_int c = 0; c < colors; c++){
				pData->Read(1,mark,false);

				tjs_int size;
				pData->Read(size);

				if(mark[0] == 0){
					// modified LZSS compressed data
					pData->Read(size,inbuf,false);

					r = TVPTLG5DecompressSlide(outbuf[c], inbuf, size, text, r);//(*TVPTLG5DecompressSlide_c)(outbuf[c], inbuf, size, text, r);

					int as = size;
					tjs_uint8* insize = outbuf[c];
				}
				else{
					// raw data
					pData->Read(size,outbuf[c],false);
				}
			}

			// compose colors and store
			tjs_int y_lim = y_blk + blockheight;

			if(y_lim > height)
				y_lim = height;

			tjs_uint8 * outbufp[4];

			for(tjs_int c = 0; c < colors; c++)
				outbufp[c] = outbuf[c];

			for(tjs_int y = y_blk; y < y_lim; y++){

//生ポインタ注意
				tjs_uint8 *current = (tjs_uint8*)rPixel.pData + (y * rPixel.Pitch);

				tjs_uint8 *current_org = current;

				if(prevline){
					// not first line
					switch(colors){
					case 3:
						TVPTLG5ComposeColors3To4(current, prevline, outbufp, rPixel.X);
						outbufp[0] += width; outbufp[1] += width;
						outbufp[2] += width;
						break;
					case 4:
						TVPTLG5ComposeColors4To4(current, prevline, outbufp, width);
						outbufp[0] += width; outbufp[1] += width;
						outbufp[2] += width; outbufp[3] += width;
						break;
					}
				}
				else{
					// first line
					switch(colors){
					case 3:
						for(tjs_int pr = 0, pg = 0, pb = 0, x = 0;x < width; x++){
							tjs_int b = outbufp[0][x];
							tjs_int g = outbufp[1][x];
							tjs_int r = outbufp[2][x];
							b += g; r += g;
							0[current++] = pb += b;
							0[current++] = pg += g;
							0[current++] = pr += r;
							0[current++] = 0xff;
						}
						outbufp[0] += width;
						outbufp[1] += width;
						outbufp[2] += width;
						break;
					case 4:
						for(tjs_int pr = 0, pg = 0, pb = 0, pa = 0, x = 0;x < width; x++){
							tjs_int b = outbufp[0][x];
							tjs_int g = outbufp[1][x];
							tjs_int r = outbufp[2][x];
							tjs_int a = outbufp[3][x];
							b += g; r += g;
							0[current++] = pb += b;
							0[current++] = pg += g;
							0[current++] = pr += r;
							0[current++] = pa += a;
						}
						outbufp[0] += width;
						outbufp[1] += width;
						outbufp[2] += width;
						outbufp[3] += width;
						break;
					}
				}
				//scanlinecallback(callbackdata, -1);

				prevline = current_org;
			}

		}
	}
	catch(...){
		if(inbuf) TJSAlignedDealloc(inbuf);
		if(text) TJSAlignedDealloc(text);
		for(tjs_int i = 0; i < colors; i++)
			if(outbuf[i]) TJSAlignedDealloc(outbuf[i]);
		throw;
	}

	if(inbuf) TJSAlignedDealloc(inbuf);
	if(text) TJSAlignedDealloc(text);

	for(tjs_int i = 0; i < colors; i++)
		if(outbuf[i]) TJSAlignedDealloc(outbuf[i]);

	return 0;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TLG6 loading handler
//---------------------------------------------------------------------------
int TVPLoadTLG6( ISurfaceInfo& rPixel, IDataStream* pData ){
	// load TLG v6.0 lossless/near-lossless compressed graphic

	unsigned char buf[12];

	pData->Read(4,buf,false);

	tjs_int colors = buf[0]; // color component count

	if(colors != 1 && colors != 4 && colors != 3)
		return 1;

	if(buf[1] != 0) // data flag
		return 3;

	if(buf[2] != 0) // color type  (currently always zero)
		return 4;

	if(buf[3] != 0) // external golomb table (currently always zero)
		return 5;

	tjs_int width, height;

	pData->Read(width);
	pData->Read(height);

	rPixel.Depth = colors * 8;
	rPixel.X = width;
	rPixel.Y = height;

	if( rPixel.pData == NULL )
		_VALLOC(rPixel.pData, width * height * colors / 8 );
	//max_bit_length = pData->ReadI32LE();
	tjs_int max_bit_length;
	pData->Read(max_bit_length);

	// compute some values
	tjs_int x_block_count = (tjs_int)((width - 1)/ TVP_TLG6_W_BLOCK_SIZE) + 1;
	tjs_int y_block_count = (tjs_int)((height - 1)/ TVP_TLG6_H_BLOCK_SIZE) + 1;
	tjs_int main_count = width / TVP_TLG6_W_BLOCK_SIZE;
	tjs_int fraction = width -  main_count * TVP_TLG6_W_BLOCK_SIZE;

	// prepare memory pointers
	tjs_uint8	*bit_pool	= NULL;
	tjs_uint32	*pixelbuf	= NULL; // pixel buffer
	tjs_uint8	*filter_types = NULL;
	tjs_uint8	*LZSS_text	= NULL;
	tjs_uint32	*zeroline	= NULL;

	try{
		// allocate memories
		bit_pool		= (tjs_uint8 *)	TJSAlignedAlloc(max_bit_length / 8 + 5, 4);
		pixelbuf		= (tjs_uint32 *)TJSAlignedAlloc(2 * sizeof(tjs_uint32) * width * TVP_TLG6_H_BLOCK_SIZE + 1, 4);
		filter_types	= (tjs_uint8 *)	TJSAlignedAlloc(x_block_count * y_block_count, 4);
		zeroline		= (tjs_uint32 *)TJSAlignedAlloc(width * sizeof(tjs_uint32), 4);
		LZSS_text		= (tjs_uint8*)	TJSAlignedAlloc(4096, 4);

		// initialize zero line (virtual y=-1 line)
		TVPFillARGB(zeroline, width, colors==3?0xff000000:0x00000000);
		// 0xff000000 for colors=3 makes alpha value opaque

		// initialize LZSS text (used by chroma filter type codes)
		{
			tjs_uint32 *p = (tjs_uint32*)LZSS_text;

			for(tjs_uint32 i = 0; i < 32*0x01010101; i+=0x01010101){
				for(tjs_uint32 j = 0; j < 16*0x01010101; j+=0x01010101)
					p[0] = i, p[1] = j, p += 2;
			}
		}

		// read chroma filter types.
		// chroma filter types are compressed via LZSS as used by TLG5.
		{
			/* = pData->ReadI32LE();*/
			tjs_int inbuf_size;
			pData->Read(inbuf_size);

			tjs_uint8* inbuf = (tjs_uint8*)TJSAlignedAlloc(inbuf_size, 4);
			try{
				pData->Read(inbuf_size,inbuf,false);
			
				TVPTLG5DecompressSlide(filter_types, inbuf, inbuf_size,	LZSS_text, 0);
			}
			catch(...){
				TJSAlignedDealloc(inbuf);
				throw;
			}

			TJSAlignedDealloc(inbuf);
		}

		// for each horizontal block group ...
		tjs_uint32 *prevline = zeroline;
		for(tjs_int y = 0; y < height; y += TVP_TLG6_H_BLOCK_SIZE){
			tjs_int ylim = y + TVP_TLG6_H_BLOCK_SIZE;
			if(ylim >= height) ylim = height;

			tjs_int pixel_count = (ylim - y) * width;

			// decode values
			for(tjs_int c = 0; c < colors; c++){
				// read bit length
				tjs_int bit_length;
				pData->Read(bit_length);

				// get compress method
				int method = (bit_length >> 30)&3;
				bit_length &= 0x3fffffff;

				// compute byte length
				tjs_int byte_length = bit_length / 8;
				if(bit_length % 8) byte_length++;

				// read source from input
				pData->Read(byte_length,bit_pool,false);

				// decode values
				// two most significant bits of bitlength are
				// entropy coding method;
				// 00 means Golomb method,
				// 01 means Gamma method (not yet suppoted),
				// 10 means modified LZSS method (not yet supported),
				// 11 means raw (uncompressed) data (not yet supported).

				switch(method){
				case 0:
					if(c == 0 && colors != 1)
						TVPTLG6DecodeGolombValuesForFirst( (tjs_int8*)pixelbuf,pixel_count, bit_pool );
					else
						TVPTLG6DecodeGolombValues((tjs_int8*)pixelbuf + c,pixel_count, bit_pool);

					break;
				default:
					return -1;
				}
			}

			// for each line
			unsigned char * ft =
				filter_types + (y / TVP_TLG6_H_BLOCK_SIZE)*x_block_count;
			int skipbytes = (ylim-y)*TVP_TLG6_W_BLOCK_SIZE;

			for(int yy = y; yy < ylim; yy++){
				tjs_uint32* curline = (tjs_uint32*)rPixel.pData + + (yy * rPixel.Pitch);

				int dir = (yy&1)^1;
				int oddskip = ((ylim - yy -1) - (yy-y));

				if(main_count){
					int start = ((width < TVP_TLG6_W_BLOCK_SIZE) ? width : TVP_TLG6_W_BLOCK_SIZE) * (yy - y);
					TVPTLG6DecodeLine(prevline,curline,width,main_count,ft,	skipbytes,pixelbuf + start, colors==3?0xff000000:0, oddskip, dir);
				}

				if(main_count != x_block_count){
					int ww = fraction;
					if(ww > TVP_TLG6_W_BLOCK_SIZE) ww = TVP_TLG6_W_BLOCK_SIZE;
					int start = ww * (yy - y);

					TVPTLG6DecodeLineGeneric(prevline,curline,width,main_count,x_block_count,ft,skipbytes,pixelbuf + start, colors==3?0xff000000:0, oddskip, dir);
				}

				prevline = curline;
			}

		}
	}
	catch(...){
		if(bit_pool) TJSAlignedDealloc(bit_pool);
		if(pixelbuf) TJSAlignedDealloc(pixelbuf);
		if(filter_types) TJSAlignedDealloc(filter_types);
		if(zeroline) TJSAlignedDealloc(zeroline);
		if(LZSS_text) TJSAlignedDealloc(LZSS_text);
		throw;
	}
	if(bit_pool) TJSAlignedDealloc(bit_pool);
	if(pixelbuf) TJSAlignedDealloc(pixelbuf);
	if(filter_types) TJSAlignedDealloc(filter_types);
	if(zeroline) TJSAlignedDealloc(zeroline);
	if(LZSS_text) TJSAlignedDealloc(LZSS_text);

	return 0;
}

//---------------------------------------------------------------------------
// TLG loading handler
//---------------------------------------------------------------------------
int	CTLGLoader::LoadData(ISurfaceInfo& rPixel,  IDataStream* pData, int malloc_method) {
	// read header
	unsigned char mark[12];
	pData->Read(11,mark,false);

	if(!memcmp("TLG5.0\x00raw\x1a\x00", mark, 11))
		return TVPLoadTLG5(rPixel,pData);
	else if(!memcmp("TLG6.0\x00raw\x1a\x00", mark, 11))
		return TVPLoadTLG6(rPixel,pData);
	return -1;
}
//---------------------------------------------------------------------------

