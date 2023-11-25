


//
//	インクルードファイルの検索
//
//	リストを使用
//



#define	CommandLine



#include	<windows.h>
#include	<stdio.h>

#include	"SearchInclude.h"



char	ClassName[] = "SearchIncludeClass";
char	AppName[] = "Search Include File";



HWND		hwndMain = NULL;
HINSTANCE	hInstance = NULL;



// 空白をスキップする SkipSpace()								//TAG_JUMP_MARK
char * SkipSpace ( char * pStr )
{

	if( pStr == NULL )		return NULL;

	char *	p = pStr;

	for( ; *p != 0; ++p ){
		if( *p == ' ' )		continue;
		if( *p == '\t' )	continue;
		break;
	}

	return p;

}
//char * SkipSpace ( char * pStr )



// 次の空白までスキップする SkipToSpace()						//TAG_JUMP_MARK
char * SkipToSpace ( char * pStr )
{

	if( pStr == NULL )		return NULL;

	char *	p = pStr;

	for( ; *p != 0; ++p ){
		if( *p == ' ' )		break;
		if( *p == '\t' )	break;
	}

	return p;

}
//char * SkipToSpace ( char * pStr )



// 指定文字をスキップする SkipChar()							//TAG_JUMP_MARK
char * SkipChar ( char * pStr, char cChar )
{

	if( pStr == NULL )		return NULL;

	char *	p = pStr;

	for( ; *p != 0; ++p ){
		if( *p == cChar )		continue;
		break;
	}

	return p;

}
//char * SkipChar ( char * pStr, char cChar )



// 次の指定文字までスキップする SkipToChar()					//TAG_JUMP_MARK
char * SkipToChar ( char * pStr, char cChar )
{

	if( pStr == NULL )		return NULL;

	char *	p = pStr;

	for( ; *p != 0; ++p ){
		if( *p == cChar )		break;
	}

	return p;

}
//char * SkipToChar ( char * pStr, char cChar )



struct ArgInfo
{
	char *		pArgv;
	ArgInfo *	pNext;
};



