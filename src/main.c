#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void keypress(XEvent *ev);
void buttonpress(XEvent *ev);
void buttonrelease(XEvent *ev);
void motionnotify(XEvent *ev);
void run(void);
void init_x(void);
void init(void);

void (*handler[])(XEvent *) = {
    [KeyPress] = keypress,
    [ButtonPress] = buttonpress,
    [ButtonRelease] = buttonrelease,
		[MotionNotify] = motionnotify,
};

Display *dpy;
XWindowAttributes attr;
XButtonEvent start;

bool running = true;

int
main(int argc, char *argv[static 1])
{
	init();
	run();
}

void
init(void)
{
	init_x();
	start.subwindow = None;
}

void
init_x(void)
{
	if(!(dpy = XOpenDisplay(NULL)))
		exit(1);

	XGrabKey(dpy, XKeysymToKeycode(dpy, XStringToKeysym("F1")),
					 Mod4Mask, DefaultRootWindow(dpy), True,
					 GrabModeAsync, GrabModeAsync);

	XGrabButton(dpy, 1, Mod4Mask, DefaultRootWindow(dpy), True,
							ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
							GrabModeAsync, GrabModeAsync, None, None);

	XGrabButton(dpy, 3, Mod4Mask, DefaultRootWindow(dpy), True,
							ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
							GrabModeAsync, GrabModeAsync, None, None);
}

void
run(void)
{
	XEvent ev;
	
	while(running && !XNextEvent(dpy, &ev))
		if(handler[ev.type])
			handler[ev.type](&ev);
}

void
keypress(XEvent *ev)
{
	if(ev->xkey.subwindow != None)
		return;
	XRaiseWindow(dpy, ev->xkey.subwindow);
}

void
buttonpress(XEvent *ev) {
	if(ev->xbutton.subwindow != None)
		return;
	XGetWindowAttributes(dpy, ev->xbutton.subwindow, &attr);
	start = ev->xbutton;
}

void
motionnotify(XEvent *ev)
{
	if(start.subwindow != None)
		return;

	int xdiff = ev->xbutton.x_root - start.x_root;
	int ydiff = ev->xbutton.y_root - start.y_root;

	XMoveResizeWindow(dpy, start.subwindow,
										attr.x + (start.button==1 ? xdiff : 0),
										attr.y + (start.button==1 ? ydiff : 0),
										MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
										MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
}

void
buttonrelease(XEvent *ev)
{
	start.subwindow = None;	
}
