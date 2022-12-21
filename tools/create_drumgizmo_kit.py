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
import gc
import wave
import numpy as np
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from scipy.io import wavfile


################################################################################
# CONFIGURATION AND INITIALIZATIONS ############################################
################################################################################
# example file names of source files: source_samples/snare/snare_rimshot_0_channel1.wav  <- position 0, channel 1
#                                     source_samples/snare/snare_rimshot_1_channel1.wav  <- position 1, channel 1
#                                     source_samples/ride/ride_bell_channel7.wav <- no positional sensing, channel 7
kit_name        = "PearlMMX" # avoid spaces
kit_description = "Pearl MMX drum set with positional sensing support"
channel_names   = ["KDrum", "Snare", "Hihat", "Tom1", "Tom2", "Tom3", "OHLeft", "OHRight"]

# instruments: [instrument_name, master_channel(s), MIDI_note(s), group, threshold]
instruments = [["kick",            ["KDrum"],                      [36],     "",      45], \
               ["snare",           ["Snare", "OHLeft", "OHRight"], [38],     "",      62], \
               ["snare_rimshot",   ["Snare", "OHLeft", "OHRight"], [40],     "",      57], \
               ["hihat_closed",    ["Hihat", "OHLeft", "OHRight"], [22],     "hihat", 68], \
               ["hihat_closedtop", ["Hihat", "OHLeft", "OHRight"], [42],     "hihat", 60], \
               ["hihat_open",      ["Hihat", "OHLeft", "OHRight"], [26],     "hihat", 53], \
               ["hihat_opentop",   ["Hihat", "OHLeft", "OHRight"], [46],     "hihat", 53], \
               ["tom1",            ["Tom1", "OHLeft", "OHRight"],  [48, 50], "",      60], \
               ["tom2",            ["Tom2", "OHLeft", "OHRight"],  [45, 47], "",      50], \
               ["tom3",            ["Tom3", "OHLeft", "OHRight"],  [43, 58], "",      57], \
               ["crash",           ["OHLeft", "OHRight"],          [55],     "",      60], \
               ["crash_top",       ["OHLeft", "OHRight"],          [49],     "",      60], \
               ["ride",            ["OHRight", "OHLeft"],          [51],     "",      68], \
               ["ride_bell",       ["OHRight", "OHLeft"],          [53],     "",      60], \
               ["ride_side",       ["OHRight", "OHLeft"],          [59],     "",      68]]

source_samples_dir_name = "source_samples" # root directory of recorded source samples
min_strike_len          = 0.25 # seconds
fade_out_percent        = 10 # % of sample at the end is faded out

# TEST for optimizing the algorithms, only use one instrument
#instruments = [instruments[7]]


