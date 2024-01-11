package com.purehero.app00;

import android.view.View;

public class NativeModule00 {
    public native void init_module();

    public native void onClickSnackbar(View view);

    static {
        System.loadLibrary("native-module00");
    }
}
