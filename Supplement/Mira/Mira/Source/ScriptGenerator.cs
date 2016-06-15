using System;
using System.IO;
using System.Collections.Generic;
using HPdf;
using NovelEx;
using Mira;

//using iFont = iTextSharp.text.Font;
//using iTextSharp.text;
//using iTextSharp.text.pdf;

/// <summary>
/// Summary description for Class1
/// </summary>
public class PDFInfo
{
	public System.Drawing.Font font;
	public float marginV;
	public float marginH;
	public string Title;
	public string Author;
};

public class ScriptGemerator
{
	public string highLightName = "";
	public float referenceFontSize = 16.0f;
	public float highLightFontSize = 19.0f;
	public float textBeginRatio = 0.20f;
	public float borderPosY = 0.0f;

	const float _INCH_ = 25.4f;
	const float A4_width_point = 297.0f * 72.0f / _INCH_;	//830くらい
	const float A4_height_point = 210.0f * 72.0f / _INCH_;		//597くらい

	HPdfPage hPage = null;
	HPdfPoint textPos;
	PDFInfo pdfInfo;

	public ScriptGemerator()
	{
		//
		// TODO: Add constructor logic here
		//
	}


	public void AddPage(HPdfDoc _hPdf)
	{
		hPage = _hPdf.AddPage();
		hPage.SetSize(HPdfPageSizes.HPDF_PAGE_SIZE_A4, HPdfPageDirection.HPDF_PAGE_LANDSCAPE);
		textPos = hPage.GetCurrentTextPos();
		textPos.x = A4_width_point - pdfInfo.marginH;
		textPos.y = A4_height_point - pdfInfo.marginV;

		float borderHeight = (A4_height_point - pdfInfo.marginV * 2) * textBeginRatio;
		borderPosY = A4_height_point - borderHeight - pdfInfo.marginV;

		hPage.SetLineWidth(1.0f);
		hPage.MoveTo(pdfInfo.marginH, borderPosY);
		hPage.LineTo(A4_width_point - pdfInfo.marginH, borderPosY);
		hPage.Stroke();
	}

	public void Output(List<Tag> tagArray,  string filePath,  PDFInfo _pdfInfo )
	{
		pdfInfo = _pdfInfo;

		HPdfDoc hPdf = new HPdfDoc();
		hPdf.SetInfoAttr(HPdfInfoType.HPDF_INFO_AUTHOR, pdfInfo.Author);
		hPdf.SetInfoAttr(HPdfInfoType.HPDF_INFO_TITLE, pdfInfo.Title);
		hPdf.UseJPFonts();
		hPdf.UseJPEncodings();
//		hPdf.SetPermission(HPdfDoc.HPDF_ENABLE_PRINT);

		HPdfFont hFont1 = hPdf.GetFont("MS-Gothic,Bold", "90ms-RKSJ-V");
		HPdfFont hFont2 = hPdf.GetFont("MS-Mincho", "90ms-RKSJ-V");
		HPdfFont hFont3 = hPdf.GetFont("MS-Gothic", "90ms-RKSJ-H");

//		string FF = hPdf.LoadTTFontFromFile2("Meiryo.ttc", 0, true);

		AddPage(hPdf);		
//		HPdfPoint textPos = hPage.GetCurrentTextPos();
//		try
//		{
			foreach( Tag tag in tagArray)
			{
				if( tag.getTagName() == "talk")
				{
					if( textPos.x < pdfInfo.marginH + (referenceFontSize / 2) )
						AddPage(hPdf);

					string  _name = tag.getParam("name");

					float fontSize = referenceFontSize;

//					if( _name == highLightName )
					{
						fontSize = highLightFontSize;
						hPage.SetFontAndSize(hFont1, highLightFontSize);
						float textWidth = hPage.TextWidth(_name);
						hPage.Rectangle( textPos.x - fontSize / 2 + 0.3f, textPos.y - fontSize / 2 + 0.3f, fontSize + 0.6f, textWidth + 0.6f);
						hPage.Stroke();
					}
//					else
//						hPage.SetFontAndSize(hFont2, referenceFontSize);

					hPage.BeginText();

					if (!String.IsNullOrEmpty(_name))
						hPage.TextOut(textPos.x, textPos.y, _name);

					string _text = tag.getParam("text");

					if (!String.IsNullOrEmpty(_text))
						hPage.TextOut(textPos.x, borderPosY - 2.0f, _text);

					textPos.x -= fontSize*1.35f;
					//textPos.y = pdfInfo.marginV;

					hPage.EndText();
				}
			}
			hPdf.SaveToFile(filePath);
			hPdf.FreeDocAll();
//		}
#if false
		catch (IOException ex)
		{
			string f = ex.Message;
			//			MessageBox.Show(ex.Message, "IOException");
		}
		finally
		{
			//ドキュメントを閉じる
//			doc.Close();
		}
#endif
	}
}
