/*
 *	oneko  -  X11 猫
 */

#ifndef	lint
static char rcsid[] = "$Header: /home/sun/unix/kato/xsam/oneko/oneko.c,v 1.5 90/10/19 21:25:16 kato Exp $";
#endif

#include "oneko.h"
#include "patchlevel.h"
/*
 *	グローバル変数
 */

char	*ClassName = "Oneko";		/* コマンド名称 */
char	*ProgramName;			/* コマンド名称 */

Display	*theDisplay;			/* ディスプレイ構造体 */
int	theScreen;			/* スクリーン番号 */
unsigned int	theDepth;		/* デプス */
Window	theRoot;			/* ルートウィンドウのＩＤ */
Window	theWindow;			/* 猫ウィンドウのＩＤ */
Window	theTarget = NOTDEFINED;		/* 目標ウィンドウのＩＤ */
Cursor	theCursor;			/* ねずみカーソル */

unsigned int	WindowWidth;		/* ルートウィンドウの幅 */
unsigned int	WindowHeight;		/* ルートウィンドウの高さ */

XColor	theForegroundColor;		/* 色 (フォアグラウンド) */
XColor	theBackgroundColor;		/* 色 (バックグラウンド) */

int Synchronous = False;
/* Types of animals */
#define BITMAPTYPES 5
typedef struct _AnimalDefaults {
  char *name;
  int speed, idle, bitmap_width, bitmap_height;
  long time;
  int off_x, off_y;
  char *cursor,*mask;
  int cursor_width,cursor_height,cursor_x_hot,cursor_y_hot;
} AnimalDefaultsData;

AnimalDefaultsData AnimalDefaultsDataTable[] = 
{
  { "neko", 13, 6, 32, 32, 125000L, 0, 0, mouse_cursor_bits,mouse_cursor_mask_bits,
      mouse_cursor_width,mouse_cursor_height, mouse_cursor_x_hot,mouse_cursor_y_hot },
  { "tora", 16, 6, 32, 32, 125000L, 0, 0, mouse_cursor_bits,mouse_cursor_mask_bits,
      mouse_cursor_width,mouse_cursor_height, mouse_cursor_x_hot,mouse_cursor_y_hot },
  { "dog" , 10, 6, 32, 32, 125000L, 0, 0, bone_cursor_bits,bone_cursor_mask_bits,
      bone_cursor_width,bone_cursor_height, bone_cursor_x_hot,bone_cursor_y_hot },
  { "bsd_daemon" , 16, 6, 32, 32, 300000L, 22, 20, bsd_cursor_bits,bsd_cursor_mask_bits,
      bsd_cursor_width,bsd_cursor_height, bsd_cursor_x_hot,bsd_cursor_y_hot },
  { "sakura" , 13, 6, 32, 32, 125000L, 0, 0, card_cursor_bits,card_cursor_mask_bits,
      card_cursor_width,card_cursor_height, card_cursor_x_hot,card_cursor_y_hot },
};

/*
 *	いろいろな初期設定 (オプション、リソースで変えられるよ)
 */

					/* Resource:	*/
char	*Foreground = NULL;		/*   foreground	*/
char	*Background = NULL;		/*   background	*/
long	IntervalTime = 0L;		/*   time	*/
double	NekoSpeed = (double)0;		/*   speed	*/
int	IdleSpace = 0;			/*   idle	*/
int	NekoMoyou = NOTDEFINED;		/*   tora	*/
int	NoShape = NOTDEFINED;		/*   noshape	*/
int	ReverseVideo = NOTDEFINED;	/*   reverse	*/
int	ToWindow = NOTDEFINED;		/*   towindow	*/
int	ToFocus = NOTDEFINED;		/*   tofocus	*/
int     XOffset=0,YOffset=0;            /* X and Y offsets for cat from mouse
					   pointer. */
/*
 *	いろいろな状態変数
 */

Bool	DontMapped = True;

int	NekoTickCount;		/* 猫動作カウンタ */
int	NekoStateCount;		/* 猫同一状態カウンタ */
int	NekoState;		/* 猫の状態 */

int	MouseX;			/* マウスＸ座標 */
int	MouseY;			/* マウスＹ座標 */

int	PrevMouseX = 0;		/* 直前のマウスＸ座標 */
int	PrevMouseY = 0;		/* 直前のマウスＹ座標 */

int	NekoX;			/* 猫Ｘ座標 */
int	NekoY;			/* 猫Ｙ座標 */

int	NekoMoveDx;		/* 猫移動距離Ｘ */
int	NekoMoveDy;		/* 猫移動距離Ｙ */

int	NekoLastX;		/* 猫最終描画Ｘ座標 */
int	NekoLastY;		/* 猫最終描画Ｙ座標 */
GC	NekoLastGC;		/* 猫最終描画 GC */
/* Variables used to set how quickly the program will chose to raise itself. */
/* Look at Interval(), Handle Visiblility Notify Event */
#define DEFAULT_RAISE_WAIT 16  /* About 2 seconds with default interval */
int     RaiseWindowDelay=0;
/*
 *	その他
 */

double	SinPiPer8Times3;	/* sin(３π／８) */
double	SinPiPer8;		/* sin(π／８) */

Pixmap	Mati2Xbm, Jare2Xbm, Kaki1Xbm, Kaki2Xbm, Mati3Xbm, Sleep1Xbm, Sleep2Xbm;
Pixmap	Mati2Msk, Jare2Msk, Kaki1Msk, Kaki2Msk, Mati3Msk, Sleep1Msk, Sleep2Msk;

Pixmap	AwakeXbm, AwakeMsk;

Pixmap	Up1Xbm, Up2Xbm, Down1Xbm, Down2Xbm, Left1Xbm, Left2Xbm;
Pixmap	Up1Msk, Up2Msk, Down1Msk, Down2Msk, Left1Msk, Left2Msk;
Pixmap	Right1Xbm, Right2Xbm, UpLeft1Xbm, UpLeft2Xbm, UpRight1Xbm;
Pixmap	Right1Msk, Right2Msk, UpLeft1Msk, UpLeft2Msk, UpRight1Msk;
Pixmap	UpRight2Xbm, DownLeft1Xbm, DownLeft2Xbm, DownRight1Xbm, DownRight2Xbm;
Pixmap	UpRight2Msk, DownLeft1Msk, DownLeft2Msk, DownRight1Msk, DownRight2Msk;

Pixmap	UpTogi1Xbm, UpTogi2Xbm, DownTogi1Xbm, DownTogi2Xbm, LeftTogi1Xbm;
Pixmap	UpTogi1Msk, UpTogi2Msk, DownTogi1Msk, DownTogi2Msk, LeftTogi1Msk;
Pixmap	LeftTogi2Xbm, RightTogi1Xbm, RightTogi2Xbm;
Pixmap	LeftTogi2Msk, RightTogi1Msk, RightTogi2Msk;

