#ifndef __QTC_DRAWING_H__
#define __QTC_DRAWING_H__

/*
  QtCurve (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com

  ----

  This program is free software; you can redistribute it and/or
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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <common/common.h>
#include "compatability.h"

#define M_PI 3.14159265358979323846

#if (CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1, 10, 0))
#define CAIRO_GRAD_END 0.999
#else
#define CAIRO_GRAD_END 1.0
#endif

#if GTK_CHECK_VERSION(2, 90, 0)
    #define CAIRO_BEGIN \
        { \
        GdkRectangle *area=NULL; \
        cairo_set_line_width(cr, 1.0);

    #define CAIRO_END }
#else
    #define CAIRO_BEGIN \
        if(GDK_IS_DRAWABLE(window)) \
        { \
        cairo_t *cr=(cairo_t*)gdk_cairo_create(window); \
        setCairoClipping(cr, area); \
        cairo_set_line_width(cr, 1.0);

    #define CAIRO_END \
        cairo_destroy(cr); \
        }
#endif

void clipToRegion(cairo_t *cr, QtcRegion *region);
void setCairoClippingRegion(cairo_t *cr, QtcRegion *region);
void setCairoClipping(cairo_t *cr, GdkRectangle *area);
#define unsetCairoClipping(A) cairo_restore(A)
void drawHLine(cairo_t *cr, double r, double g, double b, double a, int x, int y, int w);
void drawVLine(cairo_t *cr, double r, double g, double b, double a, int x, int y, int h);
#define drawAreaColor(cr, area, col, x, y, width, height) \
        drawAreaColorAlpha(cr, area, col, x, y, width, height, 1.0)
void drawAreaColorAlpha(cairo_t *cr, GdkRectangle *area, GdkColor *col, gint x, gint y, gint width, gint height, double alpha);
void drawBgnd(cairo_t *cr, GdkColor *col, GtkWidget *widget, GdkRectangle *area, int x, int y, int width, int height);
void drawAreaModColor(cairo_t *cr, GdkRectangle *area, GdkColor *orig, double mod, gint x, gint y, gint width, gint height);
#define drawAreaMod(/*cairo_t *       */  cr, \
                    /* GtkStyle *     */  style, \
                    /* GtkStateType   */  state, \
                    /* GdkRectangle * */  area, \
                    /* double         */  mod, \
                    /* gint           */  x, \
                    /* gint           */  y, \
                    /* gint           */  width, \
                    /* gint           */  height) \
    drawAreaModColor(cr, area, &style->bg[state], mod, x, y, width, height)

#define drawBevelGradient(cr, area, x, y, width, height, base, horiz, sel, bevApp, w) \
        drawBevelGradientAlpha(cr, area, x, y, width, height, base, horiz, sel, bevApp, w, 1.0)
void drawBevelGradientAlpha(cairo_t *cr, GdkRectangle *area, int x, int y, int width, int height, GdkColor *base, gboolean horiz,
                                   gboolean sel, EAppearance bevApp, EWidget w, double alpha);

typedef enum
{
    DF_DRAW_INSIDE     = 0x001,
    DF_BLEND           = 0x002,
    DF_SUNKEN          = 0x004,
    DF_DO_BORDER       = 0x008,
    DF_VERT            = 0x010,
    DF_HIDE_EFFECT     = 0x020,
    DF_HAS_FOCUS       = 0x040
} EDrawFlags;

#define drawBorder(a, b, c, d, e, f, g, h, i, j, k, l, m) \
    realDrawBorder(a, b, c, d, e, f, g, h, i, j, k, l, m, QTC_STD_BORDER)

void plotPoints(cairo_t *cr, GdkPoint *pts, int count);
void createTLPath(cairo_t *cr, double xd, double yd, double width, double height, double radius, int round);
void createBRPath(cairo_t *cr, double xd, double yd, double width, double height, double radius, int round);
void createPath(cairo_t *cr, double xd, double yd, double width, double height, double radius, int round);
void realDrawBorder(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkRectangle *area,
                           gint x, gint y, gint width, gint height, GdkColor *c_colors, int round, EBorder borderProfile,
                           EWidget widget, int flags, int borderVal);
#define drawGlow(cr, area, x, y, w, h, round, widget) \
        drawGlowReal(cr, area, x, y, w, h, round, widget, NULL)
