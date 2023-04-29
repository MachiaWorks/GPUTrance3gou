#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
//WAVのフォーマットについて記載がある
#include <Mmreg.h>

//#include "graph_shader.h"
#include "synth_shader.h"
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------
// 画面サイズ、モード
//----------------------------------------------------------------
#define	SCREEN_WIDTH	320//1920
#define	SCREEN_HEIGHT	240//1080
//#define INT_S unsigned char

#define SND_DURATION 60*4
#define INV_SNDRATE	1/44100
#define SAMPLE_RATE 44100 
#define SND_NUMCHANNELS 2 

#define SND_NUMSAMPLES  (SND_DURATION*SAMPLE_RATE)
#define SND_NUMSAMPLESC (SND_NUMSAMPLES*SND_NUMCHANNELS)
#define O_GAIN "gain"

bool stop_flag;

//WAVヘッダ
BYTE header[0x36] =
{
	'R','I','F','F',	//0000 RIFF
	0x00,	//0004
	0x00,	//0005
	0x00,	//0006
	0x00,	//0007
	'W','A','V','E',	//0008 WAVE
	'f','m','t',' ',	//000c 'fmt '

	0x12,	//0010
	0x00,	//0011
	0x00,	//0012
	0x00,	//0013
	0x01,	//0014
	0x00,	//0015
	0x02,	//0016
	0x00,	//0017
	0x44,	//0018
	0xac,	//0019
	0x00,	//001a
	0x00,	//001b
	0x10,	//001c
	0xb1,	//001d
	0x02,	//001e
	0x00,	//001f

	0x04,	//0020
	0x00,	//0021
	0x10,	//0022
	0x00,	//0023
	0x4c,	//0024
	0x00,	//0025
	0x66,	//0026
	0x61,	//0027
	0x63,	//0028
	0x74,	//0029
	0x04,	//002a
	0x00,	//002b
	0x00,	//002c
	0x00,	//002d
	0x00,	//002e
	0x00,	//002f

	0x00,	//0030
	0x00,	//0031
	0x64,0x61,0x74,0x61	//0032
};



//----------------------------------------------------------------
// とっかえ
//----------------------------------------------------------------

/*
extern "C" {
	int _fltused = 0x9875;

	int __cdecl _ftol2_sse() {
		int integral;
		short oldfcw, newfcw;
		__asm {
			fstcw[oldfcw]
			mov ax, [oldfcw]
			or ax, 0c00h; chop
			mov[newfcw], ax
			fldcw[newfcw]
			fistp dword ptr[integral]
			fldcw[oldfcw]
		}
		return integral;
	}

	int __cdecl _ftol2() { return _ftol2_sse(); }// for VS.NET 2003
}
*/

void Output() {

}

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//----------------------------------------------------------------
// グローバル変数
//----------------------------------------------------------------
// シェーダープログラム
GLuint g_pProgram;

// 一時的に使う変数。
// 容量削減のためあちこちで使いまわします。
GLuint g_pTemp;

// デバイスコンテキスト
HDC g_hDC;

// ウィンドウハンドル
HWND g_hWnd;

// レンダリングコンテキスト
HGLRC g_hGLRC;

HWAVEOUT hWOut;

//WAVの格納場所。
float samples[SND_NUMSAMPLESC];

HWAVEOUT hWaveOut = nullptr;
WAVEHDR wave_hdr = { (LPSTR)samples, sizeof(samples) };
//char str[32];

WAVEFORMATEX wave_format = {
	WAVE_FORMAT_IEEE_FLOAT,
	SND_NUMCHANNELS,
	SAMPLE_RATE,
	SAMPLE_RATE * sizeof(float)*SND_NUMCHANNELS,
	sizeof(float)*SND_NUMCHANNELS,
	sizeof(float) * 8,
	0
};

/*
inline void SoundInitialize() {

}
*/

//----------------------------------------------------------------
// 継続可能か調べ、可能ならtrueを返す
//----------------------------------------------------------------
inline bool CheckContinuable()
{
//#ifndef FULL_SCREEN
	MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
	if (msg.message == WM_NCLBUTTONDOWN && msg.wParam == HTCLOSE)
	{
		return false;
	}
	DispatchMessage(&msg);
//#endif
	return !GetAsyncKeyState(VK_ESCAPE);
}

//----------------------------------------------------------------
// シェーダープログラムをコンパイルする
//----------------------------------------------------------------
/*
void CreateShaderProgram()
{
	g_pProgram = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();
	g_pTemp = ((PFNGLCREATESHADERPROC)(wglGetProcAddress("glCreateShader")))(GL_VERTEX_SHADER);

	
	//int i = 0;
	//for( i=0;vertexShaderSource[i] != '\0'; i++)
	

	((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(g_pTemp, 1, &vertexShaderSource, 0);
	((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(g_pTemp);
	((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(g_pProgram, g_pTemp);

	g_pTemp = ((PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"))(GL_FRAGMENT_SHADER);
	((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(g_pTemp, 1, &fragmentShaderSource, 0);
	((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(g_pTemp);
	((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(g_pProgram, g_pTemp);

	((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(g_pProgram);
	((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(g_pProgram);
}
*/

//----------------------------------------------------------------
// ウィンドウ、OpenGLを初期化する
//----------------------------------------------------------------
/*
inline void InitializeWindow()
{


}
*/

