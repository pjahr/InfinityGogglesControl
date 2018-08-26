package com.adafruit.bluefruit.le.connect.app;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
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
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;

import com.adafruit.bluefruit.le.connect.BluefruitApplication;
import com.adafruit.bluefruit.le.connect.BuildConfig;
import com.adafruit.bluefruit.le.connect.R;
import com.squareup.leakcanary.RefWatcher;

public class ControllerPadFragment extends Fragment
{
  private final static String TAG = ControllerPadFragment.class.getSimpleName();

  // Config
  private final static float MinAspectRatio = 1.8f;

  // UI TextBuffer (refreshing the text buffer is managed with a timer because a lot of changes can arrive really fast and could stall the main thread)
  private Handler _uiRefreshTimerHandler = new Handler();
  private Runnable _uiRefreshTimerRunnable = new Runnable()
  {
    @Override
    public void run()
    {
      if (_isUiTimerRunning)
      {
        updateTextDataUI();
        // Log.d(TAG, "updateDataUI");
        _uiRefreshTimerHandler.postDelayed(this, 200);
      }
    }
  };
  private boolean _isUiTimerRunning = false;

  // UI
  private ViewGroup _contentView;
  private ViewGroup _rootLayout;

  // Data
  private ControllerPadFragmentListener _listener;
  private volatile StringBuilder _dataBuffer = new StringBuilder();
  private volatile StringBuilder _textSpanBuffer = new StringBuilder();
  private int _maxPacketsToPaintAsText;

  View.OnTouchListener _padButtonTouchListener = new View.OnTouchListener()
  {
    @Override
    public boolean onTouch(View view, MotionEvent event)
    {
      final int tag = Integer.valueOf((String) view.getTag());
      if (event.getAction() == MotionEvent.ACTION_DOWN)
      {
        view.setPressed(true);
        _listener.onSendControllerPadButtonStatus(tag, true);
        return true;
      }
      return false;
    }
  };

  // region Lifecycle
  @SuppressWarnings("UnnecessaryLocalVariable")
  public static ControllerPadFragment newInstance()
  {
    ControllerPadFragment fragment = new ControllerPadFragment();
    return fragment;
  }

