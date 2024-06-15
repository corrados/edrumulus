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

#include "hardware.h"

Edrumulus_hardware* edrumulus_hardware_pointer = nullptr;

Edrumulus_hardware::Edrumulus_hardware()
{
  // global pointer to this class needed for static callback function
  edrumulus_hardware_pointer = this;
}

// -----------------------------------------------------------------------------
// Teensy 4.0/4.1/3.6 ----------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef TEENSYDUINO

int Edrumulus_hardware::get_prototype_pins(int** analog_pins,
    int** analog_pins_rimshot,
    int* number_pins,
    int* status_LED_pin)
{
  // clang-format off
  // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
  static int analog_pins1[]         = { A10,   A11,   A12,        A13,       A1,     A6,    A4,    A5 };
  static int analog_pins_rimshot1[] = {  A9,    -1,    A0,         -1,       A3,     A8,    A2,    A7 };
  // clang-format on
  *analog_pins         = analog_pins1;
  *analog_pins_rimshot = analog_pins_rimshot1;
  *number_pins         = sizeof(analog_pins1) / sizeof(int);
  *status_LED_pin      = BOARD_LED_PIN;
  return 0;
}

void Edrumulus_hardware::setup(const int conf_Fs,
    const int number_pads,
    const int number_inputs[],
    int analog_pin[][MAX_NUM_PAD_INPUTS])
{
  // set essential parameters
  Fs = conf_Fs;

  // create linear vectors containing the pin information for each pad and pad-input
  total_number_inputs = 0; // we use it as a counter, too

  for (int i = 0; i < number_pads; i++)
  {
    for (int j = 0; j < number_inputs[i]; j++)
    {
      // store pin number in vector
      input_pin[total_number_inputs] = analog_pin[i][j];
      total_number_inputs++;

#  if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) // Teensy 4.0/4.1 specific code
      // disable MIMXRT1062DVL6A "keeper" on all Teensy 4.0/4.1 ADC input pins
      // NOTE: pinMode() needs absolute pin numbers, e.g. 0 for A0 will not work
      pinMode(analog_pin[i][j], INPUT_DISABLE);
#  endif
    }
  }

  // set the ADC properties: averaging 8 samples with high speed sampling gives
  // us the best compromise between ADC speed and spike protection
  adc_obj.adc0->setResolution(12); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc0->setAveraging(8);
  adc_obj.adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  adc_obj.adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  adc_obj.adc1->setResolution(12); // we want to get the full ADC resolution of the Teensy 4.0
  adc_obj.adc1->setAveraging(8);
  adc_obj.adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  adc_obj.adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

  // wait for ADC calibration to complete
  adc_obj.adc0->wait_for_cal();
  adc_obj.adc1->wait_for_cal();

  // initialize timer flag (semaphore)
  timer_ready = false;

  // prepare timer at a rate of given sampling rate
  myTimer.begin(on_timer, 1000000 / Fs); // here we define the sampling rate (1 MHz / Fs)
}

void Edrumulus_hardware::write_setting(const int pad_index,
    const int address,
    const byte value)
{
  EEPROM.update(pad_index * MAX_NUM_SET_PER_PAD + address, value);
}

byte Edrumulus_hardware::read_setting(const int pad_index,
    const int address)
{
  return EEPROM.read(pad_index * MAX_NUM_SET_PER_PAD + address);
}

void Edrumulus_hardware::on_timer()
{
  // tell the main loop that a sample can be read by setting the flag (semaphore)
  edrumulus_hardware_pointer->timer_ready = true;
}

void Edrumulus_hardware::capture_samples(const int number_pads,
    const int number_inputs[],
    int analog_pin[][MAX_NUM_PAD_INPUTS],
    int sample_org[][MAX_NUM_PAD_INPUTS])
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  while (!timer_ready) delayMicroseconds(5);
  timer_ready = false; // it is important to reset the flag here

  // read the ADC samples
  for (int i = 0; i < total_number_inputs; i++)
  {
    input_sample[i] = adc_obj.analogRead(input_pin[i]);
  }

  // copy captured samples in pad buffer
  int input_cnt = 0;

  for (int i = 0; i < number_pads; i++)
  {
    for (int j = 0; j < number_inputs[i]; j++)
    {
      sample_org[i][j] = input_sample[input_cnt++];
    }
  }
}

#endif

