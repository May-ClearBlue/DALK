#ifndef _DALK_ERROR_TYPE_
#define _DALK_ERROR_TYPE_

enum _ERROR {
	RIM_ERROR_MALLOC = -100,					//メモリ確保失敗
	RIM_ERROR_NONE = 0,							//成功
	RIM_ERROR_FILE_NOT_FOUND = 1,				//ファイルが無い
	RIM_ERROR_FORMAT,							//フォーマットが違う
	RIM_ERROR_FILE_FAILED_OPEN,					//ファイルを開けない
	RIM_ERROR_FILEHANDLE_OVERWRITE,				//ハンドルを二重に書き込みしようとした
	RIM_ERROR_REGISTWINDOWCLASS,				//ウィンドウクラスの登録失敗
	RIM_ERROR_CREATEWINDOW,						//ウィンドウ作成失敗
};

#endif