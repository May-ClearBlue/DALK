using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using NovelEx;

namespace Mira
{
	public partial class Form1 : Form
	{

		private string[] fileNameArray;

		private PDFInfo pdfInfo = new PDFInfo();

		NovelParser novelParser = new NovelParser("Mira");

		public Form1()
		{
			InitializeComponent();
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			textBox3.Text = Path .GetDirectoryName(Application.ExecutablePath);
		}

		private void Form1_DragDrop(object sender, DragEventArgs e)
		{
			fileNameArray = (string[])e.Data.GetData(DataFormats.FileDrop, false);

			foreach(string file in fileNameArray)
			{
				novelParser.Init();
				StreamReader sr = new StreamReader( file, Encoding.GetEncoding("Shift_JIS"));
				string text = sr.ReadToEnd();
				novelParser.parseScript(text);
				sr.Close();

//				string outputFileName =  Path.GetFileName(file);

				string csvFileName = textBox3.Text + "\\" + Path.GetFileNameWithoutExtension(file) + ".csv";

				StreamWriter sw = new StreamWriter(csvFileName, false, Encoding.GetEncoding("Shift_JIS"));

				foreach( Tag _tag in novelParser.tagList)
				{
					if (_tag.getTagName() == "talk")
					{
						if(!String.IsNullOrEmpty(_tag.getParam("name")))
							sw.WriteLine( _tag.getParam("name") + ", " + _tag.getParam("text"));
					}
				}

				sw.Close();

				PDFInfo pdfInfo = new PDFInfo();
				pdfInfo.font = label1.Font;
				pdfInfo.marginV = Decimal.ToInt32(numericUpDown1.Value);
				pdfInfo.marginH = Decimal.ToInt32(numericUpDown2.Value);
				pdfInfo.Title = textBox1.Text;
				pdfInfo.Author = textBox2.Text;

				ScriptGemerator _scriptGen = new ScriptGemerator();
				
				_scriptGen.Output(novelParser.tagList, textBox3.Text + "\\" + Path.GetFileNameWithoutExtension(file) + ".pdf", pdfInfo);
			}
		}

		private void Form1_DragEnter(object sender, DragEventArgs e)
		{
			//ドラッグされているデータがstring型か調べ、
			//そうであればドロップ効果をMoveにする
//			if (e.Data.GetDataPresent(typeof(string)))
				e.Effect = DragDropEffects.Copy;
//			else
//				//string型でなければ受け入れない
//				e.Effect = DragDropEffects.None;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			//FontDialogクラスのインスタンスを作成
			FontDialog fd = new FontDialog();

			//初期のフォントを設定
			fd.Font = label1.Font;
			//初期の色を設定
//			fd.Color = TextBox1.ForeColor;
			//ユーザーが選択できるポイントサイズの最大値を設定する
			fd.MaxSize = (int)fd.Font.Size;
			fd.MinSize = (int)fd.Font.Size;
			//存在しないフォントやスタイルをユーザーが選択すると
			//エラーメッセージを表示する
			fd.FontMustExist = true;
//			//横書きフォントだけを表示する
			fd.AllowVerticalFonts = true;
//			//色を選択できるようにする
			fd.ShowColor = false;
//			//取り消し線、下線、テキストの色などのオプションを指定可能にする
			//デフォルトがTrueのため必要はない
			fd.ShowEffects = false;
			//固定ピッチフォント以外も表示する
			//デフォルトがFalseのため必要はない
			fd.FixedPitchOnly = false;
			//ベクタ フォントを選択できるようにする
			//デフォルトがTrueのため必要はない
			fd.AllowVectorFonts = true;

			//ダイアログを表示する
			if (fd.ShowDialog() != DialogResult.Cancel)
			{
				//TextBox1のフォントと色を変える
				label1.Font = fd.Font;
			}
		}

		private void button2_Click(object sender, EventArgs e)
		{
			//FolderBrowserDialogクラスのインスタンスを作成
			FolderBrowserDialog fbd = new FolderBrowserDialog();

			//上部に表示する説明テキストを指定する
			fbd.Description = "フォルダ選択";
			//ルートフォルダを指定する
			//デフォルトでDesktop
			fbd.RootFolder = Environment.SpecialFolder.Desktop;
			//最初に選択するフォルダを指定する
			//RootFolder以下にあるフォルダである必要がある
			fbd.SelectedPath = @"C:\Windows";
			//ユーザーが新しいフォルダを作成できるようにする
			//デフォルトでTrue
			fbd.ShowNewFolderButton = true;

			//ダイアログを表示する
			if (fbd.ShowDialog(this) == DialogResult.OK)
			{
				//選択されたフォルダを表示する
				textBox3.Text = fbd.SelectedPath;
			}
		}
	}
}
