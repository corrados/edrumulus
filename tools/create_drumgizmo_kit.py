#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2022-2023
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
import matplotlib as mpl
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from scipy.io import wavfile


# conversion settings
disable_positional_sensing_support  = False#True#
only_master_channels_per_instrument = False#True#
raspi_optimized_drumkit             = False#True#


################################################################################
# CONFIGURATION AND INITIALIZATIONS ############################################
################################################################################
# example file names of source files: source_samples/snare/snare_rimshot_0_channel1.wav  <- position 0, channel 1
#                                     source_samples/snare/snare_rimshot_1_channel1.wav  <- position 1, channel 1
#                                     source_samples/ride/ride_bell_channel7.wav <- no positional sensing, channel 7
kit_name        = "PearlMMX" # avoid spaces
kit_description = "Pearl MMX drum set with positional sensing support"
channel_names   = ["KDrum", "Snare", "Hihat", "Tom1", "Tom2", "Tom3", "OHLeft", "OHRight"]

# instruments: [instrument_name, master_channel(s), MIDI_note(s), group, min_strike_len, threshold]
instruments = [["kick",            ["KDrum", "OHLeft", "OHRight"], [36],     "",      0.1,  15], \
               ["snare",           ["Snare", "OHLeft", "OHRight"], [38],     "",      0.08, 16], \
               ["snare_rimshot",   ["Snare", "OHLeft", "OHRight"], [40],     "",      0.3,  15], \
               ["hihat_closed",    ["Hihat", "OHLeft", "OHRight"], [22],     "hihat", 0.18, 20], \
               ["hihat_closedtop", ["Hihat", "OHLeft", "OHRight"], [42],     "hihat", 0.2,  20], \
               ["hihat_open",      ["Hihat", "OHLeft", "OHRight"], [26],     "hihat", 0.7,  23], \
               ["hihat_opentop",   ["Hihat", "OHLeft", "OHRight"], [46],     "hihat", 0.7,  24], \
               ["hihat_foot",      ["Hihat", "OHLeft", "OHRight"], [44],     "hihat", 0.1,  23], \
               ["tom1",            ["Tom1", "OHLeft", "OHRight"],  [48, 50], "",      0.2,  15], \
               ["tom2",            ["Tom2", "OHLeft", "OHRight"],  [45, 47], "",      0.2,  15], \
               ["tom3",            ["Tom3", "OHLeft", "OHRight"],  [43, 58], "",      0.4,  15], \
               ["crash",           ["OHLeft", "OHRight"],          [55],     "",      0.5,  15], \
               ["crash_top",       ["OHLeft", "OHRight"],          [49],     "",      0.4,  15], \
               ["ride",            ["OHRight", "OHLeft"],          [51],     "",      1.0,  15], \
               ["ride_bell",       ["OHRight", "OHLeft"],          [53],     "",      1.0,  16], \
               ["ride_side",       ["OHRight", "OHLeft"],          [59],     "",      1.0,  15]]

#channel_names = ["SnareL"] # for calibrating dynamic in Drumgizmo
#instruments   = [["rolandsnare", ["SnareL"], [38], "", 0.03, 23]]

source_samples_dir_name   = "source_samples" # root directory of recorded source samples
fade_out_percent          = 10   # % of sample at the end is faded out
thresh_from_max_for_start = 20   # dB
add_samples_at_start      = 20   # additional samples considered at strike start
min_time_next_strike_s    = 0.5  # minimum time in seconds between two different strikes

# TEST for optimizing the algorithms, only use one instrument
#instruments = [instruments[7]]

