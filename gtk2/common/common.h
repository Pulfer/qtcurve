#ifndef __COMMON_H__
#define __COMMON_H__

/*
  QtCurve (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com

  ----

  This program is free software; you can redistr ibute it and/or
  modify it under the terms of the GNU General Public
  License version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include <qtcurve-utils/shade.h>
#include <qtcurve-utils/options.h>

/*
    The following #define disables the rounding when scrollbar type==none.
#define SIMPLE_SCROLLBARS
*/

/*
    The following #define controls whether a scrollbar's slider should overlap
    the scrollbar buttons when at min/max. This removes the thick looking line
    between the slider and the buttons.
*/
#define INCREASE_SB_SLIDER

#include <glib.h>
#define IS_BLACK(A) (0==(A).red && 0==(A).green && 0==(A).blue)

#include <gtk/gtk.h>
#include <gdk/gdk.h>
typedef GdkColor color;
typedef gchar ** Strings;

#define SETTINGS_GROUP        "Settings"
#define KWIN_GROUP            "KWin"

/* qtc_<theme name>.themerc support */
#define THEME_DIR    "/share/apps/kstyle/themes/"
#define THEME_DIR4   "/share/kde4/apps/kstyle/themes/"
#define THEME_PREFIX "qtc_"
#define THEME_SUFFIX ".themerc"
#define BORDER_SIZE_FILE "windowBorderSizes"

#define LV_SIZE      7

#define LARGE_ARR_WIDTH  7
#define LARGE_ARR_HEIGHT 4
#define SMALL_ARR_WIDTH  5
#define SMALL_ARR_HEIGHT 3

#define NUM_EXTRA_SHADES 3

enum {
    ALPHA_ETCH_LIGHT = 0,
    ALPHA_ETCH_DARK,
    NUM_STD_ALPHAS
};

#define TOTAL_SHADES (QTC_NUM_STD_SHADES + NUM_EXTRA_SHADES)
#define ORIGINAL_SHADE   TOTAL_SHADES

#define SHADE_ORIG_HIGHLIGHT QTC_NUM_STD_SHADES
#define SHADE_4_HIGHLIGHT (QTC_NUM_STD_SHADES + 1)
#define SHADE_2_HIGHLIGHT (QTC_NUM_STD_SHADES + 2)

#define SIMPLE_SHADING (!shading)
#define DEFAULT_CONTRAST 7

#define THIN_SBAR_MOD  ((opts.sliderWidth<DEFAULT_SLIDER_WIDTH ? 3 : opts.sliderWidth>DEFAULT_SLIDER_WIDTH ? (opts.sliderWidth-9)/2 : 4)+(EFFECT_NONE==opts.buttonEffect ? 1 : 0))
#define SLIDER_SIZE (opts.sliderWidth<DEFAULT_SLIDER_WIDTH ? DEFAULT_SLIDER_WIDTH-2 : opts.sliderWidth)
#define CIRCULAR_SLIDER_SIZE 15
#define GLOW_MO           1 /*ORIGINAL_SHADE*/
#define GLOW_DEFBTN       1
#define GLOW_ALPHA(DEF)   ((DEF) ? 0.5 : 0.65)
#define DEF_BNT_TINT      0.4
#define ENTRY_INNER_ALPHA 0.4
#define INACTIVE_SEL_ALPHA 0.5

#define SUNKEN_BEVEL_DARK_ALPHA(X)  (X.value()/800.0) // 0.25
#define SUNKEN_BEVEL_LIGHT_ALPHA(X) (X.value()/500.0) // 0.40

#define BLEND_TITLEBAR     (opts.menubarAppearance==opts.titlebarAppearance && opts.menubarAppearance==opts.inactiveTitlebarAppearance && \
                           !(opts.windowBorder&WINDOW_BORDER_BLEND_TITLEBAR) && SHADE_WINDOW_BORDER==opts.shadeMenubars && opts.windowDrag)

#define STD_BORDER_BR      2
#define PBAR_BORDER        4
#define ARROW_MO_SHADE     4
#define LOWER_BORDER_ALPHA 0.35
#define BORDER_VAL(E) (/*(E) ?*/ QTC_STD_BORDER/* : DISABLED_BORDER*/)
#define SLIDER_MO_BORDER_VAL 3

#define FRAME_DARK_SHADOW 2
#define FOCUS_SHADE(SEL)         (FOCUS_GLOW==opts.focus ? GLOW_MO : ((SEL) ? 3 : ORIGINAL_SHADE))
#define MENU_STRIPE_SHADE (USE_LIGHTER_POPUP_MENU ? ORIGINAL_SHADE : 2)
#define MENU_SEP_SHADE    (USE_LIGHTER_POPUP_MENU ? 4 : 3)

#define BGND_STRIPE_SHADE 0.95

