/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file androidGraphicsWindow.h
 * @author rdb
 * @date 2013-01-11
 */

#ifndef ANDROIDGRAPHICSWINDOW_H
#define ANDROIDGRAPHICSWINDOW_H

#include "pandabase.h"

#include "androidGraphicsPipe.h"
#include "graphicsWindow.h"
#include "buttonHandle.h"

#include <android/native_window.h>
#include <android/input.h>
#include <android/native_activity.h>
#include <android/rect.h>

struct android_app;

/**
 * An interface to manage Android windows and their appropriate EGL surfaces.
 */
class AndroidGraphicsWindow : public GraphicsWindow {
public:
  AndroidGraphicsWindow(GraphicsEngine *engine, GraphicsPipe *pipe,
                        std::string name,
                        const FrameBufferProperties &fb_prop,
                        const WindowProperties &win_prop,
                        int flags,
                        GraphicsStateGuardian *gsg,
                        GraphicsOutput *host);
  virtual ~AndroidGraphicsWindow();

  virtual bool begin_frame(FrameMode mode, Thread *current_thread);
  virtual void end_frame(FrameMode mode, Thread *current_thread);
  virtual void end_flip();

  virtual void process_events();
  virtual void set_properties_now(WindowProperties &properties);

  virtual bool has_second_pointer() const;
  virtual LPoint2 get_second_pointer() const;

protected:
  virtual void close_window();
  virtual bool open_window();

  virtual void destroy_surface();
  virtual bool create_surface();

private:
  static void handle_command(struct android_app *app, int32_t command);
  static int32_t handle_input_event(struct android_app *app, AInputEvent *event);

  void ns_handle_command(int32_t command);
  int32_t handle_key_event(const AInputEvent *event);
  int32_t handle_motion_event(const AInputEvent *event);

  ButtonHandle map_button(int32_t keycode);

  // Emits the button-up for whichever touch slot (mouse1 or touch2) owns the
  // given Android pointer ID, and clears that slot.
  void release_touch_pointer(int32_t pointer_id);

  // Records the touch2 pointer's position from the event, if it carries it.
  void update_touch2_pos(const AInputEvent *event);

private:
  struct android_app* _app;

  EGLDisplay _egl_display;
  EGLSurface _egl_surface;

  // Android pointer IDs owning the two forwarded touch slots, or -1 when that
  // slot is free. A new finger takes mouse1 if it is free, otherwise touch2.
  // Tracked by ID, not index, so a finger lifting does not reassign the
  // remaining finger mid-gesture.
  int32_t _mouse1_pointer_id;
  int32_t _touch2_pointer_id;
  int32_t _mouse_button_state;

  // Normalised position (NDC, -1..1, y-up) of the touch2 pointer, valid while
  // _touch2_pointer_id >= 0.
  LPoint2 _touch2_pos;
  bool _touch2_pos_valid;

  GraphicsWindowInputDevice *_input;

public:
  // The button the touch2 slot emits. Deliberately not a mouse button:
  // MouseWatcher ties mouse buttons to its single pointer, routing them to
  // the first finger's region and suppressing them under a held widget.
  static ButtonHandle touch2() { return _touch2; }
  static void init_touch_buttons();

private:
  static ButtonHandle _touch2;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    GraphicsWindow::init_type();
    register_type(_type_handle, "AndroidGraphicsWindow",
                  GraphicsWindow::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {init_type(); return get_class_type();}

private:
  static TypeHandle _type_handle;
};

#include "androidGraphicsWindow.I"

#endif
