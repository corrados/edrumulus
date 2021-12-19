EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Edrumulus Prototype 2 (ESP32)"
Date ""
Rev "0.1"
Comp ""
Comment1 "Library: ESP32 board taken from  MightyMirko/esp32_devkit_38Pins"
Comment2 "DISCLAIMER: Use at your own risk."
Comment3 "Simple front end design."
Comment4 ""
$EndDescr
$Comp
L pspice:R R3
U 1 1 61BE1FA0
P 2050 1600
F 0 "R3" V 2225 1550 50  0000 L CNN
F 1 "100 kOhm" V 2150 1400 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 1600 50  0001 C CNN
F 3 "~" H 2050 1600 50  0001 C CNN
	1    2050 1600
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R17
U 1 1 61BFFA7F
P 3050 1450
F 0 "R17" V 2875 1375 50  0000 L CNN
F 1 "22 kOhm" V 2950 1300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 1450 50  0001 C CNN
F 3 "~" H 3050 1450 50  0001 C CNN
	1    3050 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 1150 2800 1150
Connection ~ 2800 1150
Wire Wire Line
	2800 1150 2800 1450
Wire Wire Line
	2700 1150 2800 1150
Wire Wire Line
	2400 1150 2300 1150
Wire Wire Line
	3200 1150 3300 1150
Connection ~ 2800 1450
Wire Wire Line
	2300 1150 2300 1450
$Comp
L pspice:R R9
U 1 1 61BFFA9A
P 2550 1450
F 0 "R9" V 2375 1400 50  0000 L CNN
F 1 "22 kOhm" V 2450 1300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 1450 50  0001 C CNN
F 3 "~" H 2550 1450 50  0001 C CNN
	1    2550 1450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R18
U 1 1 61C0679C
P 3050 2150
F 0 "R18" V 3225 2075 50  0000 L CNN
F 1 "22 kOhm" V 3150 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 2150 50  0001 C CNN
F 3 "~" H 3050 2150 50  0001 C CNN
	1    3050 2150
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D2
U 1 1 61C067A2
P 2550 1850
F 0 "D2" H 2600 2050 50  0000 R CNN
F 1 "BAT85" H 2650 1975 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 1850 50  0001 C CNN
	1    2550 1850
	-1   0    0    1   
$EndComp
Connection ~ 2800 1850
Wire Wire Line
	2800 1850 2800 2150
Wire Wire Line
	2700 1850 2800 1850
Wire Wire Line
	2400 1850 2300 1850
Wire Wire Line
	3200 1850 3300 1850
Wire Wire Line
	3300 1850 3300 2150
$Comp
L Diode:BAT85 D10
U 1 1 61C067AF
P 3050 1850
F 0 "D10" H 3050 2050 50  0000 C CNN
F 1 "BAT85" H 3050 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 1850 50  0001 C CNN
	1    3050 1850
	-1   0    0    1   
$EndComp
Connection ~ 2800 2150
$Comp
L pspice:R R10
U 1 1 61C067B7
P 2550 2150
F 0 "R10" V 2725 2075 50  0000 L CNN
F 1 "22 kOhm" V 2650 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 2150 50  0001 C CNN
F 3 "~" H 2550 2150 50  0001 C CNN
	1    2550 2150
	0    1    1    0   
$EndComp
$Comp
L pspice:R R1
U 1 1 61C30414
P 2050 1700
F 0 "R1" V 1875 1650 50  0000 L CNN
F 1 "100 kOhm" V 1950 1500 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 1700 50  0001 C CNN
F 3 "~" H 2050 1700 50  0001 C CNN
	1    2050 1700
	0    -1   -1   0   
$EndComp
$Comp
L doit-esp32-devkit-v1:ESP32-DEVKIT-V1 U1
U 1 1 61B5316D
P 7750 4900
F 0 "U1" H 7725 5465 50  0000 C CNN
F 1 "ESP32-DEVKIT-V1" H 7725 5374 50  0000 C CNN
F 2 "edrumulus:esp32_devkit_v1" H 7700 5350 50  0001 C CNN
F 3 "" H 7700 5350 50  0001 C CNN
	1    7750 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 1150 3300 1450
Text GLabel 6800 5900 0    50   Input ~ 0
GND
Text GLabel 6800 4700 0    50   Input ~ 0
GPIO36
Text GLabel 6800 4800 0    50   Input ~ 0
GPIO39
Text GLabel 6800 5000 0    50   Input ~ 0
GPIO35
$Comp
L Diode:BAT85 D1
U 1 1 61BFFA85
P 2550 1150
F 0 "D1" H 2600 950 50  0000 R CNN
F 1 "BAT85" H 2650 1050 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 1150 50  0001 C CNN
	1    2550 1150
	-1   0    0    1   
$EndComp
Text GLabel 2900 1600 2    50   Input ~ 0
GPIO35
Wire Wire Line
	2900 1850 2800 1850
Wire Wire Line
	2900 1700 2800 1700
Wire Wire Line
	2800 1700 2800 1850
Wire Wire Line
	2900 1600 2800 1600
Wire Wire Line
	2800 1600 2800 1450
Wire Wire Line
	2300 1600 2800 1600
Connection ~ 2800 1600
Wire Wire Line
	2300 1700 2800 1700
Connection ~ 2800 1700
Connection ~ 2300 1150
Wire Wire Line
	1800 1150 2300 1150
Text GLabel 1700 1150 0    50   Input ~ 0
GND
Wire Wire Line
	3300 1450 3300 1850
Connection ~ 3300 1450
Connection ~ 3300 1850
Text GLabel 3400 1150 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D9
U 1 1 61BFFA92
P 3050 1150
F 0 "D9" H 3050 950 50  0000 C CNN
F 1 "BAT85" H 3050 1050 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 1150 50  0001 C CNN
	1    3050 1150
	-1   0    0    1   
