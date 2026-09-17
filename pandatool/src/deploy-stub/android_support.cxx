/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file android_support.cxx
 * @author rdb
 * @date 2021-12-10
 */

#include <android/log.h>
#include "android_native_app_glue.h"
#include "config_android.h"
#include "thread.h"

#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#define PY_SSIZE_T_CLEAN 1

#include "Python.h"

/**
 * Writes a message to the Android log.
 */
static PyObject *
_py_log_write(PyObject *self, PyObject *args) {
  int prio;
  char *tag;
  char *text;
  if (PyArg_ParseTuple(args, "iss", &prio, &tag, &text)) {
    __android_log_write(prio, tag, text);
    Py_RETURN_NONE;
  }
  return NULL;
}

/**
 * Returns the path to a library, if it can be found.
 */
static PyObject *
_py_find_library(PyObject *self, PyObject *args) {
  char *lib;
  if (PyArg_ParseTuple(args, "s", &lib)) {
    Filename result = android_find_library(panda_android_app->activity, lib);
    if (!result.empty()) {
      return PyUnicode_FromStringAndSize(result.c_str(), (Py_ssize_t)result.length());
    } else {
      Py_RETURN_NONE;
    }
  }
  return NULL;
}

/**
 * Calls a no-argument PandaActivity method that returns a filesystem path.
 */
static PyObject *
get_activity_path(const char *method_name) {
  ANativeActivity *activity = panda_android_app->activity;
  Thread *thread = Thread::get_current_thread();
  JNIEnv *env = thread->get_jni_env();
  if (env == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Android app thread is not attached to the JVM");
    return nullptr;
  }

  jclass activity_class = env->GetObjectClass(activity->clazz);
  jmethodID method = env->GetMethodID(activity_class, method_name, "()Ljava/lang/String;");
  env->DeleteLocalRef(activity_class);
  if (method == nullptr) {
    env->ExceptionClear();
    PyErr_Format(PyExc_RuntimeError, "PandaActivity.%s is unavailable", method_name);
    return nullptr;
  }

  jstring value = (jstring)env->CallObjectMethod(activity->clazz, method);
  if (env->ExceptionCheck()) {
    env->ExceptionClear();
    PyErr_Format(PyExc_RuntimeError, "PandaActivity.%s failed", method_name);
    return nullptr;
  }
  if (value == nullptr) {
    Py_RETURN_NONE;
  }

  const char *chars = env->GetStringUTFChars(value, nullptr);
  if (chars == nullptr) {
    env->DeleteLocalRef(value);
    PyErr_SetString(PyExc_RuntimeError, "Unable to read Android activity path");
    return nullptr;
  }
  PyObject *result = PyUnicode_FromString(chars);
  env->ReleaseStringUTFChars(value, chars);
  env->DeleteLocalRef(value);
  return result;
}

static PyObject *
_py_get_files_dir(PyObject *self, PyObject *args) {
  return get_activity_path("getFilesDirString");
}

static PyObject *
_py_get_main_expansion_path(PyObject *self, PyObject *args) {
  return get_activity_path("getMainExpansionPath");
}

static PyObject *
_py_is_debuggable(PyObject *self, PyObject *args) {
  ANativeActivity *activity = panda_android_app->activity;
  Thread *thread = Thread::get_current_thread();
  JNIEnv *env = thread->get_jni_env();
  if (env == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Android app thread is not attached to the JVM");
    return nullptr;
  }

  jclass activity_class = env->GetObjectClass(activity->clazz);
  jmethodID method = env->GetMethodID(activity_class, "isDebuggable", "()Z");
  env->DeleteLocalRef(activity_class);
  if (method == nullptr) {
    env->ExceptionClear();
    PyErr_SetString(PyExc_RuntimeError, "PandaActivity.isDebuggable is unavailable");
    return nullptr;
  }
  jboolean value = env->CallBooleanMethod(activity->clazz, method);
  if (env->ExceptionCheck()) {
    env->ExceptionClear();
    PyErr_SetString(PyExc_RuntimeError, "PandaActivity.isDebuggable failed");
    return nullptr;
  }
  return PyBool_FromLong(value == JNI_TRUE);
}

/**
 * Gets a JNIEnv for the current thread, attaching it to the Java VM if needed.
 * The app thread is attached during android_main, but this is robust to being
 * called from any thread.
 */
static JNIEnv *
get_jni_env(ANativeActivity *activity) {
  if (activity == nullptr || activity->vm == nullptr) {
    return nullptr;
  }
  JavaVM *vm = (JavaVM *)activity->vm;
  JNIEnv *env = nullptr;
  int status = vm->GetEnv((void **)&env, JNI_VERSION_1_6);
  if (status == JNI_EDETACHED) {
    if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
      return nullptr;
    }
  } else if (status != JNI_OK) {
    return nullptr;
  }
  return env;
}

/**
 * Calls a no-argument void method on the activity instance via JNI.
 */
static PyObject *
call_activity_void(ANativeActivity *activity, const char *method_name) {
  JNIEnv *env = get_jni_env(activity);
  if (env == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Unable to obtain a JNI environment");
    return nullptr;
  }
  jclass activity_class = env->GetObjectClass(activity->clazz);
  jmethodID method = env->GetMethodID(activity_class, method_name, "()V");
  env->DeleteLocalRef(activity_class);
  if (method == nullptr) {
    env->ExceptionClear();
    PyErr_Format(PyExc_RuntimeError, "PandaActivity.%s is unavailable", method_name);
    return nullptr;
  }
  env->CallVoidMethod(activity->clazz, method);
  if (env->ExceptionCheck()) {
    env->ExceptionDescribe();
    env->ExceptionClear();
    PyErr_Format(PyExc_RuntimeError, "PandaActivity.%s failed", method_name);
    return nullptr;
  }
  Py_RETURN_NONE;
}

/**
 * Shows the system soft keyboard. Safe to call from any thread: the Java side
 * posts the work to the UI thread.
 */
static PyObject *
_py_show_ime(PyObject *self, PyObject *args) {
  ANativeActivity *activity = panda_android_app->activity;
  if (activity == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Android activity is not available");
    return nullptr;
  }
  return call_activity_void(activity, "showSoftKeyboard");
}

/**
 * Hides the system soft keyboard. Safe to call from any thread.
 */
static PyObject *
_py_hide_ime(PyObject *self, PyObject *args) {
  ANativeActivity *activity = panda_android_app->activity;
  if (activity == nullptr) {
    PyErr_SetString(PyExc_RuntimeError, "Android activity is not available");
    return nullptr;
  }
  return call_activity_void(activity, "hideSoftKeyboard");
}

static PyMethodDef python_simple_funcs[] = {
  { "log_write", &_py_log_write, METH_VARARGS },
  { "find_library", &_py_find_library, METH_VARARGS },
  { "get_files_dir", &_py_get_files_dir, METH_NOARGS },
  { "get_main_expansion_path", &_py_get_main_expansion_path, METH_NOARGS },
  { "is_debuggable", &_py_is_debuggable, METH_NOARGS },
  { "show_ime", &_py_show_ime, METH_NOARGS },
  { "hide_ime", &_py_hide_ime, METH_NOARGS },
  { NULL, NULL }
};

static struct PyModuleDef android_support_module = {
  PyModuleDef_HEAD_INIT,
  "android_support",
  NULL,
  -1,
  python_simple_funcs,
  NULL, NULL, NULL, NULL
};

__attribute__((visibility("default")))
extern "C" PyObject *PyInit_android_support() {
  return PyModule_Create(&android_support_module);
}