#define TAB_APPEARANCE(A) (A) /* (APPEARANCE_GLASS==(A) ? APPEARANCE_GRADIENT : (A)) */

#define INVERT_SHADE(A) (1.0+(1.0-(A)))

#define ROUNDED (ROUND_NONE!=opts.round)

#define TOOLBAR_SEP_GAP        (opts.fadeLines ? 5 : 6)
#define FADE_SIZE              0.4
#define ETCHED_DARK            0.95

#define IS_GLASS(A) (APPEARANCE_DULL_GLASS==(A) || APPEARANCE_SHINY_GLASS==(A))
#define IS_CUSTOM(A) ((A)>=APPEARANCE_CUSTOM1 && (A)<(APPEARANCE_CUSTOM1+NUM_CUSTOM_GRAD))
#define IS_FLAT(A)  (APPEARANCE_FLAT==(A) || APPEARANCE_RAISED==(A) || APPEARANCE_FADE==(A))
#define IS_FLAT_BGND(A)  (APPEARANCE_FLAT==(A) || APPEARANCE_RAISED==(A))

#define MENUBAR_DARK_LIMIT (160<<8)
#define TOO_DARK(A) ((A).red<MENUBAR_DARK_LIMIT || (A).green<MENUBAR_DARK_LIMIT || (A).blue<MENUBAR_DARK_LIMIT)

#define TO_FACTOR(A) ((100.0+((double)(A)))/100.0)
#define DEFAULT_HIGHLIGHT_FACTOR                   3
#define DEFAULT_SPLITTER_HIGHLIGHT_FACTOR          3
#define DEFAULT_CR_HIGHLIGHT_FACTOR                0
#define DEFAULT_EXPANDER_HIGHLIGHT_FACTOR          3
#define MAX_HIGHLIGHT_FACTOR                      50
#define MIN_HIGHLIGHT_FACTOR                     -50
#define MENUBAR_DARK_FACTOR        TO_FACTOR(-3)
#define INACTIVE_HIGHLIGHT_FACTOR  TO_FACTOR(20)
#define LV_HEADER_DARK_FACTOR      TO_FACTOR(-10)
#define DEF_POPUPMENU_LIGHT_FACTOR                 2
#define MIN_LIGHTER_POPUP_MENU                  -100
#define MAX_LIGHTER_POPUP_MENU                   100

#define MIN_GB_FACTOR -50
#define MAX_GB_FACTOR  50
#define DEF_GB_FACTOR  -3

#define TO_ALPHA(A) (((double)((A)<0 ? -(A) : (A)))/100.0)
#define DEF_COLOR_SEL_TAB_FACTOR  25
#define MIN_COLOR_SEL_TAB_FACTOR   0
#define MAX_COLOR_SEL_TAB_FACTOR 100

#define DEF_TAB_BGND         0
#define MIN_TAB_BGND        -5
#define MAX_TAB_BGND         5

#define DEFAULT_MENU_DELAY 225
#define MIN_MENU_DELAY       1
#define MAX_MENU_DELAY     500

#define DEFAULT_SLIDER_WIDTH  15
#define MIN_SLIDER_WIDTH_ROUND 7
#define MIN_SLIDER_WIDTH_THIN_GROOVE 9
#define MIN_SLIDER_WIDTH       5
#define MAX_SLIDER_WIDTH      31

#define SIZE_GRIP_SIZE 12

#define USE_LIGHTER_POPUP_MENU (opts.lighterPopupMenuBgnd)
#define USE_BORDER(B)          (GB_SHINE!=(B) && GB_NONE!=(B))
#define DRAW_MENU_BORDER       (APPEARANCE_FLAT!=opts.menuBgndAppearance && opts.version>=qtcMakeVersion(1, 7) && \
                                USE_BORDER(qtcGetGradient(opts.menuBgndAppearance, &opts)->border))

#define USE_GLOW_FOCUS(mouseOver) (FOCUS_GLOW==opts.focus && (MO_GLOW!=opts.coloredMouseOver || !(mouseOver)))

#define USE_SHADED_MENU_BAR_COLORS (SHADE_CUSTOM==opts.shadeMenubars || SHADE_BLEND_SELECTED==opts.shadeMenubars)
#define MENUBAR_GLASS_SELECTED_DARK_FACTOR 0.9

#define MENUITEM_FADE_SIZE 48

#define NUM_SPLITTER_DASHES 21

#define WIDGET_BUTTON(w) (WIDGET_STD_BUTTON==(w) || WIDGET_DEF_BUTTON==(w) || WIDGET_TOGGLE_BUTTON==(w) || \
                          WIDGET_CHECKBOX==(w) || WIDGET_RADIO_BUTTON==(w) || \
                          WIDGET_RADIO_BUTTON==(w) || WIDGET_COMBO==(w) || WIDGET_COMBO_BUTTON==(w) || WIDGET_UNCOLOURED_MO_BUTTON==(w) || \
                          WIDGET_TOOLBAR_BUTTON==(w))