// -----------------------------------------------------------------------------
// ESP32 Dual Core -------------------------------------------------------------
// -----------------------------------------------------------------------------
#ifdef ESP_PLATFORM

int Edrumulus_hardware::get_prototype_pins(int** analog_pins,
    int** analog_pins_rimshot,
    int* number_pins,
    int* status_LED_pin)
{
#  ifdef CONFIG_IDF_TARGET_ESP32
  // Definition:
  // - Pin 5 is "input enabled, pull-up resistor" -> if read value is 1, we know that we have a
  //   legacy or custom board. Boards which support the identification set this pin to low.
  // - Pin 18, 19, 22, 23 define a 4 bit sequence which identifies the prototype hardware.
  // NOTE: avoid ESP32 GPIO 25/26 for piezo inputs since they are DAC pins which cause an incorrect DC offset
  //       estimation and DC offset drift which makes the spike cancellation algorithm not working correctly
  pinMode(5, INPUT);

  // check support of protoype board identification
  if (digitalRead(5) == 0)
  {
    // read the identification bit field and check the states
    pinMode(18, INPUT);
    const int bit1 = digitalRead(18);
    pinMode(19, INPUT);
    const int bit2 = digitalRead(19);
    pinMode(22, INPUT);
    const int bit3 = digitalRead(22);
    pinMode(23, INPUT);
    const int bit4 = digitalRead(23);

    if ((bit1 == 0) && (bit2 == 0) && (bit3 == 0) && (bit4 == 0))
    {
      // Prototype 5: 0, 0, 0, 0 -----------------------------------------------
      // clang-format off
      // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
      static int analog_pins5[]         = { 12,     2,     33,        4,         34,     15,    35,    27,    32 };
      static int analog_pins_rimshot5[] = { 14,    -1,     26,       -1,         36,     13,    25,    -1,    -1 };
      // clang-format on
      *analog_pins         = analog_pins5;
      *analog_pins_rimshot = analog_pins_rimshot5;
      *number_pins         = sizeof(analog_pins5) / sizeof(int);
      *status_LED_pin      = 21; // LED is connected to IO21 on prototype 5
      return 5;
    }
    else if ((bit1 > 0) && (bit2 == 0) && (bit3 == 0) && (bit4 == 0))
    {
      // Prototype 6: 1, 0, 0, 0 -----------------------------------------------
      // clang-format off
      // analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
      static int analog_pins6[]         = { 36,    33,     32,       25,         34,     39,    27,    12,    15 };
      static int analog_pins_rimshot6[] = { 35,    -1,     26,       -1,         14,     -1,    13,    -1,    -1 };
      // clang-format on
      *analog_pins         = analog_pins6;
      *analog_pins_rimshot = analog_pins_rimshot6;
      *number_pins         = sizeof(analog_pins6) / sizeof(int);
      *status_LED_pin      = BOARD_LED_PIN;
      return 6;
    }
  }

  // default: assume that analog pins are set outside this function, only update board LED pin
  *status_LED_pin = BOARD_LED_PIN;
  return 4;
#  else // CONFIG_IDF_TARGET_ESP32S3
  // clang-format off
  // analog pins setup:                 snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3
  static int analog_pins_s3[]         = {  4,     6,      7,        9,         10,     12,    13,    15,    16 };
  static int analog_pins_rimshot_s3[] = {  5,    -1,      8,       -1,         11,     -1,    14,    -1,    -1 };
  // clang-format on
  *analog_pins         = analog_pins_s3;
  *analog_pins_rimshot = analog_pins_rimshot_s3;
  *number_pins         = sizeof(analog_pins_s3) / sizeof(int);
  *status_LED_pin      = BOARD_LED_PIN;
  return 4;
#  endif
}

void Edrumulus_hardware::write_setting(const int pad_index,
    const int address,
    const byte value)
{
  const char* key = String(pad_index * MAX_NUM_SET_PER_PAD + address).c_str();
  settings.putUChar(key, value);
}

byte Edrumulus_hardware::read_setting(const int pad_index,
    const int address)
{
  const char* key = String(pad_index * MAX_NUM_SET_PER_PAD + address).c_str();
  return settings.getUChar(key, 0);
}