// コマンドラインを分解する
//
//	呼び出し元はメモリを開放すること。
//		char ** p = BCommandLineToArgvA( pCommandLine, &Argc );
//		...
//		for( int i=0; i<Argc; ++i ){
//			delete[] p[i];
//		}
//		delete[] p;
//
// App  Arg1  "Arg2"  "Ar \"g\" 3"  A"rg4"  "Arg5
//						--->  [App]  [Arg1]  [Arg2]  [Ar "g" 3]  [Arg4]  [Arg5]
// App aaa"bbb  --->  [App]  [aaabbb]
// App aaa" bbb  --->  [App]  [aaa bbb]
// App aaa" "bbb  --->  [App]  [aaa bbb]
// App aaa" " bbb  --->  [App]  [aaa ]  [bbb]
// App aaa" "bbb ccc  --->  [App]  [aaa bbb]  [ccc]
// App "  --->  [App]  []
// 'App "        '  --->  [App]  [        ]
// App aaa" "bbb" "ccc ddd  --->  [App]  [aaa bbb ccc]  [ddd]
// App"  --->  [App"]
// App""  --->  [App""]
// App" "  --->  'App" "'を実行しようとしてエラーになる
// App"" "  --->  [App""]  []
// App""" "  --->  'App""" "'を実行しようとしてエラーになる
// App"""" "  --->  [App""""]  []
// App"aaa  --->  'App"aaa'を実行しようとしてエラーになる
// App""aaa  --->  'App""aaa'を実行しようとしてエラーになる
// App"" aaa  --->  [App""]  [aaa]
//
char ** BCommandLineToArgvA( char * pCommandLine, int * pArgc )
{

	if( pCommandLine == NULL )		return NULL;
	if( pArgc == NULL )				return NULL;

	char *		pc = pCommandLine;
	ArgInfo *	pac;	// 現在の引数
	ArgInfo *	pat;	// 引数の先頭(プログラム名)
	int			nArgv = 0;

	// プログラム名の処理
	//	空白までをコピーする。
	{
	char *	mp = pc;
	pc = SkipToSpace( pc );	// 空白までスキップ
	int	len = pc - mp;
	char *	ps = new char[len+1];
	if( ps == NULL )	return NULL;
	CopyMemory( ps, mp, len );
	ps[len] = 0;
	pat = pac = new ArgInfo;
	if( pac == NULL ){
		delete[] ps;
		return NULL;
	}
	pac->pNext = NULL;
	pac->pArgv = ps;
	++nArgv;
	pc = SkipSpace( pc );	// 引数へ
	}// end

	// 引数の処理

	int		fInArgv = 0;	// 引数処理中
	int		fInDQ = 0;		// ""内処理中
	int		fDQ = 0;		// " が出た (最初の " は除く)
	int		fYen = 0;		// ￥ が出た

	// 引数ごとの処理
	while( *pc != 0 ){

		if( *pc == '"' ){  fInArgv = fInDQ = 1;  ++pc;  }

		char *	p = pc;
		char *	mpc = pc;	// コ゜ヒーすべき引数の先頭

		for( ; *p!=0; ++p ){

			// スペースの処理
			//	""内でなければ処理終了
			if(  ( *p == ' ' )|| ( *p == '\t' )  ){
				if( ! fInDQ )	break;
				fInArgv = 1;
				fYen = fDQ = 0;
				continue;
			}

			// " の処理
			//	直前に ￥ があればそのままスルー。
			//	""内の処理中の時は、
			//		次がスペースなら、この引数の処理終了。
			//		次がスペースでない時は、""内の処理終了。
			//	""内の処理中でない時は、
			//		""内の処理を始める。
			if( *p == '"' ){
				if( ! fYen ){
					if( ! fInDQ )	fInDQ = 1;
					else{
						if(  ( *(p+1) == ' ' )||( *(p+1) == '\t' )  )
							break;
						else
							fInDQ = 0;
					}
				}
				fInArgv = 1;
				fYen = 0;
				fDQ = 1;
				continue;
			}

			fYen = 0;

			if( *p == '\\' )	fYen = 1;

			fInArgv = 1;

		}// for
		fInArgv = fInDQ = 0;
		fYen = fDQ = 0;

		// ArgInfo を確保
		int	len = p - mpc;
		ArgInfo *	pa = new ArgInfo;
		if( pa == NULL )	break;
		pa->pNext = NULL;
		pa->pArgv = new char[len+1];
		if( pa->pArgv == NULL ){
			delete pa;
			break;
		}

		// 引数をコピー
		//	￥”は￥を消して ”だけをコピー。
		//	￥無しの ”はコピーしない。
		{
		char *	pS = mpc;
		char *	pD = pa->pArgv;
		for( int i=0; i<len; ++i ){
			if( *pS == '"' ){  ++pS;  continue;  }
			if( *pS == '\\' ){
				if( *(pS+1) == '"' ){  ++pS;  ++i;  }
			}
			*pD++ = *pS++;
		}
		*pD = 0;
		}// end

		// pat はNULLではない
		pac->pNext = pa;
		pac = pa;

		++nArgv;

		// 次の引数へ
		pc = SkipToSpace( p );	// " が残っていたらそれをスキップ
		pc = SkipSpace( pc );	// スペースをスキップ

	}// while

	// 呼び出し元に渡す配列を確保
	char **	pArgv = new char*[nArgv];
	if( pArgv == NULL ){
		ArgInfo *	p = pat;
		while( p != NULL ){
			ArgInfo * mp = p->pNext;
			delete[]	p->pArgv;
			delete		p;
			p = mp;
		}
		return NULL;
	}

	// それぞれの引数のアドレスを配列にセット
	{
	ArgInfo *	p = pat;
	for( int i=0; i<nArgv; ++i ){
		pArgv[i] = NULL;
		if( p != NULL ){
			pArgv[i] = p->pArgv;
			p = p->pNext;
		}
	}
	}// end

	// ArgInfo を削除
	{
	ArgInfo *	p = pat;
	while( p != NULL ){
		ArgInfo * mp = p->pNext;
		delete	p;
		p = mp;
	}
	}// end

	*pArgc = nArgv;

	return pArgv;

}



class SplitInfo
{

	SplitInfo *	pNext;
	char *		pStr;

	public:

	SplitInfo () {
		pNext = NULL;  pStr = NULL;
	}

	SplitInfo ( char * pS ) {
		pNext = NULL;
		pStr = NULL;
		if( pS == NULL )	return;
		int	n = lstrlen( pS );
		pStr = new char[ n + 8 ];
		if( pStr == NULL )	return;
		lstrcpy( pStr, pS );
	}

