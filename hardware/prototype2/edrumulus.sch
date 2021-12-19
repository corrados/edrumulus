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
P 2150 1600
F 0 "R3" V 2250 1550 50  0000 L CNN
F 1 "100 kOhm" V 2350 1400 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 1600 50  0001 C CNN
F 3 "~" H 2150 1600 50  0001 C CNN
	1    2150 1600
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R17
U 1 1 61BFFA7F
P 3150 1450
F 0 "R17" V 2950 1400 50  0000 L CNN
F 1 "22 kOhm" V 3050 1300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 1450 50  0001 C CNN
F 3 "~" H 3150 1450 50  0001 C CNN
	1    3150 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 1150 2900 1150
Connection ~ 2900 1150
Wire Wire Line
	2900 1150 2900 1450
Wire Wire Line
	2800 1150 2900 1150
Wire Wire Line
	2500 1150 2400 1150
Wire Wire Line
	3300 1150 3400 1150
Connection ~ 2900 1450
Wire Wire Line
	2400 1150 2400 1450
$Comp
L pspice:R R9
U 1 1 61BFFA9A
P 2650 1450
F 0 "R9" V 2450 1400 50  0000 L CNN
F 1 "22 kOhm" V 2550 1300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 1450 50  0001 C CNN
F 3 "~" H 2650 1450 50  0001 C CNN
	1    2650 1450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R18
U 1 1 61C0679C
P 3150 2150
F 0 "R18" V 3350 2100 50  0000 L CNN
F 1 "22 kOhm" V 3250 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 2150 50  0001 C CNN
F 3 "~" H 3150 2150 50  0001 C CNN
	1    3150 2150
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D2
U 1 1 61C067A2
P 2650 1850
F 0 "D2" H 2700 2050 50  0000 R CNN
F 1 "BAT85" H 2750 1950 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 1850 50  0001 C CNN
	1    2650 1850
	-1   0    0    1   
$EndComp
Connection ~ 2900 1850
Wire Wire Line
	2900 1850 2900 2150
Wire Wire Line
	2800 1850 2900 1850
Wire Wire Line
	2500 1850 2400 1850
Wire Wire Line
	3300 1850 3400 1850
Wire Wire Line
	3400 1850 3400 2150
$Comp
L Diode:BAT85 D10
U 1 1 61C067AF
P 3150 1850
F 0 "D10" H 3150 2066 50  0000 C CNN
F 1 "BAT85" H 3150 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 1850 50  0001 C CNN
	1    3150 1850
	-1   0    0    1   
$EndComp
Connection ~ 2900 2150
$Comp
L pspice:R R10
U 1 1 61C067B7
P 2650 2150
F 0 "R10" V 2850 2100 50  0000 L CNN
F 1 "22 kOhm" V 2750 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 2150 50  0001 C CNN
F 3 "~" H 2650 2150 50  0001 C CNN
	1    2650 2150
	0    1    1    0   
$EndComp
$Comp
L pspice:R R1
U 1 1 61C30414
P 2150 1700
F 0 "R1" V 2050 1650 50  0000 L CNN
F 1 "100 kOhm" V 1950 1500 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 1700 50  0001 C CNN
F 3 "~" H 2150 1700 50  0001 C CNN
	1    2150 1700
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
	3400 1150 3400 1450
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
P 2650 1150
F 0 "D1" H 2700 950 50  0000 R CNN
F 1 "BAT85" H 2750 1050 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 1150 50  0001 C CNN
	1    2650 1150
	-1   0    0    1   
$EndComp
Text GLabel 3000 1600 2    50   Input ~ 0
GPIO35
Wire Wire Line
	3000 1850 2900 1850
Wire Wire Line
	3000 1700 2900 1700
Wire Wire Line
	2900 1700 2900 1850
Wire Wire Line
	3000 1600 2900 1600
Wire Wire Line
	2900 1600 2900 1450
Wire Wire Line
	2400 1600 2900 1600
Connection ~ 2900 1600
Wire Wire Line
	2400 1700 2900 1700
Connection ~ 2900 1700
Wire Wire Line
	1900 1500 1900 1300
Connection ~ 2400 1150
Wire Wire Line
	1900 1150 2400 1150
Text GLabel 1700 1300 0    50   Input ~ 0
GND
Wire Wire Line
	3400 1450 3400 1850
Connection ~ 3400 1450
Connection ~ 3400 1850
Text GLabel 3450 1150 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D9
U 1 1 61BFFA92
P 3150 1150
F 0 "D9" H 3150 950 50  0000 C CNN
F 1 "BAT85" H 3150 1050 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 1150 50  0001 C CNN
	1    3150 1150
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 1150 3400 1150
Connection ~ 3400 1150
Wire Wire Line
	1900 1150 1250 1150
Wire Wire Line
	1250 1150 1250 2150
Wire Wire Line
	1250 2150 2400 2150