$EndComp
Connection ~ 3300 1150
$Comp
L Connector:AudioJack3 J1
U 1 1 61BCFE99
P 1600 1600
F 0 "J1" H 1550 1275 50  0000 C CNN
F 1 "Snare" H 1550 1350 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1600 1600 50  0001 C CNN
F 3 "~" H 1600 1600 50  0001 C CNN
	1    1600 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 1850 2300 2150
Wire Notes Line
	3700 850  1100 850 
Text Notes 1150 950  0    50   ~ 0
Snare Trigger Input
Wire Notes Line
	3700 850  3700 2450
Wire Notes Line
	1100 2450 3700 2450
Wire Notes Line
	1100 850  1100 2450
Text GLabel 2900 1700 2    50   Input ~ 0
GPIO36
Text GLabel 6800 4900 0    50   Input ~ 0
GPIO34
Text GLabel 6800 5100 0    50   Input ~ 0
GPIO32
Text GLabel 6800 5200 0    50   Input ~ 0
GPIO33
Text GLabel 6800 5300 0    50   Input ~ 0
GPIO25
Text GLabel 6800 5400 0    50   Input ~ 0
GPIO26
Text GLabel 6800 5500 0    50   Input ~ 0
GPIO27
Text GLabel 6800 5600 0    50   Input ~ 0
GPIO14
Text GLabel 6800 5700 0    50   Input ~ 0
GPIO12
Text GLabel 6800 5800 0    50   Input ~ 0
GPIO13
$Comp
L pspice:R R33
U 1 1 61D09D3E
P 5675 2000
F 0 "R33" V 5850 1925 50  0000 L CNN
F 1 "22 kOhm" V 5775 1850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5675 2000 50  0001 C CNN
F 3 "~" H 5675 2000 50  0001 C CNN
	1    5675 2000
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D17
U 1 1 61D09D44
P 5175 1700
F 0 "D17" H 5250 1900 50  0000 R CNN
F 1 "BAT85" H 5275 1825 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5175 1525 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5175 1700 50  0001 C CNN
	1    5175 1700
	-1   0    0    1   
$EndComp
Connection ~ 5425 1700
Wire Wire Line
	5425 1700 5425 2000
Wire Wire Line
	5325 1700 5425 1700
Wire Wire Line
	5025 1700 4925 1700
Wire Wire Line
	5825 1700 5925 1700
Wire Wire Line
	5925 1700 5925 2000
$Comp
L Diode:BAT85 D21
U 1 1 61D09D50
P 5675 1700
F 0 "D21" H 5675 1900 50  0000 C CNN
F 1 "BAT85" H 5675 1825 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5675 1525 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5675 1700 50  0001 C CNN
	1    5675 1700
	-1   0    0    1   
$EndComp
Connection ~ 5425 2000
$Comp
L pspice:R R29
U 1 1 61D09D57
P 5175 2000
F 0 "R29" V 5350 1925 50  0000 L CNN
F 1 "22 kOhm" V 5275 1850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5175 2000 50  0001 C CNN
F 3 "~" H 5175 2000 50  0001 C CNN
	1    5175 2000
	0    1    1    0   
$EndComp
$Comp
L pspice:R R25
U 1 1 61D09D5D
P 4675 1550
F 0 "R25" V 4500 1475 50  0000 L CNN
F 1 "100 kOhm" V 4575 1350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4675 1550 50  0001 C CNN
F 3 "~" H 4675 1550 50  0001 C CNN
	1    4675 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5525 1700 5425 1700
Wire Wire Line
	5425 1550 5425 1700
Wire Wire Line
	4925 1550 5425 1550
Connection ~ 5425 1550
Text GLabel 4525 1350 2    50   Input ~ 0
GND
Text GLabel 5975 2000 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J5
U 1 1 61D09D89
P 4225 1450
F 0 "J5" H 4175 1100 50  0000 C CNN
F 1 "Kick" H 4175 1200 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4225 1450 50  0001 C CNN
F 3 "~" H 4225 1450 50  0001 C CNN
	1    4225 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4925 1700 4925 2000
Wire Notes Line
	6350 850  3750 850 
Text Notes 3800 950  0    50   ~ 0
Kick Trigger Input
Wire Notes Line
	6350 850  6350 2450
Wire Notes Line
	3750 2450 6350 2450
Wire Notes Line
	3750 850  3750 2450
Text GLabel 5825 1550 2    50   Input ~ 0
GPIO33
Wire Notes Line
	6350 2500 3750 2500
Text Notes 3800 2600 0    50   ~ 0
Tom1 Trigger Input
Wire Notes Line
	6350 2500 6350 4100
Wire Notes Line
	3750 4100 6350 4100
Wire Notes Line
	3750 2500 3750 4100
Wire Notes Line
	6350 4150 3750 4150
Text Notes 3800 4250 0    50   ~ 0
Tom2 Trigger Input
Wire Notes Line
	3750 5750 6350 5750
Wire Notes Line
	3750 4150 3750 5750
Wire Notes Line
	6350 5800 3750 5800
Text Notes 3800 5900 0    50   ~ 0
Tom3 Trigger Input
Wire Notes Line
	6350 5800 6350 7400
Wire Notes Line
	3750 7400 6350 7400
Wire Notes Line
	3750 5800 3750 7400
$Comp
L pspice:R R38
U 1 1 61D81CAE
P 8375 2000
F 0 "R38" V 8550 1925 50  0000 L CNN
F 1 "10 kOhm" V 8475 1850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8375 2000 50  0001 C CNN
F 3 "~" H 8375 2000 50  0001 C CNN
	1    8375 2000
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D25
U 1 1 61D81CB4
P 7875 2000
F 0 "D25" H 7950 2200 50  0000 R CNN
F 1 "BAT85" H 7975 2125 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 7875 1825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 7875 2000 50  0001 C CNN
	1    7875 2000
	-1   0    0    1   
$EndComp
Connection ~ 8125 1700
Wire Wire Line
	8125 1700 8125 2000
Wire Wire Line
	8525 1700 8625 1700
Wire Wire Line
	8625 1700 8625 2000
