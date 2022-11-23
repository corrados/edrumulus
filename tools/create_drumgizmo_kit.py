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
sample = [[]] * num_channels
for i, f in enumerate(file_names):
  file   = wave.open(f, "r")
  sample[i] = file.readframes(-1)
  sample[i] = np.frombuffer(sample[i], np.int16) # assuming 16 bit
  sample[i] = sample[i].astype(float)
  file.close()

# analyze master channel and find strikes
x = sosfilt(butter(2, 0.001, btype="low", output="sos"), np.square(sample[master_channel]))
threshold = np.power(10, (10 * np.log10(np.max(x)) - thresh_from_max) / 10)
plt.plot(10 * np.log10(np.abs(x)))
plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
above_thresh = x > threshold

above_thresh_diff = np.diff(above_thresh)

np.floor(len(x) / max_num_peaks)

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