void Edrumulus_hardware::setup(const int conf_Fs,
    const int number_pads,
    const int number_inputs[],
    int analog_pin[][MAX_NUM_PAD_INPUTS])
{
  // set essential parameters
  Fs                             = conf_Fs;
  char preferences_namespace[16] = "Edrumulus";
  settings.begin(preferences_namespace, false);

  // create linear vectors containing the pin/ADC information for each pad and pad-input
  bool input_is_used[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  int input_adc[MAX_NUM_PADS * MAX_NUM_PAD_INPUTS];
  total_number_inputs = 0; // we use it as a counter, too

  for (int i = 0; i < number_pads; i++)
  {
    for (int j = 0; j < number_inputs[i]; j++)
    {
      // store pin number in vector and identify ADC number for each pin
      input_pin[total_number_inputs]     = analog_pin[i][j];
      input_adc[total_number_inputs]     = (digitalPinToAnalogChannel(analog_pin[i][j]) >= 10); // channel < 10 -> ADC1, channel >= 10 -> ADC2
      input_is_used[total_number_inputs] = false;                                               // initialization needed for ADC pairs identification
      total_number_inputs++;
    }
  }

  // find ADC pairs, i.e., one pin uses ADC1 and the other uses ADC2
  num_pin_pairs = 0; // we use it as a counter, too

#  ifdef CONFIG_IDF_TARGET_ESP32
  for (int i = 0; i < total_number_inputs - 1; i++)
  {
    if (!input_is_used[i])
    {
      for (int j = total_number_inputs - 1; j > i; j--)
      {
        // check for different ADCs: 0+0=0, 1+0=1 (pair), 0+1=1 (pair), 1+1=2
        if (!input_is_used[j] && (input_adc[i] + input_adc[j] == 1))
        {
          if (input_adc[i] == 0)
          {
            adc1_index[num_pin_pairs] = i;
            adc2_index[num_pin_pairs] = j;
          }
          else
          {
            adc1_index[num_pin_pairs] = j;
            adc2_index[num_pin_pairs] = i;
          }

          // create the mask bit needed for SENS_SAR1_EN_PAD and SENS_SAR2_EN_PAD
          const int8_t channel_adc1          = digitalPinToAnalogChannel(input_pin[adc1_index[num_pin_pairs]]);
          const int8_t channel_adc2          = digitalPinToAnalogChannel(input_pin[adc2_index[num_pin_pairs]]) - 10;
          channel_adc1_bitval[num_pin_pairs] = (1 << channel_adc1);
          channel_adc2_bitval[num_pin_pairs] = (1 << channel_adc2);

          num_pin_pairs++;
          input_is_used[i] = true;
          input_is_used[j] = true;
          break;
        }
      }
    }
  }
#  endif

  // find remaining single pins which we cannot create an ADC pair with
  num_pin_single = 0; // we use it as a counter, too

  for (int i = 0; i < total_number_inputs; i++)
  {
    if (!input_is_used[i])
    {
      single_index[num_pin_single] = i;
      num_pin_single++;
    }
  }

  // prepare the ADC and analog GPIO inputs
  init_my_analogRead();

  // create timer semaphore
  timer_semaphore = xSemaphoreCreateBinary();

  // create task pinned to core 0 for creating the timer interrupt so that the
  // timer function is not running in our working core 1
  xTaskCreatePinnedToCore(start_timer_core0_task, "start_timer_core0_task", 1000, this, 1, NULL, 0);
}

void Edrumulus_hardware::setup_timer()
{
  // prepare timer at a rate of given sampling rate
#  if ESP_IDF_VERSION_MAJOR < 5
  timer = timerBegin(0, 80, true); // prescaler of 80 (i.e. below we have 1 MHz instead of 80 MHz)
  timerAttachInterrupt(timer, &on_timer, true);
  timerAlarmWrite(timer, 1000000 / Fs, true); // here we define the sampling rate (1 MHz / Fs)
  timerAlarmEnable(timer);
#  else
  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &on_timer);
  timerAlarm(timer, 1000000 / Fs, true, 0);
#  endif
}

void Edrumulus_hardware::start_timer_core0_task(void* param)
{
  reinterpret_cast<Edrumulus_hardware*>(param)->setup_timer();

  // tasks must not return: forever loop with delay to keep watchdog happy
  for (;;)
  {
    delay(1000);
  }
}

