/******************************************************************************\
 * Copyright (c) 2020-2024
 * Author(s): Volker Fischer
 ******************************************************************************
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
\******************************************************************************/

#include "edrumulus_signalprocessing.h"
#include "edrumulus.h"


// Pad -------------------------------------------------------------------------
void Edrumulus::Pad::overload_correction ( FastWriteFIFO& x_sq_hist,
                                           FastWriteFIFO& overload_hist,
                                           const int      first_peak_idx,
                                           const int      peak_velocity_idx,
                                           bool&          is_overloaded_state,
                                           float&         peak_val )
{
  // if the first peak is overloaded, use this position as the maximum peak
  int       peak_velocity_idx_ovhist                    = peak_velocity_idx;
  const int first_peak_velocity_idx_in_overload_history = overload_hist_len - total_scan_time + first_peak_idx;

  if ( overload_hist[first_peak_velocity_idx_in_overload_history] > 0.0f )
  {
    // overwrite peak value and index in history
    peak_val                 = x_sq_hist[x_sq_hist_len - total_scan_time + first_peak_idx];
    peak_velocity_idx_ovhist = scan_time - x_sq_hist_len + first_peak_idx;
  }

  float     right_neighbor, left_neighbor;
  const int peak_velocity_idx_in_overload_history = overload_hist_len - scan_time + peak_velocity_idx_ovhist;
  const int peak_velocity_idx_in_x_sq_hist        = x_sq_hist_len - scan_time + peak_velocity_idx_ovhist;
  int       number_overloaded_samples             = 1; // we check for overload history at peak position is > 0 below -> start with one
  bool      left_neighbor_ok                      = true; // initialize with ok
  bool      right_neighbor_ok                     = true; // initialize with ok

  // check overload status and correct the peak if necessary
  if ( overload_hist[peak_velocity_idx_in_overload_history] > 0.0f )
  {
    // NOTE: the static_cast<int> is a workaround for the ESP32 compiler issue: "unknown opcode or format name 'lsiu'"
    // run to the right to find same overloads
    int cur_idx      = peak_velocity_idx_in_overload_history;
    int cur_idx_x_sq = peak_velocity_idx_in_x_sq_hist;
    while ( ( cur_idx < overload_hist_len - 1 ) && ( static_cast<int> ( overload_hist[cur_idx] ) == static_cast<int> ( overload_hist[cur_idx + 1] ) ) )
    {
      cur_idx++;
      cur_idx_x_sq++;
      number_overloaded_samples++;
    }
    if ( cur_idx_x_sq + 1 < x_sq_hist_len )
    {
      right_neighbor = x_sq_hist[cur_idx_x_sq + 1];
    }
    else
    {
      right_neighbor_ok = false;
    }

    // run to the left to find same overloads
    cur_idx      = peak_velocity_idx_in_overload_history;
    cur_idx_x_sq = peak_velocity_idx_in_x_sq_hist;
    while ( ( cur_idx > 1 ) && ( static_cast<int> ( overload_hist[cur_idx] ) == static_cast<int> ( overload_hist[cur_idx - 1] ) ) )
    {
      cur_idx--;
      cur_idx_x_sq--;
      number_overloaded_samples++;
    }
    if ( cur_idx_x_sq - 1 >= 0 )
    {
      left_neighbor = x_sq_hist[cur_idx_x_sq - 1];
    }
    else
    {
      left_neighbor_ok = false;
    }

    is_overloaded_state = ( number_overloaded_samples > max_num_overloads );

    // clipping compensation (see tools/misc/clipping_compensation.m)
    const float peak_val_sqrt = sqrt ( peak_val );
    float       mean_neighbor = peak_val_sqrt; // if no neighbor can be calculated, use safest value, i.e., lowest resulting correction            

    if ( left_neighbor_ok && right_neighbor_ok )
    {
      mean_neighbor = ( sqrt ( left_neighbor ) + sqrt ( right_neighbor ) ) / 2.0f;
    }
    else if ( left_neighbor_ok )
    {
      mean_neighbor = sqrt ( left_neighbor ); // only left neighbor available
    }
    else if ( right_neighbor_ok )
    {
      mean_neighbor = sqrt ( right_neighbor ); // only right neighbor available
    }

    const float a_low                      = amplification_mapping[min ( length_ampmap - 2, number_overloaded_samples )];
    const float a_high                     = amplification_mapping[min ( length_ampmap - 1, number_overloaded_samples + 1 )];
    const float a_diff                     = a_high - a_low;
    const float a_diff_abs                 = a_diff * peak_val_sqrt / a_low;
    float       neighbor_to_limit_abs      = mean_neighbor - ( peak_val_sqrt - a_diff_abs );
    neighbor_to_limit_abs                  = max ( 0.0f, min ( a_diff_abs, neighbor_to_limit_abs ) );
    const float amplification_compensation = a_low + neighbor_to_limit_abs / a_diff_abs * a_diff;
    peak_val                              *= amplification_compensation * amplification_compensation;
/*
String overload_string = "";
for ( int ov_cnt = 0; ov_cnt < overload_hist_len; ov_cnt++ )
{
overload_string += String ( overload_hist[ov_cnt] ) + " ";
}
Serial.println ( overload_string );
Serial.println ( String ( peak_velocity_idx_in_x_sq_hist ) + " " +
                 String ( x_sq_hist_len ) + " " + String ( peak_velocity_idx_in_overload_history ) + " " +
                 String ( overload_hist_len ) + " " + String ( first_peak_idx ) );
Serial.println ( String ( sqrt ( left_neighbor ) ) + " " + String ( sqrt ( right_neighbor ) ) + " " +
                 String ( number_overloaded_samples ) + " " + String ( mean_neighbor ) + " " +
                 String ( mean_neighbor - ( peak_val_sqrt - a_diff_abs ) ) + " " + String ( neighbor_to_limit_abs ) + " " +
                 String ( amplification_compensation ) + " " + String ( sqrt ( peak_val ) ) );
*/
  }
}