$Comp
L Diode:BAT85 D26
U 1 1 61D81CC0
P 8375 1700
F 0 "D26" H 8375 1900 50  0000 C CNN
F 1 "BAT85" H 8375 1825 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 8375 1525 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 8375 1700 50  0001 C CNN
	1    8375 1700
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R37
U 1 1 61D81CC7
P 7925 1400
F 0 "R37" V 7750 1325 50  0000 L CNN
F 1 "10 kOhm" V 7825 1250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 7925 1400 50  0001 C CNN
F 3 "~" H 7925 1400 50  0001 C CNN
	1    7925 1400
	0    1    1    0   
$EndComp
Wire Wire Line
	8225 1700 8125 1700
Text GLabel 7225 1350 2    50   Input ~ 0
GND
Text GLabel 8675 1700 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J9
U 1 1 61D81CDE
P 6925 1450
F 0 "J9" H 6875 1100 50  0000 C CNN
F 1 "Hi-Hat Ctrl" H 6875 1200 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 6925 1450 50  0001 C CNN
F 3 "~" H 6925 1450 50  0001 C CNN
	1    6925 1450
	1    0    0    -1  
$EndComp
Text Notes 6450 950  0    50   ~ 0
Hi-Hat Control Input
Wire Notes Line
	6400 850  6400 2450
Wire Wire Line
	8675 1700 8625 1700
Connection ~ 8625 1700
Wire Wire Line
	8125 1700 8125 1550
Wire Wire Line
	8125 1550 7675 1550
Text GLabel 8525 1400 2    50   Input ~ 0
GPIO25
Wire Wire Line
	8025 2000 8125 2000
Connection ~ 8125 2000
Wire Wire Line
	5975 2000 5925 2000
Connection ~ 5925 2000
Wire Wire Line
	5425 1550 5825 1550
Wire Wire Line
	8525 1400 8175 1400
Wire Wire Line
	7675 1400 7675 1550
Connection ~ 7675 1550
Wire Wire Line
	7675 1550 7125 1550
$Comp
L Isolator:6N138 U2
U 1 1 61C14EE1
P 7950 3100
F 0 "U2" H 7800 3550 50  0000 C CNN
F 1 "6N138" H 7800 3450 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket" H 8240 2800 50  0001 C CNN
F 3 "http://www.onsemi.com/pub/Collateral/HCPL2731-D.pdf" H 8240 2800 50  0001 C CNN
	1    7950 3100
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R39
U 1 1 61C18FC5
P 7400 2850
F 0 "R39" V 7200 2750 50  0000 L CNN
F 1 "220 Ohm" V 7300 2650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 7400 2850 50  0001 C CNN
F 3 "~" H 7400 2850 50  0001 C CNN
	1    7400 2850
	0    1    1    0   
$EndComp
$Comp
L pspice:R R42
U 1 1 61C225F8
P 8500 2700
F 0 "R42" V 8400 2600 50  0000 L CNN
F 1 "220 Ohm" V 8600 2550 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8500 2700 50  0001 C CNN
F 3 "~" H 8500 2700 50  0001 C CNN
	1    8500 2700
	0    1    1    0   
$EndComp
$Comp
L pspice:R R43
U 1 1 61C2BBEE
P 8500 3000
F 0 "R43" V 8400 2900 50  0000 L CNN
F 1 "4.7 kOhm" V 8600 2850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8500 3000 50  0001 C CNN
F 3 "~" H 8500 3000 50  0001 C CNN
	1    8500 3000
	0    1    1    0   
$EndComp
$Comp
L Connector:DIN-5_180degree J10
U 1 1 61C36D48
P 6850 2750
F 0 "J10" H 7200 2700 50  0000 C CNN
F 1 "DIN-5" H 7150 2600 50  0000 C CNN
F 2 "" H 6850 2750 50  0001 C CNN
F 3 "http://www.mouser.com/ds/2/18/40_c091_abd_e-75918.pdf" H 6850 2750 50  0001 C CNN
	1    6850 2750
	-1   0    0    1   
$EndComp
Text GLabel 8750 3000 2    50   Input ~ 0
GND
Text GLabel 8900 3300 0    50   Input ~ 0
UART_RX
Text GLabel 8250 3300 2    50   Input ~ 0
GND
Text GLabel 8150 2700 0    50   Input ~ 0
3V3
Wire Wire Line
	8250 2700 8250 2900
Wire Wire Line
	8750 2700 9000 2700
Wire Wire Line
	9000 2700 9000 3200
Wire Wire Line
	9000 3200 8250 3200
Wire Wire Line
	8150 2700 8250 2700
Connection ~ 8250 2700
$Comp
L pspice:R R41
U 1 1 61D7604A
P 7200 3900
F 0 "R41" V 7300 3800 50  0000 L CNN
F 1 "220 Ohm" V 7400 3750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 7200 3900 50  0001 C CNN
F 3 "~" H 7200 3900 50  0001 C CNN
	1    7200 3900
	0    1    1    0   
$EndComp
$Comp
L pspice:R R40
U 1 1 61D80C82
P 8300 3900
F 0 "R40" V 8100 3850 50  0000 L CNN
F 1 "220 Ohm" V 8200 3750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8300 3900 50  0001 C CNN
F 3 "~" H 8300 3900 50  0001 C CNN
	1    8300 3900
	0    1    1    0   
$EndComp
Text GLabel 6950 3900 0    50   Input ~ 0
UART_TX
Text GLabel 6850 3050 3    50   Input ~ 0
GND
Text GLabel 8550 3900 2    50   Input ~ 0
3V3
Wire Wire Line
	9000 3200 9000 3300
Wire Wire Line
	9000 3300 8900 3300
Connection ~ 9000 3200
Wire Notes Line
	6400 3500 9100 3500
Wire Notes Line
	6400 2500 9100 2500
Text Notes 8750 3450 0    50   ~ 0
MIDI IN
Wire Notes Line
	6400 2450 9100 2450
Wire Notes Line
	6400 850  9100 850 
Wire Notes Line
	9100 850  9100 2450
