


//
//	�C���N���[�h�t�@�C���̌���
//
//	���X�g���g�p
//



#define	CommandLine



#include	<windows.h>
#include	<stdio.h>

#include	"SearchInclude.h"



char	ClassName[] = "SearchIncludeClass";
char	AppName[] = "Search Include File";



HWND		hwndMain = NULL;
HINSTANCE	hInstance = NULL;



// �󔒂��X�L�b�v���� SkipSpace()								//TAG_JUMP_MARK
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



// ���̋󔒂܂ŃX�L�b�v���� SkipToSpace()						//TAG_JUMP_MARK
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



// �w�蕶�����X�L�b�v���� SkipChar()							//TAG_JUMP_MARK
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



// ���̎w�蕶���܂ŃX�L�b�v���� SkipToChar()					//TAG_JUMP_MARK
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



// �R�}���h���C���𕪉�����
//
//	�Ăяo�����̓��������J�����邱�ƁB
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
// App" "  --->  'App" "'�����s���悤�Ƃ��ăG���[�ɂȂ�
// App"" "  --->  [App""]  []
// App""" "  --->  'App""" "'�����s���悤�Ƃ��ăG���[�ɂȂ�
// App"""" "  --->  [App""""]  []
// App"aaa  --->  'App"aaa'�����s���悤�Ƃ��ăG���[�ɂȂ�
// App""aaa  --->  'App""aaa'�����s���悤�Ƃ��ăG���[�ɂȂ�
// App"" aaa  --->  [App""]  [aaa]
//
char ** BCommandLineToArgvA( char * pCommandLine, int * pArgc )
{

	if( pCommandLine == NULL )		return NULL;
	if( pArgc == NULL )				return NULL;

	char *		pc = pCommandLine;
	ArgInfo *	pac;	// ���݂̈���
	ArgInfo *	pat;	// �����̐擪(�v���O������)
	int			nArgv = 0;

	// �v���O�������̏���
	//	�󔒂܂ł��R�s�[����B
	{
	char *	mp = pc;
	pc = SkipToSpace( pc );	// �󔒂܂ŃX�L�b�v
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
	pc = SkipSpace( pc );	// ������
	}// end

	// �����̏���

	int		fInArgv = 0;	// ����������
	int		fInDQ = 0;		// ""��������
	int		fDQ = 0;		// " ���o�� (�ŏ��� " �͏���)
	int		fYen = 0;		// �� ���o��

	// �������Ƃ̏���
	while( *pc != 0 ){

		if( *pc == '"' ){  fInArgv = fInDQ = 1;  ++pc;  }

		char *	p = pc;
		char *	mpc = pc;	// �R�K�q�[���ׂ������̐擪

		for( ; *p!=0; ++p ){

			// �X�y�[�X�̏���
			//	""���łȂ���Ώ����I��
			if(  ( *p == ' ' )|| ( *p == '\t' )  ){
				if( ! fInDQ )	break;
				fInArgv = 1;
				fYen = fDQ = 0;
				continue;
			}

			// " �̏���
			//	���O�� �� ������΂��̂܂܃X���[�B
			//	""���̏������̎��́A
			//		�����X�y�[�X�Ȃ�A���̈����̏����I���B
			//		�����X�y�[�X�łȂ����́A""���̏����I���B
			//	""���̏������łȂ����́A
			//		""���̏������n�߂�B
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

		// ArgInfo ���m��
		int	len = p - mpc;
		ArgInfo *	pa = new ArgInfo;
		if( pa == NULL )	break;
		pa->pNext = NULL;
		pa->pArgv = new char[len+1];
		if( pa->pArgv == NULL ){
			delete pa;
			break;
		}

		// �������R�s�[
		//	���h�́��������� �h�������R�s�[�B
		//	�������� �h�̓R�s�[���Ȃ��B
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

		// pat ��NULL�ł͂Ȃ�
		pac->pNext = pa;
		pac = pa;

		++nArgv;

		// ���̈�����
		pc = SkipToSpace( p );	// " ���c���Ă����炻����X�L�b�v
		pc = SkipSpace( pc );	// �X�y�[�X���X�L�b�v

	}// while

	// �Ăяo�����ɓn���z����m��
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

	// ���ꂼ��̈����̃A�h���X��z��ɃZ�b�g
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

	// ArgInfo ���폜
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

// ������𕪊�����
//	�w�肳�ꂽ��������؂�q�Ƃ��ĕ�����𕪊�����
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
//	�s�̒ǉ�
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

	TextLineInfo *	pPrev;	// �O�̍s
	TextLineInfo *	pNext;	// ���̍s

	int	fInList;	// ���X�g�ɑ����Ă����not0

	//////

	static TextLineInfo *	pTextLineTop;		// �ŏ��̍s
	static TextLineInfo *	pTextLineLast;		// �Ō�̍s
	static TextLineInfo *	pTextLineCurrent;	// ��ʂ̂P�ԏ�̍s
	static DWORD	nNumLine;		// ���X�g���̌��݂̍s��
	static DWORD	nMaxLine;		// ���X�g���̍ő�s��
	static int		nAddMode;		// �ǉ��ƍ폜�̕��@
		// 0:�ŏI�s�̎��֒ǉ��A�ő�s���𒴂������͐擪�s���폜����
		// 1:�擪�s�̑O�֒ǉ��A�ő�s���𒴂������͍ŏI�s���폜����

	//////

	public:

	TextLineInfo () {

		if( pTextLineTop == NULL )
			pTextLineTop = pTextLineLast = pTextLineCurrent = this;

		pText = NULL;  pPrev = NULL; pNext = NULL;
		nTextLength = 0;
		fInList = 0;	// �܂����X�g�ɑ����Ă��Ȃ�

	}

	~TextLineInfo () {

		Disconnect();	// ���X�g����؂藣��

		delete[] pText;
		nTextLength = 0;
		pText = NULL;  pPrev = NULL; pNext = NULL;
		fInList = 0;

	}

	// �e�L�X�g��o�^����
	char *	AddText ( char *pStr ) {

		if( pStr == NULL )		return NULL;

		delete[]	pText;	// �o�^���Ă����e�L�X�g���폜

		nTextLength = lstrlen( pStr );
		pText = new char[ nTextLength+16 ];
		if( pText == NULL )		return NULL;

		*pText = 0;
		if( nTextLength == 0 )	return pStr;

		lstrcpy( pText, pStr );

		return pStr;

	}

	// pP �̎��Ɏ�����}��
	TextLineInfo * Insert ( TextLineInfo * pP ) {

		if( pP == NULL )		return NULL;
		if( pP == pPrev )		return pP;
		if( pP == this )		return this;

		if( ( ! fInList ) && ( nNumLine >= nMaxLine )  )	return NULL;

		Disconnect();	// ���������X�g����؂藣��

		pNext = pP->pNext;
		pP->pNext = this;
		pPrev = pP;
		if( pNext != NULL )	pNext->pPrev = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();	// �ő�s���𒴂����璴���������폜

		return pP;

	}

	// ���������X�g����؂藣��
	void Disconnect () {

		if( ! fInList )		return;	// ���X�g�ɏ������Ă��Ȃ�

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

	// �擪�s�ֈړ��������͒ǉ�����
	//	����܂Ő擪�������s�̃A�h���X��Ԃ��B
	TextLineInfo * SetToTop () {

		if( pTextLineTop == this )	return this;
		if( ( ! fInList )&&( nAddMode != 0 )&&( nNumLine >= nMaxLine )  ){
			// �������܂����X�g�ɏ������Ă��Ȃ��āA
			// �ő�s�����I�[�o�[�������ɐ擪�s���폜���郂�[�h�̏ꍇ�́A
			// �ő�s���ɒB���Ă������͒ǉ�����߂�B
			return NULL;
		}

		Disconnect();	// ���������X�g����؂藣��

		if( pTextLineTop == NULL ){	// ���X�g����̏ꍇ
			pTextLineTop = this;
			pTextLineLast = this;
			pTextLineCurrent = this;
			fInList = 1;
			nNumLine = 1;
			return NULL;
		}

		// ���X�g�̐擪�Ɏ�����ǉ�

		TextLineInfo *	mp = pTextLineTop;

		pNext = mp;
		mp->pPrev = this;
		pTextLineTop = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();	// �ő�s�����I�[�o�[�����炻�̕����폜����

		return mp;

	}

	// �ŏI�s�ֈړ��������͒ǉ�����
	//	����܂Ŗ����������s�̃A�h���X��Ԃ��B
	TextLineInfo * SetToLast () {

		if( pTextLineLast == this )		return this;
		if( ( ! fInList )&&( nAddMode == 0 )&&( nNumLine >= nMaxLine )  ){
			// �������܂����X�g�ɏ������Ă��Ȃ��āA
			// �ő�s�����I�[�o�[�������ɍŏI�s���폜���郂�[�h�̏ꍇ�́A
			// �ő�s���ɒB���Ă������͒ǉ�����߂�B
			return NULL;
		}

		Disconnect();	// ���������X�g����؂藣��

		if( pTextLineLast == NULL ){	// ���X�g����̏ꍇ
			pTextLineTop = this;
			pTextLineLast = this;
			pTextLineCurrent = this;
			fInList = 1;
			nNumLine = 1;
			return NULL;
		}

		// ���X�g�̖����Ɏ�����ǉ�

		TextLineInfo *	mp = pTextLineLast;

		pPrev = mp;
		mp->pNext = this;
		pTextLineLast = this;

		fInList = 1;

		++nNumLine;
		CheckNumLine();

		return mp;

	}

	// ��������ʂ̂P�ԏ�̍s�ɂ���
	//	����܂ŉ�ʂ̂P�ԏゾ�����s�̃A�h���X��Ԃ��B
	TextLineInfo * SetToCurrent () {

		if( ! fInList )		return NULL;	// ���X�g�ɏ������Ă��Ȃ�

		TextLineInfo *	mp = pTextLineCurrent;
		pTextLineCurrent = this;
		return mp;

	}

	// �w��s����ʂ̂P�ԏ�̍s�ɂ���
	//	����܂ŉ�ʂ̂P�ԏゾ�����s�̃A�h���X��Ԃ��B
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

	// �擪���� nLine�Ԗڂ̍s��Ԃ�
	static TextLineInfo * GetLine ( DWORD nLine ) {

		TextLineInfo *	p = pTextLineTop;
		if( p == NULL )		return NULL;

		for( int i=0; i<nLine; ++i ){
			if( p->pNext == NULL )		break;
			p = p->pNext;
		}

		return p;

	}

	// ���X�g�̐擪����w��s�����폜����
	//	�V�����擪�s�̃A�h���X��Ԃ��B
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

	// ���X�g�̍Ōォ��w��s�����폜����
	//	�V���������s�̃A�h���X��Ԃ��B
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

	// ���X�g���폜����
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

	// �s���𒲂ׂ�
	//	�ő�s���𒴂��Ă�����A�����������폜����B
	//	�폜�̕��@�� nAddMode�ɏ]���B
	//	�V�����擪�s�������͖����s�̃A�h���X��Ԃ��B
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

TextLineInfo * TextLineInfo::pTextLineTop = NULL;		// �ŏ��̍s
TextLineInfo * TextLineInfo::pTextLineLast = NULL;		// �Ō�̍s
TextLineInfo * TextLineInfo::pTextLineCurrent = NULL;	// ��ʂ̂P�ԏ�̍s
DWORD	TextLineInfo::nNumLine = 0;		// ���X�g���̍s��
DWORD	TextLineInfo::nMaxLine = 1000;	// ���X�g���̍ő�s��
int		TextLineInfo::nAddMode = 0;		// �ǉ��ƍ폜�̕��@



DWORD	nLineHeight;	// �P�s�̍��� GetTextMetrics()
const DWORD	MaxLineNumber = 1000;		// �ő�s��
DWORD	nMaxLine = 0;		// ���݂̍ő�s��



SCROLLINFO	si;
int			nMaxDisp;	// �ő�\���\�s��
int			nPos;		// �X�N���[���ʒu�A�P�s�ڂɕ\������s�̔ԍ�
int			nMaxPos;	// nPos�̍ő�l+1



LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



// �������\������ Print()										//TAG_JUMP_MARK
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
	nMaxPos = nMaxLine - nMaxDisp;	// nPos�̍ő�l+1

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
	hInstance = GetModuleHandle( NULL );	// �R�}���h���C����

	// �E�C���h�E�N���X�̓o�^
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

	// �E�C���h�E�̍쐬
	{
	hwndMain = CreateWindowEx( WS_EX_CLIENTEDGE, ClassName, AppName,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_VISIBLE ,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL );
	}

	SendMessage( hwndMain, WM_VSCROLL, MAKELONG(SB_BOTTOM,0), 0L );

	// ���b�Z�[�W���[�v
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
		// ������

		HDC	hdc = GetDC( hWnd );
		TEXTMETRIC	tm;
		GetTextMetrics( hdc, &tm );
		nLineHeight = tm.tmHeight;
		ReleaseDC( hWnd, hdc );

		// �s�̍����Ŋ���؂��T�C�Y�łȂ��ƃX�N���[�������܂������Ȃ��̂�
		// �N���C�A���g�̈�̍������s�̍����̔{���ɂ���B
		RECT	r;
		GetClientRect( hWnd, &r );	// ������W�A���A����
		nPos = 0;
		nMaxLine = TextLineInfo::GetNumLine();
		nMaxDisp = r.bottom / nLineHeight;
		nMaxPos = nMaxLine - nMaxDisp;	// nPos�̍ő�l+1
		int		def = r.bottom - nMaxDisp * nLineHeight;

		GetWindowRect( hWnd, &r );	// ������W�A�E�����W+1
		MoveWindow( hWnd,
			r.left, r.top, r.right - r.left, r.bottom - r.top  - def, TRUE );
						// ������W�A���A����

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

		// �s�̍����Ŋ���؂��T�C�Y�łȂ��ƃX�N���[�������܂������Ȃ��̂�
		// �N���C�A���g�̈�̍������s�̍����̔{���ɂ���B
		RECT	r;
		GetClientRect( hWnd, &r );
		nMaxDisp = r.bottom / nLineHeight;
		nMaxPos = nMaxLine - nMaxDisp;	// nPos�̍ő�l+1
		int		def = r.bottom - nMaxDisp * nLineHeight;

		GetWindowRect( hWnd, &r );	// ������W�A�E�����W+1
		MoveWindow( hWnd,
			r.left, r.top, r.right - r.left, r.bottom - r.top  - def, TRUE );
						// ������W�A���A����

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
		// �j�������ۂł���
		DestroyWindow( hWnd );
		}// WM_CLOSE
		break;

	case WM_DESTROY:
		{
		// �j�������ۂł��Ȃ�
		PostQuitMessage( 0 );
		}// WM_DESTROY
		break;

	case WM_COMMAND:
		{

		if( lParam != 0 ){
			// lParam : �R���g���[���E�C���h�E�̃n���h��
			//	HIWORD(wParam) : �ʒm�R�[�h
			//	LOWORD(wParam) : �R���g���[��ID
			break;
		}

		if( HIWORD( wParam ) != 0 ){
			// HIWORD( wParam ) : �A�N�Z�����[�^�̃��b�Z�[�W
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
