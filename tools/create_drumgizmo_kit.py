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


################################################################################
# CONFIGURATION AND INITIALIZATIONS ############################################
################################################################################
# instruments: [instrument_name, master_channel, MIDI_note, threshold]
instruments = [["kick",          "KDrum",   36, 45], \
               ["snare",         "Snare",   38, 70], \
               ["snare_rimshot", "Snare",   40, 60], \
               ["hihat_closed",  "Hihat",   22, 68], \
               ["hihat_open",    "Hihat",   26, 53], \
               ["tom1",          "Tom1",    48, 60], \
               ["tom2",          "Tom2",    45, 60], \
               ["tom3",          "Tom3",    43, 57], \
               ["crash",         "OHLeft",  55, 60], \
               ["ride",          "OHRight", 51, 68], \
               ["ride_bell",     "OHRight", 53, 60]]

# TEST for optimizing the analization algorithms, only use one instrument
#instruments = [instruments[1]]
disable_positional_sensing_support = False

kit_name                = "PearlMMX" # avoid spaces
samples_dir_name        = "samples" # compatible to other Drumgizmo kits
source_samples_dir_name = "source_samples" # root directory of recorded source samples
kit_description         = "Pearl MMX drum set with positional sensing support"
channel_names           = ["KDrum", "Snare", "Hihat", "Tom1", "Tom2", "Tom3", "OHLeft", "OHRight"]
min_strike_len          = 0.25 # seconds
fade_out_percent        = 10 # % of sample at the end is faded out
num_channels            = len(channel_names)


for instrument in instruments:

  ##############################################################################
  # FILE NAME HANDLING #########################################################
  ##############################################################################
  instrument_name        = instrument[0]
  instrument_path        = kit_name + "/" + instrument_name + "/"
  instrument_sample_path = instrument_path + samples_dir_name + "/"
  base_instrument_name   = instrument_name.split("_")[0]
  print(instrument_name)

  # check if instrument has positional sensing support and extract position indexes
  file_names = os.listdir(source_samples_dir_name + "/" + base_instrument_name)
  positions  = []
  for i, file_name in enumerate(file_names):
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
    sample  = [[]] * num_channels
    pos_str = "_" + str(p) if p >= 0 else ""
    for i in range(0, num_channels):
      with wave.open(source_samples_dir_name + "/" + base_instrument_name + "/" + \
                     instrument_name + pos_str + "_channel" + str(i + 1) + ".wav", "r") as file:
        sample_rate = file.getframerate() # assuming all wave have the same rate
        sample[i]   = np.frombuffer(file.readframes(-1), np.int16) # assuming 16 bit


    ##############################################################################
    # WAVE FORM ANALYSIS #########################################################
    ##############################################################################
    thresh_from_max = instrument[3] # dB from maximum peak
    master_channel  = channel_names.index(instrument[1])

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
      # estimate power from master channel using the maximum value
      x_cur_strike_master = x[range(start[0], end[0])]
      sample_powers[p][i]    = str(np.max(x_cur_strike_master) / 32768 / 32768) # assuming 16 bit

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

    #plt.plot(10 * np.log10(np.abs(x)))
    #plt.plot([0, len(x)], 10 * np.log10([threshold, threshold]))
    #plt.plot(10 * np.log10(np.max(x)) * above_thresh)
    #plt.title(instrument_name)
    #plt.show()


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
  instrument_xml.set("file", instrument[0] + "/" + instrument[0] + ".xml")
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
for instrument in instruments:
  map_xml = ET.SubElement(midimap_xml, "map")
  map_xml.set("note", str(instrument[2]))
  map_xml.set("instr", instrument[0])
tree_xml = ET.ElementTree(midimap_xml)
ET.indent(midimap_xml, space="\t", level=0)
tree_xml.write(kit_name + "/Midimap.xml", encoding="utf-8", xml_declaration="True")