Connection ~ 1900 1150
Connection ~ 2400 2150
$Comp
L Connector:AudioJack3 J1
U 1 1 61BCFE99
P 1700 1600
F 0 "J1" H 1650 1250 50  0000 C CNN
F 1 "Snare" H 1650 1350 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1700 1600 50  0001 C CNN
F 3 "~" H 1700 1600 50  0001 C CNN
	1    1700 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 1850 2400 2150
Connection ~ 1900 1300
Wire Wire Line
	1900 1300 1900 1150
Wire Wire Line
	1700 1300 1900 1300
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
$Comp
L pspice:R R2
U 1 1 61C8D4A7
P 2150 3250
F 0 "R2" V 2250 3200 50  0000 L CNN
F 1 "100 kOhm" V 2350 3050 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 3250 50  0001 C CNN
F 3 "~" H 2150 3250 50  0001 C CNN
	1    2150 3250
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R19
U 1 1 61C8D4AD
P 3150 3100
F 0 "R19" V 2950 3050 50  0000 L CNN
F 1 "22 kOhm" V 3050 2950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 3100 50  0001 C CNN
F 3 "~" H 3150 3100 50  0001 C CNN
	1    3150 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 2800 2900 2800
Connection ~ 2900 2800
Wire Wire Line
	2900 2800 2900 3100
Wire Wire Line
	2800 2800 2900 2800
Wire Wire Line
	2500 2800 2400 2800
Wire Wire Line
	3300 2800 3400 2800
Connection ~ 2900 3100
Wire Wire Line
	2400 2800 2400 3100
$Comp
L pspice:R R11
U 1 1 61C8D4BB
P 2650 3100
F 0 "R11" V 2450 3050 50  0000 L CNN
F 1 "22 kOhm" V 2550 2950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 3100 50  0001 C CNN
F 3 "~" H 2650 3100 50  0001 C CNN
	1    2650 3100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R20
U 1 1 61C8D4C1
P 3150 3800
F 0 "R20" V 3350 3750 50  0000 L CNN
F 1 "22 kOhm" V 3250 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 3800 50  0001 C CNN
F 3 "~" H 3150 3800 50  0001 C CNN
	1    3150 3800
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D4
U 1 1 61C8D4C7
P 2650 3500
F 0 "D4" H 2700 3700 50  0000 R CNN
F 1 "BAT85" H 2750 3600 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 3500 50  0001 C CNN
	1    2650 3500
	-1   0    0    1   
$EndComp
Connection ~ 2900 3500
Wire Wire Line
	2900 3500 2900 3800
Wire Wire Line
	2800 3500 2900 3500
Wire Wire Line
	2500 3500 2400 3500
Wire Wire Line
	3300 3500 3400 3500
Wire Wire Line
	3400 3500 3400 3800
$Comp
L Diode:BAT85 D12
U 1 1 61C8D4D3
P 3150 3500
F 0 "D12" H 3150 3716 50  0000 C CNN
F 1 "BAT85" H 3150 3625 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 3500 50  0001 C CNN
	1    3150 3500
	-1   0    0    1   
$EndComp
Connection ~ 2900 3800
$Comp
L pspice:R R12
U 1 1 61C8D4DA
P 2650 3800
F 0 "R12" V 2850 3750 50  0000 L CNN
F 1 "22 kOhm" V 2750 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 3800 50  0001 C CNN
F 3 "~" H 2650 3800 50  0001 C CNN
	1    2650 3800
	0    1    1    0   
$EndComp
$Comp
L pspice:R R4
U 1 1 61C8D4E0
P 2150 3350
F 0 "R4" V 2050 3300 50  0000 L CNN
F 1 "100 kOhm" V 1950 3150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 3350 50  0001 C CNN
F 3 "~" H 2150 3350 50  0001 C CNN
	1    2150 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3400 2800 3400 3100
$Comp
L Diode:BAT85 D3
U 1 1 61C8D4E8
P 2650 2800
F 0 "D3" H 2700 2600 50  0000 R CNN
F 1 "BAT85" H 2750 2700 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 2625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 2800 50  0001 C CNN
	1    2650 2800
	-1   0    0    1   
$EndComp
Text GLabel 3000 3250 2    50   Input ~ 0
GPIO26
Text GLabel 3000 3350 2    50   Input ~ 0
GPIO32
Wire Wire Line
	3000 3500 2900 3500
Wire Wire Line
	3000 3350 2900 3350
Wire Wire Line
	2900 3350 2900 3500
Wire Wire Line
	3000 3250 2900 3250
Wire Wire Line
	2900 3250 2900 3100
Wire Wire Line
	2400 3250 2900 3250
Connection ~ 2900 3250
Wire Wire Line
	2400 3350 2900 3350
Connection ~ 2900 3350
Wire Wire Line
	1900 3150 1900 2950
Connection ~ 2400 2800
Wire Wire Line
	1900 2800 2400 2800
