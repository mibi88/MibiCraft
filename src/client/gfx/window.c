/*
 * MibiCraft - A small game in a world of cubes
 * Copyright (C) 2023  Mibi88
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/.
 */

#include "window.h"

#include <time.h>

#include <X11/keysymdef.h>
#include <X11/XKBlib.h>

#if HAS_XFIXES
#include <X11/extensions/Xfixes.h>
#endif

#define MW_EVENT_MASKS ExposureMask | ButtonPressMask | ButtonReleaseMask | \
                       PointerMotionMask | KeyPressMask | KeyReleaseMask | \
                       KeymapStateMask

#define MW_ERROR_CASE(e) case(e): puts("Error code: " #e); break

int _mw_error_handler(Display *display, XErrorEvent *event) {
    (void)display;

    fputs("[MibiCraft] X11 error: ", stdout);
    switch(event->error_code) {
        MW_ERROR_CASE(BadAccess);
        MW_ERROR_CASE(BadAlloc);
        MW_ERROR_CASE(BadAtom);
        MW_ERROR_CASE(BadColor);
        MW_ERROR_CASE(BadCursor);
        MW_ERROR_CASE(BadDrawable);
        MW_ERROR_CASE(BadFont);
        MW_ERROR_CASE(BadGC);
        MW_ERROR_CASE(BadIDChoice);
        MW_ERROR_CASE(BadImplementation);
        MW_ERROR_CASE(BadLength);
        MW_ERROR_CASE(BadMatch);
        MW_ERROR_CASE(BadName);
        MW_ERROR_CASE(BadPixmap);
        MW_ERROR_CASE(BadRequest);
        MW_ERROR_CASE(BadValue);
        MW_ERROR_CASE(BadWindow);
    }

    return 0;
}