GC      Mati2GC;

GC	Jare2GC, Kaki1GC, Kaki2GC, Mati3GC, Sleep1GC, Sleep2GC;

GC	AwakeGC;

GC	Up1GC, Up2GC, Down1GC, Down2GC, Left1GC, Left2GC, Right1GC, Right2GC;
GC	UpLeft1GC, UpLeft2GC, UpRight1GC, UpRight2GC, DownLeft1GC, DownLeft2GC;
GC	DownRight1GC, DownRight2GC;

GC	UpTogi1GC, UpTogi2GC, DownTogi1GC, DownTogi2GC, LeftTogi1GC;
GC	LeftTogi2GC, RightTogi1GC, RightTogi2GC;


typedef struct {
    GC		*GCCreatePtr;
    Pixmap	*BitmapCreatePtr;
    char	*PixelPattern[BITMAPTYPES];
    Pixmap	*BitmapMasksPtr;
    char	*MaskPattern[BITMAPTYPES];
} BitmapGCData;

BitmapGCData	BitmapGCDataTable[] =
{
    { &Mati2GC, &Mati2Xbm,  mati2_bits, mati2_tora_bits, mati2_dog_bits, mati2_bsd_bits, mati2_sakura_bits,
      &Mati2Msk, mati2_mask_bits, mati2_mask_bits, mati2_dog_mask_bits, mati2_bsd_mask_bits, mati2_sakura_mask_bits },
    { &Jare2GC, &Jare2Xbm,  jare2_bits, jare2_tora_bits, jare2_dog_bits, jare2_bsd_bits, jare2_sakura_bits,
      &Jare2Msk, jare2_mask_bits, jare2_mask_bits, jare2_dog_mask_bits, jare2_bsd_mask_bits, jare2_sakura_mask_bits },
    { &Kaki1GC, &Kaki1Xbm,  kaki1_bits, kaki1_tora_bits, kaki1_dog_bits, kaki1_bsd_bits, kaki1_sakura_bits,
      &Kaki1Msk, kaki1_mask_bits, kaki1_mask_bits, kaki1_dog_mask_bits, kaki1_bsd_mask_bits, kaki1_sakura_mask_bits },
    { &Kaki2GC, &Kaki2Xbm,  kaki2_bits, kaki2_tora_bits, kaki2_dog_bits, kaki2_bsd_bits, kaki2_sakura_bits,
      &Kaki2Msk, kaki2_mask_bits, kaki2_mask_bits, kaki2_dog_mask_bits, kaki2_bsd_mask_bits, kaki2_sakura_mask_bits },
    { &Mati3GC, &Mati3Xbm,  mati3_bits, mati3_tora_bits, mati3_dog_bits, mati3_bsd_bits, mati3_sakura_bits,
      &Mati3Msk, mati3_mask_bits, mati3_mask_bits, mati3_dog_mask_bits, mati3_bsd_mask_bits, mati3_sakura_mask_bits },
    { &Sleep1GC, &Sleep1Xbm,  sleep1_bits, sleep1_tora_bits, sleep1_dog_bits, sleep1_bsd_bits, sleep1_sakura_bits,
      &Sleep1Msk, sleep1_mask_bits, sleep1_mask_bits, sleep1_dog_mask_bits, sleep1_bsd_mask_bits, sleep1_sakura_mask_bits },
    { &Sleep2GC, &Sleep2Xbm,  sleep2_bits, sleep2_tora_bits, sleep2_dog_bits, sleep2_bsd_bits, sleep2_sakura_bits,
      &Sleep2Msk, sleep2_mask_bits, sleep2_mask_bits, sleep2_dog_mask_bits, sleep2_bsd_mask_bits, sleep2_sakura_mask_bits },
    { &AwakeGC, &AwakeXbm,  awake_bits, awake_tora_bits, awake_dog_bits, awake_bsd_bits, awake_sakura_bits,
      &AwakeMsk, awake_mask_bits, awake_mask_bits, awake_dog_mask_bits, awake_bsd_mask_bits, awake_sakura_mask_bits },
    { &Up1GC, &Up1Xbm,  up1_bits, up1_tora_bits, up1_dog_bits, up1_bsd_bits, up1_sakura_bits,
      &Up1Msk, up1_mask_bits, up1_mask_bits, up1_dog_mask_bits, up1_bsd_mask_bits, up1_sakura_mask_bits },
    { &Up2GC, &Up2Xbm,  up2_bits, up2_tora_bits, up2_dog_bits, up2_bsd_bits, up2_sakura_bits,
      &Up2Msk, up2_mask_bits, up2_mask_bits, up2_dog_mask_bits, up2_bsd_mask_bits, up2_sakura_mask_bits },
    { &Down1GC, &Down1Xbm,  down1_bits, down1_tora_bits, down1_dog_bits, down1_bsd_bits, down1_sakura_bits,
      &Down1Msk, down1_mask_bits, down1_mask_bits, down1_dog_mask_bits, down1_bsd_mask_bits, down1_sakura_mask_bits },
    { &Down2GC, &Down2Xbm,  down2_bits, down2_tora_bits, down2_dog_bits, down2_bsd_bits, down2_sakura_bits,
      &Down2Msk, down2_mask_bits, down2_mask_bits, down2_dog_mask_bits, down2_bsd_mask_bits, down2_sakura_mask_bits },
    { &Left1GC, &Left1Xbm,  left1_bits, left1_tora_bits, left1_dog_bits, left1_bsd_bits, left1_sakura_bits,
      &Left1Msk, left1_mask_bits, left1_mask_bits, left1_dog_mask_bits, left1_bsd_mask_bits, left1_sakura_mask_bits },
    { &Left2GC, &Left2Xbm,  left2_bits, left2_tora_bits, left2_dog_bits, left2_bsd_bits, left2_sakura_bits,
      &Left2Msk, left2_mask_bits, left2_mask_bits, left2_dog_mask_bits, left2_bsd_mask_bits, left2_sakura_mask_bits },
    { &Right1GC, &Right1Xbm,  right1_bits, right1_tora_bits, right1_dog_bits, right1_bsd_bits, right1_sakura_bits,
      &Right1Msk, right1_mask_bits, right1_mask_bits,right1_dog_mask_bits, right1_bsd_mask_bits, right1_sakura_mask_bits },
    { &Right2GC, &Right2Xbm,  right2_bits, right2_tora_bits, right2_dog_bits, right2_bsd_bits, right2_sakura_bits,
      &Right2Msk, right2_mask_bits, right2_mask_bits, right2_dog_mask_bits, right2_bsd_mask_bits, right2_sakura_mask_bits },
    { &UpLeft1GC, &UpLeft1Xbm,  upleft1_bits, upleft1_tora_bits, upleft1_dog_bits, upleft1_bsd_bits, upleft1_sakura_bits,
      &UpLeft1Msk, upleft1_mask_bits, upleft1_mask_bits, upleft1_dog_mask_bits, upleft1_bsd_mask_bits, upleft1_sakura_mask_bits },
    { &UpLeft2GC, &UpLeft2Xbm,  upleft2_bits, upleft2_tora_bits, upleft2_dog_bits, upleft2_bsd_bits, upleft2_sakura_bits,
      &UpLeft2Msk, upleft2_mask_bits, upleft2_mask_bits,upleft2_dog_mask_bits, upleft2_bsd_mask_bits, upleft2_sakura_mask_bits },
    { &UpRight1GC, &UpRight1Xbm,  upright1_bits, upright1_tora_bits, upright1_dog_bits, upright1_bsd_bits, upright1_sakura_bits,
      &UpRight1Msk, upright1_mask_bits, upright1_mask_bits,upright1_dog_mask_bits, upright1_bsd_mask_bits, upright1_sakura_mask_bits },
    { &UpRight2GC, &UpRight2Xbm,  upright2_bits, upright2_tora_bits, upright2_dog_bits, upright2_bsd_bits, upright2_sakura_bits,
      &UpRight2Msk, upright2_mask_bits, upright2_mask_bits,upright2_dog_mask_bits, upright2_bsd_mask_bits, upright2_sakura_mask_bits },
    { &DownLeft1GC, &DownLeft1Xbm,  dwleft1_bits, dwleft1_tora_bits, dwleft1_dog_bits, dwleft1_bsd_bits, dwleft1_sakura_bits,
      &DownLeft1Msk, dwleft1_mask_bits, dwleft1_mask_bits, dwleft1_dog_mask_bits, dwleft1_bsd_mask_bits, dwleft1_sakura_mask_bits },
    { &DownLeft2GC, &DownLeft2Xbm,  dwleft2_bits, dwleft2_tora_bits, dwleft2_dog_bits, dwleft2_bsd_bits, dwleft2_sakura_bits,
      &DownLeft2Msk, dwleft2_mask_bits, dwleft2_mask_bits, dwleft2_dog_mask_bits, dwleft2_bsd_mask_bits, dwleft2_sakura_mask_bits },
    { &DownRight1GC, &DownRight1Xbm,  dwright1_bits, dwright1_tora_bits, dwright1_dog_bits, dwright1_bsd_bits, dwright1_sakura_bits,
      &DownRight1Msk, dwright1_mask_bits, dwright1_mask_bits, dwright1_dog_mask_bits, dwright1_bsd_mask_bits, dwright1_sakura_mask_bits },
    { &DownRight2GC, &DownRight2Xbm,  dwright2_bits, dwright2_tora_bits, dwright2_dog_bits, dwright2_bsd_bits, dwright2_sakura_bits,
      &DownRight2Msk, dwright2_mask_bits, dwright2_mask_bits, dwright2_dog_mask_bits, dwright2_bsd_mask_bits, dwright2_sakura_mask_bits },
    { &UpTogi1GC, &UpTogi1Xbm,  utogi1_bits, utogi1_tora_bits, utogi1_dog_bits, utogi1_bsd_bits, utogi1_sakura_bits,
      &UpTogi1Msk, utogi1_mask_bits, utogi1_mask_bits, utogi1_dog_mask_bits, utogi1_bsd_mask_bits, utogi1_sakura_mask_bits },
    { &UpTogi2GC, &UpTogi2Xbm,  utogi2_bits, utogi2_tora_bits, utogi2_dog_bits, utogi2_bsd_bits, utogi2_sakura_bits,
      &UpTogi2Msk, utogi2_mask_bits, utogi2_mask_bits, utogi2_dog_mask_bits, utogi2_bsd_mask_bits, utogi2_sakura_mask_bits },
    { &DownTogi1GC, &DownTogi1Xbm,  dtogi1_bits, dtogi1_tora_bits, dtogi1_dog_bits, dtogi1_bsd_bits, dtogi1_sakura_bits,
      &DownTogi1Msk, dtogi1_mask_bits, dtogi1_mask_bits, dtogi1_dog_mask_bits, dtogi1_bsd_mask_bits, dtogi1_sakura_mask_bits },
    { &DownTogi2GC, &DownTogi2Xbm,  dtogi2_bits, dtogi2_tora_bits, dtogi2_dog_bits, dtogi2_bsd_bits, dtogi2_sakura_bits,
      &DownTogi2Msk, dtogi2_mask_bits, dtogi2_mask_bits, dtogi2_dog_mask_bits, dtogi2_bsd_mask_bits, dtogi2_sakura_mask_bits },
    { &LeftTogi1GC, &LeftTogi1Xbm,  ltogi1_bits, ltogi1_tora_bits, ltogi1_dog_bits, ltogi1_bsd_bits, ltogi1_sakura_bits,
      &LeftTogi1Msk, ltogi1_mask_bits, ltogi1_mask_bits,ltogi1_dog_mask_bits, ltogi1_bsd_mask_bits, ltogi1_sakura_mask_bits },
    { &LeftTogi2GC, &LeftTogi2Xbm,  ltogi2_bits, ltogi2_tora_bits, ltogi2_dog_bits, ltogi2_bsd_bits, ltogi2_sakura_bits,
      &LeftTogi2Msk, ltogi2_mask_bits, ltogi2_mask_bits,ltogi2_dog_mask_bits, ltogi2_bsd_mask_bits, ltogi2_sakura_mask_bits },
    { &RightTogi1GC, &RightTogi1Xbm,  rtogi1_bits, rtogi1_tora_bits, rtogi1_dog_bits, rtogi1_bsd_bits, rtogi1_sakura_bits,
      &RightTogi1Msk, rtogi1_mask_bits, rtogi1_mask_bits,rtogi1_dog_mask_bits, rtogi1_bsd_mask_bits, rtogi1_sakura_mask_bits },
    { &RightTogi2GC, &RightTogi2Xbm,  rtogi2_bits, rtogi2_tora_bits, rtogi2_dog_bits, rtogi2_bsd_bits, rtogi2_sakura_bits,
      &RightTogi2Msk, rtogi2_mask_bits, rtogi2_mask_bits,rtogi2_dog_mask_bits, rtogi2_bsd_mask_bits, rtogi2_sakura_mask_bits },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

typedef struct {
    GC		*TickGCPtr;
    Pixmap	*TickMaskPtr;
} Animation;

Animation	AnimationPattern[][2] =
{
  { { &Mati2GC, &Mati2Msk },
    { &Mati2GC, &Mati2Msk } },		/* NekoState == NEKO_STOP */
  { { &Jare2GC, &Jare2Msk },
    { &Mati2GC, &Mati2Msk } },		/* NekoState == NEKO_JARE */
  { { &Kaki1GC, &Kaki1Msk },
    { &Kaki2GC, &Kaki2Msk } },		/* NekoState == NEKO_KAKI */
  { { &Mati3GC, &Mati3Msk },
    { &Mati3GC, &Mati3Msk } },		/* NekoState == NEKO_AKUBI */
  { { &Sleep1GC, &Sleep1Msk },
    { &Sleep2GC, &Sleep2Msk } },		/* NekoState == NEKO_SLEEP */
  { { &AwakeGC, &AwakeMsk },
    { &AwakeGC, &AwakeMsk } },		/* NekoState == NEKO_AWAKE */
  { { &Up1GC, &Up1Msk },
    { &Up2GC, &Up2Msk } },		/* NekoState == NEKO_U_MOVE */
  { { &Down1GC, &Down1Msk },
    { &Down2GC, &Down2Msk } },		/* NekoState == NEKO_D_MOVE */
  { { &Left1GC, &Left1Msk },
    { &Left2GC, &Left2Msk } },		/* NekoState == NEKO_L_MOVE */
  { { &Right1GC, &Right1Msk },
    { &Right2GC, &Right2Msk } },		/* NekoState == NEKO_R_MOVE */
  { { &UpLeft1GC, &UpLeft1Msk },
    { &UpLeft2GC, &UpLeft2Msk } },	/* NekoState == NEKO_UL_MOVE */
  { { &UpRight1GC, &UpRight1Msk },
    { &UpRight2GC, &UpRight2Msk } },	/* NekoState == NEKO_UR_MOVE */
  { { &DownLeft1GC, &DownLeft1Msk },
    { &DownLeft2GC, &DownLeft2Msk } },	/* NekoState == NEKO_DL_MOVE */
  { { &DownRight1GC, &DownRight1Msk },
    { &DownRight2GC, &DownRight2Msk } },	/* NekoState == NEKO_DR_MOVE */
  { { &UpTogi1GC, &UpTogi1Msk },
    { &UpTogi2GC, &UpTogi2Msk } },	/* NekoState == NEKO_U_TOGI */
  { { &DownTogi1GC, &DownTogi1Msk },
    { &DownTogi2GC, &DownTogi2Msk } },	/* NekoState == NEKO_D_TOGI */
  { { &LeftTogi1GC, &LeftTogi1Msk },
    { &LeftTogi2GC, &LeftTogi2Msk } },	/* NekoState == NEKO_L_TOGI */
  { { &RightTogi1GC, &RightTogi1Msk },
    { &RightTogi2GC, &RightTogi2Msk } },	/* NekoState == NEKO_R_TOGI */
};

static void NullFunction();

/*
 *	ビットマップデータ・GC 初期化
 */

void
InitBitmapAndGCs()
{
    BitmapGCData	*BitmapGCDataTablePtr;
    XGCValues		theGCValues;

    theGCValues.function = GXcopy;
    theGCValues.foreground = theForegroundColor.pixel;
    theGCValues.background = theBackgroundColor.pixel;
    theGCValues.fill_style = FillTiled;
    theGCValues.ts_x_origin = 0;
    theGCValues.ts_y_origin = 0;

    for (BitmapGCDataTablePtr = BitmapGCDataTable;
	 BitmapGCDataTablePtr->GCCreatePtr != NULL;
	 BitmapGCDataTablePtr++) {

	*(BitmapGCDataTablePtr->BitmapCreatePtr)
	    = XCreatePixmapFromBitmapData(theDisplay, theRoot,
		BitmapGCDataTablePtr->PixelPattern[NekoMoyou],
		BITMAP_WIDTH, BITMAP_HEIGHT,
		theForegroundColor.pixel,
		theBackgroundColor.pixel,
		DefaultDepth(theDisplay, theScreen));

	theGCValues.tile = *(BitmapGCDataTablePtr->BitmapCreatePtr);

	*(BitmapGCDataTablePtr->BitmapMasksPtr)
	    = XCreateBitmapFromData(theDisplay, theRoot,
		BitmapGCDataTablePtr->MaskPattern[NekoMoyou],
		BITMAP_WIDTH, BITMAP_HEIGHT);

	*(BitmapGCDataTablePtr->GCCreatePtr)
	    = XCreateGC(theDisplay, theWindow,
		GCFunction | GCForeground | GCBackground | GCTile |
		GCTileStipXOrigin | GCTileStipYOrigin | GCFillStyle,
		&theGCValues);
    }
}

/*
 *	リソース・データベースから必要なリソースを取り出す
 */

char	*
NekoGetDefault(resource)
char	*resource;
{
	char	*value;

	if (value = XGetDefault(theDisplay, ProgramName, resource)) {
		return value;
	}
	if (value = XGetDefault(theDisplay, ClassName, resource)) {
		return value;
	}
	return NULL;
}

/*
 *	リソース・データベースからオプションを設定
 */

GetResources()
{
  char	*resource;
  int		num;
  int loop;
  if (Foreground == NULL) {
    if ((resource = NekoGetDefault("foreground")) != NULL) {
      Foreground = resource;
    }
  }

  if (Background == NULL) {
    if ((resource = NekoGetDefault("background")) != NULL) {
      Background = resource;
    }
  }

  if (IntervalTime == 0) {
    if ((resource = NekoGetDefault("time")) != NULL) {
      if (num = atoi(resource)) {
	IntervalTime = num;
      }
    }
  }

  if (NekoSpeed == (double)0) {
    if ((resource = NekoGetDefault("speed")) != NULL) {
      if (num = atoi(resource)) {
	NekoSpeed = (double)num;
      }
    }
  }

  if (IdleSpace == 0) {
    if ((resource = NekoGetDefault("idle")) != NULL) {
      if (num = atoi(resource)) {
	IdleSpace = num;
      }
    }
  }

  if (NekoMoyou == NOTDEFINED) {
    for (loop=0;loop<BITMAPTYPES;loop++)
      if ((resource = NekoGetDefault(AnimalDefaultsDataTable[loop].name)) != NULL) {
	if (IsTrue(resource))
	  NekoMoyou = loop;
      }
  }

  if (NoShape == NOTDEFINED) {
    if ((resource = NekoGetDefault("noshape")) != NULL) {
      NoShape = IsTrue(resource);
    }
  }

  if (ReverseVideo == NOTDEFINED) {
    if ((resource = NekoGetDefault("reverse")) != NULL) {
      ReverseVideo = IsTrue(resource);
    }
  }

  if (Foreground == NULL) {
    Foreground = DEFAULT_FOREGROUND;
  }
  if (Background == NULL) {
    Background = DEFAULT_BACKGROUND;
  }
  if (NekoMoyou == NOTDEFINED) {
    NekoMoyou = 0;
  }
  if (IntervalTime == 0) {
    IntervalTime = AnimalDefaultsDataTable[NekoMoyou].time;
  }
  if (NekoSpeed == (double)0) {
    NekoSpeed = (double)(AnimalDefaultsDataTable[NekoMoyou].speed);
  }
  if (IdleSpace == 0) {
    IdleSpace = AnimalDefaultsDataTable[NekoMoyou].idle;
  }
  if (XOffset == 0) {
    XOffset = AnimalDefaultsDataTable[NekoMoyou].off_x;
  }
  if (YOffset == 0) {
    YOffset = AnimalDefaultsDataTable[NekoMoyou].off_y;
  }
  if (NoShape == NOTDEFINED) {
    NoShape = False;
  }
  if (ReverseVideo == NOTDEFINED) {
    ReverseVideo = False;
  }
  if (ToWindow == NOTDEFINED) {
    ToWindow = False;
  }
  if (ToFocus == NOTDEFINED) {
    ToFocus = False;
  }
}

/*
 *	ねずみ型カーソルを作る
 */

MakeMouseCursor()
{
    Pixmap			theCursorSource;
    Pixmap			theCursorMask;

    theCursorSource
	= XCreateBitmapFromData(theDisplay, theRoot,
				AnimalDefaultsDataTable[NekoMoyou].cursor, 
				AnimalDefaultsDataTable[NekoMoyou].cursor_width,
				AnimalDefaultsDataTable[NekoMoyou].cursor_height);

    theCursorMask
	= XCreateBitmapFromData(theDisplay, theRoot,
				AnimalDefaultsDataTable[NekoMoyou].mask,
				AnimalDefaultsDataTable[NekoMoyou].cursor_width,
				AnimalDefaultsDataTable[NekoMoyou].cursor_height);

    theCursor = XCreatePixmapCursor(theDisplay, theCursorSource, theCursorMask,
				    &theBackgroundColor, &theForegroundColor,
				    AnimalDefaultsDataTable[NekoMoyou].cursor_x_hot,
				    AnimalDefaultsDataTable[NekoMoyou].cursor_y_hot);
    XFreePixmap(theDisplay,theCursorSource);
    XFreePixmap(theDisplay,theCursorMask);
}

/*
 *	色を初期設定する
 */

SetupColors()
{
    XColor	theExactColor;
    Colormap	theColormap;

    theColormap = DefaultColormap(theDisplay, theScreen);

    if (theDepth == 1) {
	Foreground = "black";
	Background = "white";
    }

    if (ReverseVideo == True) {
	char	*tmp;

	tmp = Foreground;
	Foreground = Background;
	Background = tmp;
    }

    if (!XAllocNamedColor(theDisplay, theColormap,
		Foreground, &theForegroundColor, &theExactColor)) {
	fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		ProgramName, Foreground);
	exit(1);
    }

    if (!XAllocNamedColor(theDisplay, theColormap,
		Background, &theBackgroundColor, &theExactColor)) {
	fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		ProgramName, Background);
	exit(1);
    }
}

