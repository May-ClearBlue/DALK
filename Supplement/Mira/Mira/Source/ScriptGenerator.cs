using System;
using NovelEx;

/// <summary>
/// Summary description for Class1
/// </summary>
public class ScriptGemerator
{
	public ScriptGemerator()
	{
		//
		// TODO: Add constructor logic here
		//
	}

	public void Output(string filePath)
	{
		//ドキュメントを作成
		Document doc = new Document();
		//ファイルの出力先を設定
		PdfWriter.GetInstance(doc, new FileStream("01_Hello.pdf", FileMode.Create));
		//ドキュメントを開く
		doc.Open();
		//「Hello iTextSharp」をドキュメントに追加
		doc.Add(new Paragraph("Hello iTextSharp"));
		//ドキュメントを閉じる
		doc.Close();
	}

}