int mw_init(MWWindow *window, int width, int height, char *title) {
    GLint visual_attributes[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE,
        8,
        GLX_DOUBLEBUFFER,
        None /* Always ends with None */
    };

#if HAS_XFIXES
    int minor, major;
#endif

    window->display = XOpenDisplay(NULL);
    if(window->display == NULL){
        puts("[MibiCraft] Failed to connect to the X server!");
        return 1;
    }

    window->root = DefaultRootWindow(window->display);

    window->visual_info = glXChooseVisual(window->display, 0,
                                          visual_attributes);

    if(!window->visual_info){
        puts("[MibiCraft] No visual found!");
        return 2;
    }

    window->color_map = XCreateColormap(window->display, window->root,
                                        window->visual_info->visual, AllocNone);

    window->set_window_attributes.colormap = window->color_map;
    window->set_window_attributes.event_mask = MW_EVENT_MASKS;

    window->window = XCreateWindow(window->display, window->root, 0, 0, width,
                                   height, 0, window->visual_info->depth,
                                   InputOutput, window->visual_info->visual,
                                   CWColormap | CWEventMask,
                                   &window->set_window_attributes);
    XSelectInput(window->display, window->window, MW_EVENT_MASKS);

    /* Make the window appear */
    XMapWindow(window->display, window->window);

    /* Set the text in the title bar */
    XStoreName(window->display, window->window, title);

    /* Create the OpenGL context */
    window->glx_context = glXCreateContext(window->display, window->visual_info,
                                           NULL, GL_TRUE);
    glXMakeCurrent(window->display, window->window, window->glx_context);

    window->wm_delete = XInternAtom(window->display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(window->display, window->window, &window->wm_delete, 1);

    XSetErrorHandler(_mw_error_handler);

#if HAS_XFIXES
    XFixesQueryExtension(window->display, &minor, &major);
    printf("[MibiCraft] Using XFixes %d.%d\n", major, minor);
    XFixesShowCursor(window->display, window->window);
    window->pointer_shown = 1;
#endif

    return 0;
}

int mw_get_next_event(MWWindow *window) {
    if(XPending(window->display)){
        XNextEvent(window->display, &window->event);
        if(window->event.type == MappingNotify){
            XRefreshKeyboardMapping(&(window->event.xmapping));
        }
        return 1;
    }
    return 0;
}

void mw_get_size(MWWindow *window, int *width, int *height) {
    XGetWindowAttributes(window->display, window->window,
                         &window->window_attributes);
    *width = window->window_attributes.width;
    *height = window->window_attributes.height;
}

void mw_get_pointer_pos(MWWindow *window, int *x, int *y) {
    Window root_return, child_return;
    int root_x_return, root_y_return;
    unsigned int mask_return;
    XQueryPointer(window->display, window->window, &root_return, &child_return,
                  &root_x_return, &root_y_return, x, y, &mask_return);
}

int mw_get_pointer_button(MWWindow *window) {
    Window root_return, child_return;
    int root_x_return, root_y_return;
    int win_x_return, win_y_return;
    unsigned int mask_return;
    XQueryPointer(window->display, window->window, &root_return, &child_return,
                  &root_x_return, &root_y_return, &win_x_return, &win_y_return,
                  &mask_return);
    if(mask_return & Button1Mask) return 1;
    if(mask_return & Button2Mask) return 2;
    if(mask_return & Button3Mask) return 3;
    /* TODO: Improve how I'm getting the button 4 and 5 press (mouse wheel) */
    if(window->event.xbutton.button == Button4) return 4;
    if(window->event.xbutton.button == Button5) return 5;
    return 0;
}

void mw_move_pointer(MWWindow *window, int x, int y) {
    /* XWayland needs the cursor to be hidden for some reason, but I don't know
     * if this fix works... */

    if(window->pointer_shown) mw_hide_pointer(window);

    XWarpPointer(window->display, window->window, window->window, 0, 0, 0, 0,
                 x, y);
    XFlush(window->display);

    if(window->pointer_shown) mw_show_pointer(window);
}

void mw_hide_pointer(MWWindow *window) {
#if HAS_XFIXES
    if(window->pointer_shown){
        XFixesHideCursor(window->display, window->window);
        window->pointer_shown = 0;
    }
#endif
}

void mw_show_pointer(MWWindow *window) {
#if HAS_XFIXES
    if(!window->pointer_shown){
        XFixesShowCursor(window->display, window->window);
        window->pointer_shown = 1;
    }
#endif
}

unsigned long mw_get_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return time.tv_nsec/(1e6)+time.tv_sec*1000;
}

void mw_start_drawing(void) {
    glXWaitX();
}

void mw_end_drawing(void) {
    glXWaitGL();
}

void mw_swap_buffers(MWWindow *window) {
    glXSwapBuffers(window->display, window->window);
}

int mw_should_close(MWWindow *window) {
    return (Atom)window->event.xclient.data.l[0] == window->wm_delete;
}

int mw_window_resized(MWWindow *window) {
    return window->event.type == Expose;
}

int mw_pointer_moved(MWWindow *window) {
    return window->event.type == MotionNotify;
}

int mw_button_pressed(MWWindow *window) {
    return window->event.type == ButtonPress;
}

int mw_button_released(MWWindow *window) {
    return window->event.type == ButtonRelease;
}

int mw_key_pressed(MWWindow *window) {
    return window->event.type == KeyPress;
}

int mw_key_released(MWWindow *window) {
    return window->event.type == KeyRelease;
}

unsigned int mw_get_key_code(MWWindow *window) {
    return window->event.xkey.keycode;
}

char mw_get_key_char(MWWindow *window) {
    char str[25];
    KeySym keysym;
    XLookupString(&window->event.xkey, str, 25, &keysym, NULL);
    return str[0];
}

void mw_close(MWWindow *window) {
    glXMakeCurrent(window->display, None, NULL);
    glXDestroyContext(window->display, window->glx_context);
    /*XCloseIM(window->xim);
    XDestroyIC(window->xic);*/
    XDestroyWindow(window->display, window->window);
    XCloseDisplay(window->display);
}