/*
 *	スクリーン環境初期化
 */

void
InitScreen(DisplayName)
    char	*DisplayName;
{
  XSetWindowAttributes	theWindowAttributes;
  unsigned long		theWindowMask;
  Window			theTempRoot;
  int				WindowPointX;
  int				WindowPointY;
  unsigned int		BorderWidth;
  int				event_base, error_base;

  if ((theDisplay = XOpenDisplay(DisplayName)) == NULL) {
    fprintf(stderr, "%s: Can't open display", ProgramName);
    if (DisplayName != NULL) {
      fprintf(stderr, " %s.\n", DisplayName);
    } else {
      fprintf(stderr, ".\n");
    }
    exit(1);
  }

  GetResources();

  if (Synchronous == True) {
    fprintf(stderr,"Synchronizing.\n");
    XSynchronize(theDisplay,True);
  }

#ifdef SHAPE
  if (!NoShape && XShapeQueryExtension(theDisplay,
				       &event_base, &error_base) == False) {
    fprintf(stderr, "Display not suported shape extension.\n");
    NoShape = True;
				       }
#endif SHAPE

  theScreen = DefaultScreen(theDisplay);
  theDepth = DefaultDepth(theDisplay, theScreen);

  theRoot = RootWindow(theDisplay, theScreen);

  XGetGeometry(theDisplay, theRoot, &theTempRoot,
	       &WindowPointX, &WindowPointY,
	       &WindowWidth, &WindowHeight,
	       &BorderWidth, &theDepth);

  SetupColors();
  MakeMouseCursor();

  if (ToWindow) {
    XEvent SelectEvent;
    int SelectButtons = 0;
    
    if (XGrabPointer(theDisplay, theRoot, False,
		     ButtonPressMask|ButtonReleaseMask, GrabModeSync,
		     GrabModeAsync, theRoot, theCursor, CurrentTime)
	!= GrabSuccess) {
      fprintf(stderr, "%s: Can't grab the mouse.\n", ProgramName);
      exit(1);
    }

    while ((theTarget == NOTDEFINED) || (SelectButtons != 0)) {
      XAllowEvents(theDisplay, SyncPointer, CurrentTime);
      XWindowEvent(theDisplay, theRoot, ButtonPressMask|ButtonReleaseMask,
		   &SelectEvent);
      switch (SelectEvent.type) {
      case ButtonPress:
	if (theTarget == NOTDEFINED) {
	  theTarget = SelectEvent.xbutton.subwindow;
	  if (theTarget == None) ToWindow = False;
	}
	SelectButtons++;
	break;
      case ButtonRelease:
	if (SelectButtons > 0)
	  SelectButtons--;
	break;
      }
    } 
    
    XUngrabPointer(theDisplay, CurrentTime);
  }

  theWindowAttributes.background_pixel = theBackgroundColor.pixel;
  theWindowAttributes.cursor = theCursor;
  theWindowAttributes.override_redirect = True;

  if (!ToWindow) XChangeWindowAttributes(theDisplay, theRoot, CWCursor,
					 &theWindowAttributes);

  theWindowMask = CWBackPixel		|
    CWCursor		|
      CWOverrideRedirect;

  theWindow = XCreateWindow(theDisplay, theRoot, 0, 0,
			    BITMAP_WIDTH, BITMAP_HEIGHT,
			    0, theDepth, InputOutput, CopyFromParent,
			    theWindowMask, &theWindowAttributes);

  XStoreName(theDisplay, theWindow, ProgramName);

  InitBitmapAndGCs();

  XSelectInput(theDisplay, theWindow, 
	       ExposureMask|VisibilityChangeMask|KeyPressMask);

  XFlush(theDisplay);
}


