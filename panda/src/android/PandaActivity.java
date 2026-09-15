/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file PandaActivity.java
 * @author rdb
 * @date 2013-01-22
 */

package org.panda3d.android;

import android.app.NativeActivity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageInfo;
import android.content.res.AssetFileDescriptor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import android.widget.Toast;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import dalvik.system.BaseDexClassLoader;
import org.panda3d.android.NativeIStream;
import org.panda3d.android.NativeOStream;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.text.InputType;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

/**
 * The entry point for a Panda-based activity.  Loads the Panda libraries and
 * also provides some utility functions.
 */
public class PandaActivity extends NativeActivity {
    private static final Bitmap.Config sConfigs[] = {
            null,
            Bitmap.Config.ALPHA_8,
            null,
            Bitmap.Config.RGB_565,
            Bitmap.Config.ARGB_4444,
            Bitmap.Config.ARGB_8888,
            null, //Bitmap.Config.RGBA_F16,
            null, //Bitmap.Config.HARDWARE,
        };
    private static final Bitmap.CompressFormat sFormats[] = {
            Bitmap.CompressFormat.JPEG,
            Bitmap.CompressFormat.PNG,
            Bitmap.CompressFormat.WEBP,
        };

    protected static BitmapFactory.Options readBitmapSize(long istreamPtr) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;
        options.inScaled = false;
        NativeIStream stream = new NativeIStream(istreamPtr);
        BitmapFactory.decodeStream(stream, null, options);
        return options;
    }

    protected static Bitmap readBitmap(long istreamPtr, int sampleSize) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        // options.inPreferredConfig = Bitmap.Config.RGBA_8888;
        options.inScaled = false;
        options.inSampleSize = sampleSize;
        NativeIStream stream = new NativeIStream(istreamPtr);
        return BitmapFactory.decodeStream(stream, null, options);
    }

    protected static Bitmap createBitmap(int width, int height, int config, boolean hasAlpha) {
        return Bitmap.createBitmap(width, height, sConfigs[config]);
    }

    protected static boolean compressBitmap(Bitmap bitmap, int format, int quality, long ostreamPtr) {
        NativeOStream stream = new NativeOStream(ostreamPtr);
        return bitmap.compress(sFormats[format], quality, stream);
    }

    protected static String getCurrentThreadName() {
        return Thread.currentThread().getName();
    }

    /**
     * Called by android_native_app_glue to spawn the application thread.
     * Gets passed a function pointer and a data pointer to pass to it.
     */
    protected void spawnAppThread(long ptr, long data) {
        new Thread(() -> {
            nativeThreadEntry(ptr, data);
        }).start();
    }

    /**
     * Maps the blob to memory and returns the pointer.
     */
    public long mapBlobFromResource(long offset) {
        int resourceId = 0;
        try {
            ActivityInfo ai = getPackageManager().getActivityInfo(
                    getIntent().getComponent(), PackageManager.GET_META_DATA);
            if (ai.metaData == null) {
                Log.e("Panda3D", "Failed to get activity metadata");
                return 0;
            }
            resourceId = ai.metaData.getInt("org.panda3d.android.BLOB_RESOURCE");
            if (resourceId == 0) {
                return 0;
            }

            AssetFileDescriptor afd = getResources().openRawResourceFd(resourceId);
            ParcelFileDescriptor pfd = afd.getParcelFileDescriptor();
            long off = afd.getStartOffset() + offset;
            long len = afd.getLength();
            return nativeMmap(pfd.getFd(), off, len);
        } catch (Exception e) {
            Log.e("Panda3D", "Received exception while trying to map blob: " + e);
            return 0;
        }
    }

    /**
     * Returns the path to the main native library.
     */
    public String getNativeLibraryPath() {
        String libname = "main";
        try {
            ActivityInfo ai = getPackageManager().getActivityInfo(
                    getIntent().getComponent(), PackageManager.GET_META_DATA);
            if (ai.metaData != null) {
                String ln = ai.metaData.getString(META_DATA_LIB_NAME);
                if (ln != null) libname = ln;
            }
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException("Error getting activity info", e);
        }

        BaseDexClassLoader classLoader = (BaseDexClassLoader) getClassLoader();
        return classLoader.findLibrary(libname);
    }

    /**
     * Returns the path to some other native library.
     */
    public String findLibrary(String libname) {
        BaseDexClassLoader classLoader = (BaseDexClassLoader)getClassLoader();
        return classLoader.findLibrary(libname);
    }

    public String getIntentDataPath() {
        Intent intent = getIntent();
        Uri data = intent.getData();
        if (data == null) {
            return null;
        }
        String path = data.getPath();
        if (path.startsWith("//")) {
          path = path.substring(1);
        }
        return path;
    }

    public String getIntentOutputUri() {
        Intent intent = getIntent();
        return intent.getStringExtra("org.panda3d.OUTPUT_URI");
    }

    public String getCacheDirString() {
        return getCacheDir().toString();
    }

    public String getFilesDirString() {
        return getFilesDir().toString();
    }

    public String getMainExpansionPath() {
        try {
            PackageInfo info = getPackageManager().getPackageInfo(getPackageName(), 0);
            java.io.File obbDir = getObbDir();
            if (obbDir == null) {
                throw new RuntimeException("Android OBB directory is unavailable");
            }
            return new java.io.File(
                    obbDir,
                    "main." + info.versionCode + "." + getPackageName() + ".obb")
                    .toString();
        } catch (PackageManager.NameNotFoundException e) {
            throw new RuntimeException("Unable to read package version", e);
        }
    }

    public boolean isDebuggable() {
        return (getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0;
    }

    /**
     * The view the soft keyboard types into.  NativeActivity's own content
     * view is not a text editor, so an input method gives it only a fallback
     * connection, which turns one committed character into key events and
     * anything longer (swipe typing, an autocorrection) into an ACTION_MULTIPLE
     * event whose characters the NDK cannot read.  This view instead gives the
     * input method a connection that forwards its edits to the engine as text.
     * Touch and hardware keys still reach the engine through the window's
     * input queue, whichever view has focus.
     */
    private ImeView mImeView;
    private volatile boolean mImeWanted = false;
    private boolean mImeShown = false;
    private final Handler mUiHandler = new Handler(Looper.getMainLooper());

    /**
     * Shows the system soft keyboard for a newly focused text box.  Safe to
     * call from any thread.
     */
    public void showSoftKeyboard() {
        mImeWanted = true;
        mUiHandler.post(() -> applySoftKeyboard(true));
    }

    /**
     * Hides the system soft keyboard.  Safe to call from any thread.  Delayed
     * slightly, so that one text box handing focus to another (a focus-out
     * then a focus-in) does not flicker the keyboard.
     */
    public void hideSoftKeyboard() {
        mImeWanted = false;
        mUiHandler.postDelayed(() -> applySoftKeyboard(false), 100);
    }

    private void applySoftKeyboard(boolean showRequest) {
        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm == null) {
            return;
        }
        if (mImeWanted) {
            if (!showRequest) {
                // A hide overtaken by a later show.
                return;
            }
            if (mImeView == null) {
                mImeView = new ImeView(this);
                addContentView(mImeView, new ViewGroup.LayoutParams(1, 1));
            }
            mImeView.setFocusable(true);
            mImeView.setFocusableInTouchMode(true);
            mImeView.requestFocus();
            // Start the input method over, so no word state carries into the
            // new box.
            imm.restartInput(mImeView);
            mImeShown = true;
            requestSoftInput(imm, 10);
        } else if (mImeShown) {
            mImeShown = false;
            imm.hideSoftInputFromWindow(mImeView.getWindowToken(), 0);
            // Not focusable while hidden, so the view cannot take focus back
            // and bring the keyboard up by itself when the window regains focus.
            mImeView.setFocusableInTouchMode(false);
            mImeView.setFocusable(false);
        }
    }

    /**
     * showSoftInput is refused until the input method has started serving the
     * newly focused view, which happens asynchronously, so retry briefly.
     */
    private void requestSoftInput(InputMethodManager imm, int attempts) {
        if (!mImeWanted || !mImeShown) {
            return;
        }
        if (!imm.showSoftInput(mImeView, 0) && attempts > 1) {
            mUiHandler.postDelayed(() -> requestSoftInput(imm, attempts - 1), 50);
        }
    }

    private static class ImeView extends View {
        ImeView(Context context) {
            super(context);
        }

        @Override
        public boolean onCheckIsTextEditor() {
            return true;
        }

        @Override
        public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
            outAttrs.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_CAP_SENTENCES;
            // No fullscreen extract view in landscape: it would hide the game.
            outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE
                    | EditorInfo.IME_FLAG_NO_EXTRACT_UI
                    | EditorInfo.IME_FLAG_NO_FULLSCREEN;
            outAttrs.initialSelStart = 0;
            outAttrs.initialSelEnd = 0;
            return new ImeConnection(this);
        }
    }

    /**
     * Keeps the input method's own copy of the text in an Editable, as for any
     * text view, and after every edit sends the engine the difference from what
     * it has already been sent: backspaces for what was removed, then the
     * characters that replace it.  Composing text (the word being typed, which
     * a keyboard can still autocorrect) is sent as it is typed, so the box
     * always holds exactly what is on screen, and a correction arrives as
     * backspaces and retyping.  This assumes the box's cursor stays at the end
     * of what the keyboard typed.  Every character, punctuation included, goes
     * through unchanged; the box decides what it accepts.
     */
    private static class ImeConnection extends BaseInputConnection {
        private final View mView;
        private String mSent = "";

        ImeConnection(View view) {
            super(view, true);
            mView = view;
        }

        private void sync() {
            String now = getEditable().toString();
            int common = 0;
            int limit = Math.min(now.length(), mSent.length());
            while (common < limit && now.charAt(common) == mSent.charAt(common)) {
                ++common;
            }
            if (common > 0 && Character.isHighSurrogate(now.charAt(common - 1))) {
                --common;
            }
            int removed = mSent.codePointCount(common, mSent.length());
            String added = now.substring(common);
            int[] codepoints = new int[added.codePointCount(0, added.length())];
            for (int i = 0, j = 0; i < added.length(); ++j) {
                int codepoint = added.codePointAt(i);
                codepoints[j] = codepoint;
                i += Character.charCount(codepoint);
            }
            mSent = now;
            if (removed > 0 || codepoints.length > 0) {
                nativeImeEdit(removed, codepoints);
            }
        }

        /**
         * Sends Enter, which submits or closes the box, and starts the input
         * method over on an empty editor to match.
         */
        private void submit() {
            super.finishComposingText();
            sync();
            nativeImeKey(KeyEvent.KEYCODE_ENTER, true);
            nativeImeKey(KeyEvent.KEYCODE_ENTER, false);
            getEditable().clear();
            mSent = "";
            InputMethodManager imm = (InputMethodManager)
                    mView.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            if (imm != null) {
                imm.restartInput(mView);
            }
        }

        @Override
        public boolean commitText(CharSequence text, int newCursorPosition) {
            if ("\n".contentEquals(text)) {
                submit();
                return true;
            }
            boolean result = super.commitText(text, newCursorPosition);
            sync();
            return result;
        }

        @Override
        public boolean setComposingText(CharSequence text, int newCursorPosition) {
            boolean result = super.setComposingText(text, newCursorPosition);
            sync();
            return result;
        }

        @Override
        public boolean finishComposingText() {
            boolean result = super.finishComposingText();
            sync();
            return result;
        }

        @Override
        public boolean deleteSurroundingText(int beforeLength, int afterLength) {
            boolean result = super.deleteSurroundingText(beforeLength, afterLength);
            sync();
            return result;
        }

        @Override
        public boolean performEditorAction(int actionCode) {
            submit();
            return true;
        }

        @Override
        public boolean sendKeyEvent(KeyEvent event) {
            int keycode = event.getKeyCode();
            boolean down = (event.getAction() == KeyEvent.ACTION_DOWN);
            if (keycode == KeyEvent.KEYCODE_ENTER || keycode == KeyEvent.KEYCODE_NUMPAD_ENTER) {
                if (down) {
                    submit();
                }
                return true;
            }
            if (keycode == KeyEvent.KEYCODE_DEL && getEditable().length() > 0) {
                // Delete through the editor, so it stays in step with the box.
                if (down) {
                    deleteSurroundingText(1, 0);
                }
                return true;
            }
            int unicode = event.getUnicodeChar(event.getMetaState());
            if (unicode > 0 && keycode != KeyEvent.KEYCODE_DEL) {
                // Some keyboards send digits and symbols as key events.
                if (down) {
                    commitText(new String(Character.toChars(unicode)), 1);
                }
                return true;
            }
            if (event.getAction() != KeyEvent.ACTION_MULTIPLE) {
                // Backspace on an empty editor, the arrows, and so on.
                nativeImeKey(keycode, down);
            }
            return true;
        }
    }

    /**
     * Sets the window title.
     */
    public void setWindowTitle(final CharSequence title) {
        final PandaActivity activity = this;
        runOnUiThread(new Runnable() {
            public void run() {
                activity.setTitle(title);
            }
        });
    }

    /**
     * Shows a pop-up notification.
     */
    public void showToast(final String text, final int duration) {
        final PandaActivity activity = this;
        runOnUiThread(new Runnable() {
            public void run() {
                Toast toast = Toast.makeText(activity, text, duration);
                toast.show();
            }
        });
    }

    static {
        // Load this explicitly to initialize the JVM with the thread system.
        System.loadLibrary("panda");

        // Contains our JNI calls.
        System.loadLibrary("p3android");
    }

    private static native long nativeMmap(int fd, long off, long len);
    private static native void nativeThreadEntry(long ptr, long data);
    static native void nativeImeEdit(int backspaces, int[] codepoints);
    static native void nativeImeKey(int keycode, boolean down);
}
