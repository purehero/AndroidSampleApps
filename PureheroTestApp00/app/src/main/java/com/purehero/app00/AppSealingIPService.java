package com.purehero.app00;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import androidx.annotation.Nullable;

public class AppSealingIPService extends Service implements Runnable {
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        new Thread(this).start();
        return START_NOT_STICKY;
    }

    int total = 0;

    @Override
    public void run() {
        int num = total;
        for (int i = 0; i < 30; i++ ) {
            num += 10;

            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        total += num;
    }
}