/*
 *	インターバル
 *
 *	　この関数を呼ぶと、ある一定の時間返ってこなくなる。猫
 *	の動作タイミング調整に利用すること。
 */

void
Interval()
{
    pause();
    if (RaiseWindowDelay>0)
      RaiseWindowDelay--;
}


/*
 *	ティックカウント処理
 */

void
TickCount()
{
    if (++NekoTickCount >= MAX_TICK) {
	NekoTickCount = 0;
    }

    if (NekoTickCount % 2 == 0) {
	if (NekoStateCount < MAX_TICK) {
	    NekoStateCount++;
	}
    }
}


/*
 *	猫状態設定
 */

void
SetNekoState(SetValue)
    int		SetValue;
{
    NekoTickCount = 0;
    NekoStateCount = 0;

    NekoState = SetValue;
}


/*
 *	猫描画処理
 */

void
DrawNeko(x, y, DrawAnime)
    int		x;
    int		y;
    Animation	DrawAnime;
{
/*@@@@@@*/
    register GC		DrawGC = *(DrawAnime.TickGCPtr);
    register Pixmap	DrawMask = *(DrawAnime.TickMaskPtr);

    if ((x != NekoLastX) || (y != NekoLastY)
		|| (DrawGC != NekoLastGC)) {
      XWindowChanges	theChanges;

      theChanges.x = x;
      theChanges.y = y;
      XConfigureWindow(theDisplay, theWindow, CWX | CWY, &theChanges);
#ifdef SHAPE
      if (NoShape == False) {
	XShapeCombineMask(theDisplay, theWindow, ShapeBounding,
			  0, 0, DrawMask, ShapeSet);

      }
#endif SHAPE
      if (DontMapped) {
	XMapWindow(theDisplay, theWindow);
	DontMapped = 0;
      }
      XFillRectangle(theDisplay, theWindow, DrawGC,
		     0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);
    }

    XFlush(theDisplay);

    NekoLastX = x;
    NekoLastY = y;

    NekoLastGC = DrawGC;
}


