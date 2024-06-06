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

#pragma once


// Utility functions -----------------------------------------------------------------

inline void update_fifo ( const float input,
                          const int   fifo_length,
                          float*      fifo_memory )
{
  // move all values in the history one step back and put new value on the top
  for ( int i = 0; i < fifo_length - 1; i++ )
  {
    fifo_memory[i] = fifo_memory[i + 1];
  }
  fifo_memory[fifo_length - 1] = input;
}

inline void allocate_initialize ( float**   array_memory,
                                  const int array_length )
{
  // (delete and) allocate memory
  if ( *array_memory != nullptr )
  {
    delete[] *array_memory;
  }

  *array_memory = new float[array_length];

  // initialization values
  for ( int i = 0; i < array_length; i++ )
  {
    ( *array_memory )[i] = 0.0f;
  }
}

class FastWriteFIFO
{
public:
  void initialize ( const int len )
  {
    pointer     = 0;
    fifo_length = len;
    allocate_initialize ( &fifo_memory, len );
  }

  void add ( const float input )
  {
    // write new value and increment data pointer with wrap around
    fifo_memory[pointer] = input;
    pointer              = ( pointer + 1 ) % fifo_length;
  }

  const float operator[] ( const int index )
  {
    return fifo_memory[( pointer + index ) % fifo_length];
  }

protected:
  float* fifo_memory = nullptr;
  int    pointer;
  int    fifo_length;
};


// Multiple head sensor management ---------------------------------------------

class MultiHeadSensor
{
public:
  void initialize();
  void calculate ( const bool sensor0_has_results,
                   SSensor*   sSensor );

protected:
  int multiple_sensor_cnt;

  float           get_pos_x0;
  float           get_pos_y0;
  float           get_pos_x1;
  float           get_pos_y1;
  float           get_pos_x2;
  float           get_pos_y2;
  float           get_pos_x0_sq_plus_y0_sq;
  float           get_pos_a1;
  float           get_pos_b1;
  float           get_pos_a2;
  float           get_pos_b2;
  float           get_pos_div1_fact;
  float           get_pos_div2_fact;
  float           get_pos_rim_radius;
};
