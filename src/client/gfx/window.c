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

#if defined(_WIN32) || defined(_WIN64)

/* Win32 code */

/* TODO: Find a better way to pass the window to the windowproc */
MWWindow *_win;

/*PAINTSTRUCT paint_struct;*/

LRESULT CALLBACK _mw_windowproc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
    /*LONG width = paint_struct.rcPaint.right-paint_struct.rcPaint.left;
    LONG height = paint_struct.rcPaint.bottom-paint_struct.rcPaint.top;*/
    int w, h;
    int i;
    POINTS point;

    switch(uMsg) {
        case WM_DESTROY:
            /* We want to control if the window can close, so the following
             * lines are commented, but I still want to keep these lines,
             * because I want to remember that I could handle closing that
             * way */
            /*PostQuitMessage(0);*/
            _win->should_close = 1;
            return 0;
        case WM_CLOSE:
            _win->should_close = 1;
            return 0;
        case WM_SIZE:
            w = LOWORD(lParam);
            h = HIWORD(lParam);

            _win->w = w;
            _win->h = h;

            printf("w: %d, h: %d\n", w, h);
            _win->resized = 1;
            break;
        case WM_PAINT:
            if(_win->should_swap){
                SwapBuffers(GetDC(_win->window_handle));
                _win->should_swap = 0;
            }

            break;
        case WM_KEYUP:
            /* TODO: Support all ASCII characters */
            if(wParam == VK_SPACE){
                _win->key = ' ';
                _win->key_event = 2;
                break;
            }
            for(i='A';i<='Z';i++){
                if(wParam == i){
                    _win->key = i-'A'+'a';
                    _win->key_event = 2;
                    break;
                }
            }
            break;
        case WM_KEYDOWN:
            /* TODO: Support all ASCII characters */
            if(wParam == VK_SPACE){
                _win->key = ' ';
                _win->key_event = 1;
                break;
            }
            for(i='A';i<='Z';i++){
                if(wParam == i){
                    _win->key = i-'A'+'a';
                    _win->key_event = 1;
                    break;
                }
            }
            break;
        case WM_LBUTTONDOWN:
            _win->mouse_buttons[B_LEFT] = 1;
            _win->button_click = 1;
            break;
        case WM_LBUTTONUP:
            _win->mouse_buttons[B_LEFT] = 0;
            _win->button_release = 0;
            break;
        case WM_MBUTTONDOWN:
            _win->mouse_buttons[B_MIDDLE] = 1;
            _win->button_click = 1;
            break;
        case WM_MBUTTONUP:
            _win->mouse_buttons[B_MIDDLE] = 0;
            _win->button_release = 0;
            break;
        case WM_RBUTTONDOWN:
            _win->mouse_buttons[B_RIGHT] = 1;
            _win->button_click = 1;
            break;
        case WM_RBUTTONUP:
            _win->mouse_buttons[B_RIGHT] = 0;
            _win->button_release = 0;
            break;
        case WM_MOUSEWHEEL:
            /* Somehow the event was incorrect when looking if
             * GET_WHEEL_DELTA_WPARAM(wParam) < 0 */
            _win->mouse_buttons[GET_WHEEL_DELTA_WPARAM(wParam) > 0 ?
                                B_SCROLLUP : B_SCROLLDOWN] = 1;
        case WM_MOUSEMOVE:
            point = MAKEPOINTS(lParam);

            _win->mx = point.x;
            _win->my = point.y;
            
            _win->pointer_moved = 1;

            break;
        default:
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int mw_init(MWWindow *window, int width, int height, char *title) {
    size_t i;

    PIXELFORMATDESCRIPTOR format = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        8,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        0,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    RECT rect;

    _win = window;

    window->class_name = malloc(sizeof(wchar_t)*strlen(title));

    if(window->class_name == NULL){
        fputs("[MibiCraft] Allocation failed!\n", stderr);
        free(window->class_name);
        window->class_name = NULL;

        return 1;
    }

    for(i=0;i<strlen(title);i++){
        window->class_name[i] = title[i];
    }

    memset(&window->winclass, 0, sizeof(WNDCLASS));

    window->winclass.lpfnWndProc = *_mw_windowproc;
    window->winclass.hInstance = window->instance_handle;
    window->winclass.lpszClassName = (LPCSTR)window->class_name;

    RegisterClass(&window->winclass);

    window->window_handle = CreateWindowEx(0, (LPCSTR)window->class_name,
                                           (LPCSTR)title, WS_OVERLAPPEDWINDOW |
                                           WS_EX_COMPOSITED,
                                           CW_USEDEFAULT, CW_USEDEFAULT,
                                           CW_USEDEFAULT, CW_USEDEFAULT, NULL,
                                           NULL, window->instance_handle, NULL);
    if(window->window_handle == NULL){
        fputs("[MibiCraft] Window creation failed!\n", stderr);
        free(window->class_name);
        window->class_name = NULL;

        return 1;
    }

    ShowWindow(window->window_handle, SW_SHOW);

    /* TODO: Error handling */
    SetPixelFormat(GetDC(window->window_handle),
                   ChoosePixelFormat(GetDC(window->window_handle), &format),
                   &format);
    window->opengl_context = wglCreateContext(GetDC(window->window_handle));

    if(window->opengl_context == NULL){
        fputs("[MibiCraft] Context creation failed!\n", stderr);
        
        return 1;
    }

    if(wglMakeCurrent(GetDC(window->window_handle),
                      window->opengl_context) == FALSE){
        fputs("[MibiCraft] Failed to make context current!\n", stderr);
        
        return 1;
    }

    if(GetClientRect(window->window_handle, &rect)){
        window->w = rect.right-rect.left;
        window->h = rect.bottom-rect.top;
    }

    window->should_swap = 0;
    window->should_close = 0;
    window->resized = 1;

    window->button_click = 0;
    window->button_release = 0;
    window->pointer_moved = 0;
    window->key_event = 0;

    window->key = '\0';

    /* TODO: Handle cursor visibility in a more robust way with e.g.
     * GetCursorInfo to get the cursor visibility before trying to change it,
     * but maybe it is too inefficient and/or there is a better way to change
     * it. */
    window->cursor_shown = 1;

    return 0;
}

int mw_get_next_event(MWWindow *window) {
    MSG msg;

    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
        if(msg.message == WM_QUIT){
            puts("quit asked!");
            window->should_close = 1;
        }
        _win = window;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return 1;
    }
    return 0;
}
void mw_get_size(MWWindow *window, int *width, int *height) {
    *width = window->w;
    *height = window->h;
}
void mw_get_pointer_pos(MWWindow *window, int *x, int *y) {
    *x = window->mx;
    *y = window->my;
}
int mw_get_pointer_button(MWWindow *window) {
    size_t i;
    for(i=B_AMOUNT;--i;){
        if(window->mouse_buttons[i]){
            if(i >= B_SCROLLUP){
                window->mouse_buttons[i] = 0;
                return 1;
            }
            return i;
        }
    }
    return 0;
}
unsigned long mw_get_time(void) {
    struct _timeb time;
    long out;
    _ftime(&time);
    out = time.time*1000L+time.millitm;
    return out;
}