#define ETCH_WIDGET(w) (WIDGET_STD_BUTTON==(w) || WIDGET_DEF_BUTTON==(w) || WIDGET_TOGGLE_BUTTON==(w) || WIDGET_SLIDER_TROUGH==(w) || \
                        WIDGET_CHECKBOX==(w) || WIDGET_RADIO_BUTTON==(w) || \
                        (WIDGET_SLIDER==(w) && MO_GLOW==opts.coloredMouseOver) || \
                        WIDGET_FILLED_SLIDER_TROUGH==(w) || WIDGET_COMBO==(w) || WIDGET_UNCOLOURED_MO_BUTTON==(w) || \
                        WIDGET_TOOLBAR_BUTTON==(w))
#define AGUA_WIDGET(w) (WIDGET_STD_BUTTON==(w) || WIDGET_DEF_BUTTON==(w) || WIDGET_TOGGLE_BUTTON==(w) || IS_SLIDER((w)) || \
                        WIDGET_CHECKBOX==(w) || WIDGET_RADIO_BUTTON==(w) || \
                        WIDGET_COMBO==(w) WIDGET_COMBO_BUTTON==(w))

#define SLIDER(w) (WIDGET_SB_SLIDER==(w) || WIDGET_SLIDER==(w))
#define CIRCULAR_SLIDER(w) (WIDGET_SLIDER==(w) && SLIDER_CIRCULAR==opts.sliderStyle)

#define MODIFY_AGUA_X(A, X) (APPEARANCE_AGUA==(A) ?  (X) : (A))
#define MODIFY_AGUA(A)      MODIFY_AGUA_X((A), APPEARANCE_AGUA_MOD)
#define AGUA_MAX 32.0
#define AGUA_MID_SHADE 0.85

#define COLORED_BORDER_SIZE 3
#define PROGRESS_CHUNK_WIDTH 10
#define STRIPE_WIDTH 10
#define DRAW_LIGHT_BORDER(SUKEN, WIDGET, APP) \
    (!(SUKEN) && (GB_LIGHT==qtcGetGradient(APP, &opts)->border) && WIDGET_MENU_ITEM!=(WIDGET) && !IS_TROUGH(WIDGET) && \
                          (WIDGET_DEF_BUTTON!=(WIDGET) || IND_COLORED!=opts.defBtnIndicator))

#define DRAW_3D_FULL_BORDER(SUNKEN, APP) \
    (!(SUNKEN) && GB_3D_FULL==qtcGetGradient((APP), &opts)->border)

#define DRAW_3D_BORDER(SUNKEN, APP) \
    (!(SUNKEN) && GB_3D==qtcGetGradient((APP), &opts)->border)

#define DRAW_SHINE(SUNKEN, APP) \
    (!(SUNKEN) && GB_SHINE==qtcGetGradient((APP), &opts)->border)

#define LIGHT_BORDER(APP) (APPEARANCE_DULL_GLASS==(APP) ? 1 : 0)

#define PROGRESS_ANIMATION 100
#define MIN_SLIDER_SIZE(A) (LINE_DOTS==(A) ? 24 : 20)

#define CR_SMALL_SIZE 13
#define CR_LARGE_SIZE 15

#define TAB_APP(A)   (APPEARANCE_BEVELLED==(A) || APPEARANCE_SPLIT_GRADIENT==(A) ? APPEARANCE_GRADIENT : (A))
#define NORM_TAB_APP TAB_APP(opts.tabAppearance)
#define SEL_TAB_APP  TAB_APP(opts.activeTabAppearance)

#define SLIDER_MO_SHADE  (SHADE_SELECTED==opts.shadeSliders ? 1 : (SHADE_BLEND_SELECTED==opts.shadeSliders ? 0 : ORIGINAL_SHADE))
#define SLIDER_MO_PLASTIK_BORDER (SHADE_SELECTED==opts.shadeSliders || SHADE_BLEND_SELECTED==opts.shadeSliders ? 2 : 1)
#define SLIDER_MO_LEN    (SLIDER_TRIANGULAR==opts.sliderStyle ? 2 : (SHADE_SELECTED==opts.shadeSliders || SHADE_BLEND_SELECTED==opts.shadeSliders ? 4 : 3))
#define SB_SLIDER_MO_LEN(A) ((A)<22 && !FULLLY_ROUNDED \
                                    ? 2 \
                                    : ((A)<32 || (SHADE_SELECTED!=opts.shadeSliders && SHADE_BLEND_SELECTED!=opts.shadeSliders) \
                                        ? 4 \
                                        : 6))