NoConn ~ 4425 1450
NoConn ~ 7125 1450
NoConn ~ 8650 5900
NoConn ~ 8650 5700
NoConn ~ 8650 5300
NoConn ~ 8650 5200
NoConn ~ 8650 5100
NoConn ~ 8650 5000
NoConn ~ 8650 4900
NoConn ~ 8650 4800
NoConn ~ 8650 4700
NoConn ~ 8650 4600
Text GLabel 8650 6000 2    50   Input ~ 0
3V3
Text GLabel 8650 5800 2    50   Input ~ 0
GPIO15
Text GLabel 8650 5500 2    50   Input ~ 0
UART_RX
Text GLabel 8650 5400 2    50   Input ~ 0
UART_TX
NoConn ~ 8650 5600
NoConn ~ 6800 4600
NoConn ~ 6800 6000
$Comp
L Connector:DIN-5_180degree J11
U 1 1 61C161B7
P 7750 3800
F 0 "J11" H 8150 3750 50  0000 C CNN
F 1 "DIN-5" H 8050 3650 50  0000 C CNN
F 2 "" H 7750 3800 50  0001 C CNN
F 3 "http://www.mouser.com/ds/2/18/40_c091_abd_e-75918.pdf" H 7750 3800 50  0001 C CNN
	1    7750 3800
	-1   0    0    1   
$EndComp
Text GLabel 7850 4100 2    50   Input ~ 0
GND
Wire Wire Line
	7650 2850 7650 3000
Wire Wire Line
	6550 2850 6550 3300
Wire Wire Line
	7650 3300 7650 3200
Wire Wire Line
	7450 3300 7650 3300
Wire Wire Line
	6550 3300 7450 3300
Connection ~ 7450 3300
$Comp
L Diode:1N914 D27
U 1 1 61C357CF
P 7450 3150
F 0 "D27" V 7450 2950 50  0000 L CNN
F 1 "1N914" V 7350 2900 50  0000 L CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 7450 2975 50  0001 C CNN
F 3 "http://www.vishay.com/docs/85622/1n914.pdf" H 7450 3150 50  0001 C CNN
	1    7450 3150
	0    1    1    0   
$EndComp
Wire Wire Line
	7450 3000 7650 3000
Connection ~ 7650 3000
Wire Notes Line
	9100 2500 9100 3500
Wire Notes Line
	6400 2500 6400 3500
Wire Notes Line
	9100 3550 6400 3550
Text Notes 8700 4150 0    50   ~ 0
MIDI OUT
Wire Notes Line
	9100 3550 9100 4200
Wire Notes Line
	9100 4200 6400 4200
Wire Notes Line
	6400 4200 6400 3550
Wire Wire Line
	7850 4100 7750 4100
NoConn ~ 8050 3800
NoConn ~ 7450 3800
NoConn ~ 7150 2750
NoConn ~ 6550 2750
Text Notes 6450 3100 0    157  Italic 31
NOT YET SUPPORTED!
Text Notes 6450 3950 0    157  Italic 31
NOT YET SUPPORTED!
Text GLabel 2200 2150 0    50   Input ~ 0
GND
Connection ~ 2300 2150
Wire Wire Line
	1800 1150 1800 1500
Wire Wire Line
	1700 1150 1800 1150
Connection ~ 1800 1150
Wire Wire Line
	2200 2150 2300 2150
$Comp
L pspice:R R2
U 1 1 61D71A8A
P 2050 3250
F 0 "R2" V 2225 3200 50  0000 L CNN
F 1 "100 kOhm" V 2150 3050 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 3250 50  0001 C CNN
F 3 "~" H 2050 3250 50  0001 C CNN
	1    2050 3250
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R19
U 1 1 61D71A90
P 3050 3100
F 0 "R19" V 2875 3025 50  0000 L CNN
F 1 "22 kOhm" V 2950 2950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 3100 50  0001 C CNN
F 3 "~" H 3050 3100 50  0001 C CNN
	1    3050 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 2800 2800 2800
Connection ~ 2800 2800
Wire Wire Line
	2800 2800 2800 3100
Wire Wire Line
	2700 2800 2800 2800
Wire Wire Line
	2400 2800 2300 2800
Wire Wire Line
	3200 2800 3300 2800
Connection ~ 2800 3100
Wire Wire Line
	2300 2800 2300 3100
$Comp
L pspice:R R11
U 1 1 61D71A9E
P 2550 3100
F 0 "R11" V 2375 3050 50  0000 L CNN
F 1 "22 kOhm" V 2450 2950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 3100 50  0001 C CNN
F 3 "~" H 2550 3100 50  0001 C CNN
	1    2550 3100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R20
U 1 1 61D71AA4
P 3050 3800
F 0 "R20" V 3225 3725 50  0000 L CNN
F 1 "22 kOhm" V 3150 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 3800 50  0001 C CNN
F 3 "~" H 3050 3800 50  0001 C CNN
	1    3050 3800
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D4
U 1 1 61D71AAA
P 2550 3500
F 0 "D4" H 2600 3700 50  0000 R CNN
F 1 "BAT85" H 2650 3625 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 3500 50  0001 C CNN
	1    2550 3500
	-1   0    0    1   
$EndComp
Connection ~ 2800 3500
Wire Wire Line
	2800 3500 2800 3800
Wire Wire Line
	2700 3500 2800 3500
Wire Wire Line
	2400 3500 2300 3500
Wire Wire Line
	3200 3500 3300 3500
Wire Wire Line
	3300 3500 3300 3800
$Comp
L Diode:BAT85 D12
U 1 1 61D71AB6
P 3050 3500
F 0 "D12" H 3050 3700 50  0000 C CNN
F 1 "BAT85" H 3050 3625 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 3500 50  0001 C CNN
	1    3050 3500
	-1   0    0    1   
$EndComp
Connection ~ 2800 3800
$Comp
L pspice:R R12
U 1 1 61D71ABD
P 2550 3800
F 0 "R12" V 2725 3725 50  0000 L CNN
F 1 "22 kOhm" V 2650 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 3800 50  0001 C CNN
F 3 "~" H 2550 3800 50  0001 C CNN
	1    2550 3800
	0    1    1    0   