Text GLabel 1700 2950 0    50   Input ~ 0
GND
Wire Wire Line
	3400 3100 3400 3500
Connection ~ 3400 3100
Connection ~ 3400 3500
$Comp
L Diode:BAT85 D11
U 1 1 61C8D501
P 3150 2800
F 0 "D11" H 3150 2600 50  0000 C CNN
F 1 "BAT85" H 3150 2700 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 2625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 2800 50  0001 C CNN
	1    3150 2800
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 2800 3400 2800
Connection ~ 3400 2800
Wire Wire Line
	1900 2800 1250 2800
Wire Wire Line
	1250 2800 1250 3800
Wire Wire Line
	1250 3800 2400 3800
Connection ~ 1900 2800
Connection ~ 2400 3800
$Comp
L Connector:AudioJack3 J2
U 1 1 61C8D50E
P 1700 3250
F 0 "J2" H 1650 2900 50  0000 C CNN
F 1 "Hi-Hat" H 1650 3000 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1700 3250 50  0001 C CNN
F 3 "~" H 1700 3250 50  0001 C CNN
	1    1700 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 3500 2400 3800
Connection ~ 1900 2950
Wire Wire Line
	1900 2950 1900 2800
Wire Wire Line
	1700 2950 1900 2950
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
Text GLabel 3000 1700 2    50   Input ~ 0
GPIO36
Text GLabel 3450 2800 2    50   Input ~ 0
3V3
$Comp
L pspice:R R5
U 1 1 61CA03AF
P 2150 4900
F 0 "R5" V 2250 4850 50  0000 L CNN
F 1 "100 kOhm" V 2350 4700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 4900 50  0001 C CNN
F 3 "~" H 2150 4900 50  0001 C CNN
	1    2150 4900
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R21
U 1 1 61CA03B5
P 3150 4750
F 0 "R21" V 2950 4700 50  0000 L CNN
F 1 "22 kOhm" V 3050 4600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 4750 50  0001 C CNN
F 3 "~" H 3150 4750 50  0001 C CNN
	1    3150 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 4450 2900 4450
Connection ~ 2900 4450
Wire Wire Line
	2900 4450 2900 4750
Wire Wire Line
	2800 4450 2900 4450
Wire Wire Line
	2500 4450 2400 4450
Wire Wire Line
	3300 4450 3400 4450
Connection ~ 2900 4750
Wire Wire Line
	2400 4450 2400 4750
$Comp
L pspice:R R13
U 1 1 61CA03C3
P 2650 4750
F 0 "R13" V 2450 4700 50  0000 L CNN
F 1 "22 kOhm" V 2550 4600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 4750 50  0001 C CNN
F 3 "~" H 2650 4750 50  0001 C CNN
	1    2650 4750
	0    1    1    0   
$EndComp
$Comp
L pspice:R R22
U 1 1 61CA03C9
P 3150 5450
F 0 "R22" V 3350 5400 50  0000 L CNN
F 1 "22 kOhm" V 3250 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 5450 50  0001 C CNN
F 3 "~" H 3150 5450 50  0001 C CNN
	1    3150 5450
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D6
U 1 1 61CA03CF
P 2650 5150
F 0 "D6" H 2700 5350 50  0000 R CNN
F 1 "BAT85" H 2750 5250 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 5150 50  0001 C CNN
	1    2650 5150
	-1   0    0    1   
$EndComp
Connection ~ 2900 5150
Wire Wire Line
	2900 5150 2900 5450
Wire Wire Line
	2800 5150 2900 5150
Wire Wire Line
	2500 5150 2400 5150
Wire Wire Line
	3300 5150 3400 5150
Wire Wire Line
	3400 5150 3400 5450
$Comp
L Diode:BAT85 D14
U 1 1 61CA03DB
P 3150 5150
F 0 "D14" H 3150 5366 50  0000 C CNN
F 1 "BAT85" H 3150 5275 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 5150 50  0001 C CNN
	1    3150 5150
	-1   0    0    1   
$EndComp
Connection ~ 2900 5450
$Comp
L pspice:R R14
U 1 1 61CA03E2
P 2650 5450
F 0 "R14" V 2850 5400 50  0000 L CNN
F 1 "22 kOhm" V 2750 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 5450 50  0001 C CNN
F 3 "~" H 2650 5450 50  0001 C CNN
	1    2650 5450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R6
U 1 1 61CA03E8
P 2150 5000
F 0 "R6" V 2050 4950 50  0000 L CNN
F 1 "100 kOhm" V 1950 4800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 5000 50  0001 C CNN
F 3 "~" H 2150 5000 50  0001 C CNN
	1    2150 5000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3400 4450 3400 4750
$Comp
L Diode:BAT85 D5
U 1 1 61CA03EF
P 2650 4450
F 0 "D5" H 2700 4250 50  0000 R CNN
F 1 "BAT85" H 2750 4350 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 4275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 4450 50  0001 C CNN
	1    2650 4450
	-1   0    0    1   
