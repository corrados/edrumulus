

/*
int analogPin = 34;
volatile int value;
volatile int iCnt = 0;
const int iNumSamples = 20000;
volatile int values[iNumSamples];
volatile bool sending = false;
int outCnt = 0;
hw_timer_t*  timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
*/

const int Fs           = 8000;
const int hil_filt_len = 7;
float hil_hist[hil_filt_len]; // memory allocation for hilbert filter history
float a_re[7] = { -0.037749783581601f, -0.069256807147465f, -1.443799477299919f, 2.473967088799056f,
                   0.551482327389238f, -0.224119735833791f, -0.011665324660691f };
float a_im[7] = {  0.0f,                0.213150535195075f, -1.048981722170302f, -1.797442302898130f,
                   1.697288080048948f,  0.0f,                0.035902177664014f };
const int energy_window_len = static_cast<int> ( round ( 2e-3f * Fs ) ); // scan time (e.g. 2 ms)
float mov_av_hist_re[energy_window_len]; // real part memory for moving average filter history
float mov_av_hist_im[energy_window_len]; // imaginary part memory for moving average filter history


/*
void onTimer()
{
  if ( !sending )
  {
    portENTER_CRITICAL_ISR(&timerMux);
    if ( iCnt < iNumSamples )
    {
      values[iCnt] = analogRead ( analogPin );
      //Serial.println ( iCnt );
      iCnt++;
    }
    portEXIT_CRITICAL_ISR(&timerMux);
  }
}
*/
 
void setup()
{
  Serial.begin ( 115200 );

/*
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 125, true);
  timerAlarmEnable(timer);
*/
}
 
void loop()
{
  if ( Serial.available() > 0 )
  {
    const float fIn = Serial.parseFloat();


    // Calculate peak detection -----------------------------------------------------
    // hilbert filter
    for ( int i = 0; i < hil_filt_len - 1; i++ )
    {
      hil_hist[i] = hil_hist[i + 1];
    }
    hil_hist[hil_filt_len - 1] = fIn;

    float hil_re = 0;
    float hil_im = 0;
    for ( int i = 0; i < hil_filt_len; i++ )
    {
      hil_re += hil_hist[i] * a_re[i];
      hil_im += hil_hist[i] * a_im[i];
    }

    // moving average filter
    for ( int i = 0; i < energy_window_len - 1; i++ )
    {
      mov_av_hist_re[i] = mov_av_hist_re[i + 1];
      mov_av_hist_im[i] = mov_av_hist_im[i + 1];
    }
    mov_av_hist_re[energy_window_len - 1] = hil_re;
    mov_av_hist_im[energy_window_len - 1] = hil_im;

    float mov_av_re = 0;
    float mov_av_im = 0;
    for ( int i = 0; i < energy_window_len; i++ )
    {
      mov_av_re += mov_av_hist_re[i];
      mov_av_im += mov_av_hist_im[i];
    }
    mov_av_re /= energy_window_len;
    mov_av_im /= energy_window_len;

    const float hil_filt = sqrt ( mov_av_re * mov_av_re + mov_av_im * mov_av_im );

    Serial.println ( hil_filt, 7 );

//    Serial.println ( hil_re, 7 );
//    Serial.println ( hil_im, 7 );
  }



}