	~SplitInfo () {
		delete[] pStr;
		pStr = NULL;
		pNext = NULL;
	}

	SplitInfo * GetNext () {  return pNext;  }
	char *	GetStr () {  return pStr;  }

	void Connect ( SplitInfo * p ) {
		if( p == NULL )		return;
		if( pNext != NULL )	DeleteAll();
		pNext = p;
	}

	void DeleteAll () {
		SplitInfo *	p = pNext;
		pNext = NULL;
		while( p != NULL ){
			SplitInfo *	mp = p->pNext;
			delete p;
			p = mp;
		}
	}

};

// 文字列を分割する
//	指定された文字を区切り子として文字列を分割する
SplitInfo * Split ( char *pStr, char cSeparator )
{

	if( pStr == NULL )	return NULL;

	char *	str = SkipChar( pStr, cSeparator );

	SplitInfo *	pC = NULL;
	SplitInfo *	pT = NULL;
	while( *str != 0 ) {
		char *	mp = str;
		str = SkipToChar( str, cSeparator );
		int		n = str - mp;
		char *	p = new char[ n + 1 ];
		if( p == NULL )	break;
		CopyMemory( p, mp, n );
		p[n] = 0;
		SplitInfo *	pSD = new SplitInfo( p );
		delete[] p;
		if( pSD == NULL )	break;
		if( pT == NULL )	pT = pSD;
		else	pC->Connect( pSD );
		pC = pSD;
		if( *str == 0 )		break;
		str = SkipChar( str, cSeparator );
	}

	return pT;

}



//
//	行の追加
//		TextLineInfo *	p = new TextLineInfo;
//		if( p == NULL )		...
//		if( p->AddText("text") == NULL ){
//			delete p;
//			...
//		}
//		p->SetToLast();
//
class TextLineInfo
{

	char *			pText;
	DWORD			nTextLength;

	TextLineInfo *	pPrev;	// 前の行
	TextLineInfo *	pNext;	// 次の行

	int	fInList;	// リストに属していればnot0

	//////

	static TextLineInfo *	pTextLineTop;		// 最初の行
	static TextLineInfo *	pTextLineLast;		// 最後の行
	static TextLineInfo *	pTextLineCurrent;	// 画面の１番上の行
	static DWORD	nNumLine;		// リスト中の現在の行数
	static DWORD	nMaxLine;		// リスト中の最大行数
	static int		nAddMode;		// 追加と削除の方法
		// 0:最終行の次へ追加、最大行数を超えた時は先頭行を削除する
		// 1:先頭行の前へ追加、最大行数を超えた時は最終行を削除する

	//////

	public:

	TextLineInfo () {

		if( pTextLineTop == NULL )
			pTextLineTop = pTextLineLast = pTextLineCurrent = this;

		pText = NULL;  pPrev = NULL; pNext = NULL;
		nTextLength = 0;
		fInList = 0;	// まだリストに属していない

	}

	~TextLineInfo () {

		Disconnect();	// リストから切り離す

		delete[] pText;
		nTextLength = 0;
		pText = NULL;  pPrev = NULL; pNext = NULL;
		fInList = 0;

	}

	// テキストを登録する
	char *	AddText ( char *pStr ) {

		if( pStr == NULL )		return NULL;

		delete[]	pText;	// 登録していたテキストを削除

		nTextLength = lstrlen( pStr );
		pText = new char[ nTextLength+16 ];
		if( pText == NULL )		return NULL;

		*pText = 0;
		if( nTextLength == 0 )	return pStr;

		lstrcpy( pText, pStr );

		return pStr;

	}

	// pP の次に自分を挿入
	TextLineInfo * Insert ( TextLineInfo * pP ) {

		if( pP == NULL )		return NULL;
		if( pP == pPrev )		return pP;
		if( pP == this )		return this;

		if( ( ! fInList ) && ( nNumLine >= nMaxLine )  )	return NULL;

		Disconnect();	// 自分をリストから切り離す

		pNext = pP->pNext;
		pP->pNext = this;
		pPrev = pP;
		if( pNext != NULL )	pNext->pPrev = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();	// 最大行数を超えたら超えた分を削除

		return pP;

	}

