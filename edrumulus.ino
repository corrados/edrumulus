
#define USE_MIDI

#ifdef USE_MIDI
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

const int      analogPin = 34;
volatile int   iCnt = 0;
//volatile int   outCnt = 0;
const int      iNumSamples = 1500;
volatile float values[iNumSamples];
//volatile bool  sending = false;
//hw_timer_t*    timer = NULL;
//portMUX_TYPE   timerMux = portMUX_INITIALIZER_UNLOCKED;


const int Fs           = 8000;
const int hil_filt_len = 7;
float hil_hist[hil_filt_len]; // memory allocation for hilbert filter history
const float a_re[7] = { -0.037749783581601f, -0.069256807147465f, -1.443799477299919f,  2.473967088799056f,
                         0.551482327389238f, -0.224119735833791f, -0.011665324660691f };
const float a_im[7] = {  0.0f,                0.213150535195075f, -1.048981722170302f, -1.797442302898130f,
                         1.697288080048948f,  0.0f,                0.035902177664014f };
const int energy_window_len = static_cast<int> ( round ( 2e-3f * Fs ) ); // scan time (e.g. 2 ms)
float mov_av_hist_re[energy_window_len]; // real part memory for moving average filter history
float mov_av_hist_im[energy_window_len]; // imaginary part memory for moving average filter history
const int   mask_time             = round ( 8.125e-3f * Fs ); // mask time (e.g. 8.125 ms)
int         mask_back_cnt         = 0;
const float threshold             = pow ( 10.0f, -64.0f / 20 ); // -64 dB threshold
bool        was_above_threshold   = false;
float       prev_hil_filt_val     = 0.0f;
float       prev_hil_filt_new_val = 0.0f;
const float decay_att             = pow ( 10.0f, -1.0f / 20 ); // decay attenuation of 1 dB
const int   decay_len             = round ( 0.2f * Fs ); // decay time (e.g. 200 ms)
const float decay_grad            = 200.0f / Fs; // decay gradient factor
float       decay[decay_len]; // note that the decay is calculated in the setup() function
int         decay_back_cnt        = 0;
float       decay_scaling         = 1.0f;

/*
void onTimer()
{
  if ( !sending )
  {
    portENTER_CRITICAL_ISR ( &timerMux );
    if ( iCnt < iNumSamples )
    {
      const int sample = analogRead ( analogPin );

      if ( ( iCnt >= 0 ) && ( iCnt < iNumSamples ) )
      {

//process_sample ( 0.5f );
        
        //values[iCnt] = static_cast<float> ( sample );
      }

      //values[iCnt] = process_sample ( sample );
      //Serial.println ( iCnt );
      iCnt++;
    }
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
}
*/
 
void setup()
{
#ifdef USE_MIDI
  MIDI.begin();
  Serial.begin ( 38400 );
#else
  Serial.begin ( 115200 );
#endif

  // calculate the decay curve
  for ( int i = 0; i < decay_len; i++ )
  {
    decay[i] = pow ( 10.0f, -i / 20.0f * decay_grad );
  }

/*
  timer = timerBegin   ( 0, 80, true );
  timerAttachInterrupt ( timer, &onTimer, true );
  timerAlarmWrite      ( timer, 125, true );
  timerAlarmEnable     ( timer );
*/
}

int iHitCnt = 0;

void loop()
{


float sample = analogRead ( analogPin );
sample -= 1850; // compensate DC offset
sample /= 30000; // scaling
int midi_velocity;
float debug;
const bool peak_found = process_sample ( sample, midi_velocity, debug );
values[iCnt++] = micros();//sample;//processed_sample;//

// measurement: Hilbert+moving average: about 54 kHz sampling rate possible
delayMicroseconds ( 107 ); // to get from 56 kHz to 8 kHz sampling rate

#ifdef USE_MIDI
if ( peak_found )
{
    MIDI.sendNoteOn ( 38, midi_velocity, 10 ); // (note, velocity, channel)
    MIDI.sendNoteOff ( 38, 0, 10 );
}
#endif

/*
if ( peak_found )
{
  Serial.print ( "peak_found " );
  Serial.print ( midi_velocity ); Serial.print ( "    " );
//Serial.print ( debug, 7 ); Serial.print ( "    " );
//Serial.println ( decay[iHitCnt++], 7 );
//if ( iHitCnt == decay_len ) iHitCnt = 0;
  Serial.println ( iHitCnt++ );
}
*/


/*
  if ( Serial.available() > 0 )
  {
    // for debugging: take samples from Octave, process and return result to Octave
    const float fIn = Serial.parseFloat();
    float debug;
    const bool peak_found = process_sample ( fIn, debug );
    Serial.println ( debug, 7 );
  }
*/

/*
if ( iCnt >= iNumSamples )
{
  // for usage with debugcontroller.m
  for ( int i = 0; i < iNumSamples; i++ )
  {
    Serial.println ( values[i] );
  }
  iCnt = 0;
}
*/


//Serial.println ( iHitCnt++ ); //processed_sample );

/*
  if ( iCnt >= iNumSamples )
  {
    sending = true;
    
    portENTER_CRITICAL(&timerMux);
    
    Serial.println ( values[outCnt] );
    outCnt++;

    if ( outCnt >= iNumSamples )
    {
      outCnt  = 0;
      iCnt    = 0;
      sending = false;
    }
    
    portEXIT_CRITICAL(&timerMux);
  }
*/
}

bool process_sample ( const float fIn,
                      int&        midi_velocity,
                      float&      debug )
{
  // initialize return parameter
  bool peak_found = false;

debug = 0.0f; // TEST


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


  // exponential decay assumption (note that we must not use hil_filt_org since a
  // previous peak might not be faded out and the peak detection works on hil_filt)
  // subtract decay (with clipping at zero)
  float hil_filt_new = hil_filt;

  if ( decay_back_cnt > 0 )
  {
    const float cur_decay = decay_scaling * decay[decay_len - decay_back_cnt];

// debug = cur_decay; // TEST

    hil_filt_new          = hil_filt - cur_decay;
    decay_back_cnt--;

    if ( hil_filt_new < 0.0f )
    {
      hil_filt_new = 0.0f;
    }
  }


  // threshold test
  if ( ( ( hil_filt_new > threshold ) || was_above_threshold ) && ( mask_back_cnt == 0 ) )
  {
    was_above_threshold = true;

    // climb to the maximum of the current peak
    if ( prev_hil_filt_new_val < hil_filt_new )
    {
      prev_hil_filt_new_val = hil_filt_new;
      prev_hil_filt_val     = hil_filt; // needed for further processing
    }
    else
    {
      // maximum found
      prev_hil_filt_new_val = 0.0f;
      was_above_threshold   = false;
      decay_back_cnt        = decay_len;
      decay_scaling         = prev_hil_filt_val * decay_att;
      mask_back_cnt         = mask_time;
      peak_found            = true;
    }
  }

  if ( mask_back_cnt > 0 )
  {
    mask_back_cnt--;
  }


  // Calculate positional sensing -------------------------------------------------

// TODO


// TEST
debug = hil_filt_new;
//debug = peak_found;


// TEST
// velocity/positional sensing mapping and play MIDI notes
midi_velocity = static_cast<int> ( ( 20 * log10 ( prev_hil_filt_val ) + 63.0f ) / 40 * 127 );
midi_velocity = max ( 1, min ( 127, midi_velocity ) );


  return peak_found;
}
