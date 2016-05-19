package com.dmitrykotov.flappybird;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.preference.PreferenceManager;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Map;

public class GameUtil {
    private static String TAG = "GameUtil";

    private static Context context = null;

    private static ByteBuffer textureBuffer = null;

    public static void setContext(Context ctx) {
        context = ctx;
    }

    public static void cleanUp() {
        textureBuffer = null;
    }

    public static byte[] loadTexture(String name) {

        if(context == null) return null;

        int id = context.getResources().getIdentifier(name, "drawable", context.getPackageName());
        if(id == 0)
            return null;

        Bitmap res = BitmapFactory.decodeResource(context.getResources(), id);

        int width = res.getWidth();
        int height = res.getHeight();

        Paint paint = new Paint();
        Bitmap source = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(source);
        canvas.drawBitmap(res, 0, 0, paint);

        int sz = width*height*4;
        textureBuffer = ByteBuffer.allocate(sz + 8);
        textureBuffer.order(ByteOrder.LITTLE_ENDIAN);

        source.copyPixelsToBuffer(textureBuffer);
        textureBuffer.putInt(width);
        textureBuffer.putInt(height);

        res.recycle();
        source.recycle();

        Log.i(TAG, "loadTexture(" + name + "), " + width + " x " + height);

        return textureBuffer.array();
    }

    public static void saveSettings(Map<String, String> data)
    {
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        SharedPreferences.Editor editor = sharedPref.edit();

        for (Map.Entry<String, String> i: data.entrySet()) {
            editor.putString(i.getKey(), i.getValue());
        }

        editor.commit();
    }

    public static String[] loadSettingKeys()
    {
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        Map<String, ?> map = sharedPref.getAll();

        String[] keys = new String[map.size()];
        int i = 0;
        for (String s : map.keySet()) {
            keys[i] = s;
            ++i;
        }

        return keys;
    }

    public static String[] loadSettingValues()
    {
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        Map<String, ?> map = sharedPref.getAll();

        String[] keys = new String[map.size()];
        int i = 0;
        for (Object s : map.values()) {
            keys[i] = (String)s;
            ++i;
        }

        return keys;
    }

}
