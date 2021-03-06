/***************************************************************************
 *   Copyright (C) 2013~2013 by Yichao Yu                                  *
 *   yyc1992@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "x11shadow_p.h"
#include "x11wmmove.h"
#include "x11blur.h"
#include "x11icccm.h"
#include "log.h"
#include "number.h"
#include <xcb/xcb_image.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <shadow0-png.h>
#include <shadow1-png.h>
#include <shadow2-png.h>
#include <shadow3-png.h>
#include <shadow4-png.h>
#include <shadow5-png.h>
#include <shadow6-png.h>
#include <shadow7-png.h>

/*!
  shadow atom and property specification available at
  http://community.kde.org/KWin/Shadow
*/

static uint32_t shadow_pixmaps[8];
static int shadow_size = 0;
static uint32_t shadow_data_xcb[8 + 4];
static unsigned long shadow_data_xlib[8 + 4];

static xcb_pixmap_t
qtcX11ShadowCreatePixmap(const QtcPixmap *data)
{
    xcb_pixmap_t pixmap = qtcX11GenerateId();
    shadow_size = data->width;

    // create X11 pixmap
    qtcX11CallVoid(create_pixmap, 32, pixmap, qtcX11RootWindow(),
                   data->width, data->height);
    xcb_gcontext_t cid = qtcX11GenerateId();
    qtcX11CallVoid(create_gc, cid, pixmap, 0, (const uint32_t*)0);
    qtcX11CallVoid(put_image, XCB_IMAGE_FORMAT_Z_PIXMAP, pixmap, cid,
                   data->width, data->height, 0, 0, 0, 32, data->len,
                   (unsigned char*)data->data);
    qtcX11CallVoid(free_gc, cid);
    qtcX11Flush();
    return pixmap;
}

void
qtcX11ShadowInit()
{
    shadow_pixmaps[0] = qtcX11ShadowCreatePixmap(&qtc_shadow0);
    shadow_pixmaps[1] = qtcX11ShadowCreatePixmap(&qtc_shadow1);
    shadow_pixmaps[2] = qtcX11ShadowCreatePixmap(&qtc_shadow2);
    shadow_pixmaps[3] = qtcX11ShadowCreatePixmap(&qtc_shadow3);
    shadow_pixmaps[4] = qtcX11ShadowCreatePixmap(&qtc_shadow4);
    shadow_pixmaps[5] = qtcX11ShadowCreatePixmap(&qtc_shadow5);
    shadow_pixmaps[6] = qtcX11ShadowCreatePixmap(&qtc_shadow6);
    shadow_pixmaps[7] = qtcX11ShadowCreatePixmap(&qtc_shadow7);

    memcpy(shadow_data_xcb, shadow_pixmaps, sizeof(shadow_pixmaps));
    for (int i = 0;i < 8;i++) {
        shadow_data_xlib[i] = shadow_pixmaps[i];
    }
    for (int i = 8;i < 12;i++) {
        shadow_data_xlib[i] = shadow_data_xcb[i] = shadow_size - 4;
    }
}

// Necessary?
void
qtcX11ShadowDestroy()
{
    if (!qtcX11GetConn()) {
        return;
    }
    for (unsigned int i = 0;
         i < sizeof(shadow_pixmaps) / sizeof(shadow_pixmaps[0]);i++) {
        qtcX11CallVoid(free_pixmap, shadow_pixmaps[i]);
    }
    qtcX11Flush();
}

QTC_EXPORT void
qtcX11ShadowInstall(xcb_window_t win)
{
    if (qtcUnlikely(!win))
        return;
    // In principle, I should check for _KDE_NET_WM_SHADOW in _NET_SUPPORTED.
    // However, it's complicated and we will gain nothing.
    Display *disp = qtcX11GetDisp();
    xcb_atom_t atom = qtc_x11_atoms[QTC_X11_ATOM_KDE_NET_WM_SHADOW];
    // Use XCB to set window property recieves BadWindow errors for menus in
    // Qt4 kpartsplugin here, probably because of the order of some pending
    // event/requests in Xlib. Calling XFlush() before xcb_change_property()
    // doesn't solve the problem for unknown reason but using XChangeProperty
    // works.
    // NOTE: XChangeProperty want `unsigned long` for format 32. So we need
    // two seperate data buffers.
    if (disp) {
        XChangeProperty(disp, win, atom, XA_CARDINAL, 32, PropModeReplace,
                        (unsigned char*)shadow_data_xlib, 12);
    } else {
        qtcX11CallVoid(change_property, XCB_PROP_MODE_REPLACE, win,
                       atom, XCB_ATOM_CARDINAL, 32, 12, shadow_data_xcb);
        qtcX11Flush();
    }
}

QTC_EXPORT void
qtcX11ShadowUninstall(xcb_window_t win)
{
    if (qtcUnlikely(!win))
        return;
    qtcX11CallVoid(delete_property, win,
                   qtc_x11_atoms[QTC_X11_ATOM_KDE_NET_WM_SHADOW]);
    qtcX11Flush();
}