$EndComp
$Comp
L pspice:R R4
U 1 1 61D71AC3
P 2050 3350
F 0 "R4" V 1875 3300 50  0000 L CNN
F 1 "100 kOhm" V 1950 3150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 3350 50  0001 C CNN
F 3 "~" H 2050 3350 50  0001 C CNN
	1    2050 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 2800 3300 3100
$Comp
L Diode:BAT85 D3
U 1 1 61D71ACA
P 2550 2800
F 0 "D3" H 2600 2600 50  0000 R CNN
F 1 "BAT85" H 2650 2700 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 2625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 2800 50  0001 C CNN
	1    2550 2800
	-1   0    0    1   
$EndComp
Text GLabel 2900 3250 2    50   Input ~ 0
GPIO26
Wire Wire Line
	2900 3500 2800 3500
Wire Wire Line
	2900 3350 2800 3350
Wire Wire Line
	2800 3350 2800 3500
Wire Wire Line
	2900 3250 2800 3250
Wire Wire Line
	2800 3250 2800 3100
Wire Wire Line
	2300 3250 2800 3250
Connection ~ 2800 3250
Wire Wire Line
	2300 3350 2800 3350
Connection ~ 2800 3350
Connection ~ 2300 2800
Wire Wire Line
	1800 2800 2300 2800
Text GLabel 1700 2800 0    50   Input ~ 0
GND
Wire Wire Line
	3300 3100 3300 3500
Connection ~ 3300 3100
Connection ~ 3300 3500
Text GLabel 3400 2800 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D11
U 1 1 61D71AE1
P 3050 2800
F 0 "D11" H 3050 2600 50  0000 C CNN
F 1 "BAT85" H 3050 2700 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 2625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 2800 50  0001 C CNN
	1    3050 2800
	-1   0    0    1   
$EndComp
Connection ~ 3300 2800
$Comp
L Connector:AudioJack3 J2
U 1 1 61D71AE9
P 1600 3250
F 0 "J2" H 1550 2925 50  0000 C CNN
F 1 "Snare" H 1550 3000 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1600 3250 50  0001 C CNN
F 3 "~" H 1600 3250 50  0001 C CNN
	1    1600 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 3500 2300 3800
Wire Notes Line
	3700 2500 1100 2500
Text Notes 1150 2600 0    50   ~ 0
Hi-Hat Trigger Input
Wire Notes Line
	3700 2500 3700 4100
Wire Notes Line
	1100 4100 3700 4100
Wire Notes Line
	1100 2500 1100 4100
Text GLabel 2900 3350 2    50   Input ~ 0
GPIO32
Text GLabel 2200 3800 0    50   Input ~ 0
GND
Connection ~ 2300 3800
Wire Wire Line
	1800 2800 1800 3150
Wire Wire Line
	1700 2800 1800 2800
Connection ~ 1800 2800
Wire Wire Line
	2200 3800 2300 3800
$Comp
L pspice:R R5
U 1 1 61DC5768
P 2050 4900
F 0 "R5" V 2225 4850 50  0000 L CNN
F 1 "100 kOhm" V 2150 4700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 4900 50  0001 C CNN
F 3 "~" H 2050 4900 50  0001 C CNN
	1    2050 4900
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R21
U 1 1 61DC576E
P 3050 4750
F 0 "R21" V 2875 4675 50  0000 L CNN
F 1 "22 kOhm" V 2950 4600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 4750 50  0001 C CNN
F 3 "~" H 3050 4750 50  0001 C CNN
	1    3050 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 4450 2800 4450
Connection ~ 2800 4450
Wire Wire Line
	2800 4450 2800 4750
Wire Wire Line
	2700 4450 2800 4450
Wire Wire Line
	2400 4450 2300 4450
Wire Wire Line
	3200 4450 3300 4450
Connection ~ 2800 4750
Wire Wire Line
	2300 4450 2300 4750
$Comp
L pspice:R R13
U 1 1 61DC577C
P 2550 4750
F 0 "R13" V 2375 4700 50  0000 L CNN
F 1 "22 kOhm" V 2450 4600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 4750 50  0001 C CNN
F 3 "~" H 2550 4750 50  0001 C CNN
	1    2550 4750
	0    1    1    0   
$EndComp
$Comp
L pspice:R R22
U 1 1 61DC5782
P 3050 5450
F 0 "R22" V 3225 5375 50  0000 L CNN
F 1 "22 kOhm" V 3150 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 5450 50  0001 C CNN
F 3 "~" H 3050 5450 50  0001 C CNN
	1    3050 5450
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D6
U 1 1 61DC5788
P 2550 5150
F 0 "D6" H 2600 5350 50  0000 R CNN
F 1 "BAT85" H 2650 5275 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 5150 50  0001 C CNN
	1    2550 5150
	-1   0    0    1   
$EndComp
Connection ~ 2800 5150
Wire Wire Line
	2800 5150 2800 5450
Wire Wire Line
	2700 5150 2800 5150
Wire Wire Line
	2400 5150 2300 5150
Wire Wire Line
	3200 5150 3300 5150
Wire Wire Line
	3300 5150 3300 5450
$Comp
L Diode:BAT85 D14
U 1 1 61DC5794
P 3050 5150
F 0 "D14" H 3050 5350 50  0000 C CNN
F 1 "BAT85" H 3050 5275 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 5150 50  0001 C CNN
	1    3050 5150
	-1   0    0    1   
$EndComp
Connection ~ 2800 5450
$Comp
L pspice:R R14
U 1 1 61DC579B
P 2550 5450
F 0 "R14" V 2725 5375 50  0000 L CNN
F 1 "22 kOhm" V 2650 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 5450 50  0001 C CNN
F 3 "~" H 2550 5450 50  0001 C CNN
	1    2550 5450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R6
U 1 1 61DC57A1
P 2050 5000
F 0 "R6" V 1875 4950 50  0000 L CNN
F 1 "100 kOhm" V 1950 4800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 5000 50  0001 C CNN
F 3 "~" H 2050 5000 50  0001 C CNN
	1    2050 5000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 4450 3300 4750
