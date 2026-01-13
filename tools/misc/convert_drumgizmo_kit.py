#!/usr/bin/env python3

# Copyright (c) 2023-2026 Volker Fischer
# SPDX-License-Identifier: GPL-2.0-or-later

import os
import shutil
import numpy as np
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from scipy.io import wavfile

# specify the Drumgizmo kit path
#kit_path = "/home/corrados/edrumulus/tools/CrocellKit"
kit_path = "/home/corrados/edrumulus/tools/PearlMMX"

# get all intruments (each instrument must have its own directory and configuration XML file)
instruments = []
for file_name in os.listdir(kit_path):
  if os.path.isfile(kit_path + "/" + file_name + "/" + file_name + ".xml"):
    instruments.append(file_name);


# TEST select just one instrument for testing
print(instruments)
instruments = [instruments[16]]


# get all powers which are in the instrument XML files
xml_powers  = dict.fromkeys(instruments, [])
calc_powers = dict.fromkeys(instruments, [])
for instrument in instruments:
  print(instrument)
  if not os.path.isfile(kit_path + "/" + instrument + "/" + instrument + "_org.xml"):
    shutil.copyfile(kit_path + "/" + instrument + "/" + instrument + ".xml", \
                    kit_path + "/" + instrument + "/" + instrument + "_org.xml") # backup original file

  # parse instrument XML file
  tree = ET.parse(kit_path + "/" + instrument + "/" + instrument + "_org.xml")
  root = tree.getroot()
  for samples in root:
    cur_xml_powers  = []
    cur_calc_powers = []
    for sample in samples:
      cur_xml_powers.append(float(sample.attrib["power"]))

      # read wave form and calculate our own power
      samplerate, x = wavfile.read(filename = kit_path + "/" + instrument + "/" + sample[0].attrib["file"])

      max_power      = 0
      master_channel = 0
      for ch in range(0, np.size(x, 1)):
        x2        = np.square(x[:, ch].astype(float))
        x2_max    = np.max(x2)
        if max_power < x2_max:
          max_power      = x2_max
          master_channel = ch
      cur_calc_powers.append(max_power)
      sample.attrib["power"] = "{:.19f}".format(max_power)

    xml_powers[instrument]  = cur_xml_powers
    calc_powers[instrument] = cur_calc_powers

  # write instrument XML file
  ET.indent(tree, space="\t", level=0)
  tree.write(kit_path + "/" + instrument + "/" + instrument + "_new.xml", encoding="utf-8", xml_declaration="True")


  # debug plot
  plt.plot(10 * np.log10(xml_powers[instrument]), "-*")
  plt.plot(10 * np.log10(calc_powers[instrument]), "-*")
  xml_dynamic_db  = 10 * np.log10(np.max(xml_powers[instrument]) / np.min(xml_powers[instrument]))
  calc_dynamic_db = 10 * np.log10(np.max(calc_powers[instrument]) / np.min(calc_powers[instrument]))
  plt.title(instrument + " (XML dynamic: " + "{:.2f}".format(xml_dynamic_db) + " dB, CALC dynamic: " + "{:.2f}".format(calc_dynamic_db) + " dB)")
  plt.show()