$EndComp
Text GLabel 3000 4900 2    50   Input ~ 0
GPIO14
Text GLabel 3000 5000 2    50   Input ~ 0
GPIO34
Wire Wire Line
	3000 5150 2900 5150
Wire Wire Line
	3000 5000 2900 5000
Wire Wire Line
	2900 5000 2900 5150
Wire Wire Line
	3000 4900 2900 4900
Wire Wire Line
	2900 4900 2900 4750
Wire Wire Line
	2400 4900 2900 4900
Connection ~ 2900 4900
Wire Wire Line
	2400 5000 2900 5000
Connection ~ 2900 5000
Wire Wire Line
	1900 4800 1900 4600
Connection ~ 2400 4450
Wire Wire Line
	1900 4450 2400 4450
Text GLabel 1700 4600 0    50   Input ~ 0
GND
Wire Wire Line
	3400 4750 3400 5150
Connection ~ 3400 4750
Connection ~ 3400 5150
$Comp
L Diode:BAT85 D13
U 1 1 61CA0407
P 3150 4450
F 0 "D13" H 3150 4250 50  0000 C CNN
F 1 "BAT85" H 3150 4350 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 4275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 4450 50  0001 C CNN
	1    3150 4450
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 4450 3400 4450
Connection ~ 3400 4450
Wire Wire Line
	1900 4450 1250 4450
Wire Wire Line
	1250 4450 1250 5450
Wire Wire Line
	1250 5450 2400 5450
Connection ~ 1900 4450
Connection ~ 2400 5450
$Comp
L Connector:AudioJack3 J3
U 1 1 61CA0414
P 1700 4900
F 0 "J3" H 1650 4550 50  0000 C CNN
F 1 "Crash" H 1650 4650 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1700 4900 50  0001 C CNN
F 3 "~" H 1700 4900 50  0001 C CNN
	1    1700 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 5150 2400 5450
Connection ~ 1900 4600
Wire Wire Line
	1900 4600 1900 4450
Wire Wire Line
	1700 4600 1900 4600
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
Text GLabel 3450 4450 2    50   Input ~ 0
3V3
$Comp
L pspice:R R7
U 1 1 61CAD845
P 2150 6550
F 0 "R7" V 2250 6500 50  0000 L CNN
F 1 "100 kOhm" V 2350 6350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 6550 50  0001 C CNN
F 3 "~" H 2150 6550 50  0001 C CNN
	1    2150 6550
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R23
U 1 1 61CAD84B
P 3150 6400
F 0 "R23" V 2950 6350 50  0000 L CNN
F 1 "22 kOhm" V 3050 6250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 6400 50  0001 C CNN
F 3 "~" H 3150 6400 50  0001 C CNN
	1    3150 6400
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 6100 2900 6100
Connection ~ 2900 6100
Wire Wire Line
	2900 6100 2900 6400
Wire Wire Line
	2800 6100 2900 6100
Wire Wire Line
	2500 6100 2400 6100
Wire Wire Line
	3300 6100 3400 6100
Connection ~ 2900 6400
Wire Wire Line
	2400 6100 2400 6400
$Comp
L pspice:R R15
U 1 1 61CAD859
P 2650 6400
F 0 "R15" V 2450 6350 50  0000 L CNN
F 1 "22 kOhm" V 2550 6250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 6400 50  0001 C CNN
F 3 "~" H 2650 6400 50  0001 C CNN
	1    2650 6400
	0    1    1    0   
$EndComp
$Comp
L pspice:R R24
U 1 1 61CAD85F
P 3150 7100
F 0 "R24" V 3350 7050 50  0000 L CNN
F 1 "22 kOhm" V 3250 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 3150 7100 50  0001 C CNN
F 3 "~" H 3150 7100 50  0001 C CNN
	1    3150 7100
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D8
U 1 1 61CAD865
P 2650 6800
F 0 "D8" H 2700 7000 50  0000 R CNN
F 1 "BAT85" H 2750 6900 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 6800 50  0001 C CNN
	1    2650 6800
	-1   0    0    1   
$EndComp
Connection ~ 2900 6800
Wire Wire Line
	2900 6800 2900 7100
Wire Wire Line
	2800 6800 2900 6800
Wire Wire Line
	2500 6800 2400 6800
Wire Wire Line
	3300 6800 3400 6800
Wire Wire Line
	3400 6800 3400 7100
$Comp
L Diode:BAT85 D16
U 1 1 61CAD871
P 3150 6800
F 0 "D16" H 3150 7016 50  0000 C CNN
F 1 "BAT85" H 3150 6925 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 6800 50  0001 C CNN
	1    3150 6800
	-1   0    0    1   