// WM Move
QTC_EXPORT void
qtcX11MoveTrigger(xcb_window_t wid, uint32_t x, uint32_t y)
{
    if (qtcUnlikely(!wid))
        return;
    qtcX11FlushXlib();
    qtcX11CallVoid(ungrab_pointer, XCB_TIME_CURRENT_TIME);
    union {
        char _buff[32];
        xcb_client_message_event_t ev;
    } buff;
    memset(&buff, 0, sizeof(buff));
    // ...Taken from bespin...
    // stolen... errr "adapted!" from QSizeGrip
    // Well now it is "ported"
    xcb_client_message_event_t *xev = &buff.ev;
    xev->response_type = XCB_CLIENT_MESSAGE;
    xev->format = 32;
    xev->window = wid;
    xev->type = qtc_x11_atoms[QTC_X11_ATOM_NET_WM_MOVERESIZE];
    xev->data.data32[0] = x;
    xev->data.data32[1] = y;
    xev->data.data32[2] = 8; // NET::Move
    xev->data.data32[3] = XCB_KEY_BUT_MASK_BUTTON_1;
    qtcX11CallVoid(send_event, false, qtcX11RootWindow(),
                   XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                   XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char*)xev);
    qtcX11Flush();
}

// Blur
QTC_EXPORT void
qtcX11BlurTrigger(xcb_window_t wid, bool enable, unsigned prop_num,
                  const uint32_t *props)
{
    if (qtcUnlikely(!wid)) {
        return;
    }
    Display *disp = qtcX11GetDisp();
    xcb_atom_t atom = qtc_x11_atoms[QTC_X11_ATOM_KDE_NET_WM_BLUR_BEHIND_REGION];
    if (enable) {
        if (disp) {
            QTC_DEF_LOCAL_BUFF(unsigned long, xlib_props, 256, prop_num);
            for (unsigned i = 0;i < prop_num;i++) {
                xlib_props.p[i] = props[i];
            }
            XChangeProperty(disp, wid, atom, XA_CARDINAL, 32, PropModeReplace,
                            (unsigned char*)xlib_props.p, prop_num);
            QTC_FREE_LOCAL_BUFF(xlib_props);
        } else {
            qtcX11CallVoid(change_property, XCB_PROP_MODE_REPLACE, wid, atom,
                           XCB_ATOM_CARDINAL, 32, prop_num, props);
        }
    } else {
        qtcX11CallVoid(delete_property, wid, atom);
    }
    qtcX11Flush();
}

QTC_EXPORT void
qtcX11GetSizeHint(xcb_window_t wid, QtcX11SizeHint *hint)
{
    if (qtcUnlikely(!wid))
        return;
    memset(hint, 0, sizeof(QtcX11SizeHint));
    xcb_get_property_reply_t *reply =
        qtcX11Call(get_property, 0, wid, XCB_ATOM_WM_NORMAL_HINTS,
                   XCB_ATOM_WM_SIZE_HINTS, 0, QTC_X11_SIZE_HINTS_ELEMENTS);
    if (!reply)
        return;
    if (!(reply->type == XCB_ATOM_WM_SIZE_HINTS && reply->format == 32)) {
        free(reply);
        return;
    }
    int length = qtcMin(QTC_X11_SIZE_HINTS_ELEMENTS,
                        xcb_get_property_value_length(reply) / 4);
    memcpy(hint, xcb_get_property_value(reply), length * 4);
    uint32_t flags =
        (QTC_X11_SIZE_HINT_US_POSITION | QTC_X11_SIZE_HINT_US_SIZE |
         QTC_X11_SIZE_HINT_P_POSITION | QTC_X11_SIZE_HINT_P_SIZE |
         QTC_X11_SIZE_HINT_P_MIN_SIZE | QTC_X11_SIZE_HINT_P_MAX_SIZE |
         QTC_X11_SIZE_HINT_P_RESIZE_INC | QTC_X11_SIZE_HINT_P_ASPECT);

    /* NumPropSizeElements = 18 (ICCCM version 1) */
    if (length >= 18) {
        flags |= QTC_X11_SIZE_HINT_BASE_SIZE | QTC_X11_SIZE_HINT_P_WIN_GRAVITY;
    } else {
        hint->base_width = 0;
        hint->base_height = 0;
        hint->win_gravity = 0;
    }
    /* get rid of unwanted bits */
    hint->flags &= flags;
    free(reply);
}

QTC_EXPORT void
qtcX11SetSizeHint(xcb_window_t wid, const QtcX11SizeHint *hint)
{
    if (qtcUnlikely(!wid))
        return;
    Display *disp = qtcX11GetDisp();
    if (disp) {
        unsigned long xlib_hint[QTC_X11_SIZE_HINTS_ELEMENTS];
        for (unsigned i = 0;i < QTC_X11_SIZE_HINTS_ELEMENTS;i++) {
            xlib_hint[i] = ((uint32_t*)hint)[i];
        }
        XChangeProperty(disp, wid, XA_WM_NORMAL_HINTS, XA_WM_SIZE_HINTS, 32,
                        PropModeReplace, (unsigned char*)xlib_hint,
                        QTC_X11_SIZE_HINTS_ELEMENTS);
        XFlush(disp);
    } else {
        qtcX11CallVoid(change_property, XCB_PROP_MODE_REPLACE, wid,
                       XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 32,
                       QTC_X11_SIZE_HINTS_ELEMENTS, hint);
        qtcX11Flush();
    }
}
