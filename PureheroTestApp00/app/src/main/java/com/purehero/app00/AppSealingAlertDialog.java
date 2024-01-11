package com.purehero.app00;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Gravity;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class AppSealingAlertDialog extends Activity implements Application.ActivityLifecycleCallbacks {
    private String  dialogMessage = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setTheme( android.R.style.Theme_Translucent_NoTitleBar );
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        dialogMessage   = intent.getStringExtra( "msg" );
        killTimeSec     = intent.getIntExtra("killTimeSec", 10 );

        setContentView( makeContentView( intent.getIntExtra( "type", DIALOG_TYPE_ALERT ) ) );
        if (intent.getBooleanExtra("showToast", false )) {
            Toast.makeText(this, dialogMessage, Toast.LENGTH_LONG).show();
        }

        getApplication().registerActivityLifecycleCallbacks( this );
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        // timer 값이 변경되었을 때만 Thread 종료를 기다리도록 한다.
        if (killTimeSec != -1) {
            runThread = false;
            // 2초 이내에 이전 Thread가 종료될 때까지 대기한다.
            for (int i = 0; i < 20 && !runThread; i++) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    //e.printStackTrace();
                }
            }

            outState.putInt("killTimeSec", killTimeSec);
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        killTimeSec = savedInstanceState.getInt( "killTimeSec", -1 );
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public static final int DIALOG_TYPE_TOAST           = 0;    // 기본 Toast type
    public static final int DIALOG_TYPE_ALERT           = 1;    //
    public static final int DIALOG_TYPE_ALERT_TIMER     = 2;    //

    private View makeContentView( int dialogType ) {
        View ret = null;
        switch (dialogType) {
            case DIALOG_TYPE_TOAST : ret = new DialogTypeToast(this ).makeContentView(); break;
            case DIALOG_TYPE_ALERT : ret = new DialogTypeAlert(this ).makeContentView(); break;
            case DIALOG_TYPE_ALERT_TIMER: ret = new DialogTypeAlertTimer(this, killTimeSec ).makeContentView(); break;
            default : break;
        }

        final int opacityRate = (int)( 45.0/*불투명도(%)*/ * 2.55 ) << 24;
        if (ret != null) {
            ret.setBackgroundColor( opacityRate );
        }
        return ret;
    }

    @Override
    // TODO: deprecated 처리
    // Android 13부터 추가된 swipe to left, swipe to right시 앱이 닫히고 있는 중임을 표시하는 기능이
    // swipe와 back button press와 구분이 되지 않기 때문에 deprecate되었다.
    // Android 13까지 android:enableOnBackInvokedCallback 기본값은 false이지만 Android 14부터
    // 기본값이 true로 설정되기 때문에 back button을 누르면 시스템이 이를 가로채기 때문에 의도한 동작을 보장할 수 없다.
    // 
    // 적절하게 처리되지 않으면 alert 창이 이 기능으로 BG로 전환되면 종료되지 않고 다시 출력되거나 화면이 어두어지는 문제점이 발생한다.   
    // 
    // https://developer.android.com/guide/navigation/predictive-back-gesture?hl=ko
    public void onBackPressed() {
        super.onBackPressed();

        finish();
        killMyProcess(0);
    }

    private int killTimeSec = -1;
    private boolean runThread = true;

    // Application.ActivityLifecycleCallbacks methods
    @Override
    public void onActivityCreated(Activity activity, Bundle bundle) {}

    @Override
    public void onActivityStarted(Activity activity) {}

    @Override
    public void onActivityResumed(Activity activity) {}

    @Override
    public void onActivityPaused(Activity activity) {
        moveTopAlertDialog( activity );
    }

    Handler moveTopAlertDialogHandler = null;
    private void moveTopAlertDialog( final Activity activity ) {
        if (activity.getLocalClassName().compareTo( getLocalClassName()) == 0 && moveTopAlertDialogHandler == null) {
            
            // https://developer.android.com/reference/android/os/Handler#Handler()
            HandlerThread alertDialogHandlerThread = new HandlerThread("alertDialogHandlerThread");
            alertDialogHandlerThread.start();
            
            if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.R /*Android 11, API level 30*/) {
                moveTopAlertDialogHandler = new Handler(alertDialogHandlerThread.getLooper());
            } else {
                moveTopAlertDialogHandler = new Handler();
            }
            
            moveTopAlertDialogHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    Intent intent = activity.getIntent();

                    intent.setFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
                    intent.addFlags(Intent.FLAG_ACTIVITY_NO_HISTORY );      // 실행 목록에 추가하지 않는다.
                    intent.addFlags( Intent.FLAG_ACTIVITY_SINGLE_TOP );

                    intent.putExtra("killTimeSec", killTimeSec);
                    activity.startActivity( intent );
                }
            }, 1000 );
        }
    }

    @Override

    public void onActivityStopped(Activity activity) {
        moveTopAlertDialog( activity );
    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle bundle) {}

    @Override
    public void onActivityDestroyed(Activity activity) {
        moveTopAlertDialog( activity );
    }

    /////////////////////////////////////////////////////////////////////////

    private class DialogTypeAlertTimer extends DialogTypeAlert implements Runnable {
        public DialogTypeAlertTimer(Context context, int timeSec ) {
            super(context);
            if (killTimeSec == -1) {
                killTimeSec = timeSec;
            }
        }

        @Override
        protected View makeDialogLayout() {
            View ret = super.makeDialogLayout();
            new Thread(this).start();

            return ret;
        }

        @Override
        public void run() {
            if (tvConfirmButton != null) {
                for (; killTimeSec > 0; killTimeSec--) {
                    AppSealingAlertDialog.this.runOnUiThread(new Runnable(){
                        @Override
                        public void run() {
                            // https://developer.android.com/reference/android/R.string
                            if ( Build.VERSION.SDK_INT >= Build.VERSION_CODES.R /*Android 11, API level 30*/) {
                                tvConfirmButton.setText( String.format("%s(%d)", getResources().getString( android.R.string.ok ), killTimeSec ) );                                
                            } else {
                                tvConfirmButton.setText( String.format("%s(%d)", getResources().getString( android.R.string.yes ), killTimeSec ) );
                            }
                        }
                    });
                    try {
                        for (int i = 0; i < 10; i++) {
                            Thread.sleep(100 );

                            // kill timer 동작중에 단말기의 화면이 회전되는 경우 Activity가 재 시작되는데 이떄 timer thread 을 종료 시키기 위해서 사용된다.
                            if (!runThread) {
                                runThread = true;

                                return;
                            }
                        }
                    } catch (InterruptedException e) {
                    }
                }

                alertDialogFinish();
            }
        }
    }

    private void alertDialogFinish() {
        getApplication().unregisterActivityLifecycleCallbacks( this );

        finish();
        killMyProcess(0);
    }

    private class DialogTypeAlert extends DialogTypeBase {
        final int BTN_ID_CONFIRM = 0x1111;
        protected TextView tvConfirmButton = null;

        public DialogTypeAlert(Context context) {
            super(context);
        }

        @Override
        protected View makeDialogLayout() {
            LinearLayout layout = new LinearLayout( context );
            layout.setOrientation( LinearLayout.VERTICAL );
            layout.setPadding( 50, 50, 50, 0 );

            try
            {
                makeDialogAppIconLayout( layout );
            } catch (Exception e) {
                //e.printStackTrace();
            }

            
            makeDialogTitleMessageLayout( layout );
            makeErrorCodeMessageLayout( layout );
            makeDialogMessageLayout( layout );
            makeDialogConfirmLayout( layout );

            return layout;
        }

        @Override
        public void onClick(View view) {
            alertDialogFinish();
        }

        protected void makeDialogConfirmLayout(LinearLayout layout) {
            tvConfirmButton = new TextView( context );
            tvConfirmButton.setText( getResources().getString( android.R.string.ok ));
            tvConfirmButton.setTextColor( Color.BLUE );
            tvConfirmButton.setId( BTN_ID_CONFIRM );
            tvConfirmButton.setOnClickListener( this );

            LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            params.rightMargin      = 50;
            params.topMargin        = 100;
            params.bottomMargin     = 50;
            params.gravity          = Gravity.RIGHT | Gravity.BOTTOM;
            layout.addView( tvConfirmButton, params );
        }

        protected void makeDialogMessageLayout(LinearLayout layout) {
            TextView tvDialogMessage = new TextView( context );
            tvDialogMessage.setText(String.format("%s",
                    dialogMessage.replace(". (", ". \n(").replace(". ", ".\n")));
            tvDialogMessage.setGravity( Gravity.CENTER );
            tvDialogMessage.setTextColor( Color.BLACK );
            LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            params.topMargin        = 50;
            layout.addView( tvDialogMessage, params );
        }

        protected void makeErrorCodeMessageLayout(LinearLayout layout) {
            int errorCode = parserErrorCode();
            if (errorCode == -1)
                return;

            TextView tvErrorCode = new TextView( context );
            tvErrorCode.setText( String.format( "[%d]", errorCode ));
            tvErrorCode.setTextColor( Color.BLACK );
            tvErrorCode.setGravity( Gravity.CENTER );
            tvErrorCode.setTextSize( 2, 17 );
            // tvErrorCode.setTypeface( tvErrorCode.getTypeface(), Typeface.BOLD );
            tvErrorCode.setPadding( 0, 10, 0, 10 );

            layout.addView( tvErrorCode );
        }

        protected void makeDialogTitleMessageLayout(LinearLayout layout) {
            TextView tvDialogTitle = new TextView( context );
            tvDialogTitle.setText( "Warning" );
            tvDialogTitle.setGravity( Gravity.CENTER );
            tvDialogTitle.setTextColor( Color.BLACK );
            tvDialogTitle.setTextSize( 2, 25 );
            tvDialogTitle.setTypeface( tvDialogTitle.getTypeface(), Typeface.BOLD );
            tvDialogTitle.setPadding( 0, 25, 0, 25 );
            layout.addView( tvDialogTitle );
        }

        protected void makeDialogAppIconLayout(LinearLayout layout) {
            RelativeLayout relativeLayout = new RelativeLayout( context );
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
            params.addRule( RelativeLayout.CENTER_IN_PARENT );

            ImageView imageView = new ImageView( context );
            try {
                imageView.setBackground(context.getPackageManager().getApplicationIcon(context.getPackageName()));
                RelativeLayout.LayoutParams imageParams = new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT );
                imageView.setLayoutParams( imageParams );
                // in.dishtv.subscriber 앱에서 탐지시 창이 보이지 않는 증상이 확인되어 현재 구조로 변경됨.
                //
                // 20230329] valofe 앱에서 아이콘이 안내창을 덮어 가시성을 해치는 문제가 확인되어
                // scaleType을 CENTER_INSIDE -> CENTER_CROP으로 조정함 (com.valofe.fish.one.kr, com.valofe.fish.google.sea)
                imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
            } catch (PackageManager.NameNotFoundException e) {
                //e.printStackTrace();
            }

            params.width = 200;
            params.height = 200;

            relativeLayout.addView( imageView, params );
            layout.addView( relativeLayout );
        }

        protected int parserErrorCode() {
            int ret = -1;

            try {
                int startIdx = dialogMessage.indexOf("[");
                if (startIdx < 0)
                    return -1;

                int endIdx = dialogMessage.indexOf("]", startIdx + 1);
                if (endIdx < 0)
                    return -1;

                String errorCode = dialogMessage.substring(startIdx + 1, endIdx );
                ret = Integer.valueOf(errorCode);

                dialogMessage = dialogMessage.substring( endIdx + 1 );
            } catch (Exception e) {
            }

            return ret;
        }
    }

    private class DialogTypeToast extends DialogTypeBase implements Runnable {
        public DialogTypeToast(Context context) {
            super(context);
        }

        @Override
        protected View makeDialogLayout() {
            LinearLayout layout = new LinearLayout( context );
            layout.setOrientation( LinearLayout.VERTICAL );
            layout.setPadding( 50, 35, 50, 35 );

            TextView tvDialogMessage = new TextView( context );
            tvDialogMessage.setText(String.format("%s",
                    dialogMessage.replace(". (", ". \n(").replace(". ", ".\n")));
            tvDialogMessage.setGravity( Gravity.CENTER );
            tvDialogMessage.setTextColor( Color.BLACK );
            tvDialogMessage.setTypeface( tvDialogMessage.getTypeface(), Typeface.BOLD );
            LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
            params.topMargin        = 50;
            params.bottomMargin     = 50;
            layout.addView( tvDialogMessage, params );

            new Thread( this ).start();

            return layout;
        }

        @Override
        public void run() {
            try {
                // Toast 는 대략 3-4초 후에 표시가 사라지게 한다.
                Thread.sleep( 3500 );
            } catch (InterruptedException e) {
                //e.printStackTrace();
            }

            alertDialogFinish();
        }
    }

    private class DialogTypeBase implements View.OnClickListener {
        protected final Context context;

        private final int LAYOUT_ID_LEFT = 0x2222;
        private final int LAYOUT_ID_RIGHT = 0x3333;
        private final int LAYOUT_ID_CENTER = 0x4444;

        public DialogTypeBase(Context context) {
            this.context = context;
        }

        public final View makeContentView() {
            RelativeLayout layout = new RelativeLayout(context);

            RelativeLayout leftLayout = new RelativeLayout(context);
            RelativeLayout rightLayout = new RelativeLayout(context);
            RelativeLayout centerLayout = new RelativeLayout(context);

            RelativeLayout.LayoutParams leftParams = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
            RelativeLayout.LayoutParams rightParams = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
            RelativeLayout.LayoutParams centerParams = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT);

            makeLeftLayout(leftLayout, leftParams);
            makeRightLayout(rightLayout, rightParams);
            makeCenterLayout(centerLayout, centerParams);

            layout.addView(leftLayout, leftParams);
            layout.addView(rightLayout, rightParams);
            layout.addView(centerLayout, centerParams);

            return layout;
        }

        private void makeRightLayout(RelativeLayout layout, RelativeLayout.LayoutParams params) {
            layout.setId(LAYOUT_ID_RIGHT);
            params.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
            params.width = 50;
        }

        private void makeLeftLayout(RelativeLayout layout, RelativeLayout.LayoutParams params) {
            layout.setId(LAYOUT_ID_LEFT);
            params.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
            params.width = 50;
        }

        private void makeCenterLayout(RelativeLayout layout, RelativeLayout.LayoutParams params) {
            layout.setId(LAYOUT_ID_CENTER);

            // center layout 배치
            params.addRule(RelativeLayout.CENTER_IN_PARENT);
            params.addRule(RelativeLayout.LEFT_OF, LAYOUT_ID_RIGHT);
            params.addRule(RelativeLayout.RIGHT_OF, LAYOUT_ID_LEFT);

            // dialog layout 추가
            RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT,RelativeLayout.LayoutParams.WRAP_CONTENT);
            layoutParams.addRule(RelativeLayout.CENTER_IN_PARENT);
            layout.addView(makeDialogLayout(), layoutParams);

            // 테두리 그리기
            GradientDrawable border = new GradientDrawable();
            border.setColor(0xFFFFFFFF);
            border.setStroke(3, 0xFF000000);
            border.setCornerRadius(30);
            if ( Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN /*Android 4.1.1, API level 16*/) {
                layout.setBackgroundDrawable(border);
            } else {
                layout.setBackground(border);
            }
        }

        protected View makeDialogLayout() {
            LinearLayout layout = new LinearLayout(context);
            return layout;
        }

        @Override
        public void onClick(View view) {
        }
    }

    public static void showAlertDialog(Context context, int type, Object msg, boolean showToast ) {
        final String message = (String)msg;
        try {
            if (message.length() > 0) {
                Intent intent = new Intent();
                intent.setClass( context, AppSealingAlertDialog.class );
                intent.setAction( "controller" );
                intent.putExtra( "type", type );
                intent.putExtra( "msg", message );
                intent.putExtra( "showToast", showToast );

                intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
                intent.addFlags(Intent.FLAG_ACTIVITY_NO_ANIMATION);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);         // 해당 flag 값이 없으면 startActivity 가 안되는 앱이 있음( penguin, ... )
                intent.addFlags(Intent.FLAG_ACTIVITY_NO_HISTORY );      // 실행 목록에 추가하지 않는다.

                context.startActivity( intent );
            }
        } catch (SecurityException e ) {
        } catch (IllegalStateException e) {
        } catch (Exception e ) {
        }
    }

    private void killMyProcess( final int killTimeSec ) {
        if (killTimeSec < 1) {
            android.os.Process.killProcess(android.os.Process.myPid());
        } else {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep( killTimeSec * 1000 );
                    } catch (InterruptedException e) {
                        //e.printStackTrace();
                    }

                    android.os.Process.killProcess(android.os.Process.myPid());
                }
            }).start();
        }
    }
}