void mw_move_pointer(MWWindow *window, int x, int y) {
    POINT point;
    
    point.x = x;
    point.y = y;
    
    if(ClientToScreen(window->window_handle, &point)){
        SetCursorPos(point.x, point.y);
    }
}
void mw_hide_pointer(MWWindow *window) {
    if(window->cursor_shown){
        ShowCursor(FALSE);
        window->cursor_shown = 0;
    }
}
void mw_show_pointer(MWWindow *window) {
    if(!window->cursor_shown){
        ShowCursor(TRUE);
        window->cursor_shown = 1;
    }
}

void mw_start_drawing(void) {
    /* Do I really need to call BeginPaint and EndPaint, because it also works
     * without them? */
    /*BeginPaint(_win->window_handle, &paint_struct);*/
}
void mw_end_drawing(void) {
    /*EndPaint(_win->window_handle, &paint_struct);*/
}
void mw_swap_buffers(MWWindow *window) {
    window->should_swap = 1;
    /*SwapBuffers(GetDC(window->window_handle));*/
    InvalidateRect(window->window_handle, NULL, TRUE);
}

int mw_should_close(MWWindow *window) {
    if(window->should_close){
        window->should_close = 0;
        return 1;
    }
    return 0;
}
int mw_window_resized(MWWindow *window) {
    if(window->resized){
        window->resized = 1;
        return 1;
    }
    return 0;
}
int mw_pointer_moved(MWWindow *window) {
    if(window->pointer_moved){
        window->pointer_moved = 0;
        return 1;
    }
    return 0;
}
int mw_button_pressed(MWWindow *window) {
    if(window->button_click){
        window->button_click = 0;
        return 1;
    }
    return 0;
}
int mw_button_released(MWWindow *window) {
    if(window->button_release){
        window->button_release = 0;
        return 1;
    }
    return 0;
}
int mw_key_pressed(MWWindow *window) {
    if(window->key_event&1){
        window->key_event = 0;
        return 1;
    }
    return 0;
}
int mw_key_released(MWWindow *window) {
    if(window->key_event&2){
        window->key_event = 0;
        return 1;
    }
    return 0;
}

char mw_get_key_char(MWWindow *window) {
    return window->key;
}
unsigned int mw_get_key_code(MWWindow *window) {
    return window->key;
}

void mw_close(MWWindow *window) {
    DestroyWindow(window->window_handle);
    PostQuitMessage(0);

    /* Free all the window related data */

    free(window->class_name);
    window->class_name = NULL;

    /* TODO: Error handling */
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(window->opengl_context);
    free(window->opengl_context);
    window->opengl_context = NULL;
}

#else

/* Xorg code */

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

#endif