void drawGlowReal(cairo_t *cr, GdkRectangle *area, int x, int y, int w, int h, int round, EWidget widget, const GdkColor *colors);
void drawEtch(cairo_t *cr, GdkRectangle *area, GtkWidget *widget, int x, int y, int w, int h, gboolean raised, int round, EWidget wid);
void clipPathRadius(cairo_t *cr, double x, double y, int w, int h, double radius, int round);
void clipPath(cairo_t *cr, int x, int y, int w, int h, EWidget widget, int rad, int round);
void addStripes(cairo_t *cr, int x, int y, int w, int h, bool horizontal);
void drawLightBevel(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkRectangle *area, gint x, gint y,
                           gint width, gint height, GdkColor *base, GdkColor *colors, int round, EWidget widget,
                           EBorder borderProfile, int flags, GtkWidget *wid);
#define drawFadedLine(cr, x, y, width, height, col, area, gap, fadeStart, fadeEnd, horiz) \
        drawFadedLineReal(cr, x, y, width, height, col, area, gap, fadeStart, fadeEnd, horiz, 1.0)

void drawFadedLineReal(cairo_t *cr, int x, int y, int width, int height, GdkColor *col, GdkRectangle *area, GdkRectangle *gap,
                              gboolean fadeStart, gboolean fadeEnd, gboolean horiz, double alpha);
void drawHighlight(cairo_t *cr, int x, int y, int width, int height, GdkRectangle *area, gboolean horiz, gboolean inc);
void setLineCol(cairo_t *cr, cairo_pattern_t *pt, GdkColor *col);
void drawLines(cairo_t *cr, double rx, double ry, int rwidth, int rheight, gboolean horiz,
                      int nLines, int offset, GdkColor *cols, GdkRectangle *area, int dark, ELine type);
void drawDot(cairo_t *cr, int x, int y, int w, int h, GdkColor *cols);
void drawDots(cairo_t *cr, int rx, int ry, int rwidth, int rheight, gboolean horiz, int nLines, int offset, GdkColor *cols, GdkRectangle *area,
                     int startOffset, int dark);
void drawEntryCorners(cairo_t *cr, GdkRectangle *area, int round, int x, int y, int width, int height, double r, double g, double b, double a);
void drawBgndRing(cairo_t *cr, int x, int y, int size, int size2, gboolean isWindow);
void drawBgndRings(cairo_t *cr, gint x, gint y, gint width, gint height, gboolean isWindow);
void drawBgndImage(cairo_t *cr, GtkStyle *style, GdkRectangle *area, gint x, gint y, gint w, gint h, GdkColor *col,
                          gboolean isWindow, double alpha);
void drawStripedBgnd(cairo_t *cr, GtkStyle *style, GdkRectangle *area, gint x, gint y, gint w, gint h, GdkColor *col,
                            gboolean isWindow, double alpha);
gboolean drawWindowBgnd(cairo_t *cr, GtkStyle *style, GdkRectangle *area, GdkWindow *window, GtkWidget *widget,
                               gint x, gint y, gint width, gint height);
void drawEntryField(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkWindow *window, GtkWidget *widget, GdkRectangle *area,
                           gint x, gint y, gint width, gint height, int round, EWidget w);
void setProgressStripeClipping(cairo_t *cr, GdkRectangle *area, int x, int y, int width, int height, int animShift, gboolean horiz);
void drawProgress(cairo_t *cr, GtkStyle *style, GtkStateType state, GtkWidget *widget, GdkRectangle *area, int x, int y, int width, int height,
                         gboolean rev, gboolean isEntryProg);
void drawProgressGroove(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkWindow *window, GtkWidget *widget, GdkRectangle *area,
                               int x, int y, int width, int height, gboolean isList, gboolean horiz);
void drawSliderGroove(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkWindow *window, GtkWidget *widget, const gchar *detail,
                             GdkRectangle *area, int x, int y, int width, int height, gboolean horiz);
void drawTriangularSlider(cairo_t *cr, GtkStyle *style, GtkStateType state, const gchar *detail, GdkRectangle *area, int x, int y, int width, int height);
void drawScrollbarGroove(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkWindow *window, GtkWidget *widget, const gchar *detail,
                                GdkRectangle *area, int x, int y, int width, int height, gboolean horiz);
void drawSelectionGradient(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkRectangle *area,
                                  int x, int y, int width, int height, int round, gboolean isLvSelection,
                                  double alpha, GdkColor *col, gboolean horiz);
void drawSelection(cairo_t *cr, GtkStyle *style, GtkStateType state, GdkRectangle *area, GtkWidget *widget,
                          int x, int y, int width, int height, int round, gboolean isLvSelection, double alphaMod, int factor);
