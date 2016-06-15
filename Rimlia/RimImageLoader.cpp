#include "MultiStdAfx.h"

#include "RimImageLoader.h"
#include "RimDataStream.h"

CBMPLoader CImageLoader::m_BMPLoader;

#ifdef _DALK_USE_PNG_
CPNGLoader CImageLoader::m_PNGLoader;
#endif

#ifdef _DALK_USE_DDS_
CDDSLoader CImageLoader::m_DDSLoader;
#endif

#ifdef _DALK_USE_TLG_
CTLGLoader CImageLoader::m_TLGLoader;
#endif

ICGLoaderBase* CImageLoader::GetLoader(LPCTSTR ext) {
	string_t ex = ext;

	if(ex == _T("bmp") || ex == _T("BMP") || ex == _T(".bmp") || ex == _T(".BMP") )
		return &m_BMPLoader;

#ifdef _DALK_USE_PNG_
	if(ex == _T("png") || ex == _T("PNG") || ex == _T(".png") || ex == _T(".PNG") )
		return &m_PNGLoader;
#endif

#ifdef _DALK_USE_DDS_
	if(ex == _T("dds") || ex == _T("DDS") || ex == _T(".dds") || ex == _T(".DDS") )
		return &m_DDSLoader;
#endif

#ifdef _DALK_USE_TLG_
	if(ex == _T("tlg") || ex == _T("TLG") || ex == _T(".tlg") || ex == _T(".TLG") )
		return &m_TLGLoader;
#endif

	return NULL;
}

int	CImageLoader::LoadImage( LPCTSTR name, ISurfaceInfo& rPixel, void* pStream) {
	void* pSrc = NULL;
	MemoryStream Stream;

	if( pStream == NULL ) {
		CFile file;

		int r = file.Open(name);

		if(r)
			return r;

		_VALLOC( pSrc, file.GetFileSize() );

		if( pSrc != NULL ) {
			file.ReadAll(pSrc);
			Stream.SetData( pSrc, file.GetFileSize() );
		}

		file.Close();

		if(pSrc == NULL)
			return RIM_ERROR_MALLOC;
	}

	CPathString ps(name);

	ICGLoaderBase* pLoader = GetLoader(ps.GetExtentStringBuf());

	int res =  pLoader->LoadData(rPixel, &Stream);

	_VFREE(pSrc);

	return res;
}

#if defined(_DALK_IOS_)
int	CImageLoader::LoadImage( NSString* name, ISurfaceInfo& rPixel, void* pStream){
    @autoreleasepool {
    // テクスチャ画像ファイルをロード
	NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:nil];
	UIImage* teximage = [[UIImage alloc] initWithContentsOfFile:path];
//	UIImage *teximage = [UIImage imageNamed:name];

	CGImageRef textureImage = teximage.CGImage;
	// サイズを取得
	NSInteger x = CGImageGetWidth(textureImage);
	NSInteger y = CGImageGetHeight(textureImage);
	NSInteger Depth = CGImageGetBitsPerPixel(textureImage);
	NSInteger Format = kCGImageAlphaPremultipliedLast;
	CGColorSpaceRef Color = CGImageGetColorSpace(textureImage);

	if(Depth == 8) {
		CGImageAlphaInfo Alpha = CGImageGetAlphaInfo(textureImage);
		Format = kCGImageAlphaNone;//kCGImageAlphaOnly;
		Color = CGColorSpaceCreateDeviceGray();
	}

	// テクスチャデータのメモリを確保（使わなくなったら解放を忘れずに！）
	void* textureData;
	 _VALLOC(textureData, x * y * Depth / 8);
	// テクスチャ画像のコンテキストを作成
	CGContextRef textureContext = CGBitmapContextCreate( textureData, x, y, 8, x * Depth / 8, Color, Format);
	CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)x, (float)y), textureImage);
#if 0
    if(Depth == 8){
		BYTE* p = (BYTE*)textureData;
		for(int t = 0;t < y;t++){
			for(int a = 0;a < x;a++){
                printf("%x",p[a]);
			}
			p += x;
			printf("\n");
		}
	}
#endif
    CGContextRelease(textureContext);
    if(Depth ==  8)
        CGColorSpaceRelease(Color);
    //CGImageRelease(textureImage);
	rPixel.pData = textureData;
	rPixel.X = x;
	rPixel.Y = y;
	rPixel.Depth = Depth;
    }
	return 0;
}
#endif