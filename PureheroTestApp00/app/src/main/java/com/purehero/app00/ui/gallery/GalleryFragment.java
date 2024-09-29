package com.purehero.app00.ui.gallery;

import static android.os.Build.VERSION_CODES.R;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.google.android.material.textfield.TextInputEditText;
import com.purehero.app00.databinding.FragmentGalleryBinding;
import com.purehero.app00.AppSealingIPService;

import java.io.BufferedReader;
import java.io.InputStreamReader;

public class GalleryFragment extends Fragment implements View.OnClickListener {

    private FragmentGalleryBinding binding;
    TextView textView;
    TextInputEditText editText;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        GalleryViewModel galleryViewModel =
                new ViewModelProvider(this).get(GalleryViewModel.class);

        binding = FragmentGalleryBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        final Button btn = binding.btnCheckVPhoneGaga;
        btn.setOnClickListener(this);

        editText = binding.tdKeyText;

        textView     = binding.textGallery;
        galleryViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);
        return root;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }

    @Override
    public void onClick(View view) {
        String strKey = editText.getText().toString();
        textView.setText( getSystemProperty( strKey, "\\^------,,-----^/ -> " + strKey) );
        //textView.setText( getSystemProperty("ro.vendor.gt_library", "\\^------,,-----^/") );

        this.getActivity().startService( new Intent( this.getActivity().getApplicationContext(), AppSealingIPService.class));
    }

    private String getSystemProperty(String propertyName, String def) {

        String propertyValue = def;

        try {
            Process getPropProcess = Runtime.getRuntime().exec("getprop " + propertyName);

            BufferedReader osRes =
                    new BufferedReader(new InputStreamReader(getPropProcess.getInputStream()));

            propertyValue = osRes.readLine();

            osRes.close();
        } catch (RuntimeException e) {
            e.printStackTrace();

        } catch (Exception e) {
            // Do nothing - can't get property value
            e.printStackTrace();
        }

        return propertyValue;
    }
}