	// 自分をリストから切り離す
	void Disconnect () {

		if( ! fInList )		return;	// リストに所属していない

		if( pTextLineTop == this )		pTextLineTop = pNext;
		if( pTextLineLast == this )		pTextLineLast = pPrev;
		if( pTextLineCurrent == this ){
			if( pNext != NULL )	pTextLineCurrent = pNext;
			else				pTextLineCurrent = pPrev;
		}

		if( pPrev != NULL )		pPrev->pNext = pNext;
		if( pNext != NULL )		pNext->pPrev = pPrev;

		pNext = pPrev = NULL;
		fInList = 0;

		if( nNumLine != 0 )	--nNumLine;

	}

	// 先頭行へ移動もしくは追加する
	//	それまで先頭だった行のアドレスを返す。
	TextLineInfo * SetToTop () {

		if( pTextLineTop == this )	return this;
		if( ( ! fInList )&&( nAddMode != 0 )&&( nNumLine >= nMaxLine )  ){
			// 自分がまだリストに所属していなくて、
			// 最大行数をオーバーした時に先頭行を削除するモードの場合は、
			// 最大行数に達していた時は追加を諦める。
			return NULL;
		}

		Disconnect();	// 自分をリストから切り離す

		if( pTextLineTop == NULL ){	// リストが空の場合
			pTextLineTop = this;
			pTextLineLast = this;
			pTextLineCurrent = this;
			fInList = 1;
			nNumLine = 1;
			return NULL;
		}

		// リストの先頭に自分を追加

		TextLineInfo *	mp = pTextLineTop;

		pNext = mp;
		mp->pPrev = this;
		pTextLineTop = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();	// 最大行数をオーバーしたらその分を削除する

		return mp;

	}

	// 最終行へ移動もしくは追加する
	//	それまで末尾だった行のアドレスを返す。
	TextLineInfo * SetToLast () {

		if( pTextLineLast == this )		return this;
		if( ( ! fInList )&&( nAddMode == 0 )&&( nNumLine >= nMaxLine )  ){
			// 自分がまだリストに所属していなくて、
			// 最大行数をオーバーした時に最終行を削除するモードの場合は、
			// 最大行数に達していた時は追加を諦める。
			return NULL;
		}

		Disconnect();	// 自分をリストから切り離す

		if( pTextLineLast == NULL ){	// リストが空の場合
			pTextLineTop = this;
			pTextLineLast = this;
			pTextLineCurrent = this;
			fInList = 1;
			nNumLine = 1;
			return NULL;
		}

		// リストの末尾に自分を追加

		TextLineInfo *	mp = pTextLineLast;

		pPrev = mp;
		mp->pNext = this;
		pTextLineLast = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();

		return mp;

	}

	// 自分を画面の１番上の行にする
	//	それまで画面の１番上だった行のアドレスを返す。
	TextLineInfo * SetToCurrent () {

		if( ! fInList )		return NULL;	// リストに所属していない

		TextLineInfo *	mp = pTextLineCurrent;
		pTextLineCurrent = this;
		return mp;

	}

	// 指定行を画面の１番上の行にする
	//	それまで画面の１番上だった行のアドレスを返す。
	TextLineInfo * SetToCurrent ( DWORD nLine ) {

		TextLineInfo *	mp = pTextLineCurrent;
		pTextLineCurrent = GetLine( nLine );
		return mp;

	}

	char *	GetTextLine () {  return pText;  }
	DWORD	GetTextLength () {  return nTextLength;  }
	TextLineInfo * GetNextLine () {  return pNext;  }
	TextLineInfo * GetPrevLine () {  return pPrev;  }

	TextLineInfo * GetTopLine () {  return pTextLineTop;  }
	TextLineInfo * GetLastLine () {  return pTextLineLast;  }
	static TextLineInfo * GetCurrentLine () {  return pTextLineCurrent;  }
	static TextLineInfo * SetCurrentLine ( DWORD nLine ){
		TextLineInfo *	mp = pTextLineCurrent;
		pTextLineCurrent = GetLine( nLine );
		return mp;
	}
	static DWORD	GetNumLine () {  return nNumLine;  }
	static void		SetNumLine ( DWORD n ){  nNumLine = n;  }
	static DWORD	GetMaxLine () {  return nMaxLine;  }
	static void		SetMaxLine ( DWORD n ){  nMaxLine = n;  }
	static int		GetAddMode () {  return nAddMode;  }
	static void		SetAddMode ( int n ){  nAddMode = n;  }