void IRAM_ATTR Edrumulus_hardware::on_timer()
{
  // first read the ADC pairs samples
  for (int i = 0; i < edrumulus_hardware_pointer->num_pin_pairs; i++)
  {
    edrumulus_hardware_pointer->my_analogRead_parallel(
        edrumulus_hardware_pointer->channel_adc1_bitval[i],
        edrumulus_hardware_pointer->channel_adc2_bitval[i],
        edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->adc1_index[i]],
        edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->adc2_index[i]]);
  }

  // second read the single ADC samples
  for (int i = 0; i < edrumulus_hardware_pointer->num_pin_single; i++)
  {
    edrumulus_hardware_pointer->input_sample[edrumulus_hardware_pointer->single_index[i]] =
        edrumulus_hardware_pointer->my_analogRead(edrumulus_hardware_pointer->input_pin[edrumulus_hardware_pointer->single_index[i]]);
  }

  // tell the main loop that a sample can be processed by setting the semaphore
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(edrumulus_hardware_pointer->timer_semaphore, &xHigherPriorityTaskWoken);

  if (xHigherPriorityTaskWoken == pdTRUE)
  {
    portYIELD_FROM_ISR();
  }
}

void Edrumulus_hardware::capture_samples(const int number_pads,
    const int number_inputs[],
    int analog_pin[][MAX_NUM_PAD_INPUTS],
    int sample_org[][MAX_NUM_PAD_INPUTS])
{
  // wait for the timer to get the correct sampling rate when reading the analog value
  if (xSemaphoreTake(timer_semaphore, portMAX_DELAY) == pdTRUE)
  {
    // copy captured samples in pad buffer
    int input_cnt = 0;

    for (int i = 0; i < number_pads; i++)
    {
      for (int j = 0; j < number_inputs[i]; j++)
      {
        sample_org[i][j] = input_sample[input_cnt++];
      }
    }
  }
}

// Since arduino-esp32 library version 1.0.5, the analogRead was changed to use the IDF interface
// which made the analogRead function so slow that we cannot use that anymore for Edrumulus:
// https://github.com/espressif/arduino-esp32/issues/4973, https://github.com/espressif/arduino-esp32/pull/3377
// As a workaround, we had to write our own analogRead function.
void Edrumulus_hardware::init_my_analogRead()
{
#  ifdef CONFIG_IDF_TARGET_ESP32
  // if the GIOP 25/26 are used, we have to set the DAC to 0 to get correct DC offset
  // estimates and reduce the number of large spikes
  dac_i2s_enable();
  dac_output_enable(DAC_CHANNEL_1);
  dac_output_voltage(DAC_CHANNEL_1, 0);
  dac_output_disable(DAC_CHANNEL_1);
  dac_output_enable(DAC_CHANNEL_2);
  dac_output_voltage(DAC_CHANNEL_2, 0);
  dac_output_disable(DAC_CHANNEL_2);
  dac_i2s_disable();

  // set attenuation of 11 dB
  WRITE_PERI_REG(SENS_SAR_ATTEN1_REG, 0x0FFFFFFFF);
  WRITE_PERI_REG(SENS_SAR_ATTEN2_REG, 0x0FFFFFFFF);

  // set both ADCs to 12 bit resolution using 8 cycles and 1 sample
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_CYCLE, 8, SENS_SAR1_SAMPLE_CYCLE_S); // cycles
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_CYCLE, 8, SENS_SAR2_SAMPLE_CYCLE_S);
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_NUM, 0, SENS_SAR1_SAMPLE_NUM_S); // # samples
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_NUM, 0, SENS_SAR2_SAMPLE_NUM_S);
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_CLK_DIV, 1, SENS_SAR1_CLK_DIV_S); // clock div
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_CLK_DIV, 1, SENS_SAR2_CLK_DIV_S);
  SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH, 3, SENS_SAR1_BIT_WIDTH_S); // width
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_BIT, 3, SENS_SAR1_SAMPLE_BIT_S);
  SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR2_BIT_WIDTH, 3, SENS_SAR2_BIT_WIDTH_S);
  SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_BIT, 3, SENS_SAR2_SAMPLE_BIT_S);

  // some other initializations
  SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DATA_INV);
  SET_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DATA_INV);
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_FORCE_M);                     // SAR ADC1 controller (in RTC) is started by SW
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M);                     // SAR ADC1 pad enable bitmap is controlled by SW
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_FORCE_M);                     // SAR ADC2 controller (in RTC) is started by SW
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD_FORCE_M);                     // SAR ADC2 pad enable bitmap is controlled by SW
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_M);                        // force XPD_SAR=0, use XPD_FSM
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_AMP, 0x2, SENS_FORCE_XPD_AMP_S); // force XPD_AMP=0
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_CTRL_REG, 0xfff << SENS_AMP_RST_FB_FSM_S);               // clear FSM
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT1_REG, SENS_SAR_AMP_WAIT1, 0x1, SENS_SAR_AMP_WAIT1_S);
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT1_REG, SENS_SAR_AMP_WAIT2, 0x1, SENS_SAR_AMP_WAIT2_S);
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_SAR_AMP_WAIT3, 0x1, SENS_SAR_AMP_WAIT3_S);
  while (GET_PERI_REG_BITS2(SENS_SAR_SLAVE_ADDR1_REG, 0x7, SENS_MEAS_STATUS_S) != 0)
    ;

  // configure all pins to analog read
  for (int i = 0; i < total_number_inputs; i++)
  {
    pinMode(input_pin[i], ANALOG);
  }