/*
 *	猫再描画処理
 */

void
RedrawNeko()
{
  XFillRectangle(theDisplay, theWindow, NekoLastGC,
		 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);

  XFlush(theDisplay);
}


/*
 *	猫移動方法決定
 *
 *      This sets the direction that the neko is moving.
 *
 */

void
NekoDirection()
{
    int			NewState;
    double		LargeX, LargeY;
    double		Length;
    double		SinTheta;

    if (NekoMoveDx == 0 && NekoMoveDy == 0) {
	NewState = NEKO_STOP;
    } else {
	LargeX = (double)NekoMoveDx;
	LargeY = (double)(-NekoMoveDy);
	Length = sqrt(LargeX * LargeX + LargeY * LargeY);
	SinTheta = LargeY / Length;

	if (NekoMoveDx > 0) {
	    if (SinTheta > SinPiPer8Times3) {
		NewState = NEKO_U_MOVE;
	    } else if ((SinTheta <= SinPiPer8Times3)
			&& (SinTheta > SinPiPer8)) {
		NewState = NEKO_UR_MOVE;
	    } else if ((SinTheta <= SinPiPer8)
			&& (SinTheta > -(SinPiPer8))) {
		NewState = NEKO_R_MOVE;
	    } else if ((SinTheta <= -(SinPiPer8))
			&& (SinTheta > -(SinPiPer8Times3))) {
		NewState = NEKO_DR_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	} else {
	    if (SinTheta > SinPiPer8Times3) {
		NewState = NEKO_U_MOVE;
	    } else if ((SinTheta <= SinPiPer8Times3)
			&& (SinTheta > SinPiPer8)) {
		NewState = NEKO_UL_MOVE;
	    } else if ((SinTheta <= SinPiPer8)
			&& (SinTheta > -(SinPiPer8))) {
		NewState = NEKO_L_MOVE;
	    } else if ((SinTheta <= -(SinPiPer8))
			&& (SinTheta > -(SinPiPer8Times3))) {
		NewState = NEKO_DL_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	}
    }

    if (NekoState != NewState) {
	SetNekoState(NewState);
    }
}


/*
 *	猫壁ぶつかり判定
 */

Bool
IsWindowOver()
{
    Bool	ReturnValue = False;

    if (NekoY <= 0) {
	NekoY = 0;
	ReturnValue = True;
    } else if (NekoY >= WindowHeight - BITMAP_HEIGHT) {
	NekoY = WindowHeight - BITMAP_HEIGHT;
	ReturnValue = True;
    }
    if (NekoX <= 0) {
	NekoX = 0;
	ReturnValue = True;
    } else if (NekoX >= WindowWidth - BITMAP_WIDTH) {
	NekoX = WindowWidth - BITMAP_WIDTH;
	ReturnValue = True;
    }

    return(ReturnValue);
}


/*
 *	猫移動状況判定
 */

Bool
IsNekoDontMove()
{
    if (NekoX == NekoLastX && NekoY == NekoLastY) {
	return(True);
    } else {
	return(False);
    }
}


/*
 *	猫移動開始判定
 */

Bool
IsNekoMoveStart()
{
    if ((PrevMouseX >= MouseX - IdleSpace
	 && PrevMouseX <= MouseX + IdleSpace) &&
	 (PrevMouseY >= MouseY - IdleSpace 
	 && PrevMouseY <= MouseY + IdleSpace)) {
	return(False);
    } else {
	return(True);
    }
}


/*
 *	猫移動 dx, dy 計算
 */

void
CalcDxDy()
{
    Window		QueryRoot, QueryParent;
    int			RelativeX, RelativeY;
    double		LargeX, LargeY;
    double		DoubleLength, Length;

    PrevMouseX = MouseX;
    PrevMouseY = MouseY;

    if (ToFocus) {
      int		revert;

      XGetInputFocus(theDisplay, &theTarget, &revert);
    }

    if (ToWindow
	||(ToFocus && theTarget != theRoot && theTarget != theWindow
	   && theTarget != PointerRoot && theTarget != None)) {
      Window		*QueryChildren;
      unsigned int	nchild;
      unsigned int	TargetW, TargetH;
      unsigned int	TargetBorderW, TargetD;

      for (;;) {
	XQueryTree(theDisplay, theTarget,
		   &QueryRoot, &QueryParent, &QueryChildren, &nchild);
	XFree(QueryChildren);
	if (QueryParent == QueryRoot) break;
	theTarget = QueryParent;
      }

      XGetGeometry(theDisplay, theTarget, &QueryRoot,
		   &RelativeX, &RelativeY,
		   &TargetW, &TargetH,
		   &TargetBorderW, &TargetD);

      MouseX = RelativeX+TargetW/2+XOffset;
      MouseY = RelativeY+YOffset;

      if ( RelativeX+(int)TargetW <= 0 || RelativeX >= (int)WindowWidth
	  || RelativeY+(int)TargetH <= 0 || RelativeY >= (int)WindowHeight )
	theTarget = None;
    }
    else {
      theTarget = None;
    }

    if (theTarget == NOTDEFINED || theTarget == None) {
      Window		QueryChild;
      int		AbsoluteX, AbsoluteY;
      unsigned int	ModKeyMask;

      XQueryPointer(theDisplay, theWindow,
		    &QueryRoot, &QueryChild,
		    &AbsoluteX, &AbsoluteY,
		    &RelativeX, &RelativeY,
		    &ModKeyMask);

      MouseX = AbsoluteX+XOffset;
      MouseY = AbsoluteY+YOffset;
    }

    LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);
    LargeY = (double)(MouseY - NekoY - BITMAP_HEIGHT);

    DoubleLength = LargeX * LargeX + LargeY * LargeY;

    if (DoubleLength != (double)0) {
	Length = sqrt(DoubleLength);
	if (Length <= NekoSpeed) {
	    NekoMoveDx = (int)LargeX;
	    NekoMoveDy = (int)LargeY;
	} else {
	    NekoMoveDx = (int)((NekoSpeed * LargeX) / Length);
	    NekoMoveDy = (int)((NekoSpeed * LargeY) / Length);
	}
    } else {
	NekoMoveDx = NekoMoveDy = 0;
    }
}


/*
 *	動作解析猫描画処理
 */

void
NekoThinkDraw()
{
    CalcDxDy();

    if (NekoState != NEKO_SLEEP) {
	DrawNeko(NekoX, NekoY,
		AnimationPattern[NekoState][NekoTickCount & 0x1]);
    } else {
	DrawNeko(NekoX, NekoY,
		AnimationPattern[NekoState][(NekoTickCount >> 2) & 0x1]);
    }

    TickCount();

    switch (NekoState) {
    case NEKO_STOP:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_STOP_TIME) {
	    break;
	}
	if (NekoMoveDx < 0 && NekoX <= 0) {
	    SetNekoState(NEKO_L_TOGI);
	} else if (NekoMoveDx > 0 && NekoX >= WindowWidth - BITMAP_WIDTH) {
	    SetNekoState(NEKO_R_TOGI);
	} else if (NekoMoveDy < 0 && NekoY <= 0) {
	    SetNekoState(NEKO_U_TOGI);
	} else if (NekoMoveDy > 0 && NekoY >= WindowHeight - BITMAP_HEIGHT) {
	    SetNekoState(NEKO_D_TOGI);
	} else {
	    SetNekoState(NEKO_JARE);
	}
	break;
    case NEKO_JARE:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_JARE_TIME) {
	    break;
	}
	SetNekoState(NEKO_KAKI);
	break;
    case NEKO_KAKI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_KAKI_TIME) {
	    break;
	}
	SetNekoState(NEKO_AKUBI);
	break;
    case NEKO_AKUBI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_AKUBI_TIME) {
	    break;
	}
	SetNekoState(NEKO_SLEEP);
	break;
    case NEKO_SLEEP:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	break;
    case NEKO_AWAKE:
	if (NekoStateCount < NEKO_AWAKE_TIME) {
	    break;
	}
	NekoDirection();	/* 猫が動く向きを求める */
	break;
    case NEKO_U_MOVE:
    case NEKO_D_MOVE:
    case NEKO_L_MOVE:
    case NEKO_R_MOVE:
    case NEKO_UL_MOVE:
    case NEKO_UR_MOVE:
    case NEKO_DL_MOVE:
    case NEKO_DR_MOVE:
	NekoX += NekoMoveDx;
	NekoY += NekoMoveDy;
	NekoDirection();
	if (IsWindowOver()) {
	    if (IsNekoDontMove()) {
		SetNekoState(NEKO_STOP);
	    }
	}
	break;
    case NEKO_U_TOGI:
    case NEKO_D_TOGI:
    case NEKO_L_TOGI:
    case NEKO_R_TOGI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_TOGI_TIME) {
	    break;
	}
	SetNekoState(NEKO_KAKI);
	break;
    default:
	/* Internal Error */
	SetNekoState(NEKO_STOP);
	break;
    }

    Interval();
}


