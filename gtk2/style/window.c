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

#include <qtcurve-utils/x11qtc.h>
#include <qtcurve-utils/gtkutils.h>
#include <qtcurve-utils/log.h>

#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include "compatability.h"
#include <common/common.h>
#include <common/config_file.h>
#include "qt_settings.h"
#include "window.h"
#include "menu.h"

extern Options opts;

static GtkWidget *qtcCurrentActiveWindow = NULL;

typedef struct {
    int width;
    int height;
    int timer;
    GtkWidget *widget;
    gboolean locked;
} QtCWindow;

static GHashTable *qtcWindowTable = NULL;

static QtCWindow*
qtcWindowLookupHash(void *hash, gboolean create)
{
    QtCWindow *rv = NULL;

    if (!qtcWindowTable)
        qtcWindowTable = g_hash_table_new(g_direct_hash, g_direct_equal);

    rv = (QtCWindow*)g_hash_table_lookup(qtcWindowTable, hash);

    if (!rv && create) {
        rv = (QtCWindow*)malloc(sizeof(QtCWindow));
        rv->width = rv->height = rv->timer = 0;
        rv->widget = NULL;
        rv->locked = FALSE;
        g_hash_table_insert(qtcWindowTable, hash, rv);
        rv = g_hash_table_lookup(qtcWindowTable, hash);
    }
    return rv;
}

static void
qtcWindowRemoveFromHash(void *hash)
{
    if (qtcWindowTable) {
        QtCWindow *tv = qtcWindowLookupHash(hash, FALSE);
        if (tv) {
            if (tv->timer)
                g_source_remove(tv->timer);
            g_hash_table_remove(qtcWindowTable, hash);
        }
    }
}

static void
qtcWindowCleanup(GtkWidget *widget)
{
    if (widget) {
        GObject *obj = G_OBJECT(widget);
        if (!(IS_FLAT_BGND(opts.bgndAppearance)) ||
            IMG_NONE!=opts.bgndImage.type) {
            qtcWindowRemoveFromHash(widget);
            qtcDisconnectFromData(obj, "QTC_WINDOW_CONFIGURE_ID");
            qtcDisconnectFromData(obj, "QTC_WINDOW_DESTROY_ID");
            qtcDisconnectFromData(obj, "QTC_WINDOW_STYLE_SET_ID");
        }
        if ((opts.menubarHiding & HIDE_KEYBOARD) ||
            (opts.statusbarHiding & HIDE_KEYBOARD))
            qtcDisconnectFromData(obj, "QTC_WINDOW_KEY_RELEASE_ID");
        if ((opts.menubarHiding & HIDE_KWIN) ||
            (opts.statusbarHiding & HIDE_KWIN))
            qtcDisconnectFromData(obj, "QTC_WINDOW_MAP_ID");
        if (opts.shadeMenubarOnlyWhenActive || BLEND_TITLEBAR ||
            opts.menubarHiding || opts.statusbarHiding)
            qtcDisconnectFromData(obj, "QTC_WINDOW_CLIENT_EVENT_ID");
        g_object_steal_data(obj, "QTC_WINDOW_HACK_SET");
    }
}

static gboolean
qtcWindowStyleSet(GtkWidget *widget, GtkStyle *prev_style, gpointer data)
{
    QTC_UNUSED(prev_style);
    QTC_UNUSED(data);
    qtcWindowCleanup(widget);
    return FALSE;
}

static gboolean qtcWindowToggleMenuBar(GtkWidget *widget);
static gboolean qtcWindowToggleStatusBar(GtkWidget *widget);