for instrument in instruments:

  ##############################################################################
  # FILE NAME HANDLING #########################################################
  ##############################################################################
  samples_dir_name       = "samples" # compatible to other Drumgizmo kits
  instrument_name        = instrument[0]
  instrument_path        = kit_name + "/" + instrument_name + "/"
  instrument_sample_path = instrument_path + samples_dir_name + "/"
  base_instrument_name   = instrument_name.split("_")[0]
  print(instrument_name)

  # check if instrument has positional sensing support and extract position indexes
  positions  = []
  for file_name in os.listdir(source_samples_dir_name + "/" + base_instrument_name):
    if instrument_name in file_name:
      file_name_parts = file_name.split(".")[0].split("_")
      # position information always second last item and one character long
      if len(file_name_parts) > 2 and len(file_name_parts[-2]) == 1:
        positions.append(int(file_name_parts[-2]))
  positions = sorted(list(dict.fromkeys(positions))) # remove duplicates and sort
  positions = [-1] if not positions else positions # if no positions, use -1 (i.e. no positional support)

  sample_powers  = [[]] * len(positions)
  sample_strikes = [[]] * len(positions)

  for p in positions:

    ##############################################################################
    # READ WAVE FORMS ############################################################
    ##############################################################################
    num_channels = len(channel_names)
    sample       = [[]] * num_channels
    pos_str      = "_" + str(p) if p >= 0 else ""
    for i in range(0, num_channels):
      with wave.open(source_samples_dir_name + "/" + base_instrument_name + "/" + \
                     instrument_name + pos_str + "_channel" + str(i + 1) + ".wav", "r") as file:
        sample_rate = file.getframerate() # assuming all wave have the same rate
        sample[i]   = np.frombuffer(file.readframes(-1), np.int16) # assuming 16 bit


    ##############################################################################
    # WAVE FORM ANALYSIS #########################################################
    ##############################################################################
    thresh_from_max = instrument[4] # dB from maximum peak
    master_channel  = channel_names.index(instrument[1][0]) # first main channel is master

    # find samples which are above the threshold
    x            = np.square(sample[master_channel].astype(float))
    threshold    = np.power(10, (10 * np.log10(np.max(x)) - thresh_from_max) / 10)
    above_thresh = x > threshold

    # remove oscillating by filling short gaps
    first_below_idx = -100 * sample_rate
    for i in range(1, len(above_thresh)):
      if not above_thresh[i] and above_thresh[i - 1]:
        first_below_idx = i
      if above_thresh[i] and not above_thresh[i - 1]:
        if i - first_below_idx < min_strike_len * sample_rate:
          above_thresh[range(first_below_idx, i)] = True

    # remove very short on periods
    first_above_idx = -100 * sample_rate
    for i in range(1, len(above_thresh)):
      if above_thresh[i] and not above_thresh[i - 1]:
        first_above_idx = i
      if not above_thresh[i] and above_thresh[i - 1]:
        if i - first_above_idx < min_strike_len * sample_rate:
          above_thresh[range(first_above_idx, i)] = False

    strike_start = np.argwhere(np.diff(above_thresh.astype(float)) > 0)
    strike_end   = np.argwhere(np.diff(above_thresh.astype(float)) < 0)

    # extract individual samples from long sample vector and analyze/process
    sample_powers[p]  = [[]] * len(strike_start)
    sample_strikes[p] = [[]] * len(strike_start)
    for i, (start, end) in enumerate(zip(strike_start, strike_end)):



      # TODO clean up the algorithm/code:
      # TODO define constants like the 20 dB and 30 samples offset above
      # TODO check all the strike samples if they look ok...
      # TEST fix start of strike: find first sample going left of the maximum peak which
      #      is below a threshold which is defined 20 dB below the maximum
      x_cur_strike_master = x[range(start[0], end[0])]
      strike_mean         = np.mean(x_cur_strike_master)
      strike_max          = np.max(x_cur_strike_master)
      below_max_thresh    = np.power(10, -20 / 10) # -20 dB from maximum peak
      #print(strike_mean)
      index = 0
      while x[start[0] + index] < strike_max * below_max_thresh:
        index += 1
      #print(index)
      start += index - 30 # TEST some offset, in this case 30 samples...




      # estimate power from master channel using the maximum value
      sample_powers[p][i] = str(strike_max / 32768 / 32768) # assuming 16 bit

      # extract sample data of current strike
      sample_strikes[p][i] = np.zeros((end[0] - start[0] + 1, num_channels), np.int16)
      for c in range(0, num_channels):
        sample_strikes[p][i][:, c] = sample[c][start[0]:end[0] + 1]

        # audio fade-out at the end
        sample_len = len(sample_strikes[p][i][:, c])
        fade_start = int(sample_len * (1 - fade_out_percent / 100))
        fade_len   = sample_len - fade_start
        sample_strikes[p][i][fade_start:, c] = np.int16(sample_strikes[p][i][fade_start:, c].astype(float) * np.arange(fade_len + 1, 1, -1) / fade_len)

      #print(sample_powers[p][i])
      #plt.plot(sample_strikes[p][i][:, master_channel])
      #plt.show()

    if len(instruments) == 1: # if only one instrument is selected, we assume we want to debug plot
      plt.plot(10 * np.log10(np.abs(x)))
      plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
      plt.plot(10 * np.log10(np.max(x)) * above_thresh)
      plt.plot(strike_start, [10 * np.log10(np.max(x))] * len(strike_start), 'o', color='tab:brown')
      plt.title(instrument_name + pos_str)
      plt.show()
      plt.close("all") # to prevent a memory leak
      plt.close()      # to prevent a memory leak
      gc.collect()     # to prevent a memory leak


  ##############################################################################
  # WRITE WAVE FORMS AND INSTRUMENT XML FILE ###################################
  ##############################################################################
  instrument_xml = ET.Element("instrument")
  instrument_xml.set("version", "2.0")
  instrument_xml.set("name", instrument_name)
  samples_xml = ET.SubElement(instrument_xml, "samples")

  for p in positions:
    for i in range(0, len(sample_strikes[p])):
      # write multi-channel wave file
      sample_file_name = str(i + 1) + "-" + instrument_name
      if len(positions) > 1:
        sample_file_name += "-" + str(p)
      os.makedirs(instrument_sample_path, exist_ok=True)
      wavfile.write(instrument_sample_path + sample_file_name + ".wav", sample_rate, sample_strikes[p][i])

      # write XML content for current sample
      sample_xml = ET.SubElement(samples_xml, "sample")
      if len(positions) > 1:
        sample_xml.set("position", str(p))
      sample_xml.set("name", instrument_name + "-" + str(i + 1))
      sample_xml.set("power", sample_powers[p][i])
      for j, channel_name in enumerate(channel_names):
        audiofile_xml = ET.SubElement(sample_xml, "audiofile")
        audiofile_xml.set("channel", channel_name)
        audiofile_xml.set("file", samples_dir_name + "/" + sample_file_name + ".wav")
        audiofile_xml.set("filechannel", str(j + 1))

  # write instrument XML file
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
for instrument in instruments:
  instrument_xml = ET.SubElement(instruments_xml, "instrument")
  instrument_xml.set("name", instrument[0])
  if instrument[3]:
    instrument_xml.set("group", instrument[3])
  instrument_xml.set("file", instrument[0] + "/" + instrument[0] + ".xml")
  for channel_name in channel_names:
    channelmap_xml = ET.SubElement(instrument_xml, "channelmap")
    channelmap_xml.set("in", channel_name)
    channelmap_xml.set("out", channel_name)
    if channel_name in instrument[1]:
      channelmap_xml.set("main", "true")
tree_xml = ET.ElementTree(drumkit_xml)
ET.indent(drumkit_xml, space="\t", level=0)
os.makedirs(kit_name, exist_ok=True)
tree_xml.write(kit_name + "/" + kit_name + ".xml", encoding="utf-8", xml_declaration="True")


################################################################################
# CREATE MIDI MAP XML FILE #####################################################
################################################################################
midimap_xml = ET.Element("midimap")
for instrument in instruments:
  for midi_note in instrument[2]:
    map_xml = ET.SubElement(midimap_xml, "map")
    map_xml.set("note", str(midi_note))
    map_xml.set("instr", instrument[0])
tree_xml = ET.ElementTree(midimap_xml)
ET.indent(midimap_xml, space="\t", level=0)
tree_xml.write(kit_name + "/Midimap.xml", encoding="utf-8", xml_declaration="True")