# settings for optimized drum kit for Raspberry Pi (with limited RAM)
if raspi_optimized_drumkit:
  disable_positional_sensing_support  = True
  only_master_channels_per_instrument = True
  for instrument in instruments: # remove some instruments for lowest possible memory requirement
    if "tom2" in instrument or "ride_side" in instrument or "crash_top" in instrument or "hihat_opentop" in instrument:
      instruments.remove(instrument)
  for instrument in instruments: # assign now missing MIDI notes to remaining instruments
    if "ride" in instrument:
      instrument[2].append(59)
    if "crash" in instrument:
      instrument[2].append(49)
    if "hihat_open" in instrument:
      instrument[2].append(46)


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

  # TEST for development purpose, remove later
  if disable_positional_sensing_support and len(positions) > 1:
    positions = [0]

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
    master_channel       = channel_names.index(instrument[1][0]) # first main channel is master
    min_strike_len       = int(instrument[4] * sample_rate) # calculate minimum strike length in samples
    min_time_next_strike = int(min_time_next_strike_s * sample_rate)

    # find samples which are above the threshold
    x            = np.square(sample[master_channel].astype(float))
    threshold    = np.power(10, instrument[5] / 10)
    above_thresh = x > threshold

    # remove oscillating by filling short gaps
    first_below_idx = -100 * sample_rate
    for i in range(1, len(above_thresh)):
      if not above_thresh[i] and above_thresh[i - 1]:
        first_below_idx = i
      if above_thresh[i] and not above_thresh[i - 1]:
        if i - first_below_idx < min_time_next_strike:
          above_thresh[range(first_below_idx, i)] = True

    # remove very short on periods
    first_above_idx = -100 * sample_rate
    for i in range(1, len(above_thresh)):
      if above_thresh[i] and not above_thresh[i - 1]:
        first_above_idx = i
      if not above_thresh[i] and above_thresh[i - 1]:
        if i - first_above_idx < min_strike_len:
          above_thresh[range(first_above_idx, i)] = False

    strike_start = np.argwhere(np.diff(above_thresh.astype(float)) > 0)
    strike_end   = np.argwhere(np.diff(above_thresh.astype(float)) < 0)

    # extract individual samples from long sample vector and analyze/process
    sample_powers[p]  = [[]] * len(strike_start)
    sample_strikes[p] = [[]] * len(strike_start)
    strike_cut_pos    = np.full(len(x), False)
    for i in range(0, len(strike_start)):

      # fix start of strike: find first sample going left of the maximum peak which
      # is below a threshold which is defined 20 dB below the maximum
      x_cur_strike_master = x[range(strike_start[i][0], strike_end[i][0])]
      strike_mean         = np.mean(x_cur_strike_master)
      strike_max          = np.max(x_cur_strike_master)
      below_max_thresh    = np.power(10, -thresh_from_max_for_start / 10) # -[20] dB from maximum peak
      index = 0
      while x[strike_start[i] + index] < strike_max * below_max_thresh:
        index += 1
      strike_start[i] += index - add_samples_at_start # add some offset

      # fix end position: compare regions of min strike length if next region power
      # is below previous region
      index = 0
      while strike_end[i] + index + 2 * min_strike_len < len(x) and \
            10 * np.log10(np.sum(x[strike_end[i][0] + index:strike_end[i][0] + index + min_strike_len])) - \
            10 * np.log10(np.sum(x[strike_end[i][0] + index + min_strike_len:strike_end[i][0] + index + 2 * min_strike_len])) > \
            0.0: # dB difference in power of the current two regions
        index += min_strike_len
      strike_end[i] += index

      # estimate power from master channel using the maximum value
      sample_powers[p][i] = strike_max / 32768 / 32768 # assuming 16 bit

      # extract sample data of current strike
      sample_strikes[p][i] = np.zeros((strike_end[i][0] - strike_start[i][0] + 1, num_channels), np.int16)
      for c in range(0, num_channels):
        strike_cut_pos[strike_start[i][0]:strike_end[i][0] + 1].fill(True) # for debugging
        sample_strikes[p][i][:, c] = sample[c][strike_start[i][0]:strike_end[i][0] + 1]

        # audio fade-out at the end
        sample_len = len(sample_strikes[p][i][:, c])
        fade_start = int(sample_len * (1 - fade_out_percent / 100))
        fade_len   = sample_len - fade_start
        sample_strikes[p][i][fade_start:, c] = np.int16(sample_strikes[p][i][fade_start:, c].astype(float) * np.arange(fade_len + 1, 1, -1) / fade_len)

      #print(sample_powers[p][i])
      #plt.plot(sample_strikes[p][i][:, master_channel])
      #plt.show()

    if len(instruments) == 1: # if only one instrument is selected, we assume we want to debug plot
      mpl.rcParams['agg.path.chunksize'] = 10000 # needed for long wave forms to avoid Exceeded cell block limit in Agg
      plt.plot(10 * np.log10(np.abs(x)))
      plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
      plt.plot(10 * np.log10(np.max(x)) * strike_cut_pos)
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

  # get indexes of main channels of this instrument
  instrument_master_channel_indexes = []
  for idx, channel_name in enumerate(channel_names):
    if channel_name in instrument[1]:
      instrument_master_channel_indexes.append(idx)

  for p in positions:
    power_sort_indexes = np.argsort(sample_powers[p])
    for i in range(0, len(sample_strikes[p])):
      strike_index = power_sort_indexes[i] # sort waves by power
      print(str(i) + ": " + str(10 * np.log10(sample_powers[p][strike_index])))
      # write multi-channel wave file
      sample_file_name = str(i + 1) + "-" + instrument_name
      if len(positions) > 1:
        sample_file_name += "-" + str(p)
      os.makedirs(instrument_sample_path, exist_ok=True)
      if only_master_channels_per_instrument:
        wavfile.write(instrument_sample_path + sample_file_name + ".wav", sample_rate, sample_strikes[p][strike_index][:, instrument_master_channel_indexes])
      else:
        wavfile.write(instrument_sample_path + sample_file_name + ".wav", sample_rate, sample_strikes[p][strike_index])

      # write XML content for current sample
      sample_xml = ET.SubElement(samples_xml, "sample")
      if len(positions) > 1:
        sample_xml.set("position", str(p))
      sample_xml.set("name", instrument_name + "-" + str(i + 1))
      # make sure result is positive by adding 100 dB (max. assumed dynamic)
      sample_xml.set("power", "{:.19f}".format(10 * np.log10(sample_powers[p][strike_index]) + 100))
      for j, channel_name in enumerate(channel_names):
        if only_master_channels_per_instrument:
          if channel_name in instrument[1]:
            audiofile_xml = ET.SubElement(sample_xml, "audiofile")
            audiofile_xml.set("channel", channel_name)
            audiofile_xml.set("file", samples_dir_name + "/" + sample_file_name + ".wav")
            audiofile_xml.set("filechannel", str(instrument[1].index(channel_name) + 1))
        else:
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
drumkit_xml.set("islogpower", "true")
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