$EndComp
Connection ~ 2900 7100
$Comp
L pspice:R R16
U 1 1 61CAD878
P 2650 7100
F 0 "R16" V 2850 7050 50  0000 L CNN
F 1 "22 kOhm" V 2750 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 7100 50  0001 C CNN
F 3 "~" H 2650 7100 50  0001 C CNN
	1    2650 7100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R8
U 1 1 61CAD87E
P 2150 6650
F 0 "R8" V 2050 6600 50  0000 L CNN
F 1 "100 kOhm" V 1950 6450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 6650 50  0001 C CNN
F 3 "~" H 2150 6650 50  0001 C CNN
	1    2150 6650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3400 6100 3400 6400
$Comp
L Diode:BAT85 D7
U 1 1 61CAD885
P 2650 6100
F 0 "D7" H 2700 5900 50  0000 R CNN
F 1 "BAT85" H 2750 6000 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 5925 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 6100 50  0001 C CNN
	1    2650 6100
	-1   0    0    1   
$EndComp
Text GLabel 3000 6550 2    50   Input ~ 0
GPIO13
Text GLabel 3000 6650 2    50   Input ~ 0
GPIO27
Wire Wire Line
	3000 6800 2900 6800
Wire Wire Line
	3000 6650 2900 6650
Wire Wire Line
	2900 6650 2900 6800
Wire Wire Line
	3000 6550 2900 6550
Wire Wire Line
	2900 6550 2900 6400
Wire Wire Line
	2400 6550 2900 6550
Connection ~ 2900 6550
Wire Wire Line
	2400 6650 2900 6650
Connection ~ 2900 6650
Wire Wire Line
	1900 6450 1900 6250
Connection ~ 2400 6100
Wire Wire Line
	1900 6100 2400 6100
Text GLabel 1700 6250 0    50   Input ~ 0
GND
Wire Wire Line
	3400 6400 3400 6800
Connection ~ 3400 6400
Connection ~ 3400 6800
$Comp
L Diode:BAT85 D15
U 1 1 61CAD89D
P 3150 6100
F 0 "D15" H 3150 5900 50  0000 C CNN
F 1 "BAT85" H 3150 6000 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3150 5925 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3150 6100 50  0001 C CNN
	1    3150 6100
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 6100 3400 6100
Connection ~ 3400 6100
Wire Wire Line
	1900 6100 1250 6100
Wire Wire Line
	1250 6100 1250 7100
Wire Wire Line
	1250 7100 2400 7100
Connection ~ 1900 6100
Connection ~ 2400 7100
$Comp
L Connector:AudioJack3 J4
U 1 1 61CAD8AA
P 1700 6550
F 0 "J4" H 1650 6200 50  0000 C CNN
F 1 "Ride" H 1650 6300 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1700 6550 50  0001 C CNN
F 3 "~" H 1700 6550 50  0001 C CNN
	1    1700 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 6800 2400 7100
Connection ~ 1900 6250
Wire Wire Line
	1900 6250 1900 6100
Wire Wire Line
	1700 6250 1900 6250
Wire Notes Line
	3700 5800 1100 5800
Wire Notes Line
	3700 5800 3700 7400
Wire Notes Line
	1100 7400 3700 7400
Wire Notes Line
	1100 5800 1100 7400
Text GLabel 3450 6100 2    50   Input ~ 0
3V3
Text Notes 1150 5900 0    50   ~ 0
Ride Trigger Input
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
P 5800 2150
F 0 "R33" V 6000 2100 50  0000 L CNN
F 1 "22 kOhm" V 5900 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5800 2150 50  0001 C CNN
F 3 "~" H 5800 2150 50  0001 C CNN
	1    5800 2150
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D17
U 1 1 61D09D44
P 5300 1850
F 0 "D17" H 5350 2050 50  0000 R CNN
F 1 "BAT85" H 5400 1950 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5300 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5300 1850 50  0001 C CNN
	1    5300 1850
	-1   0    0    1   
$EndComp
Connection ~ 5550 1850
Wire Wire Line
	5550 1850 5550 2150
Wire Wire Line
	5450 1850 5550 1850
Wire Wire Line
	5150 1850 5050 1850
Wire Wire Line
	5950 1850 6050 1850
Wire Wire Line
	6050 1850 6050 2150
$Comp
L Diode:BAT85 D21
U 1 1 61D09D50
P 5800 1850
F 0 "D21" H 5800 2066 50  0000 C CNN
F 1 "BAT85" H 5800 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5800 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5800 1850 50  0001 C CNN
	1    5800 1850
	-1   0    0    1   
$EndComp
Connection ~ 5550 2150
$Comp
L pspice:R R29
U 1 1 61D09D57
P 5300 2150
F 0 "R29" V 5500 2100 50  0000 L CNN
F 1 "22 kOhm" V 5400 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5300 2150 50  0001 C CNN
F 3 "~" H 5300 2150 50  0001 C CNN
	1    5300 2150
	0    1    1    0   
