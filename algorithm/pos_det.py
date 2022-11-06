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
  ser = [] # in error case "ser" is a list (is checked below)
print('close plot window to quit')
click_point  = (None, None)
sensor1      = np.array(0.433, dtype=complex)
sensor1.imag = 0.25
sensor2      = np.array(0, dtype=complex)
sensor2.imag = -0.5
sensor3      = np.array(-0.433, dtype=complex)
sensor3.imag = 0.25
rim_radius   = 0.75
fig          = plt.figure(tight_layout=True)
gs           = gridspec.GridSpec(1, 1)
ax0          = fig.add_subplot(gs[0])
fignum       = fig.number
plt.ion()
plt.axis('square')


def onclick(event):
  global click_point
  click_point = (event.xdata, event.ydata)


def get_position(r1, r2):
  # code taken from https://github.com/corrados/edrumulus/discussions/70#discussioncomment-4014893
  # created by jstma:
  # these equations can be calculated in the initialization
  x0               = sensor1.real
  y0               = sensor1.imag
  x1               = sensor2.real
  y1               = sensor2.imag
  x2               = sensor3.real
  y2               = sensor3.imag
  x0_sq_plus_y0_sq = x0 * x0 + y0 * y0
  a1               = 2 * (x0 - x1)
  b1               = 2 * (y0 - y1)
  a2               = 2 * (x0 - x2)
  b2               = 2 * (y0 - y2)
  div1             = a1 * b2 - a2 * b1
  div2             = a2 * b1 - a1 * b2

  # these equations have to calculated for each position detection
  c1      = r1 * r1 + x0_sq_plus_y0_sq - x1 * x1 - y1 * y1
  c2      = r2 * r2 + x0_sq_plus_y0_sq - x2 * x2 - y2 * y2
  d1      = (2 * r1 * b2 - 2 * r2 * b1) / div1
  e1      = (    c1 * b2 -     c2 * b1) / div1
  d2      = (2 * r1 * a2 - 2 * r2 * a1) / div2
  e2      = (    c1 * a2 -     c2 * a1) / div2
  d_e1_x0 = e1 - x0
  d_e2_y0 = e2 - y0
  a       = d1 * d1 + d2 * d2 - 1
  b       = 2 * d_e1_x0 * d1 + 2 * d_e2_y0 * d2
  c       = d_e1_x0 * d_e1_x0 + d_e2_y0 * d_e2_y0

  # two solutions to the quadratic equation, only one solution seems to always be correct
  r_2 = (-b - np.sqrt(b * b - 4 * a * c)) / (2 * a)
  x   = d1 * r_2 + e1
  y   = d2 * r_2 + e2
  r   = np.sqrt(x * x + y * y)

  # clip calculated radius to rim radius
  if r > rim_radius:
    r = rim_radius
  return x, y, r


cid = fig.canvas.mpl_connect('button_press_event', onclick)
while True:
  ax0.cla()

  # pad edge boundary circle
  ax0.add_patch(mpatches.Circle((0.5, 0.5), rim_radius, fill=False, linewidth=4))
  plt.text(0.44, -0.17, "Drums", weight="bold")

  # sensors
  plt.scatter(sensor1.real + 0.5, sensor1.imag + 0.5, marker="x", c="g", s=150)
  plt.scatter(sensor2.real + 0.5, sensor2.imag + 0.5, marker="x", c="g", s=150)
  plt.scatter(sensor3.real + 0.5, sensor3.imag + 0.5, marker="x", c="g", s=150)

  # create current mouse click vector
  click_vector      = np.array(click_point[0], dtype=complex)
  click_vector.imag = click_point[1];
  click_vector     -= 0.5 + 0.5j

  l1 = np.abs(click_vector - sensor1)
  l2 = np.abs(click_vector - sensor2)
  l3 = np.abs(click_vector - sensor3)

  l21 = l2 - l1
  l31 = l3 - l1
  l32 = l3 - l2

  # approximation of distance: r = max(|L21|, |L31|, |L32|) (using some magic factor of 0.7)
  r_est = np.max(np.array([np.abs(l21), np.abs(l31), np.abs(l32)])) * 0.7

  # get position algorithm
  x_get, y_get, r_get = get_position(l21, l31)

  # show ideal circle (dashed) and circle based on approximation
  ax0.add_patch(mpatches.Circle((0.5, 0.5), np.abs(click_vector), fill=False, color="b", ls='--'))
  ax0.add_patch(mpatches.Circle((0.5, 0.5), r_est, fill=False, color="b", lw=2))

  # show click point and get position point
  plt.scatter(click_point[0], click_point[1], marker="*", c="r", s=100)
  plt.scatter(x_get + 0.5, y_get + 0.5, marker="*", c="g", s=100)

  # for a test return measured differences via a serial string from Edrumulus
  if type(ser) is not list: # check for valid "ser" object
    a = ser.readline().decode("utf-8")
    if len(a) > 0:
      a = a.split(",")[0:3]
      a = [int(x) / 17 for x in a]
      print(a)
      x_get, y_get, r_get = get_position(a[0], a[1])
      if np.sqrt(x_get * x_get + y_get * y_get) <= rim_radius:
        plt.scatter(x_get + 0.5, y_get + 0.5, marker="*", c="b", s=700)
      ax0.add_patch(mpatches.Circle((0.5, 0.5), r_get, fill=False, color="g", ls='--'))
      plt.pause(0.05)

  plt.show()
  plt.pause(0.03)
  if not plt.fignum_exists(fignum): # if plot window is closed then quit
    break