	// 先頭から nLine番目の行を返す
	static TextLineInfo * GetLine ( DWORD nLine ) {

		TextLineInfo *	p = pTextLineTop;
		if( p == NULL )		return NULL;

		for( int i=0; i<nLine; ++i ){
			if( p->pNext == NULL )		break;
			p = p->pNext;
		}

		return p;

	}

	// リストの先頭から指定行数を削除する
	//	新しい先頭行のアドレスを返す。
	TextLineInfo * DeleteLinesTop ( DWORD nLine ) {

		TextLineInfo *	p = pTextLineTop;
		for( int i=0; i<nLine; ++i ){
			if( p == NULL )	break;
			TextLineInfo *	mp = p->pNext;
			delete p;
			p = mp;
		}
		return pTextLineTop = p;

	}

	// リストの最後から指定行数を削除する
	//	新しい末尾行のアドレスを返す。
	TextLineInfo * DeleteLinesLast ( DWORD nLine ) {

		TextLineInfo *	p = pTextLineLast;
		for( int i=0; i<nLine; ++i ){
			if( p == NULL )	break;
			TextLineInfo *	mp = p->pPrev;
			delete p;
			p = mp;
		}
		return pTextLineLast = p;

	}

	// リストを削除する
	static void DeleteTextLineAll () {

		TextLineInfo *	p = pTextLineTop;
		while( p != NULL ){
			TextLineInfo *	mp = p->pNext;
			delete p;
			p = mp;
		}
		nNumLine = 0;
		pTextLineTop = pTextLineLast = pTextLineCurrent = NULL;

	}

	// 行数を調べる
	//	最大行数を超えていたら、超えた分を削除する。
	//	削除の方法は nAddModeに従う。
	//	新しい先頭行もしくは末尾行のアドレスを返す。
	TextLineInfo * CheckNumLine () {

		if( nNumLine <= nMaxLine )		return NULL;

		int		n = nMaxLine - nNumLine;
		if( nAddMode == 0 )	return DeleteLinesTop( n );
		else				return DeleteLinesLast( n );

	}

	static void PrintAll () {

		TextLineInfo *	p = pTextLineTop;
		while( p != NULL ){
			printf( "%s\n", p->pText );
			p = p->pNext;
		}

	}

};

TextLineInfo * TextLineInfo::pTextLineTop = NULL;		// 最初の行
TextLineInfo * TextLineInfo::pTextLineLast = NULL;		// 最後の行
TextLineInfo * TextLineInfo::pTextLineCurrent = NULL;	// 画面の１番上の行
DWORD	TextLineInfo::nNumLine = 0;		// リスト中の行数
DWORD	TextLineInfo::nMaxLine = 1000;	// リスト中の最大行数
int		TextLineInfo::nAddMode = 0;		// 追加と削除の方法



DWORD	nLineHeight;	// １行の高さ GetTextMetrics()
const DWORD	MaxLineNumber = 1000;		// 最大行数
DWORD	nMaxLine = 0;		// 現在の最大行数



SCROLLINFO	si;
int			nMaxDisp;	// 最大表示可能行数
int			nPos;		// スクロール位置、１行目に表示する行の番号
int			nMaxPos;	// nPosの最大値+1



LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



// 文字列を表示する Print()										//TAG_JUMP_MARK
void Print ( char * pStr )
{

	if( pStr == NULL )		return;

	#ifdef CommandLine
	printf( "%s\n", pStr );
	#endif

	TextLineInfo * p = new TextLineInfo;
	if( p == NULL )		return;
	if( p->AddText( pStr ) == NULL ){  delete p;  return;  }
	p->SetToLast();

	nMaxLine = p->GetNumLine();
	nMaxPos = nMaxLine - nMaxDisp;	// nPosの最大値+1

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE ;
	si.nPos = nPos;
	si.nMin = 0;
	si.nMax = nMaxLine;
	si.nPage = nMaxDisp;
	SetScrollInfo( hwndMain, SB_VERT, &si, TRUE );
	InvalidateRect( hwndMain, NULL, TRUE );
	UpdateWindow( hwndMain );

	SendMessage( hwndMain, WM_VSCROLL, MAKELONG(SB_BOTTOM,0), 0L );

}
//void Print ( char * pStr )



// WinMain()													//TAG_JUMP_MARK
#ifdef CommandLine
int main()
#else
int WinMain( HINSTANCE hInst, HINSTANCE hPrevInst,
												char * CmdLine, int CmdShow )