/*
 *	キーイベント処理
 */

Bool
ProcessKeyPress(theKeyEvent)
    XKeyEvent	*theKeyEvent;
{
  int			Length;
  int			theKeyBufferMaxLen = AVAIL_KEYBUF;
  char		theKeyBuffer[AVAIL_KEYBUF + 1];
  KeySym		theKeySym;
  XComposeStatus	theComposeStatus;
  Bool		ReturnState;

  ReturnState = True;

  Length = XLookupString(theKeyEvent,
			 theKeyBuffer, theKeyBufferMaxLen,
			 &theKeySym, &theComposeStatus);

  if (Length > 0) {
    switch (theKeyBuffer[0]) {
    case 'q':
    case 'Q':
      if (theKeyEvent->state & Mod1Mask) { /* META (Alt) キー */
	ReturnState = False;
      }
      break;
    default:
      break;
    }
    puts("Key Event");
  }

  return(ReturnState);
}


/*
 *	イベント処理
 */

Bool
ProcessEvent()
{
    XEvent	theEvent;
    Bool	ContinueState = True;

    while (XPending(theDisplay)) {
        XNextEvent(theDisplay,&theEvent);
	switch (theEvent.type) {
	case Expose:
	    if (theEvent.xexpose.count == 0) {
		RedrawNeko();
	    }
	    break;
	case KeyPress:
	    ContinueState = ProcessKeyPress(&theEvent);
	    if (!ContinueState) {
		    return(ContinueState);
	    }
	    break;
	case VisibilityNotify:
	    if (RaiseWindowDelay==0) {
	      XRaiseWindow(theDisplay,theWindow);
	      RaiseWindowDelay=DEFAULT_RAISE_WAIT;
	    } 
	default:
	    /* Unknown Event */
	    break;
	}
    }

    return(ContinueState);
}