//----------------------------------------------------------------
// エントリポイント
//----------------------------------------------------------------
void WinMainCRTStartup()
{
	//InitializeWindow();だったもの
#ifdef FULL_SCREEN
	static DEVMODE dmScreenSettings =
	{
		"", 0, 0, sizeof(dmScreenSettings), 0, DM_PELSWIDTH | DM_PELSHEIGHT,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	g_hWnd = CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 0);
	ShowCursor(0);
#else
	g_hWnd = CreateWindow("edit", 0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 0);
#endif



	g_hDC = GetDC(g_hWnd);

	static const PIXELFORMATDESCRIPTOR g_pixelFormatDescriptor =
	{
		0, 1, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 32, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0
	};
	SetPixelFormat(g_hDC, ChoosePixelFormat(g_hDC, &g_pixelFormatDescriptor), &g_pixelFormatDescriptor);
	g_hGLRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hGLRC);

	//OpenGLコンテキストとウィンドウはセットみたいなので、
	//ウィドウを隠す処理を入れる。
	ShowWindow(g_hWnd, SW_HIDE);


	//SoundInitialize();
	GLuint programMzk;
	GLint result;
	GLuint tmp;

	//ゼロクリアする（しないと前のデータが残るため）
	/*
	for (int i = 0; i < SND_NUMSAMPLESC; i++) {
		samples[i] = 0.0;
	}
	*/

	HANDLE hFile;

	const char* c_str;


	//サウンド部分。何故か代入しないと容量が大きく縮む。
	//stop_flag = false;

	//プログラム作成。
	programMzk = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();

	//シェーダ作成。
	tmp = ((PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"))(GL_VERTEX_SHADER);
	((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(tmp, 1, &msh, 0);

	//readShaderSource("music.vs", tmp);
	((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(tmp);
	((PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv"))(tmp, GL_COMPILE_STATUS, &result);

	//エラーが出た際のチェック。
#ifdef ERR_CHECK
	if (result == GL_FALSE) {
		GLsizei bufSize;
		((PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv"))(tmp, GL_INFO_LOG_LENGTH, &bufSize);
		//最初malloc入れてたけどライブラリをリンクすることになるのでやめた
		//GLchar *infoLog = (GLchar *)malloc(bufSize);
		GLchar* infoLog = (GLchar*)GlobalAlloc(GPTR, bufSize);

		if (infoLog != NULL) {
			GLsizei length;

			((PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog"))(tmp, bufSize, &length, infoLog);
			//MyOutputDebugString("InfoLog:\n%s\n\n", infoLog);
			//free(infoLog);

			//正常終了しないケースでは、ここを通るので、ExitProcess()を仕込んでおく予定
			ExitProcess(0);

		}
		else {
			ExitProcess(0);
		}
	}
#endif

	((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(programMzk, tmp);

	const GLchar* outs[] = { O_GAIN };
	((PFNGLTRANSFORMFEEDBACKVARYINGSPROC)wglGetProcAddress("glTransformFeedbackVaryings"))(programMzk, 1, outs, GL_INTERLEAVED_ATTRIBS);
	((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(programMzk);
	((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"))(programMzk);

	//バッファ作成
	((PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers"))(1, &tmp);
	((PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer"))(GL_ARRAY_BUFFER, tmp);
	((PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData"))(GL_ARRAY_BUFFER, SND_NUMSAMPLESC * sizeof(float), 0, GL_STATIC_READ);
	((PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase"))(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tmp);

	/*
	((PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f"))(
		((PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation"))(programMzk, U_SAMPLERATE),
		(float)SAMPLE_RATE
		);
		*/
	//フィードバック開始。
	glEnable(GL_RASTERIZER_DISCARD);
	((PFNGLBEGINTRANSFORMFEEDBACKPROC)wglGetProcAddress("glBeginTransformFeedback"))(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, SND_NUMSAMPLES);
	((PFNGLENDTRANSFORMFEEDBACKPROC)wglGetProcAddress("glEndTransformFeedback"))();
	glDisable(GL_RASTERIZER_DISCARD);
	((PFNGLGETBUFFERSUBDATAPROC)wglGetProcAddress("glGetBufferSubData"))(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(samples), samples);

	//何故かこれ消したら容量減ったんだけど
	Sleep(1);

	//既に再生していたときの処理。
#ifdef ERR_CHECK
	if (hWaveOut != nullptr)
	{
		waveOutReset(hWaveOut);
#endif
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wave_format, (DWORD_PTR)g_hWnd, 0, CALLBACK_WINDOW);
	waveOutPrepareHeader(hWaveOut, &wave_hdr, sizeof(wave_hdr));
	waveOutWrite(hWaveOut, &wave_hdr, sizeof(wave_hdr));

	//CreateShaderProgram();

	//コントロールをOS側に返すためにSleep入れる
	Sleep(1);

	DWORD startTime = GetTickCount();
	int time = 0;

	/*
	while (CheckContinuable())
	{
		//時間を取得
		time = (int)GetTickCount() - startTime;

		//1920
		//1080 (16:9)

		//glRecti(1, 1, -1, -1);

		SwapBuffers(g_hDC);
		Sleep(1);

		//120000=120秒（2分）、実際はもうちょっと多いので2:30くらい
		if (time > 1)
			break;
	}
	*/

	MessageBox(NULL, "GPU Trance 3gou. \n\nby machia/machiaworks 2023", "Executable Music", MB_OK);

	//終了処理（GL側）
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(g_hGLRC);

	//終了処理（OS側）
	ReleaseDC(g_hWnd, g_hDC);

	//終了処理
#ifdef FULL_SCREEN
	PostQuitMessage(0);
#endif
	ExitProcess(0);
}