$Comp
L Diode:BAT85 D5
U 1 1 61DC57A8
P 2550 4450
F 0 "D5" H 2600 4250 50  0000 R CNN
F 1 "BAT85" H 2650 4350 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 4275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 4450 50  0001 C CNN
	1    2550 4450
	-1   0    0    1   
$EndComp
Text GLabel 2900 4900 2    50   Input ~ 0
GPIO14
Wire Wire Line
	2900 5150 2800 5150
Wire Wire Line
	2900 5000 2800 5000
Wire Wire Line
	2800 5000 2800 5150
Wire Wire Line
	2900 4900 2800 4900
Wire Wire Line
	2800 4900 2800 4750
Wire Wire Line
	2300 4900 2800 4900
Connection ~ 2800 4900
Wire Wire Line
	2300 5000 2800 5000
Connection ~ 2800 5000
Connection ~ 2300 4450
Wire Wire Line
	1800 4450 2300 4450
Text GLabel 1700 4450 0    50   Input ~ 0
GND
Wire Wire Line
	3300 4750 3300 5150
Connection ~ 3300 4750
Connection ~ 3300 5150
Text GLabel 3400 4450 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D13
U 1 1 61DC57BF
P 3050 4450
F 0 "D13" H 3050 4250 50  0000 C CNN
F 1 "BAT85" H 3050 4350 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 4275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 4450 50  0001 C CNN
	1    3050 4450
	-1   0    0    1   
$EndComp
Connection ~ 3300 4450
$Comp
L Connector:AudioJack3 J3
U 1 1 61DC57C7
P 1600 4900
F 0 "J3" H 1550 4575 50  0000 C CNN
F 1 "Snare" H 1550 4650 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1600 4900 50  0001 C CNN
F 3 "~" H 1600 4900 50  0001 C CNN
	1    1600 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 5150 2300 5450
Wire Notes Line
	3700 4150 1100 4150
Text Notes 1150 4250 0    50   ~ 0
Crash Trigger Input
Wire Notes Line
	3700 4150 3700 5750
Wire Notes Line
	1100 5750 3700 5750
Wire Notes Line
	1100 4150 1100 5750
Text GLabel 2900 5000 2    50   Input ~ 0
GPIO34
Text GLabel 2200 5450 0    50   Input ~ 0
GND
Connection ~ 2300 5450
Wire Wire Line
	1800 4450 1800 4800
Wire Wire Line
	1700 4450 1800 4450
Connection ~ 1800 4450
Wire Wire Line
	2200 5450 2300 5450
$Comp
L pspice:R R7
U 1 1 61E10219
P 2050 6550
F 0 "R7" V 2225 6500 50  0000 L CNN
F 1 "100 kOhm" V 2150 6350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 6550 50  0001 C CNN
F 3 "~" H 2050 6550 50  0001 C CNN
	1    2050 6550
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R23
U 1 1 61E1021F
P 3050 6400
F 0 "R23" V 2875 6325 50  0000 L CNN
F 1 "22 kOhm" V 2950 6250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 6400 50  0001 C CNN
F 3 "~" H 3050 6400 50  0001 C CNN
	1    3050 6400
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 6100 2800 6100
Connection ~ 2800 6100
Wire Wire Line
	2800 6100 2800 6400
Wire Wire Line
	2700 6100 2800 6100
Wire Wire Line
	2400 6100 2300 6100
Wire Wire Line
	3200 6100 3300 6100
Connection ~ 2800 6400
Wire Wire Line
	2300 6100 2300 6400
$Comp
L pspice:R R15
U 1 1 61E1022D
P 2550 6400
F 0 "R15" V 2375 6350 50  0000 L CNN
F 1 "22 kOhm" V 2450 6250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 6400 50  0001 C CNN
F 3 "~" H 2550 6400 50  0001 C CNN
	1    2550 6400
	0    1    1    0   
$EndComp
$Comp
L pspice:R R24
U 1 1 61E10233
P 3050 7100
F 0 "R24" V 3225 7025 50  0000 L CNN
F 1 "22 kOhm" V 3150 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3050 7100 50  0001 C CNN
F 3 "~" H 3050 7100 50  0001 C CNN
	1    3050 7100
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D8
U 1 1 61E10239
P 2550 6800
F 0 "D8" H 2600 7000 50  0000 R CNN
F 1 "BAT85" H 2650 6925 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 6800 50  0001 C CNN
	1    2550 6800
	-1   0    0    1   
$EndComp
Connection ~ 2800 6800
Wire Wire Line
	2800 6800 2800 7100
Wire Wire Line
	2700 6800 2800 6800
Wire Wire Line
	2400 6800 2300 6800
Wire Wire Line
	3200 6800 3300 6800
Wire Wire Line
	3300 6800 3300 7100
$Comp
L Diode:BAT85 D16
U 1 1 61E10245
P 3050 6800
F 0 "D16" H 3050 7000 50  0000 C CNN
F 1 "BAT85" H 3050 6925 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 6800 50  0001 C CNN
	1    3050 6800
	-1   0    0    1   
$EndComp
Connection ~ 2800 7100
$Comp
L pspice:R R16
U 1 1 61E1024C
P 2550 7100
F 0 "R16" V 2725 7025 50  0000 L CNN
F 1 "22 kOhm" V 2650 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2550 7100 50  0001 C CNN
F 3 "~" H 2550 7100 50  0001 C CNN
	1    2550 7100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R8
U 1 1 61E10252
P 2050 6650
F 0 "R8" V 1875 6600 50  0000 L CNN
F 1 "100 kOhm" V 1950 6450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2050 6650 50  0001 C CNN
F 3 "~" H 2050 6650 50  0001 C CNN
	1    2050 6650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 6100 3300 6400
$Comp
L Diode:BAT85 D7
U 1 1 61E10259
P 2550 6100
F 0 "D7" H 2600 5900 50  0000 R CNN
F 1 "BAT85" H 2650 6000 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2550 5925 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2550 6100 50  0001 C CNN
	1    2550 6100
	-1   0    0    1   