$EndComp
$Comp
L pspice:R R25
U 1 1 61D09D5D
P 4800 1700
F 0 "R25" V 4700 1650 50  0000 L CNN
F 1 "100 kOhm" V 4600 1500 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4800 1700 50  0001 C CNN
F 3 "~" H 4800 1700 50  0001 C CNN
	1    4800 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 1850 5550 1850
Wire Wire Line
	5550 1700 5550 1850
Wire Wire Line
	5050 1700 5550 1700
Connection ~ 5550 1700
Text GLabel 6050 1150 2    50   Input ~ 0
GND
Text GLabel 6100 2150 2    50   Input ~ 0
3V3
Wire Wire Line
	4550 1150 3900 1150
Wire Wire Line
	3900 1150 3900 2150
Wire Wire Line
	3900 2150 5050 2150
Connection ~ 5050 2150
$Comp
L Connector:AudioJack3 J5
U 1 1 61D09D89
P 4350 1600
F 0 "J5" H 4300 1250 50  0000 C CNN
F 1 "Kick" H 4300 1350 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4350 1600 50  0001 C CNN
F 3 "~" H 4350 1600 50  0001 C CNN
	1    4350 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 1850 5050 2150
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
Text GLabel 5950 1700 2    50   Input ~ 0
GPIO33
Wire Wire Line
	4550 1150 4550 1500
Connection ~ 4550 1150
$Comp
L pspice:R R34
U 1 1 61D36A81
P 5800 3800
F 0 "R34" V 6000 3750 50  0000 L CNN
F 1 "22 kOhm" V 5900 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5800 3800 50  0001 C CNN
F 3 "~" H 5800 3800 50  0001 C CNN
	1    5800 3800
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D18
U 1 1 61D36A87
P 5300 3500
F 0 "D18" H 5350 3700 50  0000 R CNN
F 1 "BAT85" H 5400 3600 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5300 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5300 3500 50  0001 C CNN
	1    5300 3500
	-1   0    0    1   
$EndComp
Connection ~ 5550 3500
Wire Wire Line
	5550 3500 5550 3800
Wire Wire Line
	5450 3500 5550 3500
Wire Wire Line
	5150 3500 5050 3500
Wire Wire Line
	5950 3500 6050 3500
Wire Wire Line
	6050 3500 6050 3800
$Comp
L Diode:BAT85 D22
U 1 1 61D36A93
P 5800 3500
F 0 "D22" H 5800 3716 50  0000 C CNN
F 1 "BAT85" H 5800 3625 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5800 3325 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5800 3500 50  0001 C CNN
	1    5800 3500
	-1   0    0    1   
$EndComp
Connection ~ 5550 3800
$Comp
L pspice:R R30
U 1 1 61D36A9A
P 5300 3800
F 0 "R30" V 5500 3750 50  0000 L CNN
F 1 "22 kOhm" V 5400 3650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5300 3800 50  0001 C CNN
F 3 "~" H 5300 3800 50  0001 C CNN
	1    5300 3800
	0    1    1    0   
$EndComp
$Comp
L pspice:R R26
U 1 1 61D36AA0
P 4800 3350
F 0 "R26" V 4700 3300 50  0000 L CNN
F 1 "100 kOhm" V 4600 3150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4800 3350 50  0001 C CNN
F 3 "~" H 4800 3350 50  0001 C CNN
	1    4800 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 3500 5550 3500
Wire Wire Line
	5550 3350 5550 3500
Wire Wire Line
	5050 3350 5550 3350
Connection ~ 5550 3350
Text GLabel 6050 2800 2    50   Input ~ 0
GND
Text GLabel 6100 3800 2    50   Input ~ 0
3V3
Wire Wire Line
	4550 2800 3900 2800
Wire Wire Line
	3900 2800 3900 3800
Wire Wire Line
	3900 3800 5050 3800
Connection ~ 5050 3800
$Comp
L Connector:AudioJack3 J6
U 1 1 61D36AB1
P 4350 3250
F 0 "J6" H 4300 2900 50  0000 C CNN
F 1 "Tom1" H 4300 3000 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4350 3250 50  0001 C CNN
F 3 "~" H 4350 3250 50  0001 C CNN
	1    4350 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 3500 5050 3800
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
Text GLabel 5950 3350 2    50   Input ~ 0
GPIO39
Wire Wire Line
	4550 2800 4550 3150
Connection ~ 4550 2800
$Comp
L pspice:R R35
U 1 1 61D3FA03
P 5800 5450
F 0 "R35" V 6000 5400 50  0000 L CNN
F 1 "22 kOhm" V 5900 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5800 5450 50  0001 C CNN
F 3 "~" H 5800 5450 50  0001 C CNN
	1    5800 5450
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D19
U 1 1 61D3FA09
P 5300 5150
F 0 "D19" H 5350 5350 50  0000 R CNN
F 1 "BAT85" H 5400 5250 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5300 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5300 5150 50  0001 C CNN
	1    5300 5150
	-1   0    0    1   
