/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file jni_PandaActivity.cxx
 * @author rdb
 * @date 2025-11-09
 */

#include <jni.h>
#include <sys/mman.h>
#include <unistd.h>

#if __GNUC__ >= 4
#define EXPORT_JNI extern "C" __attribute__((visibility("default")))
#else
#define EXPORT_JNI extern "C"
#endif

/**
 *
 */
EXPORT_JNI jlong
Java_org_panda3d_android_PandaActivity_nativeMmap(JNIEnv* env, jclass, jint fd, jlong off, jlong len) {
  // Align the offset down to the page size boundary.
  size_t page_size = getpagesize();
  off_t aligned = off & ~((off_t)page_size - 1);
  size_t delta = (size_t)(off - aligned);

  void *ptr = mmap(nullptr, (size_t)len + delta, PROT_READ, MAP_PRIVATE, fd, aligned);
  if (ptr != MAP_FAILED && ptr != nullptr) {
    return (jlong)((uintptr_t)ptr + delta);
  } else {
    return (jlong)0;
  }
}

/**
 * Calls the given function pointer, passing the given data pointer.
 */
EXPORT_JNI void
Java_org_panda3d_android_PandaActivity_nativeThreadEntry(JNIEnv* env, jobject self, jlong func, jlong data) {
  ((void (*)(void *))(void *)func)((void *)data);
}

/**
 * Queues a soft keyboard edit: delete some characters before the cursor, then
 * type the given codepoints.  Called on the UI thread.
 */
EXPORT_JNI void
Java_org_panda3d_android_PandaActivity_nativeImeEdit(JNIEnv* env, jclass, jint backspaces, jintArray codepoints) {
  AndroidImeEvent event;
  event._backspaces = backspaces;
  event._keycode = -1;
  event._down = false;

  jsize length = env->GetArrayLength(codepoints);
  jint *elements = env->GetIntArrayElements(codepoints, nullptr);
  for (jsize i = 0; i < length; ++i) {
    event._text.push_back((wchar_t)elements[i]);
  }
  env->ReleaseIntArrayElements(codepoints, elements, JNI_ABORT);

  android_queue_ime_event(event);
}

/**
 * Queues a key the soft keyboard pressed or released that is not text, such as
 * Enter or Backspace.  Called on the UI thread.
 */
EXPORT_JNI void
Java_org_panda3d_android_PandaActivity_nativeImeKey(JNIEnv* env, jclass, jint keycode, jboolean down) {
  AndroidImeEvent event;
  event._backspaces = 0;
  event._keycode = keycode;
  event._down = (down != JNI_FALSE);
  android_queue_ime_event(event);
}