// Multiple head sensor management ---------------------------------------------
void Edrumulus::Pad::MultiHeadSensor::initialize()
{
  multiple_sensor_cnt = 0;

  // pre-calculate equations needed for 3 sensor get position function
  get_pos_x0 =  0.433f; get_pos_y0 =  0.25f; // sensor 0 position
  get_pos_x1 =  0.0;    get_pos_y1 = -0.5f;  // sensor 1 position
  get_pos_x2 = -0.433f; get_pos_y2 =  0.25f; // sensor 2 position
  get_pos_rim_radius       = 0.75f;          // rim radius
  get_pos_x0_sq_plus_y0_sq = get_pos_x0 * get_pos_x0 + get_pos_y0 * get_pos_y0;
  get_pos_a1               = 2 * ( get_pos_x0 - get_pos_x1 );
  get_pos_b1               = 2 * ( get_pos_y0 - get_pos_y1 );
  get_pos_a2               = 2 * ( get_pos_x0 - get_pos_x2 );
  get_pos_b2               = 2 * ( get_pos_y0 - get_pos_y2 );
  get_pos_div1_fact        = 1.0f / ( get_pos_a1 * get_pos_b2 - get_pos_a2 * get_pos_b1 );
  get_pos_div2_fact        = 1.0f / ( get_pos_a2 * get_pos_b1 - get_pos_a1 * get_pos_b2 );
}


void Edrumulus::Pad::MultiHeadSensor::calculate_subsample_peak_value ( FastWriteFIFO& x_sq_hist,
                                                                       const int      x_sq_hist_len,
                                                                       const int      total_scan_time,
                                                                       const int      first_peak_idx,
                                                                       float&         first_peak_sub_sample )
{
  // calculate sub-sample first peak value using simplified metric:
  // m = (x_sq[2] - x_sq[0]) / (x_sq[1] - x_sq[0]) -> sub_sample = m * m / 2
  first_peak_sub_sample = 0.0; // in case no sub-sample value can be calculated
  const int cur_index   = x_sq_hist_len - total_scan_time + first_peak_idx;

  if ( ( cur_index > 0 ) && ( cur_index < x_sq_hist_len - 1 ) )
  {
    if ( x_sq_hist[cur_index - 1] > x_sq_hist[cur_index + 1] )
    {
      // sample left of main peak is bigger than right sample
      const float sub_sample_metric = ( x_sq_hist[cur_index - 1] - x_sq_hist[cur_index + 1] ) /
                                      ( x_sq_hist[cur_index]     - x_sq_hist[cur_index + 1] );

      first_peak_sub_sample = sub_sample_metric * sub_sample_metric / 2;
    }
    else
    {
      // sample right of main peak is bigger than left sample
      const float sub_sample_metric = ( x_sq_hist[cur_index + 1] - x_sq_hist[cur_index - 1] ) /
                                      ( x_sq_hist[cur_index]     - x_sq_hist[cur_index - 1] );

      first_peak_sub_sample = -sub_sample_metric * sub_sample_metric / 2;
    }
  }
}


