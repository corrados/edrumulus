// Copyright (c) 2020-2026 Volker Fischer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define USE_MIDI

// #define USE_SERIAL_DEBUG_PLOTTING
// #define USE_OCTAVE_SAMPLE_IMPORT_EXPORT
// #define USE_LOW_SAMPLING_RATE_SAMPLE_MONITOR

#define VERSION_MAJOR 0
#define VERSION_MINOR 10

#define MAX_NUM_PADS 12      // a maximum of 12 pads are supported
#define MAX_NUM_PAD_INPUTS 5 // a maximum of 5 sensors per pad is supported (where one is rim and one is the sum of three)

inline void update_fifo(const float input,
                        const int   fifo_length,
                        float*      fifo_memory)
{
  // move all values in the history one step back and put new value on the top
  const int fifo_length_minus_one = fifo_length - 1;
  for (int i = 0; i < fifo_length_minus_one; i++)
  {
    fifo_memory[i] = fifo_memory[i + 1];
  }
  fifo_memory[fifo_length_minus_one] = input;
}

inline void allocate_initialize(float**   array_memory,
                                const int array_length)
{
  // (delete and) allocate memory
  if (*array_memory != nullptr)
  {
    delete[] *array_memory;
  }

  *array_memory = new float[array_length];

  // initialization values
  for (int i = 0; i < array_length; i++)
  {
    (*array_memory)[i] = 0.0f;
  }
}

class FastWriteFIFO
{
 public:
  void initialize(const int len)
  {
    pointer     = 0;
    fifo_length = len;
    allocate_initialize(&fifo_memory, len);
  }

  void add(const float input)
  {
    // write new value and increment data pointer with wrap around
    fifo_memory[pointer] = input;
    pointer              = (pointer + 1) % fifo_length;
  }

  const float operator[](const int index)
  {
    return fifo_memory[(pointer + index) % fifo_length];
  }

 protected:
  float* fifo_memory = nullptr;
  int    pointer;
  int    fifo_length;
};

// Debugging functions ---------------------------------------------------------
// Debugging: take samples from Octave, process and return result to Octave
#ifdef USE_OCTAVE_SAMPLE_IMPORT_EXPORT
#  undef USE_MIDI
#  define DBG_FCT_OCTAVE_SAMPLE_IMPORT_EXPORT()                                                                                                                   \
    if (Serial.available() > 0)                                                                                                                                   \
    {                                                                                                                                                             \
      static int m = micros();                                                                                                                                    \
      if (micros() - m > 500000) pad[0].set_velocity_threshold(14.938);                                                                                           \
      m         = micros();                                                                                                                                       \
      float fIn = Serial.parseFloat();                                                                                                                            \
      float y   = pad[0].process_sample(&fIn, 1, overload_detected, peak_found[0], midi_velocity[0], midi_pos[0], rim_state[0], is_choke_on[0], is_choke_off[0]); \
      Serial.println(y, 7);                                                                                                                                       \
    }                                                                                                                                                             \
    return;
#else
#  define DBG_FCT_OCTAVE_SAMPLE_IMPORT_EXPORT()
#endif

// Debugging: for plotting all captures samples in the serial plotter (but with low sampling rate)
#ifdef USE_LOW_SAMPLING_RATE_SAMPLE_MONITOR
#  undef USE_MIDI
#  define DBG_FCT_LOW_SAMPLING_RATE_SAMPLE_MONITOR()     \
    String serial_print;                                 \
    for (int i = 0; i < number_pads; i++)                \
    {                                                    \
      for (int j = 0; j < number_inputs[i]; j++)         \
      {                                                  \
        serial_print += String(sample_org[i][j]) + "\t"; \
      }                                                  \
    }                                                    \
    Serial.println(serial_print);
#else
#  define DBG_FCT_LOW_SAMPLING_RATE_SAMPLE_MONITOR()
#endif