$EndComp
Text GLabel 2900 6550 2    50   Input ~ 0
GPIO13
Wire Wire Line
	2900 6800 2800 6800
Wire Wire Line
	2900 6650 2800 6650
Wire Wire Line
	2800 6650 2800 6800
Wire Wire Line
	2900 6550 2800 6550
Wire Wire Line
	2800 6550 2800 6400
Wire Wire Line
	2300 6550 2800 6550
Connection ~ 2800 6550
Wire Wire Line
	2300 6650 2800 6650
Connection ~ 2800 6650
Connection ~ 2300 6100
Wire Wire Line
	1800 6100 2300 6100
Text GLabel 1700 6100 0    50   Input ~ 0
GND
Wire Wire Line
	3300 6400 3300 6800
Connection ~ 3300 6400
Connection ~ 3300 6800
Text GLabel 3400 6100 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D15
U 1 1 61E10270
P 3050 6100
F 0 "D15" H 3050 5900 50  0000 C CNN
F 1 "BAT85" H 3050 6000 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3050 5925 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3050 6100 50  0001 C CNN
	1    3050 6100
	-1   0    0    1   
$EndComp
Connection ~ 3300 6100
$Comp
L Connector:AudioJack3 J4
U 1 1 61E10278
P 1600 6550
F 0 "J4" H 1550 6225 50  0000 C CNN
F 1 "Snare" H 1550 6300 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1600 6550 50  0001 C CNN
F 3 "~" H 1600 6550 50  0001 C CNN
	1    1600 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 6800 2300 7100
Wire Notes Line
	3700 5800 1100 5800
Text Notes 1150 5900 0    50   ~ 0
Ride Trigger Input
Wire Notes Line
	3700 5800 3700 7400
Wire Notes Line
	1100 7400 3700 7400
Wire Notes Line
	1100 5800 1100 7400
Text GLabel 2900 6650 2    50   Input ~ 0
GPIO27
Text GLabel 2200 7100 0    50   Input ~ 0
GND
Connection ~ 2300 7100
Wire Wire Line
	1800 6100 1800 6450
Wire Wire Line
	1700 6100 1800 6100
Connection ~ 1800 6100
Wire Wire Line
	2200 7100 2300 7100
Wire Wire Line
	3300 1150 3400 1150
Wire Wire Line
	3300 2800 3400 2800
Wire Wire Line
	3300 4450 3400 4450
Wire Wire Line
	3300 6100 3400 6100
Wire Wire Line
	4425 1350 4525 1350
Text GLabel 4850 2000 0    50   Input ~ 0
GND
Wire Wire Line
	4850 2000 4925 2000
Connection ~ 4925 2000
$Comp
L pspice:R R35
U 1 1 62004A7B
P 5650 3625
F 0 "R35" V 5825 3550 50  0000 L CNN
F 1 "22 kOhm" V 5750 3475 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5650 3625 50  0001 C CNN
F 3 "~" H 5650 3625 50  0001 C CNN
	1    5650 3625
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D19
U 1 1 62004A81
P 5150 3325
F 0 "D19" H 5225 3525 50  0000 R CNN
F 1 "BAT85" H 5250 3450 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5150 3150 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5150 3325 50  0001 C CNN
	1    5150 3325
	-1   0    0    1   
$EndComp
Connection ~ 5400 3325
Wire Wire Line
	5400 3325 5400 3625
Wire Wire Line
	5300 3325 5400 3325
Wire Wire Line
	5000 3325 4900 3325
Wire Wire Line
	5800 3325 5900 3325
Wire Wire Line
	5900 3325 5900 3625
$Comp
L Diode:BAT85 D23
U 1 1 62004A8D
P 5650 3325
F 0 "D23" H 5650 3525 50  0000 C CNN
F 1 "BAT85" H 5650 3450 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5650 3150 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5650 3325 50  0001 C CNN
	1    5650 3325
	-1   0    0    1   
$EndComp
Connection ~ 5400 3625
$Comp
L pspice:R R31
U 1 1 62004A94
P 5150 3625
F 0 "R31" V 5325 3550 50  0000 L CNN
F 1 "22 kOhm" V 5250 3475 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5150 3625 50  0001 C CNN
F 3 "~" H 5150 3625 50  0001 C CNN
	1    5150 3625
	0    1    1    0   
$EndComp
$Comp
L pspice:R R27
U 1 1 62004A9A
P 4650 3175
F 0 "R27" V 4475 3100 50  0000 L CNN
F 1 "100 kOhm" V 4550 2975 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4650 3175 50  0001 C CNN
F 3 "~" H 4650 3175 50  0001 C CNN
	1    4650 3175
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5500 3325 5400 3325
Wire Wire Line
	5400 3175 5400 3325
Wire Wire Line
	4900 3175 5400 3175
Connection ~ 5400 3175
Text GLabel 4500 2975 2    50   Input ~ 0
GND
Text GLabel 5950 3625 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J7
U 1 1 62004AA6
P 4200 3075
F 0 "J7" H 4150 2725 50  0000 C CNN
F 1 "Kick" H 4150 2825 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4200 3075 50  0001 C CNN
F 3 "~" H 4200 3075 50  0001 C CNN
	1    4200 3075
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3325 4900 3625
Text GLabel 5800 3175 2    50   Input ~ 0
GPIO39
Wire Wire Line
	5950 3625 5900 3625
Connection ~ 5900 3625
Wire Wire Line
	5400 3175 5800 3175
NoConn ~ 4400 3075
Wire Wire Line
	4400 2975 4500 2975
Text GLabel 4825 3625 0    50   Input ~ 0
GND
Wire Wire Line
	4825 3625 4900 3625
Connection ~ 4900 3625
Wire Notes Line
	6350 4150 6350 5750