#define CR_MO_FILL          1
#define MO_DEF_BTN          2
#define MO_PLASTIK_DARK(W)  (WIDGET_DEF_BUTTON==(W) && IND_COLORED==opts.defBtnIndicator ? 3 : 2) /*? 2 : 1) */
#define MO_PLASTIK_LIGHT(W) (WIDGET_DEF_BUTTON==(W) && IND_COLORED==opts.defBtnIndicator ? 4 : 1) /*? 2 : 0) */

#define MO_STD_DARK(W)     (MO_GLOW==opts.coloredMouseOver \
                                    ? 1 \
                                    : MO_PLASTIK_DARK(W))
#define MO_STD_LIGHT(W, S) (MO_GLOW==opts.coloredMouseOver \
                                    ? 1 \
                                    : MO_PLASTIK_LIGHT(W))

#define FULLLY_ROUNDED     (opts.round>=ROUND_FULL)
#define DO_EFFECT          (EFFECT_NONE!=opts.buttonEffect)
#define SLIDER_GLOW        (DO_EFFECT && MO_GLOW==opts.coloredMouseOver /*&& SLIDER_TRIANGULAR!=opts.sliderStyle*/ ? 2 : 0)

#define ENTRY_MO (opts.unifyCombo && opts.unifySpin)


#define FOCUS_ALPHA              0.08
#define FOCUS_GLOW_LINE_ALPHA    0.5
#define BORDER_BLEND_ALPHA(W)    (WIDGET_ENTRY==(W) || WIDGET_SCROLLVIEW==(W) || WIDGET_SPIN==(W) || WIDGET_COMBO_BUTTON==(W) ? 0.4 : 0.7)

#define ETCH_TOP_ALPHA           0.055
#define ETCH_BOTTOM_ALPHA        0.1
#define ETCH_RADIO_TOP_ALPHA     0.09
#define ETCH_RADIO_BOTTOM_ALPHA  1.0

#define RINGS_INNER_ALPHA(T) qtc_ring_alpha[IMG_PLAIN_RINGS==(T) ? 1 : 0] //(IMG_PLAIN_RINGS==opts.bgndImage.type ? 0.25 :  0.125)
#define RINGS_OUTER_ALPHA    qtc_ring_alpha[2] //0.5
#define RINGS_WIDTH(T)       (IMG_SQUARE_RINGS==T ? 260 : 450)
#define RINGS_HEIGHT(T)      (IMG_SQUARE_RINGS==T ? 220 : 360)

#define RINGS_SQUARE_LARGE_ALPHA (RINGS_OUTER_ALPHA*0.675)
#define RINGS_SQUARE_SMALL_ALPHA (RINGS_OUTER_ALPHA*0.50)
#define RINGS_SQUARE_LINE_WIDTH  20.0
#define RINGS_SQUARE_RADIUS      18.0
#define RINGS_SQUARE_LARGE_SIZE  120.0
#define RINGS_SQUARE_SMALL_SIZE  100.0

#define MENU_AND_TOOLTIP_RADIUS   (opts.round>=ROUND_FULL ? 5.0 : 3.5)

#define CUSTOM_BGND (!(IS_FLAT_BGND(opts.bgndAppearance)) || IMG_NONE!=opts.bgndImage.type || 100!=opts.bgndOpacity || 100!=opts.dlgOpacity)

#define GLOW_PROG_ALPHA 0.55

typedef enum {
    WINDOW_BORDER_COLOR_TITLEBAR_ONLY            = 0x01, // colorTitlebarOnly
    WINDOW_BORDER_USE_MENUBAR_COLOR_FOR_TITLEBAR = 0x02, // titlebarMenuColor
    WINDOW_BORDER_ADD_LIGHT_BORDER               = 0x04, // titlebarBorder
    WINDOW_BORDER_BLEND_TITLEBAR                 = 0x08, // titlebarBlend
    WINDOW_BORDER_SEPARATOR                      = 0x10,
    WINDOW_BORDER_FILL_TITLEBAR                  = 0x20
} EWindowBorder;

typedef enum {
    IMG_NONE,
    IMG_BORDERED_RINGS,
    IMG_PLAIN_RINGS,
    IMG_SQUARE_RINGS,
    IMG_FILE
} EImageType;

typedef struct {
    const char *file;
    GdkPixbuf *img;
} QtCPixmap;

#define BGND_IMG_ON_BORDER (IMG_FILE==opts.bgndImage.type && opts.bgndImage.onBorder)

typedef enum {
    PP_TL,
    PP_TM,
    PP_TR,
    PP_BL,
    PP_BM,
    PP_BR,
    PP_LM,
    PP_RM,
    PP_CENTRED,
} EPixPos;

typedef struct {
    EImageType type;
    bool       loaded,
               onBorder;
    QtCPixmap  pixmap;
    int        width,
               height;
    EPixPos    pos;
} QtCImage;

