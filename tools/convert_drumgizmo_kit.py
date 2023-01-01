#!/usr/bin/env python3

#*******************************************************************************
# Copyright (c) 2023-2023
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
import numpy as np
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from scipy.io import wavfile

# specify the Drumgizmo kit path
kit_path = "/home/corrados/edrumulus/tools/CrocellKit"

# get all intruments (each instrument must have its own directory and configuration XML file)
instruments = []
for file_name in os.listdir(kit_path):
  if os.path.isfile(kit_path + "/" + file_name + "/" + file_name + ".xml"):
    instruments.append(file_name);


print(instruments)


# TEST
#filename = "/home/corrados/edrumulus/tools/CrocellKit/Snare/samples/75-Snare.wav"
#samplerate, x = wavfile.read(filename)

#for i in range(0, 32):
#  ch = x[:, i]

#  plt.plot(10 * np.log10(np.abs(ch)))
#  plt.show()



