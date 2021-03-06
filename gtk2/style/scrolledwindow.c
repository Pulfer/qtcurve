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

#include <qtcurve-utils/gtkutils.h>

#include "compatability.h"
#include <common/common.h>

extern Options opts;

static void
qtcScrolledWindowCleanup(GtkWidget *widget)
{
    GObject *obj;
    if (widget && (obj = G_OBJECT(widget)) &&
        g_object_get_data(obj, "QTC_SCROLLED_WINDOW_SET")) {
        qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_DESTROY_ID");
        qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_UNREALIZE_ID");
        qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_STYLE_SET_ID");
        if (ENTRY_MO) {
            qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_ENTER_ID");
            qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_LEAVE_ID");
        }
        qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_FOCUS_IN_ID");
        qtcDisconnectFromData(obj, "QTC_SCROLLED_WINDOW_FOCUS_OUT_ID");
        g_object_steal_data(obj, "QTC_SCROLLED_WINDOW_SET");
    }
}

static gboolean
qtcScrolledWindowStyleSet(GtkWidget *widget, GtkStyle *prev, gpointer data)
{
    QTC_UNUSED(prev);
    QTC_UNUSED(data);
    qtcScrolledWindowCleanup(widget);
    return FALSE;
}

static gboolean
qtcScrolledWindowDestroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    QTC_UNUSED(event);
    QTC_UNUSED(data);
    qtcScrolledWindowCleanup(widget);
    return FALSE;
}

static GtkWidget *qtcScrolledWindowFocus = NULL;
static GtkWidget *qtcScrolledWindowHover = NULL;

gboolean qtcScrolledWindowHasFocus(GtkWidget *widget)
{
    return widget && (gtk_widget_has_focus(widget) || widget==qtcScrolledWindowFocus);
}

gboolean qtcScrolledWindowHovered(GtkWidget *widget)
{
    return widget && (GTK_STATE_PRELIGHT==gtk_widget_get_state(widget) || widget==qtcScrolledWindowHover);
}

static gboolean
qtcScrolledWindowEnter(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    QTC_UNUSED(event);
    GtkWidget *w = data ? (GtkWidget*)data : widget;
    if (GTK_IS_SCROLLED_WINDOW(w) && qtcScrolledWindowHover != w) {
        qtcScrolledWindowHover = w;
        gtk_widget_queue_draw(w);
    }
    return FALSE;
}

static gboolean
qtcScrolledWindowLeave(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    QTC_UNUSED(event);
    GtkWidget *w = data ? (GtkWidget*)data : widget;
    if (GTK_IS_SCROLLED_WINDOW(w) && qtcScrolledWindowHover == w) {
        qtcScrolledWindowHover = NULL;
        gtk_widget_queue_draw(w);
    }
    return FALSE;
}

static gboolean
qtcScrolledWindowFocusIn(GtkWidget *widget, GdkEventMotion *e, gpointer data)
{
    QTC_UNUSED(e);
    GtkWidget *w = data ? (GtkWidget*)data : widget;
    if (GTK_IS_SCROLLED_WINDOW(w) && qtcScrolledWindowFocus != w) {
        qtcScrolledWindowFocus = w;
        gtk_widget_queue_draw(w);
    }
    return FALSE;
}

static gboolean
qtcScrolledWindowFocusOut(GtkWidget *widget, GdkEventMotion *e, gpointer data)
{
    QTC_UNUSED(e);
    GtkWidget *w = data ? (GtkWidget*)data : widget;
    if (GTK_IS_SCROLLED_WINDOW(w) && qtcScrolledWindowFocus == w) {
        qtcScrolledWindowFocus = NULL;
        gtk_widget_queue_draw(w);
    }
    return FALSE;
}

static void qtcScrolledWindowSetupConnections(GtkWidget *widget, GtkWidget *parent)
{
    GObject *obj;
    if (widget && (obj = G_OBJECT(widget)) &&
        !g_object_get_data(obj, "QTC_SCROLLED_WINDOW_SET")) {
        gtk_widget_add_events(widget, GDK_LEAVE_NOTIFY_MASK |
                              GDK_ENTER_NOTIFY_MASK | GDK_FOCUS_CHANGE_MASK);
        g_object_set_data(obj, "QTC_SCROLLED_WINDOW_SET", (gpointer)1);
        qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_DESTROY_ID",
                         "destroy-event", qtcScrolledWindowDestroy, parent);
        qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_UNREALIZE_ID",
                         "unrealize", qtcScrolledWindowDestroy, parent);
        qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_STYLE_SET_ID",
                         "style-set", qtcScrolledWindowStyleSet, parent);
        if (ENTRY_MO) {
            qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_ENTER_ID",
                             "enter-notify-event",
                             qtcScrolledWindowEnter, parent);
            qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_LEAVE_ID",
                             "leave-notify-event",
                             qtcScrolledWindowLeave, parent);
        }
        qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_FOCUS_IN_ID",
                         "focus-in-event", qtcScrolledWindowFocusIn, parent);
        qtcConnectToData(obj, "QTC_SCROLLED_WINDOW_FOCUS_OUT_ID",
                         "focus-out-event", qtcScrolledWindowFocusOut, parent);
        if (parent && ENTRY_MO) {
            gint x, y;
            GtkAllocation alloc = qtcWidgetGetAllocation(parent);

            gdk_window_get_pointer(gtk_widget_get_window(parent), &x, &y, 0L);
            if (x >= 0 && x <alloc.width && y >= 0 && y < alloc.height) {
                qtcScrolledWindowHover = parent;
            }
        }
    }
}

void qtcScrolledWindowRegisterChild(GtkWidget *child)
{
    GtkWidget *parent=child ? gtk_widget_get_parent(child) : NULL;

    if(parent && GTK_IS_SCROLLED_WINDOW(parent) && g_object_get_data(G_OBJECT(parent), "QTC_SCROLLED_WINDOW_SET"))
        qtcScrolledWindowSetupConnections(child, parent);
}

void qtcScrolledWindowSetup(GtkWidget *widget)
{
    if (widget && GTK_IS_SCROLLED_WINDOW(widget) && !g_object_get_data(G_OBJECT(widget), "QTC_SCROLLED_WINDOW_SET"))
    {
        GtkScrolledWindow *scrolledWindow=GTK_SCROLLED_WINDOW(widget);
        GtkWidget         *child;

        if((child=gtk_scrolled_window_get_hscrollbar(scrolledWindow)))
            qtcScrolledWindowSetupConnections(child, widget);
        if((child=gtk_scrolled_window_get_vscrollbar(scrolledWindow)))
            qtcScrolledWindowSetupConnections(child, widget);
        if((child=gtk_bin_get_child(GTK_BIN(widget))))
        {
            if(GTK_IS_TREE_VIEW(child) || GTK_IS_TEXT_VIEW(child) || GTK_IS_ICON_VIEW(child))
                qtcScrolledWindowSetupConnections(child, widget);
            else
            {
                const gchar *type=g_type_name(G_OBJECT_TYPE(child));

                if(type && (0==strcmp(type, "ExoIconView") || 0==strcmp(type, "FMIconContainer")))
                    qtcScrolledWindowSetupConnections(child, widget);
            }
        }

        g_object_set_data(G_OBJECT(widget), "QTC_SCROLLED_WINDOW_SET", (gpointer)1);
    }
}