#endif
{

	//hInstance = hInst;
	hInstance = GetModuleHandle( NULL );	// コマンドライン版

	// ウインドウクラスの登録
	{
	WNDCLASSEX	wc;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	//wc.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "FirstMenu";
	wc.lpszClassName = ClassName;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	RegisterClassEx( &wc );
	}

	// ウインドウの作成
	{
	hwndMain = CreateWindowEx( WS_EX_CLIENTEDGE, ClassName, AppName,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_VISIBLE ,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );
	}

	SendMessage( hwndMain, WM_VSCROLL, MAKELONG(SB_BOTTOM,0), 0L );

	// メッセージループ
	MSG	msg;
	while( GetMessage( &msg, NULL, 0, 0 ) ){
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	TextLineInfo::DeleteTextLineAll();

	return msg.wParam;

}



// WndProc()												//TAG_JUMP_MARK
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{


	switch( uMsg ){

	case WM_CREATE:
		{
		// 初期化

		HDC	hdc = GetDC( hWnd );
		TEXTMETRIC	tm;
		GetTextMetrics( hdc, &tm );
		nLineHeight = tm.tmHeight;
		ReleaseDC( hWnd, hdc );

		// 行の高さで割り切れるサイズでないとスクロールがうまくいかないので
		// クライアント領域の高さを行の高さの倍数にする。
		RECT	r;
		GetClientRect( hWnd, &r );	// 左上座標、幅、高さ
		nPos = 0;
		nMaxLine = TextLineInfo::GetNumLine();
		nMaxDisp = r.bottom / nLineHeight;
		nMaxPos = nMaxLine - nMaxDisp;	// nPosの最大値+1
		int		def = r.bottom - nMaxDisp * nLineHeight;

		GetWindowRect( hWnd, &r );	// 左上座標、右下座標+1
		MoveWindow( hWnd,
			r.left, r.top, r.right - r.left, r.bottom - r.top  - def, TRUE );
						// 左上座標、幅、高さ

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE ;
		si.nPos = nPos;
		si.nMin = 0;
		si.nMax = nMaxLine;
		si.nPage = nMaxDisp;
		SetScrollInfo( hWnd, SB_VERT, &si, TRUE );

		}// WM_CREATE
		break;

	case WM_SIZE:
		{

		// 行の高さで割り切れるサイズでないとスクロールがうまくいかないので
		// クライアント領域の高さを行の高さの倍数にする。
		RECT	r;
		GetClientRect( hWnd, &r );
		nMaxDisp = r.bottom / nLineHeight;
		nMaxPos = nMaxLine - nMaxDisp;	// nPosの最大値+1
		int		def = r.bottom - nMaxDisp * nLineHeight;

		GetWindowRect( hWnd, &r );	// 左上座標、右下座標+1
		MoveWindow( hWnd,
			r.left, r.top, r.right - r.left, r.bottom - r.top  - def, TRUE );
						// 左上座標、幅、高さ

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE ;
		si.nPos = nPos;
		si.nMin = 0;
		si.nMax = nMaxLine;
		si.nPage = nMaxDisp;
		SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
		UpdateWindow( hWnd );

		SendMessage( hwndMain, WM_VSCROLL, MAKELONG(SB_BOTTOM,0), 0L );

		}// WM_SIZE
		break;

	case WM_VSCROLL:
		{

		if( nMaxLine < nMaxDisp )	break;

		int		dy;

		switch( LOWORD(wParam) ){

		case SB_LINEUP:		dy = -1;						break;
		case SB_LINEDOWN:	dy = 1;							break;
		case SB_PAGEUP:		dy = -1 * si.nPage;				break;
		case SB_PAGEDOWN:	dy = 1 * si.nPage;				break;
		case SB_TOP:		dy = -nPos;						break;
		case SB_BOTTOM:		dy = nMaxLine - nPos;			break;
		case SB_THUMBTRACK:	dy = HIWORD(wParam) - si.nPos;	break;
		default:			dy = 0;							break;

		}// switch

		if( dy == 0 )	return 0;

		int	n = nPos + dy;
		if( n < 0 )				n = 0;
		if( n > nMaxPos )		n = nMaxPos;
		dy = n - nPos;

		//printf( "nPos = %d  n = %d  dy = %d  nMaxLine = %d\n",
		//											nPos, n, dy, nMaxLine );

		if( dy == 0 )	return 0;
		if( n == nPos )	return 0;

		//printf( "nPos = %d  n = %d  dy = %d  nMaxLine = %d\n",
		//											nPos, n, dy, nMaxLine );

		si.nPos = n;
		nPos = n;
		TextLineInfo::SetCurrentLine( n );
		SetScrollInfo( hWnd, SB_VERT, &si, TRUE );
		//ScrollWindow( hWnd, 0, -dy*nLineHeight, NULL, NULL );
		ScrollWindowEx( hWnd, 0, -dy*nLineHeight, NULL, NULL, NULL, NULL,
													SW_INVALIDATE | SW_ERASE );
		UpdateWindow( hWnd );

		}// WM_VSCROLL
		return 0;

	case WM_PAINT:
		{

		PAINTSTRUCT	ps;
		BeginPaint( hWnd, &ps );
		TextLineInfo * pLine = TextLineInfo::GetCurrentLine();
		for( int i=0; i<nMaxDisp; ++i ){

			if( i + si.nPos > nMaxLine )	break;
			if( pLine == NULL )				break;

			TextOut( ps.hdc, 10, i*nLineHeight,
							pLine->GetTextLine(), pLine->GetTextLength() );
			pLine = pLine->GetNextLine();

		}// for
		EndPaint( hWnd, &ps );

		}// WM_PAINT
		break;

	case WM_KEYDOWN:
		{
		WORD	wScrollNotify = 0xFFFF;

		switch( wParam ){
		case VK_UP:			wScrollNotify = SB_LINEUP;		break;
		case VK_DOWN:		wScrollNotify = SB_LINEDOWN;	break;
		case VK_PRIOR:		wScrollNotify = SB_PAGEUP;		break;
		case VK_NEXT:		wScrollNotify = SB_PAGEDOWN;	break;
		case VK_HOME:		wScrollNotify = SB_TOP;			break;
		case VK_END:		wScrollNotify = SB_BOTTOM;		break;
		}// switch

		if( wScrollNotify != 0xffff )
			SendMessage( hwndMain, WM_VSCROLL, MAKELONG(wScrollNotify,0), 0L );

		}// WM_KEYDOWN
		break;

	case WM_MOUSEWHEEL:
		// WORD GET_KEYSTATE_WPARAM(wParam)
		// short GET_WHEEL_DELTA_WPARAM(wParam)
		// short GET_X_LPARAM(lParam)
		// short GET_Y_LPARAM(lParam)
		{

		WORD	wScrollNotify = SB_LINEUP;
		if( GET_WHEEL_DELTA_WPARAM(wParam) < 0 )
			wScrollNotify = SB_LINEDOWN;
		SendMessage( hwndMain, WM_VSCROLL, MAKELONG(wScrollNotify,0), 0L );

		//printf( "%d\n", GET_WHEEL_DELTA_WPARAM(wParam) );

		}// WM_MOUSEWHEEL
		break;

	case WM_CLOSE:
		{
		// 破棄を拒否できる
		DestroyWindow( hWnd );
		}// WM_CLOSE
		break;

	case WM_DESTROY:
		{
		// 破棄を拒否できない
		PostQuitMessage( 0 );
		}// WM_DESTROY
		break;

	case WM_COMMAND:
		{

		if( lParam != 0 ){
			// lParam : コントロールウインドウのハンドル
			//	HIWORD(wParam) : 通知コード
			//	LOWORD(wParam) : コントロールID
			break;
		}

		if( HIWORD( wParam ) != 0 ){
			// HIWORD( wParam ) : アクセラレータのメッセージ
			break;
		}

		switch( LOWORD( wParam ) ){

		case IDM_OPEN:
			{

			// MessageBox( NULL, getenv("INCLUDE"), AppName, MB_OK );

			SplitInfo *	p = Split( getenv("INCLUDE"), ';' );
			SplitInfo *	mp = p;
			while ( p != NULL ){
				Print( p->GetStr() );
				p = p->GetNext();
			}
			mp->DeleteAll();
			delete mp;

			//TextLineInfo::PrintAll();

			}// IDM_OPEN
			break;

		case IDM_EXIT:
			{

			PostQuitMessage( 0 );

			}// IDM_EXIT
			break;

		default:
			DestroyWindow( hWnd );

		}// switch

		}// WM_COMMAND
		break;

	default:

		return DefWindowProc( hWnd, uMsg, wParam, lParam );

	}// switch


	return 0;

}



// end of this file : ChkExe.cpp