typedef enum {
    THIN_BUTTONS    = 0x0001,
    THIN_MENU_ITEMS = 0x0002,
    THIN_FRAMES     = 0x0004
} EThinFlags;

typedef enum {
    SQUARE_NONE               = 0x0000,
    SQUARE_ENTRY              = 0x0001,
    SQUARE_PROGRESS           = 0x0002,
    SQUARE_SCROLLVIEW         = 0x0004,
    SQUARE_LISTVIEW_SELECTION = 0x0008,
    SQUARE_FRAME              = 0x0010,
    SQUARE_TAB_FRAME          = 0x0020,
    SQUARE_SLIDER             = 0x0040,
    SQUARE_SB_SLIDER          = 0x0080,
    SQUARE_WINDOWS            = 0x0100,
    SQUARE_TOOLTIPS           = 0x0200,
    SQUARE_POPUP_MENUS        = 0x0400,

    SQUARE_ALL                = 0xFFFF
} ESquare;

typedef enum {
    WM_DRAG_NONE             = 0,
    WM_DRAG_MENUBAR          = 1,
    WM_DRAG_MENU_AND_TOOLBAR = 2,
    WM_DRAG_ALL              = 3
} EWmDrag;

typedef enum
{
    EFFECT_NONE,
    EFFECT_ETCH,
    EFFECT_SHADOW
} EEffect;

typedef enum
{
    PIX_CHECK,
    PIX_BLANK
} EPixmap;

typedef enum {
    WIDGET_TAB_TOP,
    WIDGET_TAB_BOT,
    WIDGET_STD_BUTTON,
    WIDGET_DEF_BUTTON,
    WIDGET_TOOLBAR_BUTTON,
    WIDGET_LISTVIEW_HEADER,
    WIDGET_SLIDER,
    WIDGET_SLIDER_TROUGH,
    WIDGET_FILLED_SLIDER_TROUGH,
    WIDGET_SB_SLIDER,
    WIDGET_SB_BUTTON,
    WIDGET_SB_BGND,
    WIDGET_TROUGH,
    WIDGET_CHECKBOX,
    WIDGET_RADIO_BUTTON,
    WIDGET_COMBO,
    WIDGET_COMBO_BUTTON,
    WIDGET_MENU_ITEM,
    WIDGET_PROGRESSBAR,
    WIDGET_PBAR_TROUGH,
    WIDGET_ENTRY_PROGRESSBAR,
    WIDGET_TOGGLE_BUTTON,
    WIDGET_SPIN_UP,
    WIDGET_SPIN_DOWN,
    WIDGET_UNCOLOURED_MO_BUTTON,
    WIDGET_SPIN,
    WIDGET_ENTRY,
    WIDGET_SCROLLVIEW,
    WIDGET_SELECTION,
    WIDGET_FRAME,
    WIDGET_NO_ETCH_BTN,
    WIDGET_MENU_BUTTON,        // Qt4 only
    WIDGET_FOCUS,
    WIDGET_TAB_FRAME,
    WIDGET_TOOLTIP,
    WIDGET_OTHER
} EWidget;

typedef enum
{
    APP_ALLOW_BASIC,
    APP_ALLOW_FADE,
    APP_ALLOW_STRIPED,
    APP_ALLOW_NONE
} EAppAllow;

typedef enum
{
    APPEARANCE_CUSTOM1,
    APPEARANCE_CUSTOM2,
    APPEARANCE_CUSTOM3,
    APPEARANCE_CUSTOM4,
    APPEARANCE_CUSTOM5,
    APPEARANCE_CUSTOM6,
    APPEARANCE_CUSTOM7,
    APPEARANCE_CUSTOM8,
    APPEARANCE_CUSTOM9,
    APPEARANCE_CUSTOM10,
    APPEARANCE_CUSTOM11,
    APPEARANCE_CUSTOM12,
    APPEARANCE_CUSTOM13,
    APPEARANCE_CUSTOM14,
    APPEARANCE_CUSTOM15,
    APPEARANCE_CUSTOM16,
    APPEARANCE_CUSTOM17,
    APPEARANCE_CUSTOM18,
    APPEARANCE_CUSTOM19,
    APPEARANCE_CUSTOM20,
    APPEARANCE_CUSTOM21,
    APPEARANCE_CUSTOM22,
    APPEARANCE_CUSTOM23,

        NUM_CUSTOM_GRAD,

    APPEARANCE_FLAT = NUM_CUSTOM_GRAD,
    APPEARANCE_RAISED,
    APPEARANCE_DULL_GLASS,
    APPEARANCE_SHINY_GLASS,
    APPEARANCE_AGUA,
    APPEARANCE_SOFT_GRADIENT,
    APPEARANCE_GRADIENT,
    APPEARANCE_HARSH_GRADIENT,
    APPEARANCE_INVERTED,
    APPEARANCE_DARK_INVERTED,
    APPEARANCE_SPLIT_GRADIENT,
    APPEARANCE_BEVELLED,
        APPEARANCE_FADE, /* Only for poupmenu items! */
        APPEARANCE_STRIPED = APPEARANCE_FADE, /* Only for windows  and menus */
        APPEARANCE_NONE = APPEARANCE_FADE, /* Only for titlebars */
        APPEARANCE_FILE,  /* Only for windows  and menus */
        APPEARANCE_LV_BEVELLED, /* To be used only with qtcGetGradient */
        APPEARANCE_AGUA_MOD,
        APPEARANCE_LV_AGUA,
    NUM_STD_APP = (APPEARANCE_LV_AGUA-NUM_CUSTOM_GRAD)+1
} EAppearance;

