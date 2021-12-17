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
L pspice:R R1
U 1 1 61B54CAD
P 5850 3450
F 0 "R1" H 5918 3496 50  0000 L CNN
F 1 "22 kOhm" H 5918 3405 50  0000 L CNN
F 2 "" H 5850 3450 50  0001 C CNN
F 3 "~" H 5850 3450 50  0001 C CNN
	1    5850 3450
	1    0    0    -1  
$EndComp
$Comp
L doit-esp32-devkit-v1:DOIT-ESP32-DEVKIT-V1 U1
U 1 1 61B5316D
P 8500 2700
F 0 "U1" H 8475 3265 50  0000 C CNN
F 1 "DOIT-ESP32-DEVKIT-V1" H 8475 3174 50  0000 C CNN
F 2 "" H 8450 3150 50  0001 C CNN
F 3 "" H 8450 3150 50  0001 C CNN
	1    8500 2700
	1    0    0    -1  
$EndComp
$Comp
L Connector:AudioJack3 J?
U 1 1 61BCFE99
P 4350 3100
F 0 "J?" H 4332 3425 50  0000 C CNN
F 1 "AudioJack3" H 4332 3334 50  0000 C CNN
F 2 "" H 4350 3100 50  0001 C CNN
F 3 "~" H 4350 3100 50  0001 C CNN
	1    4350 3100
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 61BD3273
P 5850 2650
F 0 "R?" H 5918 2696 50  0000 L CNN
F 1 "22 kOhm" H 5918 2605 50  0000 L CNN
F 2 "" H 5850 2650 50  0001 C CNN
F 3 "~" H 5850 2650 50  0001 C CNN
	1    5850 2650
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT85 D?
U 1 1 61BD4DF0
P 5500 3450
F 0 "D?" V 5546 3371 50  0000 R CNN
F 1 "BAT85" V 5455 3371 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5500 3275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5500 3450 50  0001 C CNN
	1    5500 3450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R?
U 1 1 61BE1FA0
P 4800 3200
F 0 "R?" H 4868 3246 50  0000 L CNN
F 1 "100 kOhm" H 4868 3155 50  0000 L CNN
F 2 "" H 4800 3200 50  0001 C CNN
F 3 "~" H 4800 3200 50  0001 C CNN
	1    4800 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 3200 5500 3200
Wire Wire Line
	5500 3200 5500 3300
Connection ~ 5500 3200
Wire Wire Line
	5850 2500 5850 2400
Wire Wire Line
	5500 3200 5850 3200
Wire Wire Line
	5850 2900 5850 3200
Connection ~ 5850 3200
Wire Wire Line
	5500 3700 5850 3700
Wire Wire Line
	5850 3600 5850 3700
Wire Wire Line
	5500 2400 5850 2400
Wire Wire Line
	5500 2800 5500 3200
$Comp
L Diode:BAT85 D?
U 1 1 61BD3463
P 5500 2650
F 0 "D?" H 5500 2866 50  0000 C CNN
F 1 "BAT85" H 5500 2775 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5500 2475 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5500 2650 50  0001 C CNN
	1    5500 2650
	0    1    1    0   
$EndComp
Connection ~ 5850 2400
Wire Wire Line
	5500 2400 5500 2500
Connection ~ 5850 3700
Wire Wire Line
	5500 3700 5500 3600
Wire Wire Line
	7550 4000 5850 4000
Wire Wire Line
	5850 4000 5850 3700
Wire Wire Line
	9400 4100 9900 4100
Wire Wire Line
	9900 4100 9900 1600
Wire Wire Line
	9900 1600 5850 1600
Wire Wire Line
	5850 1600 5850 2400
Wire Wire Line
	6800 3200 6800 2500
Wire Wire Line
	6800 2500 7550 2500
Wire Wire Line
	5850 3200 6800 3200
$EndSCHEMATC
