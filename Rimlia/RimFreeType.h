#ifndef _RIM_FREEETYPE_TEXTRENDER_
#define _RIM_FREEETYPE_TEXTRENDER_

#include <ft2build.h>
#include FT_FREETYPE_H	// <freetype/freetype.h>
#include FT_OUTLINE_H	// <freetype/ftoutln.h>


class CFreeType {
protected:
static	FT_Library	g_Instance;
		FT_Face		m_Face;
		FT_Bitmap*	m_Bitmap;
public:
FT_Error	Init()			{ return FT_Init_FreeType( &g_Instance );
FT_Error	Load(char* name){ FT_New_Face( g_Instance, name, 0, &m_Face ); }

FT_Error	SetSize(DWORD point, DWORD dpi = 300) { 
	return  FT_Set_Char_Size( m_Face, 0, point * 16, dpi, dpi);
	//return FT_Set_Pixel_Sizes( m_Face, 0, point );

void		GetCharData(DWORD code) {
	FT_Error err;
	err = FT_Load_Char( m_Face, code, FT_LOAD_RENDER);
	err = FT_Render_Glyph(m_Face->glyph, FT_RENDER_MODE_MONO);
	m_Bitmap = &face->glyph->bitmap;
	}
};

error = FT_Set_Transform(
            face,       /* target face object    */
            &matrix,    /* pointer to 2x2 matrix */
            &delta );   /* pointer to 2d vector  */


                FT_GlyphSlot slot = meiryo_face->glyph;
                unsigned char* bitmap_buffer = slot->bitmap.buffer;
                const int bitmap_w = slot->bitmap.width;
                const int bitmap_h = slot->bitmap.rows;
                const int advance_x = static_cast<int>(slot->metrics.horiAdvance/64.0);
                const int advance_y = static_cast<int>(slot->metrics.vertAdvance/64.0);
                const int iyoffset = font_size - slot->bitmap_top;
                const int ixoffset = slot->bitmap_left;
                
                const int ysize = posy + bitmap_h;
                for (int y = posy, iy = imagey; y < ysize; ++y, ++iy)
                {
                    const int xsize = posx + bitmap_w;
                    for (int x = posx, ix = imagex; x < xsize; ++x, ++ix)
                    {
                        const int buffer_pos = bitmap_w * (y - posy) + (x - posx);
                        const int image_pos = image_w * (iy + iyoffset) + (ix + ixoffset);
                        const double alpha = bitmap_buffer[buffer_pos] / static_cast<double>(0xFF);
                        const double inv_alpha = 1.0 - alpha;
                        // for alpha image
                        // image->mutable_list().at(image_pos) = umbase::UMVec4d(0, 0, 0, alpha);
                        image->mutable_list().at(image_pos) = umbase::UMVec4d(inv_alpha, inv_alpha, inv_alpha, 1.0);
                    };
                }
                posx += bitmap_w;
                imagex += advance_x;
                if (imagex >= image_w - font_size)
                {
                    imagex = 0;
                    imagey += advance_y;
                }
            }


#endif