#define IS_SLIDER(W)        (WIDGET_SLIDER==(W) || WIDGET_SB_SLIDER==(W))
#define IS_TROUGH(W)        (WIDGET_SLIDER_TROUGH==(W) || WIDGET_PBAR_TROUGH==(W) || WIDGET_TROUGH==(W) || WIDGET_FILLED_SLIDER_TROUGH==(W))
#define IS_TOGGLE_BUTTON(W) (WIDGET_TOGGLE_BUTTON==(W) || WIDGET_CHECKBOX==(W))

typedef enum
{
    CORNER_TL = 0x1,
    CORNER_TR = 0x2,
    CORNER_BR = 0x4,
    CORNER_BL = 0x8
} ECornerBits;

#define ROUNDED_NONE        0x0
#define ROUNDED_TOP         (CORNER_TL|CORNER_TR)
#define ROUNDED_BOTTOM      (CORNER_BL|CORNER_BR)
#define ROUNDED_LEFT        (CORNER_TL|CORNER_BL)
#define ROUNDED_RIGHT       (CORNER_TR|CORNER_BR)
#define ROUNDED_TOPRIGHT    CORNER_TR
#define ROUNDED_BOTTOMRIGHT CORNER_BR
#define ROUNDED_TOPLEFT     CORNER_TL
#define ROUNDED_BOTTOMLEFT  CORNER_BL
#define ROUNDED_ALL         (CORNER_TL|CORNER_TR|CORNER_BR|CORNER_BL)

typedef enum
{
    IND_CORNER,
    IND_FONT_COLOR,
    IND_COLORED,
    IND_TINT,
    IND_GLOW,
    IND_DARKEN,
    IND_SELECTED,
    IND_NONE
} EDefBtnIndicator;

typedef enum
{
    LINE_NONE,
    LINE_SUNKEN,
    LINE_FLAT,
    LINE_DOTS,
    LINE_1DOT,
    LINE_DASHES,
} ELine;

typedef enum
{
    TB_NONE,
    TB_LIGHT,
    TB_DARK,
    TB_LIGHT_ALL,
    TB_DARK_ALL
} ETBarBorder;

typedef enum
{
    TBTN_STANDARD,
    TBTN_RAISED,
    TBTN_JOINED
} ETBarBtn;

typedef enum
{
    BORDER_FLAT,
    BORDER_RAISED,
    BORDER_SUNKEN,
    BORDER_LIGHT
} EBorder;

/*
    This whole EShade enum is a complete mess!
    For menubars, we dont blend - so blend is selected, and selected is darken
    For check/radios - we dont blend, so blend is selected, and we dont allow darken
*/
typedef enum
{
    SHADE_NONE,
    SHADE_CUSTOM,
    SHADE_SELECTED,
    SHADE_BLEND_SELECTED,
    SHADE_DARKEN,
    SHADE_WINDOW_BORDER
} EShade;

typedef enum
{
    ECOLOR_BASE,
    ECOLOR_BACKGROUND,
    ECOLOR_DARK,
} EColor;

typedef enum
{
    ROUND_NONE,
    ROUND_SLIGHT,
    ROUND_FULL,
    ROUND_EXTRA,
    ROUND_MAX
} ERound;

typedef enum
{
    FRAME_NONE,
    FRAME_PLAIN,
    FRAME_LINE,
    FRAME_SHADED,
    FRAME_FADED
} EFrame;

typedef enum
{
    GB_LBL_BOLD     = 0x01,
    GB_LBL_CENTRED  = 0x02,
    GB_LBL_INSIDE   = 0x04,
    GB_LBL_OUTSIDE  = 0x08
} EGBLabel;

#define NO_FRAME(A) (FRAME_NONE==(A) || FRAME_LINE==(A))

typedef enum
{
    MO_NONE,
    MO_COLORED,
    MO_COLORED_THICK,
    MO_PLASTIK,
    MO_GLOW
} EMouseOver;

typedef enum
{
    STRIPE_NONE,
    STRIPE_PLAIN,
    STRIPE_DIAGONAL,
    STRIPE_FADE
} EStripe;