/*
 *	猫処理
 */

void
ProcessNeko()
{
  struct itimerval	Value;

  /* 猫の初期化 */

  NekoX = (WindowWidth - BITMAP_WIDTH / 2) / 2;
  NekoY = (WindowHeight - BITMAP_HEIGHT / 2) / 2;

  NekoLastX = NekoX;
  NekoLastY = NekoY;

  SetNekoState(NEKO_STOP);

  /* タイマー設定 */

  timerclear(&Value.it_interval);
  timerclear(&Value.it_value);

  Value.it_interval.tv_usec = IntervalTime;
  Value.it_value.tv_usec = IntervalTime;

  setitimer(ITIMER_REAL, &Value, 0);

  /* メイン処理 */

  do {
    NekoThinkDraw();
  } while (ProcessEvent());
}


/*
 *	SIGALRM シグナル処理
 */

static void
NullFunction()
{
  /* No Operation */
#ifdef SYSV
  signal(SIGALRM, NullFunction);
#endif /* SYSV */
}

/*
 *	SIGINT シグナル処理
 */

void
RestoreCursor()
{
  XSetWindowAttributes	theWindowAttributes;
  BitmapGCData *BitmapGCDataTablePtr;

  theWindowAttributes.cursor = None;
  XChangeWindowAttributes(theDisplay, theRoot, CWCursor,
			  &theWindowAttributes);
  for (BitmapGCDataTablePtr = BitmapGCDataTable;
       BitmapGCDataTablePtr->GCCreatePtr != NULL;
       BitmapGCDataTablePtr++) {
    XFreePixmap(theDisplay,*(BitmapGCDataTablePtr->BitmapCreatePtr));
    XFreePixmap(theDisplay,*(BitmapGCDataTablePtr->BitmapMasksPtr));
    XFreeGC(theDisplay,*(BitmapGCDataTablePtr->GCCreatePtr));
       }
  XFreeCursor(theDisplay,theCursor);
  XCloseDisplay(theDisplay);
  exit(0);
}


