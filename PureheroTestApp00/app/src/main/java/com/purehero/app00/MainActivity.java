package com.purehero.app00;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.widget.Button;
import android.widget.TextView;

import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.navigation.NavigationView;

import androidx.annotation.NonNull;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.appcompat.app.AppCompatActivity;

import com.google.firebase.BuildConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.purehero.app00.databinding.ActivityMainBinding;

import java.util.HashMap;
import java.util.Scanner;

import kotlinx.coroutines.scheduling.Task;

public class MainActivity extends AppCompatActivity {

    private AppBarConfiguration mAppBarConfiguration;
    private ActivityMainBinding binding;
    private NativeModule00 nativeModule00 = new NativeModule00();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        checkVersion();

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.appBarMain.toolbar);
        binding.appBarMain.fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                nativeModule00.onClickSnackbar(view);
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
        DrawerLayout drawer = binding.drawerLayout;
        NavigationView navigationView = binding.navView;
        // Passing each menu ID as a set of Ids because each
        // menu should be considered as top level destinations.
        mAppBarConfiguration = new AppBarConfiguration.Builder(
                R.id.nav_home, R.id.nav_gallery, R.id.nav_slideshow)
                .setOpenableLayout(drawer)
                .build();
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        NavigationUI.setupActionBarWithNavController(this, navController, mAppBarConfiguration);
        NavigationUI.setupWithNavController(navigationView, navController);

        nativeModule00.init_module();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, mAppBarConfiguration)
                || super.onSupportNavigateUp();
    }

    // Firebase remote config 기능을 이용한 업데이트 버전 체크
    private void checkVersion() {
        final String DEF_MINIMUM_VERSION_STRING = "minimum_version";
        final String DEF_FORCE_UPDATE_STRING = "force_update";

        FirebaseRemoteConfig mConfig = FirebaseRemoteConfig.getInstance();
        FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings
                .Builder()
                .setMinimumFetchIntervalInSeconds(60 * 60)  // 1시간 : Google 권장이 12시간임, 테스트를 위한 것이니 1시간으로 고정
                .build();

        HashMap defaultMap = new HashMap <String, String>();
        defaultMap.put(DEF_MINIMUM_VERSION_STRING, "0");
        defaultMap.put(DEF_FORCE_UPDATE_STRING, "false");

        mConfig.setDefaultsAsync(defaultMap);
        mConfig.setConfigSettingsAsync(configSettings);

        mConfig.fetchAndActivate().addOnCompleteListener(task -> {
            boolean force_update = mConfig.getBoolean(DEF_FORCE_UPDATE_STRING);
            Log.d("TEST_APP", "[Firebase config] force_update : " + force_update );

            if (force_update) { //
                long minimum_version = mConfig.getLong(DEF_MINIMUM_VERSION_STRING);

                long current_version_code = 0;
                try {
                    current_version_code = this.getPackageManager().getPackageInfo(getPackageName(), 0).versionCode;
                } catch (PackageManager.NameNotFoundException e) {
                    throw new RuntimeException(e);
                }

                Log.d("TEST_APP", String.format("[Firebase config] minimum_version[%d], current_version_code[%d] ", minimum_version, current_version_code ));
                if (minimum_version > current_version_code) {
                    // 강제 앱 업데이트
                    String msg = String.format( "최신 버전을 사용해 주세요.\n\n현재 앱 버전(%d)\n최소 권장 버전(%d)", current_version_code, minimum_version ) ;
                    AppSealingAlertDialog.showAlertDialog(this, AppSealingAlertDialog.DIALOG_TYPE_ALERT_TIMER, msg, false );
                }
            }
        });
    }
}