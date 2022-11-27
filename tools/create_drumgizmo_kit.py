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

import os
import wave
import numpy as np
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from scipy.io import wavfile
from scipy.signal import butter, sosfilt


################################################################################
# INITIALIZATIONS ##############################################################
################################################################################
kit_name         = "PearlMMX" # avoid spaces
samples_dir_name = "samples" # compatible to other Drumgizmo kits
kit_description  = "Pearl MMX drum set with positional sensing support"
channel_names    = ["KDrum", "Snare", "Hihat", "Tom1", "Tom2", "Tom3", "OHLeft", "OHRight"]
num_channels     = len(channel_names)
instrument_names = ["snare"]
instrument_midis = [38]
sub_instrument   = "snare_0" # TODO
master_channel   = 1 # master channel index (zero-based index)
thresh_from_max  = 60 # 60 dB from maximum peak


for instrument_name in instrument_names:

  ##############################################################################
  # FILE NAME HANDLING #########################################################
  ##############################################################################
  # create file names of all audio channels
  file_names = []
  for i in range(0, num_channels):
    file_names.append(("source_samples/%s/%s_channel%d.wav") % (instrument_name, sub_instrument, i + 1))


  ##############################################################################
  # READ WAVE FORMS ############################################################
  ##############################################################################
  # read samples from all audio channels
  sample       = [[]] * num_channels
  sample_float = [[]] * num_channels
  for i, f in enumerate(file_names):
    file            = wave.open(f, "r")
    sample_rate     = file.getframerate() # assuming all wave have the same rate
    sample[i]       = np.frombuffer(file.readframes(-1), np.int16) # assuming 16 bit
    sample_float[i] = sample[i].astype(float)
    file.close()


  ##############################################################################
  # WAVE FORM ANALYSIS #########################################################
  ##############################################################################
  # analyze master channel and find strikes
  x            = sosfilt(butter(2, 0.001, btype="low", output="sos"), np.square(sample_float[master_channel]))
  threshold    = np.power(10, (10 * np.log10(np.max(x)) - thresh_from_max) / 10)
  above_thresh = x > threshold

  # TODO: quick hack to remove oscillating at the end of a detected block
  last_above_idx = -1000000
  for i in range(1, len(above_thresh)):
    if above_thresh[i] and not above_thresh[i - 1]:
      if i - last_above_idx < 40000:
        above_thresh[i] = False
    if above_thresh[i]:
      last_above_idx = i

  strike_start = np.argwhere(np.diff(above_thresh.astype(float)) > 0)
  strike_end   = np.argwhere(np.diff(above_thresh.astype(float)) < 0)

  # extract individual samples from long sample vector
  sample_strikes = [[]] * len(strike_start)
  for i, (start, end) in enumerate(zip(strike_start, strike_end)):
    sample_strikes[i] = np.zeros((strike_end[i][0] - strike_start[i][0] + 1, num_channels), np.int16)
    for c in range(0, num_channels):
      sample_strikes[i][:, c] = sample[c][start[0]:end[0] + 1]

  #plt.plot(sample_strikes[7][:, 0])
  #plt.show()

  ##plt.plot(20 * np.log10(np.abs(sample_float[master_channel])))
  #plt.plot(10 * np.log10(np.abs(x)))
  #plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
  #plt.plot(10 * np.log10(np.max(x)) * above_thresh)
  #plt.show()


  ##############################################################################
  # STORE WAVE FORMS ###########################################################
  ##############################################################################
  instrument_path        = kit_name + "/" + instrument_name + "/"
  instrument_sample_path = instrument_path + samples_dir_name + "/"
  os.makedirs(instrument_sample_path, exist_ok=True)

  # TODO
  test_sample_names  = ["snare_test"]
  test_sample_powers = ["1.0"] # TODO must be estimated in the signal analysis part

  # write multi-channel wave file
  # TODO
  #wavfile.write("snare_test.wav", sample_rate, np.array(sample).T)
  wavfile.write(instrument_sample_path + test_sample_names[0] + ".wav", sample_rate, sample_strikes[7])


  ##############################################################################
  # CREATE INSTRUMENT XML FILE #################################################
  ##############################################################################
  instrument_xml = ET.Element("instrument")
  instrument_xml.set("version", "2.0")
  instrument_xml.set("name", instrument_name)
  samples_xml = ET.SubElement(instrument_xml, "samples")
  for i, sample_name in enumerate(test_sample_names):
    sample_xml = ET.SubElement(samples_xml, "sample")
    sample_xml.set("name", sample_name)
    sample_xml.set("power", test_sample_powers[i])
    for j, channel_name in enumerate(channel_names):
      audiofile_xml = ET.SubElement(sample_xml, "audiofile")
      audiofile_xml.set("channel", channel_name)
      audiofile_xml.set("file", samples_dir_name + "/" + sample_name + ".wav")
      audiofile_xml.set("filechannel", str(j + 1))
  tree_xml = ET.ElementTree(instrument_xml)
  ET.indent(instrument_xml, space="\t", level=0)
  tree_xml.write(instrument_path + instrument_name + ".xml", encoding="utf-8", xml_declaration="True")


################################################################################
# CREATE DRUM KIT XML FILE #####################################################
################################################################################
drumkit_xml = ET.Element("drumkit")
drumkit_xml.set("name", kit_name)
drumkit_xml.set("description", kit_description)
drumkit_xml.set("samplerate", str(sample_rate))
channels_xml = ET.SubElement(drumkit_xml, "channels")
for channel_name in channel_names:
  channel_xml = ET.SubElement(channels_xml, "channel")
  channel_xml.set("name", channel_name)
instruments_xml = ET.SubElement(drumkit_xml, "instruments")
for instrument_name in instrument_names:
  instrument_xml = ET.SubElement(instruments_xml, "instrument")
  instrument_xml.set("name", instrument_name)
  instrument_xml.set("file", "%s/%s.xml" % (instrument_name, instrument_name))
  for channel_name in channel_names:
    channelmap_xml = ET.SubElement(instrument_xml, "channelmap")
    channelmap_xml.set("in", channel_name)
    channelmap_xml.set("out", channel_name)
tree_xml = ET.ElementTree(drumkit_xml)
ET.indent(drumkit_xml, space="\t", level=0)
os.makedirs(kit_name, exist_ok=True)
tree_xml.write(kit_name + "/" + kit_name + ".xml", encoding="utf-8", xml_declaration="True")


################################################################################
# CREATE MIDI MAP XML FILE #####################################################
################################################################################
midimap_xml = ET.Element("midimap")
for i, instrument_midi in enumerate(instrument_midis):
  map_xml = ET.SubElement(midimap_xml, "map")
  map_xml.set("note", str(instrument_midi))
  map_xml.set("instr", instrument_names[i])
tree_xml = ET.ElementTree(midimap_xml)
ET.indent(midimap_xml, space="\t", level=0)
tree_xml.write(kit_name + "/Midimap.xml", encoding="utf-8", xml_declaration="True")