$Comp
L pspice:R R36
U 1 1 62096937
P 5650 5250
F 0 "R36" V 5825 5175 50  0000 L CNN
F 1 "22 kOhm" V 5750 5100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5650 5250 50  0001 C CNN
F 3 "~" H 5650 5250 50  0001 C CNN
	1    5650 5250
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D20
U 1 1 6209693D
P 5150 4950
F 0 "D20" H 5225 5150 50  0000 R CNN
F 1 "BAT85" H 5250 5075 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5150 4775 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5150 4950 50  0001 C CNN
	1    5150 4950
	-1   0    0    1   
$EndComp
Connection ~ 5400 4950
Wire Wire Line
	5400 4950 5400 5250
Wire Wire Line
	5300 4950 5400 4950
Wire Wire Line
	5000 4950 4900 4950
Wire Wire Line
	5800 4950 5900 4950
Wire Wire Line
	5900 4950 5900 5250
$Comp
L Diode:BAT85 D24
U 1 1 62096949
P 5650 4950
F 0 "D24" H 5650 5150 50  0000 C CNN
F 1 "BAT85" H 5650 5075 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5650 4775 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5650 4950 50  0001 C CNN
	1    5650 4950
	-1   0    0    1   
$EndComp
Connection ~ 5400 5250
$Comp
L pspice:R R32
U 1 1 62096950
P 5150 5250
F 0 "R32" V 5325 5175 50  0000 L CNN
F 1 "22 kOhm" V 5250 5100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5150 5250 50  0001 C CNN
F 3 "~" H 5150 5250 50  0001 C CNN
	1    5150 5250
	0    1    1    0   
$EndComp
$Comp
L pspice:R R28
U 1 1 62096956
P 4650 4800
F 0 "R28" V 4475 4725 50  0000 L CNN
F 1 "100 kOhm" V 4550 4600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4650 4800 50  0001 C CNN
F 3 "~" H 4650 4800 50  0001 C CNN
	1    4650 4800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5500 4950 5400 4950
Wire Wire Line
	5400 4800 5400 4950
Wire Wire Line
	4900 4800 5400 4800
Connection ~ 5400 4800
Text GLabel 4500 4600 2    50   Input ~ 0
GND
Text GLabel 5950 5250 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J8
U 1 1 62096962
P 4200 4700
F 0 "J8" H 4150 4350 50  0000 C CNN
F 1 "Kick" H 4150 4450 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4200 4700 50  0001 C CNN
F 3 "~" H 4200 4700 50  0001 C CNN
	1    4200 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 4950 4900 5250
Text GLabel 5800 4800 2    50   Input ~ 0
GPIO12
Wire Wire Line
	5950 5250 5900 5250
Connection ~ 5900 5250
Wire Wire Line
	5400 4800 5800 4800
NoConn ~ 4400 4700
Wire Wire Line
	4400 4600 4500 4600
Text GLabel 4825 5250 0    50   Input ~ 0
GND
Wire Wire Line
	4825 5250 4900 5250
Connection ~ 4900 5250
$Comp
L pspice:R R34
U 1 1 620E003D
P 5625 6900
F 0 "R34" V 5800 6825 50  0000 L CNN
F 1 "22 kOhm" V 5725 6750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5625 6900 50  0001 C CNN
F 3 "~" H 5625 6900 50  0001 C CNN
	1    5625 6900
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D18
U 1 1 620E0043
P 5125 6600
F 0 "D18" H 5200 6800 50  0000 R CNN
F 1 "BAT85" H 5225 6725 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5125 6425 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5125 6600 50  0001 C CNN
	1    5125 6600
	-1   0    0    1   
$EndComp
Connection ~ 5375 6600
Wire Wire Line
	5375 6600 5375 6900
Wire Wire Line
	5275 6600 5375 6600
Wire Wire Line
	4975 6600 4875 6600
Wire Wire Line
	5775 6600 5875 6600
Wire Wire Line
	5875 6600 5875 6900
$Comp
L Diode:BAT85 D22
U 1 1 620E004F
P 5625 6600
F 0 "D22" H 5625 6800 50  0000 C CNN
F 1 "BAT85" H 5625 6725 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5625 6425 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5625 6600 50  0001 C CNN
	1    5625 6600
	-1   0    0    1   
$EndComp
Connection ~ 5375 6900
$Comp
L pspice:R R30
U 1 1 620E0056
P 5125 6900
F 0 "R30" V 5300 6825 50  0000 L CNN
F 1 "22 kOhm" V 5225 6750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5125 6900 50  0001 C CNN
F 3 "~" H 5125 6900 50  0001 C CNN
	1    5125 6900
	0    1    1    0   
$EndComp
$Comp
L pspice:R R26
U 1 1 620E005C
P 4625 6450
F 0 "R26" V 4450 6375 50  0000 L CNN
F 1 "100 kOhm" V 4525 6250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4625 6450 50  0001 C CNN
F 3 "~" H 4625 6450 50  0001 C CNN
	1    4625 6450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5475 6600 5375 6600
Wire Wire Line
	5375 6450 5375 6600
Wire Wire Line
	4875 6450 5375 6450
Connection ~ 5375 6450
Text GLabel 4475 6250 2    50   Input ~ 0
GND
Text GLabel 5925 6900 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J6
U 1 1 620E0068
P 4175 6350
F 0 "J6" H 4125 6000 50  0000 C CNN
F 1 "Kick" H 4125 6100 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4175 6350 50  0001 C CNN
F 3 "~" H 4175 6350 50  0001 C CNN
	1    4175 6350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4875 6600 4875 6900
Text GLabel 5775 6450 2    50   Input ~ 0
GPIO15
Wire Wire Line
	5925 6900 5875 6900
Connection ~ 5875 6900
Wire Wire Line
	5375 6450 5775 6450
NoConn ~ 4375 6350
Wire Wire Line
	4375 6250 4475 6250
Text GLabel 4800 6900 0    50   Input ~ 0
GND
Wire Wire Line
	4800 6900 4875 6900
Connection ~ 4875 6900
Text GLabel 7650 2000 0    50   Input ~ 0
GND
Wire Wire Line
	7650 2000 7725 2000
Wire Wire Line
	7225 1350 7125 1350
$EndSCHEMATC