void Edrumulus::Pad::MultiHeadSensor::calculate ( SSensor*   sSensor,
                                                  const bool sensor0_has_results,
                                                  const int  number_head_sensors,
                                                  const int  pos_sensitivity,
                                                  const int  pos_threshold,
                                                  bool&      peak_found,
                                                  int&       midi_velocity,
                                                  int&       midi_pos,
                                                  Erimstate& rim_state )
{

// TODO do not use hard coded "17" at the three places here but define a pad specific value and use that instead
//      -> use that value also for definition of max_sensor_sample_diff
const int sensor_distance_factor = 17;
//
// TODO put number somewhere else
const int max_sensor_sample_diff = 20; // 2.5 ms at 8 kHz sampling rate
//
// TODO calculate phase and return it with a special MIDI command
//
// TODO implement positional sensing if only two head sensor peaks are available

  // start condition of delay process to query all head sensor results
  if ( sensor0_has_results && ( multiple_sensor_cnt == 0 ) )
  {
    multiple_sensor_cnt = max_sensor_sample_diff;
  }

  // special case with multiple head sensors
  if ( multiple_sensor_cnt > 0 )
  {
    multiple_sensor_cnt--;

    // end condition
    if ( multiple_sensor_cnt == 0 )
    {

// TODO quick hack tests
int number_sensors_with_results      = 0;
int head_sensor_idx_highest_velocity = 0;
int max_velocity                     = 0;
int velocity_sum                     = 0;
int sensor0_first_peak_delay         = sSensor[0].sResults.first_peak_delay;

for ( int head_sensor_cnt = 1; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ ) // do not use sensor 0
{
if ( abs ( sSensor[head_sensor_cnt].sResults.first_peak_delay - sensor0_first_peak_delay ) < max_sensor_sample_diff )
{
  number_sensors_with_results++;
  velocity_sum += sSensor[head_sensor_cnt].sResults.midi_velocity;

  if ( sSensor[head_sensor_cnt].sResults.midi_velocity > max_velocity )
  {
    max_velocity                     = sSensor[head_sensor_cnt].sResults.midi_velocity;
    head_sensor_idx_highest_velocity = head_sensor_cnt;
  }
}
}

      if ( number_sensors_with_results == 3 )
      {
        // calculate time delay differences
        const float diff_1_0 = -( ( sSensor[2].sResults.first_peak_delay + sSensor[2].sResults.first_peak_sub_sample ) -
                                  ( sSensor[1].sResults.first_peak_delay + sSensor[1].sResults.first_peak_sub_sample ) );
        const float diff_2_0 = -( ( sSensor[3].sResults.first_peak_delay + sSensor[3].sResults.first_peak_sub_sample ) -
                                  ( sSensor[1].sResults.first_peak_delay + sSensor[1].sResults.first_peak_sub_sample ) );

        // get_position function from pos_det.py
        // see: https://math.stackexchange.com/questions/3373011/how-to-solve-this-system-of-hyperbola-equations
        // and discussion post of jstma: https://github.com/corrados/edrumulus/discussions/70#discussioncomment-4014893
        const float r1      = diff_1_0 / sensor_distance_factor;
        const float r2      = diff_2_0 / sensor_distance_factor;
        const float c1      = r1 * r1 + get_pos_x0_sq_plus_y0_sq - get_pos_x1 * get_pos_x1 - get_pos_y1 * get_pos_y1;
        const float c2      = r2 * r2 + get_pos_x0_sq_plus_y0_sq - get_pos_x2 * get_pos_x2 - get_pos_y2 * get_pos_y2;
        const float d1      = ( 2 * r1 * get_pos_b2 - 2 * r2 * get_pos_b1 ) * get_pos_div1_fact;
        const float e1      = (     c1 * get_pos_b2 -     c2 * get_pos_b1 ) * get_pos_div1_fact;
        const float d2      = ( 2 * r1 * get_pos_a2 - 2 * r2 * get_pos_a1 ) * get_pos_div2_fact;
        const float e2      = (     c1 * get_pos_a2 -     c2 * get_pos_a1 ) * get_pos_div2_fact;
        const float d_e1_x0 = e1 - get_pos_x0;
        const float d_e2_y0 = e2 - get_pos_y0;
        const float a       = d1 * d1 + d2 * d2 - 1;
        const float b       = 2 * d_e1_x0 * d1 + 2 * d_e2_y0 * d2;
        const float c       = d_e1_x0 * d_e1_x0 + d_e2_y0 * d_e2_y0;

        // two solutions to the quadratic equation, only one solution seems to always be correct
        const float r_2 = ( -b - sqrt ( b * b - 4 * a * c ) ) / ( 2 * a );
        const float x   = d1 * r_2 + e1;
        const float y   = d2 * r_2 + e2;
        float       r   = sqrt ( x * x + y * y );

// TEST
//Serial.println ( String ( x ) + "," + String ( y ) + ",1000.0," );

        // clip calculated radius to rim radius
        if ( ( r > get_pos_rim_radius ) || ( isnan ( r ) ) )
        {
          r = get_pos_rim_radius;
        }
        const int max_abs_diff = r * sensor_distance_factor;

// TEST use maximum offset for middle from each sensor pair
//const int diff_2_1 = -( ( sSensor[3].sResults.first_peak_delay + sSensor[3].sResults.first_peak_sub_sample ) -
//                        ( sSensor[2].sResults.first_peak_delay + sSensor[2].sResults.first_peak_sub_sample ) );
//Serial.println ( String ( diff_1_0 ) + "," + String ( diff_2_0 ) + "," + String ( diff_2_1 ) + "," );
//const int max_abs_diff = ( max ( max ( abs ( diff_1_0 ), abs ( diff_2_0 ) ), abs ( diff_2_1 ) ) );

        midi_pos = min ( 127, max ( 0, pos_sensitivity * ( max_abs_diff - pos_threshold ) ) );

        // use average MIDI velocity
        midi_velocity = velocity_sum / number_sensors_with_results;

//rim_state = sSensor[head_sensor_idx_highest_velocity].sResults.rim_state;
// TEST use second highest velocity sensor for rim shot detection
if ( head_sensor_idx_highest_velocity == 1 )
{
if ( sSensor[2].sResults.midi_velocity > sSensor[3].sResults.midi_velocity )
{
  rim_state = sSensor[2].sResults.rim_state;
}
else
{
  rim_state = sSensor[3].sResults.rim_state;
}
}
else if ( head_sensor_idx_highest_velocity == 2 )
{
if ( sSensor[1].sResults.midi_velocity > sSensor[3].sResults.midi_velocity )
{
  rim_state = sSensor[1].sResults.rim_state;
}
else
{
  rim_state = sSensor[3].sResults.rim_state;
}
}
else
{
if ( sSensor[1].sResults.midi_velocity > sSensor[2].sResults.midi_velocity )
{
  rim_state = sSensor[1].sResults.rim_state;
}
else
{
  rim_state = sSensor[2].sResults.rim_state;
}
}

}
else if ( ( number_sensors_with_results == 2 ) || ( number_sensors_with_results == 1 ) )
{

// TODO
midi_pos = 0;

// TEST use average MIDI velocity
midi_velocity = velocity_sum / number_sensors_with_results;
rim_state     = sSensor[head_sensor_idx_highest_velocity].sResults.rim_state;

}
else
{

// TODO
midi_pos = 0;

// TEST
midi_velocity = sSensor[0].sResults.midi_velocity;
rim_state     = sSensor[0].sResults.rim_state;

}
peak_found = true;


      // reset the first_peak_delay since this is our marker if a peak was in the interval
      for ( int head_sensor_cnt = 1; head_sensor_cnt < number_head_sensors; head_sensor_cnt++ ) // do not use sensor 0
      {
        sSensor[head_sensor_cnt].sResults.first_peak_delay = max_sensor_sample_diff;
      }
    }
  }
}