static gboolean
qtcWindowClientEvent(GtkWidget *widget, GdkEventClient *event, gpointer data)
{
    QTC_UNUSED(data);
    if (gdk_x11_atom_to_xatom(event->message_type) ==
        qtc_x11_atoms[QTC_X11_ATOM_QTC_ACTIVE_WINDOW]) {
        if (event->data.l[0]) {
            qtcCurrentActiveWindow = widget;
        } else if (qtcCurrentActiveWindow == widget) {
            qtcCurrentActiveWindow = 0L;
        }
        gtk_widget_queue_draw(widget);
    } else if (gdk_x11_atom_to_xatom(event->message_type) ==
               qtc_x11_atoms[QTC_X11_ATOM_QTC_TITLEBAR_SIZE]) {
        qtcGetWindowBorderSize(true);
        GtkWidget *menubar = qtcWindowGetMenuBar(widget, 0);

        if (menubar) {
            gtk_widget_queue_draw(menubar);
        }
    } else if (gdk_x11_atom_to_xatom(event->message_type) ==
               qtc_x11_atoms[QTC_X11_ATOM_QTC_TOGGLE_MENUBAR]) {
        if (opts.menubarHiding & HIDE_KWIN && qtcWindowToggleMenuBar(widget)) {
            gtk_widget_queue_draw(widget);
        }
    } else if (gdk_x11_atom_to_xatom(event->message_type) ==
               qtc_x11_atoms[QTC_X11_ATOM_QTC_TOGGLE_STATUSBAR]) {
        if (opts.statusbarHiding & HIDE_KWIN &&
            qtcWindowToggleStatusBar(widget)) {
            gtk_widget_queue_draw(widget);
        }
    }
    return FALSE;
}

static gboolean
qtcWindowDestroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    QTC_UNUSED(event);
    QTC_UNUSED(data);
    qtcWindowCleanup(widget);
    return FALSE;
}

gboolean
qtcWindowIsActive(GtkWidget *widget)
{
    return widget && (gtk_window_is_active(GTK_WINDOW(widget)) ||
                      qtcCurrentActiveWindow == widget);
}

static gboolean
qtcWindowSizeRequest(GtkWidget *widget)
{
    if (widget && (!(IS_FLAT_BGND(opts.bgndAppearance)) ||
                   IMG_NONE != opts.bgndImage.type)) {
        GtkAllocation alloc = qtcWidgetGetAllocation(widget);
        GdkRectangle rect;
        rect.x = 0;
        rect.y = 0;
        if (IS_FLAT(opts.bgndAppearance) &&
            IMG_NONE != opts.bgndImage.type) {
            EPixPos pos = (IMG_FILE == opts.bgndImage.type ?
                           opts.bgndImage.pos : PP_TR);
            if (IMG_FILE == opts.bgndImage.type) {
                qtcLoadBgndImage(&opts.bgndImage);
            }
            switch (pos) {
            case PP_TL:
                rect.width  = opts.bgndImage.width + 1;
                rect.height = opts.bgndImage.height + 1;
                break;
            case PP_TM:
            case PP_TR:
                rect.width = alloc.width;
                rect.height = (IMG_FILE == opts.bgndImage.type ?
                               opts.bgndImage.height :
                               RINGS_HEIGHT(opts.bgndImage.type)) + 1;
                break;
            case PP_LM:
            case PP_BL:
                rect.width = opts.bgndImage.width + 1;
                rect.height = alloc.height;
                break;
            case PP_CENTRED:
            case PP_BR:
            case PP_BM:
            case PP_RM:
                rect.width = alloc.width;
                rect.height = alloc.height;
                break;
            }
            if (alloc.width < rect.width) {
                rect.width = alloc.width;
            }
            if (alloc.height < rect.height) {
                rect.height = alloc.height;
            }
        } else {
            rect.width = alloc.width, rect.height = alloc.height;
        }
        gdk_window_invalidate_rect(gtk_widget_get_window(widget), &rect, FALSE);
    }
    return FALSE;
}

static gboolean
qtcWindowDelayedUpdate(gpointer user_data)
{
    QtCWindow *window = (QtCWindow*)user_data;

    if (window) {
        if (window->locked) {
            window->locked = FALSE;
            return TRUE;
        } else {
            g_source_remove(window->timer);
            window->timer = 0;
            // otherwise, trigger update
            gdk_threads_enter();
            qtcWindowSizeRequest(window->widget);
            gdk_threads_leave();
            return FALSE;
        }
    }
    return FALSE;
}