void createRoundedMask(cairo_t *cr, GtkWidget *widget, gint x, gint y, gint width, gint height, double radius, gboolean isToolTip);
void clearRoundedMask(GtkWidget *widget, gboolean isToolTip);
void drawTreeViewLines(cairo_t *cr, GdkColor *col, int x, int y, int h, int depth, int levelIndent, int expanderSize,
                              GtkTreeView *treeView, GtkTreePath *path, GtkTreeViewColumn *column);
void drawPolygon(GdkWindow *window, GtkStyle *style, GdkColor *col, GdkRectangle *area, GdkPoint *points, int npoints, gboolean fill);
void drawArrow(GdkWindow *window, GtkStyle *style, GdkColor *col, GdkRectangle *area, GtkArrowType arrow_type,
                      gint x, gint y, gboolean small, gboolean fill);
void drawLayout(GtkStyle *style, GdkWindow *window, GtkStateType state, gboolean use_text, GdkRectangle *area, gint x, gint y, PangoLayout *layout);
void fillTab(cairo_t *cr, GtkStyle *style, GtkWidget *widget, GdkRectangle *area, GtkStateType state,
                    GdkColor *col, int x, int y, int width, int height, gboolean horiz, EWidget tab, gboolean grad);
void colorTab(cairo_t *cr, int x, int y, int width, int height, int round, EWidget tab, gboolean horiz);
void drawToolTip(cairo_t *cr, GtkWidget *widget, GdkRectangle *area, int x, int y, int width, int height);
void drawSplitter(cairo_t *cr, GtkStateType state, GtkStyle *style, GdkRectangle *area, int x, int y, int width, int height);
void drawSidebarButton(cairo_t *cr, GtkStateType state, GtkStyle *style, GdkRectangle *area, int x, int y, int width, int height);
void drawMenuItem(cairo_t *cr, GtkStateType state, GtkStyle *style, GtkWidget *widget, GdkRectangle *area, int x, int y, int width, int height);
void drawMenu(cairo_t *cr, GtkStateType state, GtkStyle *style, GtkWidget *widget, GdkRectangle *area, int x, int y, int width, int height);
void drawBoxGap(cairo_t *cr, GtkStyle *style, GtkShadowType shadow, GtkStateType state,
                       GtkWidget *widget, GdkRectangle *area, gint x, gint y, gint width, gint height, GtkPositionType gap_side,
                       gint gapX, gint gapWidth, EBorder borderProfile, gboolean isTab);
void drawBoxGapFixes(cairo_t *cr, GtkWidget *widget, gint x, gint y, gint width, gint height, GtkPositionType gapSide, gint gapX, gint gapWidth);
void drawShadowGap(cairo_t *cr, GtkStyle *style, GtkShadowType shadow, GtkStateType state,
                          GtkWidget *widget, GdkRectangle *area, gint x, gint y, gint width, gint height, GtkPositionType gapSide,
                          gint gapX, gint gapWidth);
void drawCheckBox(cairo_t *cr, GtkStateType state, GtkShadowType shadow, GtkStyle *style, GtkWidget *widget, const gchar *detail,
                         GdkRectangle *area, int x, int y, int width, int height);
void drawTab(cairo_t *cr, GtkStateType state, GtkStyle *style, GtkWidget *widget, const gchar *detail,
                    GdkRectangle *area, int x, int y, int width, int height, GtkPositionType gapSide);
void drawRadioButton(cairo_t *cr, GtkStateType state, GtkShadowType shadow, GtkStyle *style, GtkWidget *widget, const gchar *detail,
                            GdkRectangle *area, int x, int y, int width, int height);
void drawToolbarBorders(cairo_t *cr, GtkStateType state, int x, int y, int width, int height, gboolean isActiveWindowMenubar, const char *detail);
void drawListViewHeader(cairo_t *cr, GtkStateType state, GdkColor *btnColors, int bgnd, GdkRectangle *area, int x, int y, int width, int height);
void drawDefBtnIndicator(cairo_t *cr, GtkStateType state, GdkColor *btnColors, int bgnd, gboolean sunken, GdkRectangle *area, int x, int y, int width, int height);
GdkPixbuf * renderIcon(GtkStyle *style, const GtkIconSource *source, GtkTextDirection direction,
                              GtkStateType state, GtkIconSize size, GtkWidget *widget, const char *detail);
#endif
