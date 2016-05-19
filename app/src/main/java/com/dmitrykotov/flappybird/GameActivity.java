package com.dmitrykotov.flappybird;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;

public class GameActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    private static String TAG = "GameActivity";

    static {
        System.loadLibrary("native-engine");
    }

    public static native void nativeOnPause();

    public static native void nativeOnResume();

    public static native void nativeOnTap();

    public static native void nativeSetSurface(Surface surface);

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.i(TAG, "onCreate()");

        GameUtil.setContext(this);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null)
            actionBar.hide();

        SurfaceView surfaceView = new SurfaceView(this);
        setContentView(surfaceView);
        surfaceView.getHolder().addCallback(this);

        surfaceView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN)
                    nativeOnTap();

                return true;
            }
        });
    }

    @Override
    public void onPause()
    {
        super.onPause();
        Log.i(TAG, "onPause");

        nativeOnPause();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        Log.i(TAG, "onResume");

        nativeOnResume();
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.i(TAG, "surfaceChanged(" + w + ", " + h + ")");

        nativeSetSurface(holder.getSurface());
    }

    public void surfaceCreated(SurfaceHolder holder) {
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed()");

        nativeSetSurface(null);
    }

}