$EndComp
Connection ~ 5550 5150
Wire Wire Line
	5550 5150 5550 5450
Wire Wire Line
	5450 5150 5550 5150
Wire Wire Line
	5150 5150 5050 5150
Wire Wire Line
	5950 5150 6050 5150
Wire Wire Line
	6050 5150 6050 5450
$Comp
L Diode:BAT85 D23
U 1 1 61D3FA15
P 5800 5150
F 0 "D23" H 5800 5366 50  0000 C CNN
F 1 "BAT85" H 5800 5275 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5800 4975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5800 5150 50  0001 C CNN
	1    5800 5150
	-1   0    0    1   
$EndComp
Connection ~ 5550 5450
$Comp
L pspice:R R31
U 1 1 61D3FA1C
P 5300 5450
F 0 "R31" V 5500 5400 50  0000 L CNN
F 1 "22 kOhm" V 5400 5300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5300 5450 50  0001 C CNN
F 3 "~" H 5300 5450 50  0001 C CNN
	1    5300 5450
	0    1    1    0   
$EndComp
$Comp
L pspice:R R27
U 1 1 61D3FA22
P 4800 5000
F 0 "R27" V 4700 4950 50  0000 L CNN
F 1 "100 kOhm" V 4600 4800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4800 5000 50  0001 C CNN
F 3 "~" H 4800 5000 50  0001 C CNN
	1    4800 5000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 5150 5550 5150
Wire Wire Line
	5550 5000 5550 5150
Wire Wire Line
	5050 5000 5550 5000
Connection ~ 5550 5000
Text GLabel 6050 4450 2    50   Input ~ 0
GND
Text GLabel 6100 5450 2    50   Input ~ 0
3V3
Wire Wire Line
	4550 4450 3900 4450
Wire Wire Line
	3900 4450 3900 5450
Wire Wire Line
	3900 5450 5050 5450
Connection ~ 5050 5450
$Comp
L Connector:AudioJack3 J7
U 1 1 61D3FA33
P 4350 4900
F 0 "J7" H 4300 4550 50  0000 C CNN
F 1 "Tom2" H 4300 4650 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4350 4900 50  0001 C CNN
F 3 "~" H 4350 4900 50  0001 C CNN
	1    4350 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 5150 5050 5450
Wire Notes Line
	6350 4150 3750 4150
Text Notes 3800 4250 0    50   ~ 0
Tom2 Trigger Input
Wire Notes Line
	6350 4150 6350 5750
Wire Notes Line
	3750 5750 6350 5750
Wire Notes Line
	3750 4150 3750 5750
Text GLabel 5950 5000 2    50   Input ~ 0
GPIO12
Wire Wire Line
	4550 4450 4550 4800
Connection ~ 4550 4450
$Comp
L pspice:R R36
U 1 1 61D48FE5
P 5800 7100
F 0 "R36" V 6000 7050 50  0000 L CNN
F 1 "22 kOhm" V 5900 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5800 7100 50  0001 C CNN
F 3 "~" H 5800 7100 50  0001 C CNN
	1    5800 7100
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D20
U 1 1 61D48FEB
P 5300 6800
F 0 "D20" H 5350 7000 50  0000 R CNN
F 1 "BAT85" H 5400 6900 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5300 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5300 6800 50  0001 C CNN
	1    5300 6800
	-1   0    0    1   
$EndComp
Connection ~ 5550 6800
Wire Wire Line
	5550 6800 5550 7100
Wire Wire Line
	5450 6800 5550 6800
Wire Wire Line
	5150 6800 5050 6800
Wire Wire Line
	5950 6800 6050 6800
Wire Wire Line
	6050 6800 6050 7100
$Comp
L Diode:BAT85 D24
U 1 1 61D48FF7
P 5800 6800
F 0 "D24" H 5800 7016 50  0000 C CNN
F 1 "BAT85" H 5800 6925 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5800 6625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5800 6800 50  0001 C CNN
	1    5800 6800
	-1   0    0    1   
$EndComp
Connection ~ 5550 7100
$Comp
L pspice:R R32
U 1 1 61D48FFE
P 5300 7100
F 0 "R32" V 5500 7050 50  0000 L CNN
F 1 "22 kOhm" V 5400 6950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5300 7100 50  0001 C CNN
F 3 "~" H 5300 7100 50  0001 C CNN
	1    5300 7100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R28
U 1 1 61D49004
P 4800 6650
F 0 "R28" V 4700 6600 50  0000 L CNN
F 1 "100 kOhm" V 4600 6450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4800 6650 50  0001 C CNN
F 3 "~" H 4800 6650 50  0001 C CNN
	1    4800 6650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 6800 5550 6800
Wire Wire Line
	5550 6650 5550 6800
Wire Wire Line
	5050 6650 5550 6650