  public ControllerPadFragment()
  {
    // Required empty public constructor
  }

  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);

    // Retain this fragment across configuration changes
    setRetainInstance(true);
  }

  @Override
  public View onCreateView(@NonNull LayoutInflater inflater,
                           ViewGroup container,
                           Bundle savedInstanceState)
  {
    // Inflate the layout for this fragment
    return inflater.inflate(R.layout.fragment_controller_pad, container, false);
  }

  @Override
  public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState)
  {
    super.onViewCreated(view, savedInstanceState);

    // Set title
    AppCompatActivity activity = (AppCompatActivity) getActivity();
    if (activity != null)
    {
      ActionBar actionBar = activity.getSupportActionBar();
      if (actionBar != null)
      {
        actionBar.setTitle(R.string.controlpad_title);
        actionBar.setDisplayHomeAsUpEnabled(true);
      }
    }

    // UI
    _rootLayout = view.findViewById(R.id.rootLayout);

    Button buttonA0  = view.findViewById(R.id.button0);
    Button buttonA1  = view.findViewById(R.id.button1);
    Button buttonA2  = view.findViewById(R.id.button2);
    Button buttonA3  = view.findViewById(R.id.button3);
    Button buttonA4  = view.findViewById(R.id.button4);
    Button buttonA5  = view.findViewById(R.id.button5);
    Button buttonA6  = view.findViewById(R.id.button6);
    Button buttonA7  = view.findViewById(R.id.button7);
    Button buttonA8  = view.findViewById(R.id.button8);
    Button buttonA9  = view.findViewById(R.id.button9);
    Button buttonA10 = view.findViewById(R.id.button10);
    Button buttonA11 = view.findViewById(R.id.button11);
    Button buttonA12 = view.findViewById(R.id.button12);
    Button buttonA13 = view.findViewById(R.id.button13);
    Button buttonA14 = view.findViewById(R.id.button14);
    Button buttonA15 = view.findViewById(R.id.button15);
    Button buttonA16 = view.findViewById(R.id.button16);
    Button buttonA17 = view.findViewById(R.id.button17);
    Button buttonA18 = view.findViewById(R.id.button18);
    Button buttonA19 = view.findViewById(R.id.button19);
    Button buttonA20 = view.findViewById(R.id.button20);

    buttonA0.setOnTouchListener(_padButtonTouchListener);
    buttonA1.setOnTouchListener(_padButtonTouchListener);
    buttonA2.setOnTouchListener(_padButtonTouchListener);
    buttonA3.setOnTouchListener(_padButtonTouchListener);
    buttonA4.setOnTouchListener(_padButtonTouchListener);
    buttonA5.setOnTouchListener(_padButtonTouchListener);
    buttonA6.setOnTouchListener(_padButtonTouchListener);
    buttonA7.setOnTouchListener(_padButtonTouchListener);
    buttonA8.setOnTouchListener(_padButtonTouchListener);
    buttonA9.setOnTouchListener(_padButtonTouchListener);
    buttonA10.setOnTouchListener(_padButtonTouchListener);
    buttonA11.setOnTouchListener(_padButtonTouchListener);
    buttonA12.setOnTouchListener(_padButtonTouchListener);
    buttonA13.setOnTouchListener(_padButtonTouchListener);
    buttonA14.setOnTouchListener(_padButtonTouchListener);
    buttonA15.setOnTouchListener(_padButtonTouchListener);
    buttonA16.setOnTouchListener(_padButtonTouchListener);
    buttonA17.setOnTouchListener(_padButtonTouchListener);
    buttonA18.setOnTouchListener(_padButtonTouchListener);
    buttonA19.setOnTouchListener(_padButtonTouchListener);
    buttonA20.setOnTouchListener(_padButtonTouchListener);


    // Read shared preferences
    _maxPacketsToPaintAsText = UartBaseFragment.kDefaultMaxPacketsToPaintAsText; //PreferencesFragment.getUartTextMaxPackets(this);
  }

  @Override
  public void onAttach(Context context)
  {
    super.onAttach(context);
    if (context instanceof ControllerPadFragmentListener)
    {
      _listener = (ControllerPadFragmentListener) context;
    }
    else if (getTargetFragment() instanceof ControllerPadFragmentListener)
    {
      _listener = (ControllerPadFragmentListener) getTargetFragment();
    }
    else
    {
      throw new RuntimeException(context.toString() + " must implement ControllerPadFragmentListener");
    }
  }

  @Override
  public void onDetach()
  {
    super.onDetach();
    _listener = null;
  }

  @Override
  public void onResume()
  {
    super.onResume();

    ViewTreeObserver observer = _rootLayout.getViewTreeObserver();
    observer.addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener()
    {
      @Override
      public void onGlobalLayout()
      {
        _rootLayout.getViewTreeObserver().removeOnGlobalLayoutListener(this);
      }
    });

    // Refresh timer
    _isUiTimerRunning = true;
    _uiRefreshTimerHandler.postDelayed(_uiRefreshTimerRunnable, 0);
  }

  @Override
  public void onPause()
  {
    _isUiTimerRunning = false;
    _uiRefreshTimerHandler.removeCallbacksAndMessages(_uiRefreshTimerRunnable);

    super.onPause();
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
            CommonHelpFragment helpFragment = CommonHelpFragment.newInstance(getString(R.string.controlpad_help_title),
                                                                             getString(R.string.controlpad_help_text));
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

  // region UI

  public synchronized void addText(String text)
  {
    _dataBuffer.append(text);
  }


  private int _dataBufferLastSize = 0;

  private synchronized void updateTextDataUI()
  {

    final int bufferSize = _dataBuffer.length();
    if (_dataBufferLastSize != bufferSize)
    {

      if (bufferSize > _maxPacketsToPaintAsText)
      {
        _dataBufferLastSize = bufferSize - _maxPacketsToPaintAsText;
        _textSpanBuffer.setLength(0);
        _textSpanBuffer.append(getString(R.string.uart_text_dataomitted)).append("\n");
        _dataBuffer.replace(0, _dataBufferLastSize, "");
        _textSpanBuffer.append(_dataBuffer);

      }
      else
      {
        _textSpanBuffer.append(_dataBuffer.substring(_dataBufferLastSize, bufferSize));
      }

      _dataBufferLastSize = _dataBuffer.length();
    }
  }
  // endregion

  // region ControllerPadFragmentListener
  public interface ControllerPadFragmentListener
  {
    void onSendControllerPadButtonStatus(int tag, boolean isPressed);
  }
  // endregion
}