static gboolean
qtcWindowConfigure(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    QTC_UNUSED(widget);
    QtCWindow *window = (QtCWindow*)data;

    if (window && (event->width != window->width ||
                   event->height != window->height)) {
        window->width = event->width;
        window->height = event->height;

        // schedule delayed timeOut
        if (!window->timer) {
            window->timer =
                g_timeout_add(50, (GSourceFunc)qtcWindowDelayedUpdate, window);
            window->locked = FALSE;
        } else {
            window->locked = TRUE;
        }
    }
    return FALSE;
}

static gboolean
canGetChildren(GtkWidget *widget)
{
    return (GTK_APP_GHB != qtSettings.app ||
            0 != strcmp(g_type_name(G_OBJECT_TYPE(widget)), "GhbCompositor") ||
            gtk_widget_get_realized(widget));
}

GtkWidget*
qtcWindowGetMenuBar(GtkWidget *parent, int level)
{
    if (level < 3 && GTK_IS_CONTAINER(parent) && canGetChildren(parent)
        /* && gtk_widget_get_realized(parent)*/) {
        GtkWidget *rv = NULL;
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        GList *child = children;

        for (;child && !rv;child = child->next) {
            GtkWidget *boxChild = (GtkWidget*)child->data;

            if (GTK_IS_MENU_BAR(boxChild)) {
                rv = GTK_WIDGET(boxChild);
            } else if (GTK_IS_CONTAINER(boxChild)) {
                rv=qtcWindowGetMenuBar(GTK_WIDGET(boxChild), level + 1);
            }
        }

        if (children)
            g_list_free(children);
        return rv;
    }
    return NULL;
}

GtkWidget*
qtcWindowGetStatusBar(GtkWidget *parent, int level)
{
    if (level < 3 && GTK_IS_CONTAINER(parent) && canGetChildren(parent)
        /* && gtk_widget_get_realized(parent)*/) {
        GtkWidget *rv = NULL;
        GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
        GList *child = children;

        for(;child && !rv;child = child->next) {
            GtkWidget *boxChild = (GtkWidget*)child->data;

            if (GTK_IS_STATUSBAR(boxChild)) {
                rv=GTK_WIDGET(boxChild);
            } else if (GTK_IS_CONTAINER(boxChild)) {
                rv=qtcWindowGetStatusBar(GTK_WIDGET(boxChild), level + 1);
            }
        }
        if (children) {
            g_list_free(children);
        }
        return rv;
    }
    return NULL;
}

void
qtcWindowMenuBarDBus(GtkWidget *widget, int size)
{
    GtkWindow *topLevel = GTK_WINDOW(gtk_widget_get_toplevel(widget));
    unsigned int xid = GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(topLevel)));

    char cmd[160];
    //sprintf(cmd, "qdbus org.kde.kwin /QtCurve menuBarSize %u %d", xid, size);
    sprintf(cmd, "dbus-send --type=method_call --session --dest=org.kde.kwin /QtCurve org.kde.QtCurve.menuBarSize uint32:%u int32:%d",
            xid, size);
    system(cmd);
    /*
      char         xidS[16],
      sizeS[16];
      char         *args[]={"qdbus", "org.kde.kwin", "/QtCurve", "menuBarSize", xidS, sizeS, NULL};

      sprintf(xidS, "%u", xid);
      sprintf(sizeS, "%d", size);
      g_spawn_async("/tmp", args, NULL, (GSpawnFlags)0, NULL, NULL, NULL, NULL);
    */
}

void
qtcWindowStatusBarDBus(GtkWidget *widget, gboolean state)
{
    GtkWindow *topLevel = GTK_WINDOW(gtk_widget_get_toplevel(widget));
    unsigned int xid = GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(topLevel)));

    char cmd[160];
    //sprintf(cmd, "qdbus org.kde.kwin /QtCurve statusBarState %u %s", xid, state ? "true" : "false");
    sprintf(cmd, "dbus-send --type=method_call --session --dest=org.kde.kwin /QtCurve org.kde.QtCurve.statusBarState uint32:%u boolean:%s",
            xid, state ? "true" : "false");
    system(cmd);
    /*
      char         xidS[16],
      stateS[6];
      char         *args[]={"qdbus", "org.kde.kwin", "/QtCurve", "statusBarState", xidS, stateS, NULL};

      sprintf(xidS, "%u", xid);
      sprintf(stateS, "%s", state ? "true" : "false");
      g_spawn_async("/tmp", args, NULL, (GSpawnFlags)0, NULL, NULL, NULL, NULL);
    */
}