typedef enum
{
    SLIDER_PLAIN,
    SLIDER_ROUND,
    SLIDER_PLAIN_ROTATED,
    SLIDER_ROUND_ROTATED,
    SLIDER_TRIANGULAR,
    SLIDER_CIRCULAR
} ESliderStyle;

#define ROTATED_SLIDER (SLIDER_PLAIN_ROTATED==opts.sliderStyle || SLIDER_ROUND_ROTATED==opts.sliderStyle)

typedef enum
{
    FOCUS_STANDARD,
    FOCUS_RECTANGLE,
    FOCUS_FULL,
    FOCUS_FILLED,
    FOCUS_LINE,
    FOCUS_GLOW
} EFocus;

typedef enum
{
    TAB_MO_TOP,
    TAB_MO_BOTTOM,
    TAB_MO_GLOW
} ETabMo;

typedef enum
{
    GT_HORIZ,
    GT_VERT
} EGradType;

typedef enum
{
    GLOW_NONE,
    GLOW_START,
    GLOW_MIDDLE,
    GLOW_END
} EGlow;

#define FULL_FOCUS     (FOCUS_FULL==opts.focus  || FOCUS_FILLED==opts.focus)

enum
{
    HIDE_NONE     = 0x00,
    HIDE_KEYBOARD = 0x01,
    HIDE_KWIN     = 0x02
};

typedef struct {
    double pos,
           val,
           alpha;
} GradientStop;

typedef enum {
    GB_NONE,
    GB_LIGHT,
    GB_3D,
    GB_3D_FULL,
    GB_SHINE
} EGradientBorder;

typedef struct {
    int titleHeight,
        toolTitleHeight,
        bottom,
        sides;
} WindowBorders;

typedef struct {
    EGradientBorder  border;
    int              numStops;
    GradientStop     *stops;
} Gradient;

#define USE_CUSTOM_SHADES(A) ((A).customShades[0]>0.00001)
#define USE_CUSTOM_ALPHAS(A) ((A).customAlphas[0]>0.00001)

typedef struct {
    int              version,
                     contrast,
                     passwordChar,
                     highlightFactor,
                     lighterPopupMenuBgnd,
                     menuDelay,
                     sliderWidth,
                     tabBgnd,
                     colorSelTab,
                     expanderHighlight,
                     crHighlight,
                     splitterHighlight,
                     crSize,
                     gbFactor,
                     gbLabel,
                     thin;
    ERound           round;
    bool             embolden,
                     highlightTab,
                     roundAllTabs,
                     animatedProgress,
#ifdef QTC_GTK2_ENABLE_PARENTLESS_DIALOG_FIX_SUPPORT
                     fixParentlessDialogs,
#endif
                     customMenuTextColor,
                     menubarMouseOver,
                     useHighlightForMenu,
                     shadeMenubarOnlyWhenActive,
                     lvButton,
                     drawStatusBarFrames,
                     fillSlider,
                     roundMbTopOnly,
                     gtkScrollViews,
                     stdSidebarButtons,
                     toolbarTabs,
                     gtkComboMenus,
                     mapKdeIcons,
                     gtkButtonOrder,
                     fadeLines,
                     reorderGtkButtons,
                     borderMenuitems,
                     colorMenubarMouseOver,
                     darkerBorders,
                     vArrows,
                     xCheck,
                     crButton,
                     smallRadio,
                     fillProgress,
                     comboSplitter,
                     highlightScrollViews,
                     etchEntry,
                     colorSliderMouseOver,
                     thinSbarGroove,
                     flatSbarButtons,
                     borderSbarGroove,
                     borderProgress,
                     popupBorder,
                     unifySpinBtns,
                     unifyCombo,
                     unifySpin,
                     borderTab,
                     borderInactiveTab,
                     doubleGtkComboArrow,
                     menuIcons,
                     forceAlternateLvCols,
                     invertBotTab,
                     boldProgress,
                     coloredTbarMo,
                     borderSelection,
                     stripedSbar,
                     shadePopupMenu,
                     hideShortcutUnderline;
    EFrame           groupBox;
    EGlow            glowProgress;
    bool             lvLines;
    EGradType        bgndGrad,
                     menuBgndGrad;
    int              menubarHiding,
                     statusbarHiding,
                     square,
                     windowDrag,
                     windowBorder,
                     bgndOpacity,
                     menuBgndOpacity,
                     dlgOpacity;
    EStripe          stripedProgress;
    ESliderStyle     sliderStyle;
    EMouseOver       coloredMouseOver;
    ETBarBorder      toolbarBorders;
    ETBarBtn         tbarBtns;
    EDefBtnIndicator defBtnIndicator;
    ELine            sliderThumbs,
                     handles,
                     toolbarSeparators,
                     splitters;
    ETabMo           tabMouseOver;
/* NOTE: If add an appearance setting, increase the number of custmo gradients to match! */
    EAppearance      appearance,
                     bgndAppearance,
                     menuBgndAppearance,
                     menubarAppearance,
                     menuitemAppearance,
                     toolbarAppearance,
                     lvAppearance,
                     tabAppearance,
                     activeTabAppearance,
                     sliderAppearance,
                     titlebarAppearance,
                     inactiveTitlebarAppearance,
                     selectionAppearance,
                     menuStripeAppearance,
                     progressAppearance,
                     progressGrooveAppearance,
                     grooveAppearance,
                     sunkenAppearance,
                     sbarBgndAppearance,
                     sliderFill,
                     tooltipAppearance,
                     tbarBtnAppearance;
    EShade           shadeSliders,
                     shadeMenubars,
                     menuStripe,
                     shadeCheckRadio,
                     comboBtn,
                     sortedLv,
                     crColor,
                     progressColor;
    EColor           progressGrooveColor;
    EEffect          buttonEffect,
                     tbarBtnEffect;
    EScrollbar       scrollbarType;
    EFocus           focus;
    color            customMenubarsColor,
                     customSlidersColor,
                     customMenuNormTextColor,
                     customMenuSelTextColor,
                     customMenuStripeColor,
                     customCheckRadioColor,
                     customComboBtnColor,
                     customSortedLvColor,
                     customCrBgndColor,
                     customProgressColor;
    EShading         shading;
    double           customShades[QTC_NUM_STD_SHADES],
                     customAlphas[NUM_STD_ALPHAS];
    Gradient         *customGradient[NUM_CUSTOM_GRAD];
    QtCPixmap        bgndPixmap;
    QtCPixmap        menuBgndPixmap;
    QtCImage         bgndImage,
                     menuBgndImage;
    /* NOTE: If add any more settings here, need to alter
       copyOpts/freeOpts/defaultSettings in config_file.c */
    Strings          noBgndGradientApps,
                     noBgndOpacityApps,
                     noMenuBgndOpacityApps,
                     noBgndImageApps;
#ifdef QTC_GTK2_ENABLE_PARENTLESS_DIALOG_FIX_SUPPORT
    Strings          noDlgFixApps;
#endif
    Strings          noMenuStripeApps;
} Options;

