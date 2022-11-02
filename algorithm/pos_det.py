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

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.patches as mpatches
import serial

# initializations
try:
  ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0.01)
except:
  ser = [] # in error case ser is a list (is checked below)
print('close plot window to quit')
click_point  = (0.5, 0.5)
sensor1      = np.array(0.5, dtype=complex)
sensor1.imag = 1;
sensor2      = np.array(0.066987, dtype=complex)
sensor2.imag = 0.25;
sensor3      = np.array(0.933, dtype=complex)
sensor3.imag = 0.25;
fig          = plt.figure(tight_layout=True)
gs           = gridspec.GridSpec(1, 1)
ax0          = fig.add_subplot(gs[0])
fignum       = fig.number
plt.ion()

def onclick(event):
  global click_point
  click_point = (event.xdata, event.ydata)

cid = fig.canvas.mpl_connect('button_press_event', onclick)
while True:
  ax0.cla()

  # pad edge boundary circle
  ax0.add_patch(mpatches.Circle((0.5, 0.5), 0.5, fill=False, linewidth=4))

  # sensors
  plt.scatter(sensor1.real, sensor1.imag, marker="x", c="g", s=150)
  plt.scatter(sensor2.real, sensor2.imag, marker="x", c="g", s=150)
  plt.scatter(sensor3.real, sensor3.imag, marker="x", c="g", s=150)

  # create current mouse click vector
  click_vector      = np.array(click_point[0], dtype=complex)
  click_vector.imag = click_point[1];

  l1 = np.abs(click_vector - sensor1)
  l2 = np.abs(click_vector - sensor2)
  l3 = np.abs(click_vector - sensor3)

  l21 = l2 - l1
  l31 = l3 - l1
  l32 = l3 - l2

  # approximation of distance: r = max(|L21|, |L31|, |L32|) (using some magic factor of 0.7)
  r_est = np.max(np.array([np.abs(l21), np.abs(l31), np.abs(l32)])) * 0.7

  # show ideal circle (dashed) and circle based on approximation
  ax0.add_patch(mpatches.Circle((0.5, 0.5), np.abs(click_vector - (0.5 + 0.5j)), fill=False, color="b", ls='--'))
  ax0.add_patch(mpatches.Circle((0.5, 0.5), r_est, fill=False, color="b", lw=2))

  # show click point
  plt.scatter(click_point[0], click_point[1], marker="*", c="r", s=100)


  # for a test return measured differences via a serial string from Edrumulus
  if type(ser) is not list: # check for valid ser object
    a = ser.readline().decode("utf-8")
    if len(a) > 0:
      a = a.split(",")[0:3]
      a = [int(x) for x in a]
      print(a)



  plt.show()
  plt.pause(0.03)
  if not plt.fignum_exists(fignum): # if plot window is closed then quit
    break