static gboolean
qtcWindowToggleMenuBar(GtkWidget *widget)
{
    GtkWidget *menuBar = qtcWindowGetMenuBar(widget, 0);

    if (menuBar) {
        int size = 0;
        qtcSetMenuBarHidden(qtSettings.appName, gtk_widget_get_visible(menuBar));
        if (gtk_widget_get_visible(menuBar)) {
            gtk_widget_hide(menuBar);
        } else {
            size = qtcWidgetGetAllocation(menuBar).height;
            gtk_widget_show(menuBar);
        }

        qtcMenuEmitSize(menuBar, size);
        qtcWindowMenuBarDBus(widget, size);
        return TRUE;
    }
    return FALSE;
}

gboolean
qtcWindowSetStatusBarProp(GtkWidget *w)
{
    if (w && !g_object_get_data(G_OBJECT(w), QTC_STATUSBAR)) {
        GtkWindow *topLevel = GTK_WINDOW(gtk_widget_get_toplevel(w));
        xcb_window_t wid =
            GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(topLevel)));

        g_object_set_data(G_OBJECT(w), QTC_STATUSBAR, GINT_TO_POINTER(1));
        qtcX11SetStatusBar(wid);
        qtcX11Flush();
        return TRUE;
    }
    return FALSE;
}

static gboolean
qtcWindowToggleStatusBar(GtkWidget *widget)
{
    GtkWidget *statusBar = qtcWindowGetStatusBar(widget, 0);

    if (statusBar) {
        gboolean state=gtk_widget_get_visible(statusBar);
        qtcSetStatusBarHidden(qtSettings.appName, state);
        if (state) {
            gtk_widget_hide(statusBar);
        } else {
            gtk_widget_show(statusBar);
        }
        qtcWindowStatusBarDBus(widget, state);
        return TRUE;
    }
    return FALSE;
}

static void
qtcWindowSetProperties(GtkWidget *w, unsigned short opacity)
{
    GtkWindow *topLevel = GTK_WINDOW(gtk_widget_get_toplevel(w));
    unsigned long prop = (IS_FLAT_BGND(opts.bgndAppearance) ?
                          (IMG_NONE != opts.bgndImage.type ?
                           APPEARANCE_RAISED : APPEARANCE_FLAT) :
                          opts.bgndAppearance) & 0xFF;
    //GtkRcStyle *rcStyle=gtk_widget_get_modifier_style(w);
    GdkColor *bgnd = /* rcStyle ? &rcStyle->bg[GTK_STATE_NORMAL] : */
        &qtcPalette.background[ORIGINAL_SHADE];
    xcb_window_t wid =
        GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(topLevel)));

    if (opacity != 100)
        qtcX11SetOpacity(wid, opacity);

    prop |= (((toQtColor(bgnd->red) & 0xFF) << 24) |
             ((toQtColor(bgnd->green) & 0xFF) << 16) |
             ((toQtColor(bgnd->blue) & 0xFF) << 8));
    qtcX11CallVoid(change_property, XCB_PROP_MODE_REPLACE, wid,
                   qtc_x11_atoms[QTC_X11_ATOM_QTC_BGND], XCB_ATOM_CARDINAL,
                   32, 1, (unsigned char*)&prop);
    qtcX11Flush();
}