#include <qtcurve-utils/color.h>
#define tint(COLA, COLB, FACTOR) qtcColorTint((COLA), (COLB), (FACTOR))
#define midColor(COLA, COLB) qtcColorMix((COLA), (COLB), 0.5)

void qtcSetupGradient(Gradient *grad, EGradientBorder border, int numStops, ...);
const Gradient *qtcGetGradient(EAppearance app, const Options *opts);

EAppearance qtcWidgetApp(EWidget w, const Options *opts);

typedef enum {
    RADIUS_SELECTION,
    RADIUS_INTERNAL,
    RADIUS_EXTERNAL,
    RADIUS_ETCH
} ERadius;

#define MIN_ROUND_MAX_HEIGHT    12
#define MIN_ROUND_MAX_WIDTH     24
#define BGND_SHINE_SIZE 300
#define BGND_SHINE_STEPS  8

#define MIN_ROUND_FULL_SIZE     8
#define MIN_ROUND_EXTRA_SIZE(W) (WIDGET_SPIN_UP==(W) || WIDGET_SPIN_DOWN==(W) || WIDGET_SPIN==(W) ? 7 : 14)

#define IS_MAX_ROUND_WIDGET(A) \
            (WIDGET_STD_BUTTON==A || WIDGET_DEF_BUTTON==A || WIDGET_TOGGLE_BUTTON==A /*|| WIDGET_MENU_BUTTON==A*/)
#define IS_EXTRA_ROUND_WIDGET(A) \
            (A!=WIDGET_MENU_ITEM && A!=WIDGET_TAB_FRAME && A!=WIDGET_PBAR_TROUGH && A!=WIDGET_PROGRESSBAR)

#define EXTRA_INNER_RADIUS   4
#define EXTRA_OUTER_RADIUS   5
#define EXTRA_ETCH_RADIUS    6
#define FULL_INNER_RADIUS    2
#define FULL_OUTER_RADIUS    3
#define FULL_ETCH_RADIUS     4
#define SLIGHT_INNER_RADIUS  1
#define SLIGHT_OUTER_RADIUS  2
#define SLIGHT_ETCH_RADIUS   3

#define MAX_RADIUS_INTERNAL 9.0
#define MAX_RADIUS_EXTERNAL (MAX_RADIUS_INTERNAL+2.0)

ERound qtcGetWidgetRound(const Options *opts, int w, int h, EWidget widget);
double qtcGetRadius(const Options *opts, int w, int h, EWidget widget,
                    ERadius rad);

#endif // __COMMON_H__