/*
 *	Usage
 */

char	*message[] = {
"",
"Options are:",
"-display <display>	: Neko appears on specified display.",
"-fg <color>		: Foreground color",
"-bg <color>		: Background color",
"-speed <dots>",
"-time <microseconds>",
"-idle <dots>",
"-towindow	       	: Neko chases any window (include another oneko).",
"-tofocus      		: Neko chases your focus window",
"-rv			: Reverse video. (effects monochrome display only)",
"-position <geometry>   : as position of geometry, relative to mouse pointer.",
"-debug                 : puts you in synchronous mode.",
"-patchlevel            : print out your current patchlevel.",
NULL };

void
Usage()
{
  char	**mptr;
  int loop;

  mptr = message;
  fprintf(stderr, "Usage: %s [<options>]\n", ProgramName);
  while (*mptr) {
    fprintf(stderr,"%s\n", *mptr);
    mptr++;
  }
  for (loop=0;loop<BITMAPTYPES;loop++)
    fprintf(stderr,"-%s Use %s bitmaps\n",AnimalDefaultsDataTable[loop].name,AnimalDefaultsDataTable[loop].name);
}


/*
 *	オプションの理解
 */

Bool
GetArguments(argc, argv, theDisplayName)
    int		argc;
    char	*argv[];
    char	*theDisplayName;
{
  int		ArgCounter;
  int    result,foo,bar;
  extern int XOffset,YOffset;
  int loop,found=0;

  theDisplayName[0] = '\0';

  for (ArgCounter = 0; ArgCounter < argc; ArgCounter++) {

    if (strncmp(argv[ArgCounter], "-h", 2) == 0) {
      Usage();
      exit(0);
    }
    if (strcmp(argv[ArgCounter], "-display") == 0) {
      ArgCounter++;
      if (ArgCounter < argc) {
	strcpy(theDisplayName, argv[ArgCounter]);
      } else {
	fprintf(stderr, "%s: -display option error.\n", ProgramName);
	exit(1);
      }
    }
    else if (strcmp(argv[ArgCounter], "-speed") == 0) {
      ArgCounter++;
      if (ArgCounter < argc) {
	NekoSpeed = atof(argv[ArgCounter]);
      } else {
	fprintf(stderr, "%s: -speed option error.\n", ProgramName);
	exit(1);
      }
    }
    else if (strcmp(argv[ArgCounter], "-time") == 0) {
      ArgCounter++;
      if (ArgCounter < argc) {
	IntervalTime = atol(argv[ArgCounter]);
      } else {
	fprintf(stderr, "%s: -time option error.\n", ProgramName);
	exit(1);
      }
    }
    else if (strcmp(argv[ArgCounter], "-idle") == 0) {
      ArgCounter++;
      if (ArgCounter < argc) {
	IdleSpace = atol(argv[ArgCounter]);
      } else {
	fprintf(stderr, "%s: -idle option error.\n", ProgramName);
	exit(1);
      }
    }
    else if (strcmp(argv[ArgCounter], "-towindow") == 0) {
      ToWindow = True;
      ToFocus = False;
    }
    else if (strcmp(argv[ArgCounter], "-tofocus") == 0) {
      ToFocus = True;
      ToWindow = False;
    }
    else if ((strcmp(argv[ArgCounter], "-fg") == 0) ||
	     (strcmp(argv[ArgCounter], "-foreground") == 0)) {
      ArgCounter++;
      Foreground = argv[ArgCounter];
	     }
    else if ((strcmp(argv[ArgCounter], "-bg") == 0) ||
	     (strcmp(argv[ArgCounter], "-background") == 0)) {
      ArgCounter++;
      Background = argv[ArgCounter];
	     }
    else if (strcmp(argv[ArgCounter], "-rv") == 0) {
      ReverseVideo = True;
    }
    else if (strcmp(argv[ArgCounter], "-noshape") == 0) {
      NoShape = True;
    }
    else if (strcmp(argv[ArgCounter], "-position") == 0) {
      ArgCounter++;
      result=XParseGeometry(argv[ArgCounter],&XOffset,&YOffset,&foo,&bar);
    }
    else if (strcmp(argv[ArgCounter], "-debug") ==0) {
      Synchronous = True;
    }
    else if (strcmp(argv[ArgCounter], "-patchlevel") == 0) {
      fprintf(stderr,"Patchlevel :%s\n",PATCHLEVEL);
    }
    else {
      char *av = argv[ArgCounter] + 1;
      if (strcmp(av, "bsd") == 0)
	av = "bsd_daemon";
      for (loop=0;loop<BITMAPTYPES;loop++) {
	if (strcmp(av,AnimalDefaultsDataTable[loop].name)==0)
	  {NekoMoyou = loop;found=1;}
      }
      if (!found) {
	fprintf(stderr,
		"%s: Unknown option \"%s\".\n", ProgramName,
		argv[ArgCounter]);
	Usage();
	exit(1);
      }
    }
  }

  if (strlen(theDisplayName) < 1) {
    theDisplayName = NULL;
  }
}


/*
 *	メイン関数
 */

int
main(argc, argv)
    int		argc;
    char	*argv[];
{
  char	theDisplayName[MAXDISPLAYNAME];

  ProgramName = argv[0];

  argc--;
  argv++;

  GetArguments(argc, argv, theDisplayName);

  InitScreen(theDisplayName);

  signal(SIGALRM, NullFunction);
  signal(SIGINT, RestoreCursor);
  signal(SIGTERM, RestoreCursor);
  signal(SIGQUIT, RestoreCursor);

  SinPiPer8Times3 = sin(PI_PER8 * (double)3);
  SinPiPer8 = sin(PI_PER8);

  ProcessNeko();

  RestoreCursor();
}
