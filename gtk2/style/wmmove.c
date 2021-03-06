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
#include <qtcurve-utils/x11wmmove.h>

#include <gdk/gdkx.h>
#include "compatability.h"
#include "helpers.h"
#include "qt_settings.h"
#include "menu.h"
#include "tab.h"

extern Options opts;

static int qtcWMMoveLastX = -1;
static int qtcWMMoveLastY = -1;
static int qtcWMMoveTimer = 0;
static GtkWidget *qtcWMMoveDragWidget = NULL;
//! keep track of the last rejected button event to reject it again if passed to some parent widget
/*! this spares some time (by not processing the same event twice), and prevents some bugs */
GdkEventButton *qtcWMMoveLastRejectedEvent=NULL;

static int qtcWMMoveBtnReleaseSignalId = 0;
static int qtcWMMoveBtnReleaseHookId = 0;

static gboolean qtcWMMoveDragEnd();

static gboolean
qtcWMMoveBtnReleaseHook(GSignalInvocationHint *a, guint b,
                        const GValue *c, gpointer d)
{
    QTC_UNUSED(a);
    QTC_UNUSED(b);
    QTC_UNUSED(c);
    QTC_UNUSED(d);
    if (qtcWMMoveDragWidget)
        qtcWMMoveDragEnd();
    return TRUE;
}

static void
qtcWMMoveRegisterBtnReleaseHook()
{
    if (qtcWMMoveBtnReleaseSignalId == 0 && qtcWMMoveBtnReleaseHookId == 0) {
        qtcWMMoveBtnReleaseSignalId =
            g_signal_lookup("button-release-event", GTK_TYPE_WIDGET);
        if (qtcWMMoveBtnReleaseSignalId) {
            qtcWMMoveBtnReleaseHookId =
                g_signal_add_emission_hook(qtcWMMoveBtnReleaseSignalId,
                                           (GQuark)0L, qtcWMMoveBtnReleaseHook,
                                           0L, 0L);
        }
    }
}

static void
qtcWMMoveStopTimer()
{
    if (qtcWMMoveTimer)
        g_source_remove(qtcWMMoveTimer);
    qtcWMMoveTimer = 0;
}

static void
qtcWMMoveReset()
{
    qtcWMMoveLastX = -1;
    qtcWMMoveLastY = -1;
    qtcWMMoveDragWidget = NULL;
    qtcWMMoveLastRejectedEvent = NULL;
    qtcWMMoveStopTimer();
}

static void
qtcWMMoveStore(GtkWidget *widget, GdkEventButton *event)
{
    qtcWMMoveLastX = event ? event->x_root : -1;
    qtcWMMoveLastY = event ? event->y_root : -1;
    qtcWMMoveDragWidget = widget;
}

#if !GTK_CHECK_VERSION(2, 12, 0)
GdkWindow*
gtk_widget_get_window(GtkWidget *widget)
{
    return widget->window;
}
#endif

static void
qtcWMMoveTrigger(GtkWidget *w, int x, int y)
{
    GtkWindow *topLevel = GTK_WINDOW(gtk_widget_get_toplevel(w));
    xcb_window_t wid =
        GDK_WINDOW_XID(gtk_widget_get_window(GTK_WIDGET(topLevel)));
    qtcX11MoveTrigger(wid, x, y);
    qtcWMMoveDragEnd(w);
}

static gboolean qtcWMMoveWithinWidget(GtkWidget *widget, GdkEventButton *event)
{
    // get top level widget
    GtkWidget *topLevel=gtk_widget_get_toplevel(widget);;
    GdkWindow *window = topLevel ? gtk_widget_get_window(topLevel) : NULL;

    if(window)
    {
        GtkAllocation allocation;
        int           wx=0, wy=0, nx=0, ny=0;

        // translate widget position to topLevel
        gtk_widget_translate_coordinates(widget, topLevel, wx, wy, &wx, &wy);

        // translate to absolute coordinates
        gdk_window_get_origin(window, &nx, &ny);
        wx += nx;
        wy += ny;

        // get widget size.
        // for notebooks, only consider the tabbar rect
        if(GTK_IS_NOTEBOOK(widget))
        {
            GtkAllocation widgetAlloc=qtcWidgetGetAllocation(widget);
            allocation=qtcTabGetTabbarRect(GTK_NOTEBOOK(widget));
            allocation.x += wx - widgetAlloc.x;
            allocation.y += wy - widgetAlloc.y;
        }
        else
        {
            allocation = qtcWidgetGetAllocation(widget);
            allocation.x = wx;
            allocation.y = wy;
        }

        return allocation.x<=event->x_root && allocation.y<=event->y_root &&
            (allocation.x+allocation.width)>event->x_root &&(allocation.y+allocation.height)>event->y_root;
    }
    return TRUE;
}

