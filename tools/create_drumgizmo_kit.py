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
from scipy.signal import butter, sosfilt
import xml.etree.ElementTree as ET

num_channels    = 8
sample_rate     = 48000
kit_name        = "Pearl MMX"
instrument      = "snare"
sub_instrument  = "snare_0"
master_channel  = 1 # master channel index (zero-based index)
thresh_from_max = 60 # 60 dB from maximum peak
max_num_peaks   = 200 # maximum 200 strikes per recording assumed

# create file names of all audio channels
file_names = []
for i in range(0, num_channels):
  file_names.append(("source_samples/%s/%s_channel%d.wav") % (instrument, sub_instrument, i + 1))

# read samples from all audio channels
sample       = [[]] * num_channels
sample_float = [[]] * num_channels
for i, f in enumerate(file_names):
  file            = wave.open(f, "r")
  sample[i]       = np.frombuffer(file.readframes(-1), np.int16) # assuming 16 bit
  sample_float[i] = sample[i].astype(float)
  file.close()

# analyze master channel and find strikes
x            = sosfilt(butter(2, 0.001, btype="low", output="sos"), np.square(sample_float[master_channel]))
threshold    = np.power(10, (10 * np.log10(np.max(x)) - thresh_from_max) / 10)
above_thresh = x > threshold


# tests...
#above_thresh_diff = np.diff(above_thresh)
#np.floor(len(x) / max_num_peaks)

# quick hack to remove oscillating at the end of a detected block
last_above_idx = -1000000
for i in range(1, len(above_thresh)):
  if above_thresh[i] and not above_thresh[i - 1]:
    if i - last_above_idx < 40000:
      above_thresh[i] = False
  if above_thresh[i]:
    last_above_idx = i

strike_start = np.argwhere(np.diff(above_thresh.astype(float)) > 0)[0]
strike_end   = np.argwhere(np.diff(above_thresh.astype(float)) < 0)[0]
strike_len   = strike_end - strike_start + 1
sample_np    = np.array(sample)

# test: extract individual samples from long sample vector
sample_strikes = [np.zeros((strike_len[0], num_channels))]
for i, (start, end) in enumerate(zip(strike_start, strike_end)):
  for c in range(0, num_channels):
    sample_strikes[0][:, c] = sample_np[c, start:end + 1]


#plt.plot(20 * np.log10(np.abs(sample_float[master_channel])))
plt.plot(10 * np.log10(np.abs(x)))
plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
plt.plot(10 * np.log10(np.max(x)) * above_thresh)
plt.show()

# write multi-channel wave file
wavfile.write("snare_test.wav", sample_rate, np.array(sample).T)

# write drumkit XML file
drumkit_xml = ET.Element("drumkit")
drumkit_xml.set("name", kit_name)
drumkit_xml.set("description", "") # TODO
channels_xml = ET.SubElement(drumkit_xml, "channels")
for i in range(0, num_channels):
  channel_xml = ET.SubElement(channels_xml, "channel")
  channel_xml.set("name", file_names[i]) # TODO
channels_xml = ET.SubElement(drumkit_xml, "instruments")
tree_xml = ET.ElementTree(drumkit_xml)
ET.indent(drumkit_xml, space="\t", level=0)
tree_xml.write("drumkit.xml", encoding="utf-8", xml_declaration="True")



