package com.adafruit.bluefruit.le.connect.app;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.adafruit.bluefruit.le.connect.BluefruitApplication;
import com.adafruit.bluefruit.le.connect.BuildConfig;
import com.adafruit.bluefruit.le.connect.R;
import com.adafruit.bluefruit.le.connect.app.neopixel.NeopixelColorPickerFragment;
import com.larswerkman.holocolorpicker.ColorPicker;
import com.larswerkman.holocolorpicker.SaturationBar;
import com.larswerkman.holocolorpicker.ValueBar;
import com.squareup.leakcanary.RefWatcher;

public class ControllerColorPickerFragment extends Fragment implements ColorPicker.OnColorChangedListener
{
  // Log
  @SuppressWarnings("unused")
  private final static String TAG = ControllerColorPickerFragment.class.getSimpleName();

  // Constants
  private final static boolean kPersistValues = true;
  private final static String kPreferences = "ColorPickerActivity_prefs";
  private final static String kPreferences_color = "color";

  private final static int kFirstTimeColor = 0x0000ff;

  // UI
  private ColorPicker _colorPicker;
  private View _rgbColorView;
  private TextView _rgbTextView;

  // Data
  private int _selectedColor;
  private ControllerColorPickerFragmentListener _listener;

  // region Lifecycle
  @SuppressWarnings("UnnecessaryLocalVariable")
  public static ControllerColorPickerFragment newInstance()
  {
    ControllerColorPickerFragment fragment = new ControllerColorPickerFragment();
    return fragment;
  }

  public ControllerColorPickerFragment()
  {
    // Required empty public constructor
  }

  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);

    AppCompatActivity activity = (AppCompatActivity) getActivity();
    if (activity != null)
    {
      ActionBar actionBar = activity.getSupportActionBar();
      if (actionBar != null)
      {
        actionBar.setTitle(R.string.colorpicker_title);
        actionBar.setDisplayHomeAsUpEnabled(true);
      }
    }
  }

  @Override
  public View onCreateView(@NonNull LayoutInflater inflater,
                           ViewGroup container,
                           Bundle savedInstanceState)
  {
    // Inflate the layout for this fragment
    return inflater.inflate(R.layout.fragment_controller_colorpicker, container, false);
  }

  @Override
  public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
  {
    super.onViewCreated(view, savedInstanceState);

    // UI
    _rgbColorView = view.findViewById(R.id.rgbColorView);
    _rgbTextView = view.findViewById(R.id.rgbTextView);

    Button sendButton = view.findViewById(R.id.sendButton);
    sendButton.setOnClickListener(view1 ->
                                  {
                                    // Set the old color
                                    _colorPicker.setOldCenterColor(_selectedColor);
                                    _listener.onSendColorComponents(_selectedColor);
                                  });

    SaturationBar mSaturationBar = view.findViewById(R.id.brightnessbar);
    ValueBar mValueBar = view.findViewById(R.id.valuebar);

    _colorPicker = view.findViewById(R.id.colorPicker);

    if (_colorPicker != null)
    {
      _colorPicker.addSaturationBar(mSaturationBar);
      _colorPicker.addValueBar(mValueBar);
      _colorPicker.setOnColorChangedListener(this);
    }

    final Context context = getContext();
    if (context != null && kPersistValues)
    {
      SharedPreferences preferences = context.getSharedPreferences(kPreferences,
                                                                   Context.MODE_PRIVATE);
      _selectedColor = preferences.getInt(kPreferences_color, kFirstTimeColor);
    }
    else
    {
      _selectedColor = kFirstTimeColor;
    }

    _colorPicker.setOldCenterColor(_selectedColor);
    _colorPicker.setColor(_selectedColor);

    onColorChanged(_selectedColor);
  }

  @Override
  public void onStop()
  {

    final Context context = getContext();
    // Preserve values
    if (context != null && kPersistValues)
    {
      SharedPreferences settings = context.getSharedPreferences(kPreferences, Context.MODE_PRIVATE);
      SharedPreferences.Editor editor = settings.edit();
      editor.putInt(kPreferences_color, _selectedColor);
      editor.apply();
    }

    super.onStop();
  }

  @Override
  public void onAttach(Context context)
  {
    super.onAttach(context);
    if (context instanceof NeopixelColorPickerFragment.NeopixelColorPickerFragmentListener)
    {
      _listener = (ControllerColorPickerFragmentListener) context;
    }
    else if (getTargetFragment() instanceof ControllerColorPickerFragmentListener)
    {
      _listener = (ControllerColorPickerFragmentListener) getTargetFragment();
    }
    else
    {
      throw new RuntimeException(context.toString() + " must implement NeopixelColorPickerFragmentListener");
    }
  }

  @Override
  public void onDetach()
  {
    super.onDetach();
    _listener = null;
  }

  @Override
  public void onDestroy()
  {
    if (BuildConfig.DEBUG && getActivity() != null)
    {
      RefWatcher refWatcher = BluefruitApplication.getRefWatcher(getActivity());
      refWatcher.watch(this);
    }

    super.onDestroy();
  }

  @Override
  public void onCreateOptionsMenu(Menu menu, MenuInflater inflater)
  {
    super.onCreateOptionsMenu(menu, inflater);
    inflater.inflate(R.menu.menu_help, menu);
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item)
  {
    FragmentActivity activity = getActivity();

    switch (item.getItemId())
    {
      case R.id.action_help:
        if (activity != null)
        {
          FragmentManager fragmentManager = activity.getSupportFragmentManager();
          if (fragmentManager != null)
          {
            CommonHelpFragment helpFragment = CommonHelpFragment.newInstance(getString(R.string.colorpicker_help_title),
                                                                             getString(R.string.colorpicker_help_text));
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction().replace(R.id.contentLayout,
                                                                                                 helpFragment,
                                                                                                 "Help");
            fragmentTransaction.addToBackStack(null);
            fragmentTransaction.commit();
          }
        }
        return true;

      default:
        return super.onOptionsItemSelected(item);
    }
  }

  // endregion

  // region OnColorChangedListener

  @SuppressWarnings("PointlessBitwiseExpression")
  @Override
  public void onColorChanged(int color)
  {
    // Save selected color
    _selectedColor = color;

    // Update UI
    _rgbColorView.setBackgroundColor(color);

    final int r = (color >> 16) & 0xFF;
    final int g = (color >> 8) & 0xFF;
    final int b = (color >> 0) & 0xFF;
    final String text = String.format(getString(R.string.colorpicker_rgb_format), r, g, b);
    _rgbTextView.setText(text);
  }

  // endregion

  interface ControllerColorPickerFragmentListener
  {
    void onSendColorComponents(int color);
  }
}
