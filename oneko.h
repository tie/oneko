/*
 *	oneko  -  X11 猫
 *
 *	$Header: /home/sun/unix/kato/xsam/oneko/oneko.h,v 1.4 90/10/18 16:52:57 kato Exp Locker: kato $
 */

/*
 *	インクルードファイル
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>


/*
 *	カーソルビットマップファイルの読み込み
 */

#include "cursors/cursor.include"

/*
 *	普通の猫用ビットマップファイルの読み込み
 *      These include files in the directories noted.
 *      These are the bitmaps.
 */

#include "bitmaps/neko/neko.include"
#include "bitmaps/tora/tora.include"
#include "bitmaps/dog/dog.include"
#include "bitmaps/bsd/bsd.include"
#include "bitmaps/sakura/sakura.include"
#include "bitmaps/tomoyo/tomoyo.include"

/* These are the bitmasks that allow the use of the shape extension. */

#include "bitmasks/neko/neko.mask.include"
#include "bitmasks/dog/dog.mask.include"
#include "bitmasks/bsd/bsd.mask.include"
#include "bitmasks/sakura/sakura.mask.include"
#include "bitmasks/tomoyo/tomoyo.mask.include"

/*
 *	定数定義
 */

#define	BITMAP_WIDTH		32	/* １キャラクタの幅 (ピクセル) */
#define	BITMAP_HEIGHT		32	/* １キャラクタの高さ (ピクセル) */

#define	AVAIL_KEYBUF		255
#define	MAX_TICK		9999		/* Odd Only! */

#define	DEFAULT_FOREGROUND	"black"		/* フォアグラウンドカラー */
#define	DEFAULT_BACKGROUND	"white"		/* バックグラウンドカラー */

/*
 *	猫の状態定数
 */

#define	NEKO_STOP		0	/* 立ち止まった */
#define	NEKO_JARE		1	/* 顔を洗っている */
#define	NEKO_KAKI		2	/* 頭を掻いている */
#define	NEKO_AKUBI		3	/* あくびをしている */
#define	NEKO_SLEEP		4	/* 寝てしまった */
#define	NEKO_AWAKE		5	/* 目が覚めた */
#define	NEKO_U_MOVE		6	/* 上に移動中 */
#define	NEKO_D_MOVE		7	/* 下に移動中 */
#define	NEKO_L_MOVE		8	/* 左に移動中 */
#define	NEKO_R_MOVE		9	/* 右に移動中 */
#define	NEKO_UL_MOVE		10	/* 左上に移動中 */
#define	NEKO_UR_MOVE		11	/* 右上に移動中 */
#define	NEKO_DL_MOVE		12	/* 左下に移動中 */
#define	NEKO_DR_MOVE		13	/* 右下に移動中 */
#define	NEKO_U_TOGI		14	/* 上の壁を引っ掻いている */
#define	NEKO_D_TOGI		15	/* 下の壁を引っ掻いている */
#define	NEKO_L_TOGI		16	/* 左の壁を引っ掻いている */
#define	NEKO_R_TOGI		17	/* 右の壁を引っ掻いている */

/*
 *	猫のアニメーション繰り返し回数
 */

#define	NEKO_STOP_TIME		4
#define	NEKO_JARE_TIME		10
#define	NEKO_KAKI_TIME		4
#define	NEKO_AKUBI_TIME		6	/* もともと３だったけど長い方がいい */
#define	NEKO_AWAKE_TIME		3
#define	NEKO_TOGI_TIME		10

#define	PI_PER8			((double)3.1415926535/(double)8)

#define	MAXDISPLAYNAME		(64 + 5)	/* ディスプレイ名の長さ */

#define	NOTDEFINED		(-1)

/*
 *	マクロ定義
 */

#define IsTrue(str)     (strcmp((str), "true") == 0)