#  else // CONFIG_IDF_TARGET_ESP32S3
  int cur_sample;
  for (int channel = 0; channel < 10; channel++) // 10 channels per ADC
  {
    // configure the attenuation and let the get_raw() do all the ADC initialization for us...
    adc1_config_channel_atten(static_cast<adc1_channel_t>(channel), ADC_ATTEN_DB_12);
    adc2_config_channel_atten(static_cast<adc2_channel_t>(channel), ADC_ATTEN_DB_12);
    adc1_get_raw(static_cast<adc1_channel_t>(channel));
    adc2_get_raw(static_cast<adc2_channel_t>(channel), ADC_WIDTH_BIT_12, &cur_sample);
  }
  adc_power_acquire();
#  endif
}

uint16_t Edrumulus_hardware::my_analogRead(const uint8_t pin)
{
  const int8_t channel = digitalPinToAnalogChannel(pin);

  if (channel > 9)
  {
    const int8_t channel_modified = channel - 10;
#  ifdef CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, (1 << channel_modified), SENS_SAR2_EN_PAD_S);
    SET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M);
    while (GET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR) == 0)
      ;
    return GET_PERI_REG_BITS2(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S);
#  else // CONFIG_IDF_TARGET_ESP32S3
    SENS.sar_meas2_ctrl2.meas2_start_sar = 0;
    SENS.sar_meas2_ctrl2.sar2_en_pad     = (1 << channel_modified);
    SENS.sar_meas2_ctrl2.meas2_start_sar = 1;
    while (!SENS.sar_meas2_ctrl2.meas2_done_sar)
      ;
    return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas2_ctrl2, meas2_data_sar);
#  endif
  }
  else
  {
#  ifdef CONFIG_IDF_TARGET_ESP32
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, (1 << channel), SENS_SAR1_EN_PAD_S);
    SET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M);
    while (GET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR) == 0)
      ;
    return GET_PERI_REG_BITS2(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S);
#  else // CONFIG_IDF_TARGET_ESP32S3
    SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    SENS.sar_meas1_ctrl2.sar1_en_pad     = (1 << channel);
    SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    while (!SENS.sar_meas1_ctrl2.meas1_done_sar)
      ;
    return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas1_ctrl2, meas1_data_sar);
#  endif
  }
}

void Edrumulus_hardware::my_analogRead_parallel(const uint32_t channel_adc1_bitval,
    const uint32_t channel_adc2_bitval,
    uint16_t& out_adc1,
    uint16_t& out_adc2)
{
#  ifdef CONFIG_IDF_TARGET_ESP32
  // start ADC1
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M);
  SET_PERI_REG_BITS(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD, channel_adc1_bitval, SENS_SAR1_EN_PAD_S);
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_SAR_M);

  // start ADC2
  CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M);
  SET_PERI_REG_BITS(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD, channel_adc2_bitval, SENS_SAR2_EN_PAD_S);
  SET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_SAR_M);

  // wait for ADC1 and read value
  while (GET_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DONE_SAR) == 0)
    ;
  out_adc1 = GET_PERI_REG_BITS2(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_DATA_SAR, SENS_MEAS1_DATA_SAR_S);

  // wait for ADC2 and read value
  while (GET_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DONE_SAR) == 0)
    ;
  out_adc2 = GET_PERI_REG_BITS2(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_DATA_SAR, SENS_MEAS2_DATA_SAR_S);
#  endif
}

#endif