static gboolean
qtcWindowKeyRelease(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    QTC_UNUSED(user_data);
    // Ensure only ctrl/alt/shift/capsLock are pressed...
    if (GDK_CONTROL_MASK & event->state && GDK_MOD1_MASK & event->state &&
        !event->is_modifier && 0 == (event->state & 0xFF00)) {
        gboolean toggled = FALSE;
        if (opts.menubarHiding & HIDE_KEYBOARD &&
            (QTC_KEY_m == event->keyval || QTC_KEY_M == event->keyval)) {
            toggled = qtcWindowToggleMenuBar(widget);
        }
        if (opts.statusbarHiding & HIDE_KEYBOARD &&
            (QTC_KEY_s == event->keyval || QTC_KEY_S == event->keyval)) {
            toggled = qtcWindowToggleStatusBar(widget);
        }
        if (toggled) {
            gtk_widget_queue_draw(widget);
        }
    }
    return FALSE;
}

static gboolean
qtcWindowMap(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    QTC_UNUSED(event);
    QTC_UNUSED(user_data);
    int opacity = GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(widget), "QTC_WINDOW_OPACITY"));
    qtcWindowSetProperties(widget, (unsigned short)opacity);

    if(opts.menubarHiding&HIDE_KWIN)
    {
        GtkWidget *menuBar=qtcWindowGetMenuBar(widget, 0);

        if(menuBar)
        {
            int size=gtk_widget_get_visible(menuBar) ? qtcWidgetGetAllocation(menuBar).height : 0;

            qtcMenuEmitSize(menuBar, size);
            qtcWindowMenuBarDBus(widget, size);
        }
    }

    if(opts.statusbarHiding&HIDE_KWIN)
    {
        GtkWidget *statusBar=qtcWindowGetStatusBar(widget, 0);

        if(statusBar)
            qtcWindowStatusBarDBus(widget, !gtk_widget_get_visible(statusBar));
    }
    return FALSE;
}

gboolean
qtcWindowSetup(GtkWidget *widget, int opacity)
{
    GObject *obj = NULL;
    if (widget && (obj = G_OBJECT(widget)) &&
        !g_object_get_data(obj, "QTC_WINDOW_HACK_SET")) {
        g_object_set_data(obj, "QTC_WINDOW_HACK_SET", (gpointer)1);
        if (!(IS_FLAT_BGND(opts.bgndAppearance)) ||
            IMG_NONE != opts.bgndImage.type) {
            QtCWindow *window = qtcWindowLookupHash(widget, TRUE);
            if (window) {
                GtkAllocation alloc = qtcWidgetGetAllocation(widget);
                qtcConnectToData(obj, "QTC_WINDOW_CONFIGURE_ID",
                                 "configure-event", qtcWindowConfigure, window);
                window->width = alloc.width;
                window->height = alloc.height;
                window->widget = widget;
            }
        }
        qtcConnectToData(obj, "QTC_WINDOW_DESTROY_ID", "destroy-event",
                         qtcWindowDestroy, NULL);
        qtcConnectToData(obj, "QTC_WINDOW_STYLE_SET_ID", "style-set",
                         qtcWindowStyleSet, NULL);
        if ((opts.menubarHiding & HIDE_KEYBOARD) ||
            (opts.statusbarHiding & HIDE_KEYBOARD))
            qtcConnectToData(obj, "QTC_WINDOW_KEY_RELEASE_ID",
                             "key-release-event", qtcWindowKeyRelease, NULL);
        g_object_set_data(G_OBJECT(widget), "QTC_WINDOW_OPACITY",
                          GINT_TO_POINTER(opacity));
        qtcWindowSetProperties(widget, (unsigned short)opacity);

        if ((opts.menubarHiding & HIDE_KWIN) ||
            (opts.statusbarHiding & HIDE_KWIN) || 100 != opacity)
            qtcConnectToData(obj, "QTC_WINDOW_MAP_ID", "map-event",
                             qtcWindowMap, NULL);
        if (opts.shadeMenubarOnlyWhenActive || BLEND_TITLEBAR ||
            opts.menubarHiding || opts.statusbarHiding)
            qtcConnectToData(obj, "QTC_WINDOW_CLIENT_EVENT_ID", "client-event",
                             qtcWindowClientEvent, NULL);
        return TRUE;
    }

    return FALSE;
}