static gboolean qtcWMMoveIsBlackListed(GObject *object)
{
    static const char *widgets[] = {
        "GtkPizza", "GladeDesignLayout", "MetaFrames", "SPHRuler", "SPVRuler", 0
    };

    for (int i = 0;widgets[i];i++) {
        if (objectIsA(object, widgets[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

static gboolean qtcWMMoveChildrenUseEvent(GtkWidget *widget, GdkEventButton *event, gboolean inNoteBook)
{
    // accept, by default
    gboolean usable = TRUE;

    // get children and check
    GList *children = gtk_container_get_children(GTK_CONTAINER(widget)),
        *child;

    for(child = g_list_first(children); child && usable; child = g_list_next(child))
    {
        // cast child to GtkWidget
        if(GTK_IS_WIDGET(child->data))
        {
            GtkWidget *childWidget=GTK_WIDGET(child->data);
            GdkWindow *window=NULL;

            // check widget state and type
            if(GTK_STATE_PRELIGHT==gtk_widget_get_state(childWidget))
            {
                // if widget is prelight, we don't need to check where event happen,
                // any prelight widget indicate we can't do a move
                usable = FALSE;
                continue;
            }

            window = gtk_widget_get_window(childWidget);
            if(!(window && gdk_window_is_visible(window)))
                continue;

            if(GTK_IS_NOTEBOOK(childWidget))
                inNoteBook = true;

            if(!(event && qtcWMMoveWithinWidget(childWidget, event)))
                continue;

            // check special cases for which grab should not be enabled
            if(
                (qtcWMMoveIsBlackListed(G_OBJECT(childWidget))) ||
                (GTK_IS_NOTEBOOK(widget) && qtcTabIsLabel(GTK_NOTEBOOK(widget), childWidget)) ||
                (GTK_IS_BUTTON(childWidget) && gtk_widget_get_state(childWidget) != GTK_STATE_INSENSITIVE) ||
                (gtk_widget_get_events(childWidget) & (GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK)) ||
                (GTK_IS_MENU_ITEM(childWidget)) ||
                (GTK_IS_SCROLLED_WINDOW(childWidget) && (!inNoteBook || gtk_widget_is_focus(childWidget))))
            {
                usable = false;
            }

            // if child is a container and event has been accepted so far, also check it, recursively
            if(usable && GTK_IS_CONTAINER(childWidget))
                usable = qtcWMMoveChildrenUseEvent(childWidget, event, inNoteBook);
        }
    }

    if(children)
        g_list_free(children);

    return usable;
}

static gboolean qtcWMMoveUseEvent(GtkWidget *widget, GdkEventButton *event)
{
    if(qtcWMMoveLastRejectedEvent && qtcWMMoveLastRejectedEvent==event)
        return FALSE;

    if(!GTK_IS_CONTAINER(widget))
        return TRUE;

    // if widget is a notebook, accept if there is no hovered tab
    if(GTK_IS_NOTEBOOK(widget))
        return !qtcTabHasVisibleArrows(GTK_NOTEBOOK(widget)) && -1==qtcTabCurrentHoveredIndex(widget) && qtcWMMoveChildrenUseEvent(widget, event, FALSE);
    else
        return qtcWMMoveChildrenUseEvent(widget, event, FALSE);
}

static gboolean
qtcWWMoveStartDelayedDrag(gpointer data)
{
    QTC_UNUSED(data);
    if (qtcWMMoveDragWidget) {
        gdk_threads_enter();
        qtcWMMoveTrigger(qtcWMMoveDragWidget, qtcWMMoveLastX, qtcWMMoveLastY);
        gdk_threads_leave();
    }
    return FALSE;
}

static gboolean
qtcWMMoveIsWindowDragWidget(GtkWidget *widget, GdkEventButton *event)
{
    if(opts.windowDrag && (!event || (qtcWMMoveWithinWidget(widget, event) && qtcWMMoveUseEvent(widget, event))))
    {
        qtcWMMoveStore(widget, event);
        // Start timer
        qtcWMMoveStopTimer();
        qtcWMMoveTimer=g_timeout_add(qtSettings.startDragTime, (GSourceFunc)qtcWWMoveStartDelayedDrag, NULL);
        return TRUE;
    }
    qtcWMMoveLastRejectedEvent=event;
    return FALSE;
}

static gboolean
qtcWMMoveButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    QTC_UNUSED(data);
    if (GDK_BUTTON_PRESS == event->type && 1 == event->button &&
        qtcWMMoveIsWindowDragWidget(widget, event)) {
        qtcWMMoveDragWidget = widget;
        return TRUE;
    }
    return FALSE;
}

static gboolean qtcWMMoveDragEnd()
{
    if (qtcWMMoveDragWidget)
    {
        //gtk_grab_remove(widget);
        gdk_pointer_ungrab(CurrentTime);
        qtcWMMoveReset();
        return TRUE;
    }

    return FALSE;
}

static void qtcWMMoveCleanup(GtkWidget *widget)
{
    GObject *obj = G_OBJECT(widget);
    if (g_object_get_data(obj, "QTC_WM_MOVE_HACK_SET")) {
        if (widget == qtcWMMoveDragWidget)
            qtcWMMoveReset();
        qtcDisconnectFromData(obj, "QTC_WM_MOVE_MOTION_ID");
        qtcDisconnectFromData(obj, "QTC_WM_MOVE_LEAVE_ID");
        qtcDisconnectFromData(obj, "QTC_WM_MOVE_DESTROY_ID");
        qtcDisconnectFromData(obj, "QTC_WM_MOVE_STYLE_SET_ID");
        qtcDisconnectFromData(obj, "QTC_WM_MOVE_BUTTON_PRESS_ID");
        g_object_steal_data(obj, "QTC_WM_MOVE_HACK_SET");
    }
}

static gboolean
qtcWMMoveStyleSet(GtkWidget *widget, GtkStyle *previous_style, gpointer data)
{
    QTC_UNUSED(previous_style);
    QTC_UNUSED(data);
    qtcWMMoveCleanup(widget);
    return FALSE;
}

static gboolean
qtcWMMoveDestroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    QTC_UNUSED(event);
    QTC_UNUSED(data);
    qtcWMMoveCleanup(widget);
    return FALSE;
}

static gboolean
qtcWMMoveMotion(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    QTC_UNUSED(data);
    if (qtcWMMoveDragWidget == widget) {
        // check displacement with respect to drag start
        const int distance = (abs(qtcWMMoveLastX - event->x_root) +
                              abs(qtcWMMoveLastY - event->y_root));

        if (distance > 0)
            qtcWMMoveStopTimer();

        /* if (distance < qtSettings.startDragDist) */
        /*     return FALSE; */
        qtcWMMoveTrigger(widget, event->x_root, event->y_root);
        return TRUE;
    }
    return FALSE;
}

static gboolean
qtcWMMoveLeave(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    QTC_UNUSED(widget);
    QTC_UNUSED(event);
    QTC_UNUSED(data);
    return qtcWMMoveDragEnd();
}

void qtcWMMoveSetup(GtkWidget *widget)
{
    GtkWidget *parent = NULL;

    if (widget && GTK_IS_WINDOW(widget) &&
        !gtk_window_get_decorated(GTK_WINDOW(widget)))
        return;

    if (GTK_IS_EVENT_BOX(widget) &&
        gtk_event_box_get_above_child(GTK_EVENT_BOX(widget)))
        return;

    parent = gtk_widget_get_parent(widget);

    // widgets used in tabs also must be ignored (happens, unfortunately)
    if (GTK_IS_NOTEBOOK(parent) && qtcTabIsLabel(GTK_NOTEBOOK(parent), widget))
        return;

    /*
      check event mask (for now we only need to do that for GtkWindow)
      The idea is that if the window has been set to recieve button_press
      and button_release events (which is not done by default), it likely
      means that it does something with such events, in which case we should
      not use them for grabbing
    */
    if (0 == strcmp(g_type_name(G_OBJECT_TYPE(widget)), "GtkWindow") &&
       (gtk_widget_get_events(widget) &
        (GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK)))
        return;

    GObject *obj = NULL;
    if (widget && !isFakeGtk() && (obj = G_OBJECT(widget)) &&
        !g_object_get_data(obj, "QTC_WM_MOVE_HACK_SET")) {
        gtk_widget_add_events(widget, GDK_BUTTON_RELEASE_MASK |
                              GDK_BUTTON_PRESS_MASK | GDK_LEAVE_NOTIFY_MASK |
                              GDK_BUTTON1_MOTION_MASK);
        qtcWMMoveRegisterBtnReleaseHook();
        g_object_set_data(G_OBJECT(widget), "QTC_WM_MOVE_HACK_SET", (gpointer)1);
        qtcConnectToData(obj, "QTC_WM_MOVE_MOTION_ID", "motion-notify-event",
                         qtcWMMoveMotion, NULL);
        qtcConnectToData(obj, "QTC_WM_MOVE_LEAVE_ID", "leave-notify-event",
                         qtcWMMoveLeave, NULL);
        qtcConnectToData(obj, "QTC_WM_MOVE_DESTROY_ID", "destroy-event",
                         qtcWMMoveDestroy, NULL);
        qtcConnectToData(obj, "QTC_WM_MOVE_STYLE_SET_ID", "style-set",
                         qtcWMMoveStyleSet, NULL);
        qtcConnectToData(obj, "QTC_WM_MOVE_BUTTON_PRESS_ID",
                         "button-press-event", qtcWMMoveButtonPress, NULL);
    }
}
