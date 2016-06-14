#include <MultiStdafx.h>

#include "RimExtAudioDecoder.h"

CExtAudioStream::CExtAudioStream() :
	m_TotalFrame(0)
{ }

// オーディオファイルを開く
int CExtAudioStream::Open(NSString* fileName) {
    NSString*	fileName2 = [fileName stringByDeletingPathExtension];
    NSString*   path = [[NSBundle mainBundle] pathForResource:fileName2 ofType:nil];

//	NSBundle *bundle = [NSBundle mainBundle];
//	CFURLRef fileURL = (__bridge CFURLRef)[NSURL fileURLWithPath:[bundle pathForResource:fileName ofType:@"caf"]];

	OSStatus res;
    res = ExtAudioFileOpenURL( (__bridge CFURLRef)[NSURL fileURLWithPath:path], &m_ExtAudioFileRef);
	DALK_ERRORRETURNINT(res,"Failed@ExtAudioFileOpenURL");

	GetFormat(m_Format);
    GetTotalFrame(m_TotalFrame);

	return 0;
}

// オーディオデータフォーマットを取得する
int CExtAudioStream::GetFormat(_PCMFORMAT& Format) {
    UInt32 VarSize = sizeof(Format);
 	OSStatus res  = ExtAudioFileGetProperty( m_ExtAudioFileRef, kExtAudioFileProperty_FileDataFormat, &VarSize, &Format);
	DALK_ERRORRETURNINT(res,"Failed@ExtAudioFileGetProperty");
//    m_ch = Format.mChannelsPerFrame;
 
	return res;
}

// フレーム数を取得する
int CExtAudioStream::GetTotalFrame(SInt64& frame) {
    UInt32 VarSize = sizeof(frame);
    int res = ExtAudioFileGetProperty(m_ExtAudioFileRef, kExtAudioFileProperty_FileLengthFrames, &VarSize, &frame);
//    m_TotalFrame = frame;
	return res;
}

size_t CExtAudioStream::Seek(size_t offset,DWORD type) {
	OSStatus res = ExtAudioFileSeek(m_ExtAudioFileRef, offset);
	
	if(res != 0)
		return 0;

	m_Offset += offset;
	return offset;
}

DWORD CExtAudioStream::Read(DWORD byte, void* pBuffer, bool bLoop) {
	DWORD length = byte;
	AudioBufferList dataBuffer = { 0 };
    dataBuffer.mNumberBuffers = 1;
    dataBuffer.mBuffers[0].mDataByteSize = byte;
    dataBuffer.mBuffers[0].mNumberChannels = m_Format.mChannelsPerFrame;
    dataBuffer.mBuffers[0].mData = pBuffer;
	OSStatus res = ExtAudioFileRead(m_ExtAudioFileRef, &length, &dataBuffer);
	return length;
}

DWORD CExtAudioStream::Write(DWORD byte,void* pBuffer,bool bLoop) {
	DWORD length = byte;
	AudioBufferList dataBuffer = { 0 };
    dataBuffer.mNumberBuffers = 1;
    dataBuffer.mBuffers[0].mDataByteSize = byte;
    dataBuffer.mBuffers[0].mNumberChannels = m_Format.mChannelsPerFrame;
    dataBuffer.mBuffers[0].mData = pBuffer;
	OSStatus res = ExtAudioFileWrite(m_ExtAudioFileRef, length, &dataBuffer);
	return length;
}

size_t CExtAudioStream::GetOffset()	{
	SInt64 temp = 0;
	OSStatus res = ExtAudioFileTell(m_ExtAudioFileRef, (SInt64*)&temp);

	if(res)
		return (size_t)-1;
//size_t - SInt64	
	temp = m_Offset;

	return m_Offset;
}

int CExtAudioDecoder::SetStream( void* pStream) {
	m_pExtAudioFileSteram = (CExtAudioStream*)pStream;

	int res;
        
    SInt64 t;
	res = m_pExtAudioFileSteram->GetTotalFrame(t);
	res = m_pExtAudioFileSteram->GetFormat(m_SourceFormat);
        
	m_TotalFrame = t;

	int  ch		= _PCM_CHANNELS(m_SourceFormat);
	WORD bits	= _PCM_BITS(m_SourceFormat);
	int  rate	= _PCM_SAMPLERATE(m_SourceFormat);

		
	SetPCMFormat(m_DestFormat, rate != 0 ? rate : 44100, bits != 0 ? bits : 16, ch != 0 ? ch : 1);
	res = m_pExtAudioFileSteram->SetFormat(m_DestFormat);

	return res;
}
