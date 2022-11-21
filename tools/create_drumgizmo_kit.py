#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2022
# Author(s): Volker Fischer
#*******************************************************************************
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#*******************************************************************************

import wave
import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

num_channels   = 8
sample_rate    = 48000
instrument     = "snare"
sub_instrument = "snare_0"

# create file names of all audio channels
file_names = []
for i in range(0, num_channels):
  file_names.append(("source_samples/%s/%s_channel%d.wav") % (instrument, sub_instrument, i + 1))

# read samples from all audio channels
sample = [[]] * num_channels
for i, f in enumerate(file_names):
  file   = wave.open(f, "r")
  sample[i] = file.readframes(-1)
  sample[i] = np.frombuffer(sample[i], np.int16) # assuming 16 bit
  file.close()

# write multi-channel wave file
wavfile.write("snare_test.wav", sample_rate, np.array(sample).T)



#fig, ax = plt.subplots(3, 1)
#ax[0].plot(sample[0])
#ax[1].plot(sample[1])
#ax[2].plot(sample[2])
#plt.show()