Connection ~ 5550 6650
Text GLabel 6050 6100 2    50   Input ~ 0
GND
Text GLabel 6100 7100 2    50   Input ~ 0
3V3
Wire Wire Line
	4550 6100 3900 6100
Wire Wire Line
	3900 6100 3900 7100
Wire Wire Line
	3900 7100 5050 7100
Connection ~ 5050 7100
$Comp
L Connector:AudioJack3 J8
U 1 1 61D49015
P 4350 6550
F 0 "J8" H 4300 6200 50  0000 C CNN
F 1 "Tom3" H 4300 6300 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 4350 6550 50  0001 C CNN
F 3 "~" H 4350 6550 50  0001 C CNN
	1    4350 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 6800 5050 7100
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
Text GLabel 5950 6650 2    50   Input ~ 0
GPIO15
Wire Wire Line
	4550 6100 4550 6450
Connection ~ 4550 6100
$Comp
L pspice:R R38
U 1 1 61D81CAE
P 8450 2150
F 0 "R38" V 8650 2100 50  0000 L CNN
F 1 "10 kOhm" V 8550 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8450 2150 50  0001 C CNN
F 3 "~" H 8450 2150 50  0001 C CNN
	1    8450 2150
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D25
U 1 1 61D81CB4
P 7950 2150
F 0 "D25" H 8000 2350 50  0000 R CNN
F 1 "BAT85" H 8050 2250 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 7950 1975 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 7950 2150 50  0001 C CNN
	1    7950 2150
	-1   0    0    1   
$EndComp
Connection ~ 8200 1850
Wire Wire Line
	8200 1850 8200 2150
Wire Wire Line
	8600 1850 8700 1850
Wire Wire Line
	8700 1850 8700 2150
$Comp
L Diode:BAT85 D26
U 1 1 61D81CC0
P 8450 1850
F 0 "D26" H 8450 2066 50  0000 C CNN
F 1 "BAT85" H 8450 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 8450 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 8450 1850 50  0001 C CNN
	1    8450 1850
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R37
U 1 1 61D81CC7
P 8000 1550
F 0 "R37" V 7800 1500 50  0000 L CNN
F 1 "10 kOhm" V 7900 1400 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 8000 1550 50  0001 C CNN
F 3 "~" H 8000 1550 50  0001 C CNN
	1    8000 1550
	0    1    1    0   
$EndComp
Wire Wire Line
	8300 1850 8200 1850
Text GLabel 8700 1150 2    50   Input ~ 0
GND
Text GLabel 8750 1850 2    50   Input ~ 0
3V3
Wire Wire Line
	7200 1150 6550 1150
Wire Wire Line
	6550 1150 6550 2150
$Comp
L Connector:AudioJack3 J9
U 1 1 61D81CDE
P 7000 1600
F 0 "J9" H 6950 1250 50  0000 C CNN
F 1 "Hi-Hat Ctrl" H 6950 1350 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 7000 1600 50  0001 C CNN
F 3 "~" H 7000 1600 50  0001 C CNN
	1    7000 1600
	1    0    0    -1  
$EndComp
Text Notes 6450 950  0    50   ~ 0
Hi-Hat Control Input
Wire Notes Line
	6400 850  6400 2450
Wire Wire Line
	8750 1850 8700 1850
Connection ~ 8700 1850
Wire Wire Line
	7200 1150 7200 1500
Connection ~ 7200 1150
Wire Wire Line
	8200 1850 8200 1700
Wire Wire Line
	8200 1700 7750 1700
Text GLabel 8600 1550 2    50   Input ~ 0
GPIO25
Wire Wire Line
	6550 2150 7800 2150
Wire Wire Line
	8100 2150 8200 2150
Connection ~ 8200 2150
Wire Wire Line
	6100 2150 6050 2150
Connection ~ 6050 2150
Wire Wire Line
	6100 3800 6050 3800
Connection ~ 6050 3800
Wire Wire Line
	6100 5450 6050 5450
Connection ~ 6050 5450
Wire Wire Line
	6100 7100 6050 7100
Connection ~ 6050 7100
Wire Wire Line
	5550 6650 5950 6650
Wire Wire Line
	5550 5000 5950 5000
Wire Wire Line
	5550 3350 5950 3350
Wire Wire Line
	5550 1700 5950 1700
Wire Wire Line
	4550 6100 6050 6100
Wire Wire Line
	4550 4450 6050 4450
Wire Wire Line
	4550 2800 6050 2800
Wire Wire Line
	4550 1150 6050 1150
Wire Wire Line
	8600 1550 8250 1550
Wire Wire Line
	7750 1550 7750 1700
Connection ~ 7750 1700
Wire Wire Line
	7750 1700 7200 1700
Wire Wire Line
	7200 1150 8700 1150
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
NoConn ~ 4550 1600
NoConn ~ 4550 3250
NoConn ~ 4550 4900
NoConn ~ 4550 6550
NoConn ~ 7200 1600
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
$EndSCHEMATC
