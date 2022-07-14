EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Edrumulus Prototype 4 (ESP32)"
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
P 1650 1450
F 0 "R3" V 1825 1400 50  0000 L CNN
F 1 "100 kOhm" V 1750 1250 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 1450 50  0001 C CNN
F 3 "~" H 1650 1450 50  0001 C CNN
	1    1650 1450
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R17
U 1 1 61BFFA7F
P 2650 1300
F 0 "R17" V 2475 1225 50  0000 L CNN
F 1 "22 kOhm" V 2550 1150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 1300 50  0001 C CNN
F 3 "~" H 2650 1300 50  0001 C CNN
	1    2650 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 1000 2400 1000
Connection ~ 2400 1000
Wire Wire Line
	2400 1000 2400 1300
Wire Wire Line
	2300 1000 2400 1000
Wire Wire Line
	2000 1000 1900 1000
Wire Wire Line
	2800 1000 2900 1000
Connection ~ 2400 1300
Wire Wire Line
	1900 1000 1900 1300
$Comp
L pspice:R R9
U 1 1 61BFFA9A
P 2150 1300
F 0 "R9" V 1975 1250 50  0000 L CNN
F 1 "22 kOhm" V 2050 1150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 1300 50  0001 C CNN
F 3 "~" H 2150 1300 50  0001 C CNN
	1    2150 1300
	0    1    1    0   
$EndComp
$Comp
L pspice:R R18
U 1 1 61C0679C
P 2650 2000
F 0 "R18" V 2825 1925 50  0000 L CNN
F 1 "22 kOhm" V 2750 1850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 2000 50  0001 C CNN
F 3 "~" H 2650 2000 50  0001 C CNN
	1    2650 2000
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D2
U 1 1 61C067A2
P 2150 1700
F 0 "D2" H 2200 1900 50  0000 R CNN
F 1 "BAT85" H 2250 1825 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 1525 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 1700 50  0001 C CNN
	1    2150 1700
	-1   0    0    1   
$EndComp
Connection ~ 2400 1700
Wire Wire Line
	2400 1700 2400 2000
Wire Wire Line
	2300 1700 2400 1700
Wire Wire Line
	2000 1700 1900 1700
Wire Wire Line
	2800 1700 2900 1700
Wire Wire Line
	2900 1700 2900 2000
$Comp
L Diode:BAT85 D10
U 1 1 61C067AF
P 2650 1700
F 0 "D10" H 2650 1900 50  0000 C CNN
F 1 "BAT85" H 2650 1825 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 1525 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 1700 50  0001 C CNN
	1    2650 1700
	-1   0    0    1   
$EndComp
Connection ~ 2400 2000
$Comp
L pspice:R R10
U 1 1 61C067B7
P 2150 2000
F 0 "R10" V 2325 1925 50  0000 L CNN
F 1 "22 kOhm" V 2250 1850 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 2000 50  0001 C CNN
F 3 "~" H 2150 2000 50  0001 C CNN
	1    2150 2000
	0    1    1    0   
$EndComp
$Comp
L pspice:R R1
U 1 1 61C30414
P 1650 1550
F 0 "R1" V 1475 1500 50  0000 L CNN
F 1 "100 kOhm" V 1550 1350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 1550 50  0001 C CNN
F 3 "~" H 1650 1550 50  0001 C CNN
	1    1650 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 1000 2900 1300
Text GLabel 6425 3975 0    50   Input ~ 0
GND
Text GLabel 6425 2775 0    50   Input ~ 0
GPIO36
Text GLabel 6425 2875 0    50   Input ~ 0
GPIO39
Text GLabel 6425 3075 0    50   Input ~ 0
GPIO35
$Comp
L Diode:BAT85 D1
U 1 1 61BFFA85
P 2150 1000
F 0 "D1" H 2200 800 50  0000 R CNN
F 1 "BAT85" H 2250 900 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 1000 50  0001 C CNN
	1    2150 1000
	-1   0    0    1   
$EndComp
Text GLabel 2500 1450 2    50   Input ~ 0
GPIO35
Wire Wire Line
	2500 1700 2400 1700
Wire Wire Line
	2500 1550 2400 1550
Wire Wire Line
	2400 1550 2400 1700
Wire Wire Line
	2500 1450 2400 1450
Wire Wire Line
	2400 1450 2400 1300
Wire Wire Line
	1900 1450 2400 1450
Connection ~ 2400 1450
Wire Wire Line
	1900 1550 2400 1550
Connection ~ 2400 1550
Text GLabel 1800 1000 0    50   Input ~ 0
GND
Wire Wire Line
	2900 1300 2900 1700
Connection ~ 2900 1300
Connection ~ 2900 1700
Text GLabel 3000 1000 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D9
U 1 1 61BFFA92
P 2650 1000
F 0 "D9" H 2650 800 50  0000 C CNN
F 1 "BAT85" H 2650 900 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 1000 50  0001 C CNN
	1    2650 1000
	-1   0    0    1   
$EndComp
Connection ~ 2900 1000
Wire Wire Line
	1900 1700 1900 2000
Text Notes 750  800  0    50   ~ 0
Snare Trigger Input
Wire Notes Line
	700  700  700  2300
Text GLabel 2500 1550 2    50   Input ~ 0
GPIO36
Text GLabel 6425 2975 0    50   Input ~ 0
GPIO34
Text GLabel 6425 3175 0    50   Input ~ 0
GPIO32
Text GLabel 6425 3275 0    50   Input ~ 0
GPIO33
Text GLabel 6425 3375 0    50   Input ~ 0
GPIO25
Text GLabel 6425 3475 0    50   Input ~ 0
GPIO26
Text GLabel 6425 3575 0    50   Input ~ 0
GPIO27
Text GLabel 6425 3675 0    50   Input ~ 0
GPIO14
Text GLabel 6425 3775 0    50   Input ~ 0
GPIO12
Text GLabel 6425 3875 0    50   Input ~ 0
GPIO13
Wire Notes Line
	5950 5650 5950 7250
$Comp
L pspice:R R38
U 1 1 61D81CAE
P 7950 1850
F 0 "R38" V 8125 1775 50  0000 L CNN
F 1 "10 kOhm" V 8050 1700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 7950 1850 50  0001 C CNN
F 3 "~" H 7950 1850 50  0001 C CNN
	1    7950 1850
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D25
U 1 1 61D81CB4
P 7450 1850
F 0 "D25" H 7525 2050 50  0000 R CNN
F 1 "BAT85" H 7550 1975 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 7450 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 7450 1850 50  0001 C CNN
	1    7450 1850
	-1   0    0    1   
$EndComp
Connection ~ 7700 1550
Wire Wire Line
	7700 1550 7700 1850
Wire Wire Line
	8100 1550 8200 1550
Wire Wire Line
	8200 1550 8200 1850
$Comp
L Diode:BAT85 D26
U 1 1 61D81CC0
P 7950 1550
F 0 "D26" H 7950 1750 50  0000 C CNN
F 1 "BAT85" H 7950 1675 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 7950 1375 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 7950 1550 50  0001 C CNN
	1    7950 1550
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R37
U 1 1 61D81CC7
P 7500 1250
F 0 "R37" V 7325 1175 50  0000 L CNN
F 1 "10 kOhm" V 7400 1100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 7500 1250 50  0001 C CNN
F 3 "~" H 7500 1250 50  0001 C CNN
	1    7500 1250
	0    1    1    0   
$EndComp
Wire Wire Line
	7800 1550 7700 1550
Text GLabel 6800 1200 2    50   Input ~ 0
GND
Text GLabel 8250 1550 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J9
U 1 1 61D81CDE
P 6500 1300
F 0 "J9" H 6450 950 50  0000 C CNN
F 1 "Hi-Hat Ctrl" H 6450 1050 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 6500 1300 50  0001 C CNN
F 3 "~" H 6500 1300 50  0001 C CNN
	1    6500 1300
	1    0    0    -1  
$EndComp
Text Notes 6025 800  0    50   ~ 0
Hi-Hat Control Input
Wire Notes Line
	5975 700  5975 2300
Wire Wire Line
	8250 1550 8200 1550
Connection ~ 8200 1550
Wire Wire Line
	7700 1550 7700 1400
Wire Wire Line
	7700 1400 7250 1400
Text GLabel 8100 1250 2    50   Input ~ 0
GPIO25
Wire Wire Line
	7600 1850 7700 1850
Connection ~ 7700 1850
Wire Wire Line
	8100 1250 7750 1250
Wire Wire Line
	7250 1250 7250 1400
Connection ~ 7250 1400
Wire Wire Line
	7250 1400 6700 1400
Wire Notes Line
	5975 2300 8675 2300
Wire Notes Line
	5975 700  8675 700 
Wire Notes Line
	8675 700  8675 2300
NoConn ~ 6700 1300
NoConn ~ 8275 3975
NoConn ~ 8275 3775
NoConn ~ 8275 3375
NoConn ~ 8275 3275
NoConn ~ 8275 3075
NoConn ~ 8275 2975
NoConn ~ 8275 2875
NoConn ~ 8275 2775
NoConn ~ 8275 2675
Text GLabel 8275 4075 2    50   Input ~ 0
3V3
Text GLabel 8275 3875 2    50   Input ~ 0
GPIO15
NoConn ~ 8275 3675
NoConn ~ 6425 2675
NoConn ~ 6425 4075
Text GLabel 1800 2000 0    50   Input ~ 0
GND
Connection ~ 1900 2000
Wire Wire Line
	1800 2000 1900 2000
$Comp
L pspice:R R2
U 1 1 61D71A8A
P 1650 3100
F 0 "R2" V 1825 3050 50  0000 L CNN
F 1 "100 kOhm" V 1750 2900 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 3100 50  0001 C CNN
F 3 "~" H 1650 3100 50  0001 C CNN
	1    1650 3100
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R19
U 1 1 61D71A90
P 2650 2950
F 0 "R19" V 2475 2875 50  0000 L CNN
F 1 "22 kOhm" V 2550 2800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 2950 50  0001 C CNN
F 3 "~" H 2650 2950 50  0001 C CNN
	1    2650 2950
	0    1    1    0   
$EndComp
Wire Wire Line
	2500 2650 2400 2650
Connection ~ 2400 2650
Wire Wire Line
	2400 2650 2400 2950
Wire Wire Line
	2300 2650 2400 2650
Wire Wire Line
	2000 2650 1900 2650
Wire Wire Line
	2800 2650 2900 2650
Connection ~ 2400 2950
Wire Wire Line
	1900 2650 1900 2950
$Comp
L pspice:R R11
U 1 1 61D71A9E
P 2150 2950
F 0 "R11" V 1975 2900 50  0000 L CNN
F 1 "22 kOhm" V 2050 2800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 2950 50  0001 C CNN
F 3 "~" H 2150 2950 50  0001 C CNN
	1    2150 2950
	0    1    1    0   
$EndComp
$Comp
L pspice:R R20
U 1 1 61D71AA4
P 2650 3650
F 0 "R20" V 2825 3575 50  0000 L CNN
F 1 "22 kOhm" V 2750 3500 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 3650 50  0001 C CNN
F 3 "~" H 2650 3650 50  0001 C CNN
	1    2650 3650
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D4
U 1 1 61D71AAA
P 2150 3350
F 0 "D4" H 2200 3550 50  0000 R CNN
F 1 "BAT85" H 2250 3475 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 3175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 3350 50  0001 C CNN
	1    2150 3350
	-1   0    0    1   
$EndComp
Connection ~ 2400 3350
Wire Wire Line
	2400 3350 2400 3650
Wire Wire Line
	2300 3350 2400 3350
Wire Wire Line
	2000 3350 1900 3350
Wire Wire Line
	2800 3350 2900 3350
Wire Wire Line
	2900 3350 2900 3650
$Comp
L Diode:BAT85 D12
U 1 1 61D71AB6
P 2650 3350
F 0 "D12" H 2650 3550 50  0000 C CNN
F 1 "BAT85" H 2650 3475 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 3175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 3350 50  0001 C CNN
	1    2650 3350
	-1   0    0    1   
$EndComp
Connection ~ 2400 3650
$Comp
L pspice:R R12
U 1 1 61D71ABD
P 2150 3650
F 0 "R12" V 2325 3575 50  0000 L CNN
F 1 "22 kOhm" V 2250 3500 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 3650 50  0001 C CNN
F 3 "~" H 2150 3650 50  0001 C CNN
	1    2150 3650
	0    1    1    0   
$EndComp
$Comp
L pspice:R R4
U 1 1 61D71AC3
P 1650 3200
F 0 "R4" V 1475 3150 50  0000 L CNN
F 1 "100 kOhm" V 1550 3000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 3200 50  0001 C CNN
F 3 "~" H 1650 3200 50  0001 C CNN
	1    1650 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 2650 2900 2950
$Comp
L Diode:BAT85 D3
U 1 1 61D71ACA
P 2150 2650
F 0 "D3" H 2200 2450 50  0000 R CNN
F 1 "BAT85" H 2250 2550 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 2475 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 2650 50  0001 C CNN
	1    2150 2650
	-1   0    0    1   
$EndComp
Text GLabel 2500 3100 2    50   Input ~ 0
GPIO26
Wire Wire Line
	2500 3350 2400 3350
Wire Wire Line
	2500 3200 2400 3200
Wire Wire Line
	2400 3200 2400 3350
Wire Wire Line
	2500 3100 2400 3100
Wire Wire Line
	2400 3100 2400 2950
Wire Wire Line
	1900 3100 2400 3100
Connection ~ 2400 3100
Wire Wire Line
	1900 3200 2400 3200
Connection ~ 2400 3200
Connection ~ 1900 2650
Wire Wire Line
	1400 2650 1900 2650
Wire Wire Line
	2900 2950 2900 3350
Connection ~ 2900 2950
Connection ~ 2900 3350
Text GLabel 3000 2650 2    50   Input ~ 0
3V3
$Comp
L Diode:BAT85 D11
U 1 1 61D71AE1
P 2650 2650
F 0 "D11" H 2650 2450 50  0000 C CNN
F 1 "BAT85" H 2650 2550 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 2475 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 2650 50  0001 C CNN
	1    2650 2650
	-1   0    0    1   
$EndComp
Connection ~ 2900 2650
$Comp
L Connector:AudioJack3 J2
U 1 1 61D71AE9
P 1200 3100
F 0 "J2" H 1150 2775 50  0000 C CNN
F 1 "Hi-Hat" H 1150 2850 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1200 3100 50  0001 C CNN
F 3 "~" H 1200 3100 50  0001 C CNN
	1    1200 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1900 3350 1900 3650
Text Notes 750  2450 0    50   ~ 0
Hi-Hat Trigger Input
Wire Notes Line
	700  2350 700  3950
Text GLabel 2500 3200 2    50   Input ~ 0
GPIO32
Text GLabel 1800 3650 0    50   Input ~ 0
GND
Connection ~ 1900 3650
Wire Wire Line
	1400 2650 1400 3000
Wire Wire Line
	1800 3650 1900 3650
Wire Wire Line
	2900 1000 3000 1000
Wire Wire Line
	2900 2650 3000 2650
Text GLabel 7225 1850 0    50   Input ~ 0
GND
Wire Wire Line
	7225 1850 7300 1850
Wire Wire Line
	6800 1200 6700 1200
NoConn ~ 8275 3175
$Comp
L doit-esp32-devkit-v1:ESP32-DEVKIT-V1 U1
U 1 1 61B5316D
P 7375 2975
F 0 "U1" H 7350 3540 50  0000 C CNN
F 1 "ESP32-DEVKIT-V1" H 7350 3449 50  0000 C CNN
F 2 "edrumulus:esp32_devkit_v1" H 7325 3425 50  0001 C CNN
F 3 "" H 7325 3425 50  0001 C CNN
	1    7375 2975
	1    0    0    -1  
$EndComp
Wire Notes Line
	5950 4000 5950 5600
Connection ~ 4500 3475
Wire Wire Line
	4425 3475 4500 3475
Text GLabel 4425 3475 0    50   Input ~ 0
GND
Wire Wire Line
	4000 2825 4100 2825
NoConn ~ 4000 2925
Wire Wire Line
	5000 3025 5400 3025
Connection ~ 5500 3475
Wire Wire Line
	5550 3475 5500 3475
Text GLabel 5400 3025 2    50   Input ~ 0
GPIO39
Wire Wire Line
	4500 3175 4500 3475
$Comp
L Connector:AudioJack3 J7
U 1 1 62004AA6
P 3800 2925
F 0 "J7" H 3750 2575 50  0000 C CNN
F 1 "Tom1" H 3750 2675 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 3800 2925 50  0001 C CNN
F 3 "~" H 3800 2925 50  0001 C CNN
	1    3800 2925
	1    0    0    -1  
$EndComp
Text GLabel 5550 3475 2    50   Input ~ 0
3V3
Text GLabel 4100 2825 2    50   Input ~ 0
GND
Connection ~ 5000 3025
Wire Wire Line
	4500 3025 5000 3025
Wire Wire Line
	5000 3025 5000 3175
Wire Wire Line
	5100 3175 5000 3175
$Comp
L pspice:R R27
U 1 1 62004A9A
P 4250 3025
F 0 "R27" V 4075 2950 50  0000 L CNN
F 1 "100 kOhm" V 4150 2825 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4250 3025 50  0001 C CNN
F 3 "~" H 4250 3025 50  0001 C CNN
	1    4250 3025
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R31
U 1 1 62004A94
P 4750 3475
F 0 "R31" V 4925 3400 50  0000 L CNN
F 1 "22 kOhm" V 4850 3325 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4750 3475 50  0001 C CNN
F 3 "~" H 4750 3475 50  0001 C CNN
	1    4750 3475
	0    1    1    0   
$EndComp
Connection ~ 5000 3475
$Comp
L Diode:BAT85 D23
U 1 1 62004A8D
P 5250 3175
F 0 "D23" H 5250 3375 50  0000 C CNN
F 1 "BAT85" H 5250 3300 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5250 3000 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5250 3175 50  0001 C CNN
	1    5250 3175
	-1   0    0    1   
$EndComp
Wire Wire Line
	5500 3175 5500 3475
Wire Wire Line
	5400 3175 5500 3175
Wire Wire Line
	4600 3175 4500 3175
Wire Wire Line
	4900 3175 5000 3175
Wire Wire Line
	5000 3175 5000 3475
Connection ~ 5000 3175
$Comp
L Diode:BAT85 D19
U 1 1 62004A81
P 4750 3175
F 0 "D19" H 4825 3375 50  0000 R CNN
F 1 "BAT85" H 4850 3300 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4750 3000 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4750 3175 50  0001 C CNN
	1    4750 3175
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R35
U 1 1 62004A7B
P 5250 3475
F 0 "R35" V 5425 3400 50  0000 L CNN
F 1 "22 kOhm" V 5350 3325 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5250 3475 50  0001 C CNN
F 3 "~" H 5250 3475 50  0001 C CNN
	1    5250 3475
	0    1    1    0   
$EndComp
Connection ~ 4525 1850
Wire Wire Line
	4450 1850 4525 1850
Text GLabel 4450 1850 0    50   Input ~ 0
GND
Wire Wire Line
	4025 1200 4125 1200
Wire Notes Line
	700  3950 3300 3950
Wire Notes Line
	3300 2350 3300 3950
Wire Notes Line
	3300 2350 700  2350
NoConn ~ 4025 1300
Wire Wire Line
	5025 1400 5425 1400
Connection ~ 5525 1850
Wire Wire Line
	5575 1850 5525 1850
Wire Notes Line
	3350 2350 3350 3950
Wire Notes Line
	3350 3950 5950 3950
Wire Notes Line
	5950 2350 5950 3950
Text Notes 3400 2450 0    50   ~ 0
Tom1 Trigger Input
Wire Notes Line
	5950 2350 3350 2350
Text GLabel 5425 1400 2    50   Input ~ 0
GPIO33
Wire Notes Line
	3350 700  3350 2300
Wire Notes Line
	3350 2300 5950 2300
Wire Notes Line
	5950 700  5950 2300
Text Notes 3400 800  0    50   ~ 0
Kick Trigger Input
Wire Notes Line
	5950 700  3350 700 
Wire Wire Line
	4525 1550 4525 1850
$Comp
L Connector:AudioJack3 J5
U 1 1 61D09D89
P 3825 1300
F 0 "J5" H 3775 950 50  0000 C CNN
F 1 "Kick" H 3775 1050 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 3825 1300 50  0001 C CNN
F 3 "~" H 3825 1300 50  0001 C CNN
	1    3825 1300
	1    0    0    -1  
$EndComp
Text GLabel 5575 1850 2    50   Input ~ 0
3V3
Text GLabel 4125 1200 2    50   Input ~ 0
GND
Connection ~ 5025 1400
Wire Wire Line
	4525 1400 5025 1400
Wire Wire Line
	5025 1400 5025 1550
Wire Wire Line
	5125 1550 5025 1550
$Comp
L pspice:R R25
U 1 1 61D09D5D
P 4275 1400
F 0 "R25" V 4100 1325 50  0000 L CNN
F 1 "100 kOhm" V 4175 1200 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4275 1400 50  0001 C CNN
F 3 "~" H 4275 1400 50  0001 C CNN
	1    4275 1400
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R29
U 1 1 61D09D57
P 4775 1850
F 0 "R29" V 4950 1775 50  0000 L CNN
F 1 "22 kOhm" V 4875 1700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4775 1850 50  0001 C CNN
F 3 "~" H 4775 1850 50  0001 C CNN
	1    4775 1850
	0    1    1    0   
$EndComp
Connection ~ 5025 1850
$Comp
L Diode:BAT85 D21
U 1 1 61D09D50
P 5275 1550
F 0 "D21" H 5275 1750 50  0000 C CNN
F 1 "BAT85" H 5275 1675 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5275 1375 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5275 1550 50  0001 C CNN
	1    5275 1550
	-1   0    0    1   
$EndComp
Wire Wire Line
	5525 1550 5525 1850
Wire Wire Line
	5425 1550 5525 1550
Wire Wire Line
	4625 1550 4525 1550
Wire Wire Line
	4925 1550 5025 1550
Wire Wire Line
	5025 1550 5025 1850
Connection ~ 5025 1550
$Comp
L Diode:BAT85 D17
U 1 1 61D09D44
P 4775 1550
F 0 "D17" H 4850 1750 50  0000 R CNN
F 1 "BAT85" H 4875 1675 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4775 1375 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4775 1550 50  0001 C CNN
	1    4775 1550
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R33
U 1 1 61D09D3E
P 5275 1850
F 0 "R33" V 5450 1775 50  0000 L CNN
F 1 "22 kOhm" V 5375 1700 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5275 1850 50  0001 C CNN
F 3 "~" H 5275 1850 50  0001 C CNN
	1    5275 1850
	0    1    1    0   
$EndComp
Wire Notes Line
	700  2300 3300 2300
Wire Notes Line
	3300 700  3300 2300
Wire Notes Line
	3300 700  700  700 
$Comp
L Connector:DB25_Female_MountingHoles J?
U 1 1 62DF6D0B
P 9275 2025
F 0 "J?" H 9193 3517 50  0000 C CNN
F 1 "Trigger Connector" H 9193 3426 50  0000 C CNN
F 2 "" H 9275 2025 50  0001 C CNN
F 3 " ~" H 9275 2025 50  0001 C CNN
	1    9275 2025
	-1   0    0    -1  
$EndComp
Connection ~ 1400 2650
Wire Wire Line
	1300 2650 1400 2650
Text GLabel 1300 2650 0    50   Input ~ 0
GND
Text GLabel 1300 2650 0    50   Input ~ 0
GND
Text GLabel 9575 825  2    50   Input ~ 0
KICK-TIP
Text GLabel 9575 925  2    50   Input ~ 0
TOM1-TIP
Text GLabel 9575 1025 2    50   Input ~ 0
KICK-SLEEVE
Text GLabel 9575 1125 2    50   Input ~ 0
TOM1-RING
Text GLabel 9575 1225 2    50   Input ~ 0
SNARE-TIP
Text GLabel 9575 1325 2    50   Input ~ 0
TOM1-SLEEVE
Text GLabel 9575 1425 2    50   Input ~ 0
SNARE-RING
Text GLabel 9575 1525 2    50   Input ~ 0
TOM2-TIP
Text GLabel 9575 1625 2    50   Input ~ 0
SNARE-SLEEVE
Text GLabel 9575 1725 2    50   Input ~ 0
TOM2-RING
Text GLabel 9575 1825 2    50   Input ~ 0
HI-HAT-SLEEVE&TOM2-SLEEVE
Text GLabel 9575 1925 2    50   Input ~ 0
TOM3-SLEEVE
Text GLabel 9575 2025 2    50   Input ~ 0
HI-HAT-TIP
Text GLabel 9575 2125 2    50   Input ~ 0
TOM3-TIP
Text GLabel 9575 2225 2    50   Input ~ 0
HI-HAT-RING
Text GLabel 9575 2325 2    50   Input ~ 0
TOM3-RING
Text GLabel 9575 2425 2    50   Input ~ 0
HI-HAT-CONTROL-SLEEVE
Text GLabel 9575 2525 2    50   Input ~ 0
RIDE-SLEEVE&RIDE-BELL_SLEEVE
Text GLabel 9575 2625 2    50   Input ~ 0
HI-HAT-CONTROL-TIP
Text GLabel 9575 2725 2    50   Input ~ 0
RIDE-RING
Text GLabel 9575 2825 2    50   Input ~ 0
CRASH1-SLEEVE
Text GLabel 9575 2925 2    50   Input ~ 0
RIDE-TIP
Text GLabel 9575 3025 2    50   Input ~ 0
CRASH1-TIP
Text GLabel 9575 3125 2    50   Input ~ 0
RIDE-BELL-RING
Text GLabel 9575 3225 2    50   Input ~ 0
CRASH1-RING
Wire Notes Line
	5950 4000 3350 4000
Text Notes 3400 4100 0    50   ~ 0
Tom2 Trigger Input
Wire Notes Line
	3350 5600 5950 5600
Wire Notes Line
	3350 4000 3350 5600
Wire Notes Line
	3300 4000 700  4000
Wire Notes Line
	3300 4000 3300 5600
Wire Notes Line
	700  5600 3300 5600
$Comp
L pspice:R R36
U 1 1 62096937
P 5250 5100
F 0 "R36" V 5425 5025 50  0000 L CNN
F 1 "22 kOhm" V 5350 4950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5250 5100 50  0001 C CNN
F 3 "~" H 5250 5100 50  0001 C CNN
	1    5250 5100
	0    1    1    0   
$EndComp
$Comp
L Diode:BAT85 D20
U 1 1 6209693D
P 4750 4800
F 0 "D20" H 4825 5000 50  0000 R CNN
F 1 "BAT85" H 4850 4925 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4750 4625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4750 4800 50  0001 C CNN
	1    4750 4800
	-1   0    0    1   
$EndComp
Connection ~ 5000 4800
Wire Wire Line
	5000 4800 5000 5100
Wire Wire Line
	4900 4800 5000 4800
Wire Wire Line
	4600 4800 4500 4800
Wire Wire Line
	5400 4800 5500 4800
Wire Wire Line
	5500 4800 5500 5100
$Comp
L Diode:BAT85 D24
U 1 1 62096949
P 5250 4800
F 0 "D24" H 5250 5000 50  0000 C CNN
F 1 "BAT85" H 5250 4925 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5250 4625 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5250 4800 50  0001 C CNN
	1    5250 4800
	-1   0    0    1   
$EndComp
Connection ~ 5000 5100
$Comp
L pspice:R R32
U 1 1 62096950
P 4750 5100
F 0 "R32" V 4925 5025 50  0000 L CNN
F 1 "22 kOhm" V 4850 4950 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4750 5100 50  0001 C CNN
F 3 "~" H 4750 5100 50  0001 C CNN
	1    4750 5100
	0    1    1    0   
$EndComp
$Comp
L pspice:R R28
U 1 1 62096956
P 4250 4650
F 0 "R28" V 4075 4575 50  0000 L CNN
F 1 "100 kOhm" V 4150 4450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4250 4650 50  0001 C CNN
F 3 "~" H 4250 4650 50  0001 C CNN
	1    4250 4650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5100 4800 5000 4800
Wire Wire Line
	5000 4650 5000 4800
Wire Wire Line
	4500 4650 5000 4650
Connection ~ 5000 4650
Text GLabel 4100 4450 2    50   Input ~ 0
GND
Text GLabel 5550 5100 2    50   Input ~ 0
3V3
$Comp
L Connector:AudioJack3 J8
U 1 1 62096962
P 3800 4550
F 0 "J8" H 3750 4200 50  0000 C CNN
F 1 "Tom2" H 3750 4300 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 3800 4550 50  0001 C CNN
F 3 "~" H 3800 4550 50  0001 C CNN
	1    3800 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 4800 4500 5100
Text GLabel 5400 4650 2    50   Input ~ 0
GPIO12
Wire Wire Line
	5550 5100 5500 5100
Connection ~ 5500 5100
Wire Wire Line
	5000 4650 5400 4650
NoConn ~ 4000 4550
Wire Wire Line
	4000 4450 4100 4450
Text GLabel 4425 5100 0    50   Input ~ 0
GND
Wire Wire Line
	4425 5100 4500 5100
Connection ~ 4500 5100
Connection ~ 4475 6750
Wire Wire Line
	4400 6750 4475 6750
Text GLabel 4400 6750 0    50   Input ~ 0
GND
Wire Wire Line
	3975 6100 4075 6100
NoConn ~ 3975 6200
Wire Wire Line
	4975 6300 5375 6300
Connection ~ 5475 6750
Wire Wire Line
	5525 6750 5475 6750
Text GLabel 5375 6300 2    50   Input ~ 0
GPIO15
Wire Wire Line
	4475 6450 4475 6750
$Comp
L Connector:AudioJack3 J6
U 1 1 620E0068
P 3775 6200
F 0 "J6" H 3725 5850 50  0000 C CNN
F 1 "Tom3" H 3725 5950 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 3775 6200 50  0001 C CNN
F 3 "~" H 3775 6200 50  0001 C CNN
	1    3775 6200
	1    0    0    -1  
$EndComp
Text GLabel 5525 6750 2    50   Input ~ 0
3V3
Text GLabel 4075 6100 2    50   Input ~ 0
GND
Connection ~ 4975 6300
Wire Wire Line
	4475 6300 4975 6300
Wire Wire Line
	4975 6300 4975 6450
Wire Wire Line
	5075 6450 4975 6450
$Comp
L pspice:R R26
U 1 1 620E005C
P 4225 6300
F 0 "R26" V 4050 6225 50  0000 L CNN
F 1 "100 kOhm" V 4125 6100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4225 6300 50  0001 C CNN
F 3 "~" H 4225 6300 50  0001 C CNN
	1    4225 6300
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R30
U 1 1 620E0056
P 4725 6750
F 0 "R30" V 4900 6675 50  0000 L CNN
F 1 "22 kOhm" V 4825 6600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 4725 6750 50  0001 C CNN
F 3 "~" H 4725 6750 50  0001 C CNN
	1    4725 6750
	0    1    1    0   
$EndComp
Connection ~ 4975 6750
$Comp
L Diode:BAT85 D22
U 1 1 620E004F
P 5225 6450
F 0 "D22" H 5225 6650 50  0000 C CNN
F 1 "BAT85" H 5225 6575 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 5225 6275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 5225 6450 50  0001 C CNN
	1    5225 6450
	-1   0    0    1   
$EndComp
Wire Wire Line
	5475 6450 5475 6750
Wire Wire Line
	5375 6450 5475 6450
Wire Wire Line
	4575 6450 4475 6450
Wire Wire Line
	4875 6450 4975 6450
Wire Wire Line
	4975 6450 4975 6750
Connection ~ 4975 6450
$Comp
L Diode:BAT85 D18
U 1 1 620E0043
P 4725 6450
F 0 "D18" H 4800 6650 50  0000 R CNN
F 1 "BAT85" H 4825 6575 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4725 6275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4725 6450 50  0001 C CNN
	1    4725 6450
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R34
U 1 1 620E003D
P 5225 6750
F 0 "R34" V 5400 6675 50  0000 L CNN
F 1 "22 kOhm" V 5325 6600 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 5225 6750 50  0001 C CNN
F 3 "~" H 5225 6750 50  0001 C CNN
	1    5225 6750
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 5950 3000 5950
Wire Wire Line
	2900 4300 3000 4300
Wire Wire Line
	1800 6950 1900 6950
Connection ~ 1400 5950
Wire Wire Line
	1300 5950 1400 5950
Wire Wire Line
	1400 5950 1400 6300
Connection ~ 1900 6950
Text GLabel 1800 6950 0    50   Input ~ 0
GND
Text GLabel 2500 6500 2    50   Input ~ 0
GPIO27
Wire Notes Line
	700  5650 700  7250
Wire Notes Line
	700  7250 3300 7250
Wire Notes Line
	3300 5650 3300 7250
Text Notes 750  5750 0    50   ~ 0
Ride Trigger Input
Wire Notes Line
	3300 5650 700  5650
Wire Wire Line
	1900 6650 1900 6950
$Comp
L Connector:AudioJack3 J4
U 1 1 61E10278
P 1200 6400
F 0 "J4" H 1150 6075 50  0000 C CNN
F 1 "Ride" H 1150 6150 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1200 6400 50  0001 C CNN
F 3 "~" H 1200 6400 50  0001 C CNN
	1    1200 6400
	1    0    0    -1  
$EndComp
Connection ~ 2900 5950
$Comp
L Diode:BAT85 D15
U 1 1 61E10270
P 2650 5950
F 0 "D15" H 2650 5750 50  0000 C CNN
F 1 "BAT85" H 2650 5850 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 5775 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 5950 50  0001 C CNN
	1    2650 5950
	-1   0    0    1   
$EndComp
Text GLabel 3000 5950 2    50   Input ~ 0
3V3
Connection ~ 2900 6650
Connection ~ 2900 6250
Wire Wire Line
	2900 6250 2900 6650
Text GLabel 1300 5950 0    50   Input ~ 0
GND
Wire Wire Line
	1400 5950 1900 5950
Connection ~ 1900 5950
Connection ~ 2400 6500
Wire Wire Line
	1900 6500 2400 6500
Connection ~ 2400 6400
Wire Wire Line
	1900 6400 2400 6400
Wire Wire Line
	2400 6400 2400 6250
Wire Wire Line
	2500 6400 2400 6400
Wire Wire Line
	2400 6500 2400 6650
Wire Wire Line
	2500 6500 2400 6500
Wire Wire Line
	2500 6650 2400 6650
Text GLabel 2500 6400 2    50   Input ~ 0
GPIO13
$Comp
L Diode:BAT85 D7
U 1 1 61E10259
P 2150 5950
F 0 "D7" H 2200 5750 50  0000 R CNN
F 1 "BAT85" H 2250 5850 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 5775 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 5950 50  0001 C CNN
	1    2150 5950
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 5950 2900 6250
$Comp
L pspice:R R8
U 1 1 61E10252
P 1650 6500
F 0 "R8" V 1475 6450 50  0000 L CNN
F 1 "100 kOhm" V 1550 6300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 6500 50  0001 C CNN
F 3 "~" H 1650 6500 50  0001 C CNN
	1    1650 6500
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R16
U 1 1 61E1024C
P 2150 6950
F 0 "R16" V 2325 6875 50  0000 L CNN
F 1 "22 kOhm" V 2250 6800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 6950 50  0001 C CNN
F 3 "~" H 2150 6950 50  0001 C CNN
	1    2150 6950
	0    1    1    0   
$EndComp
Connection ~ 2400 6950
$Comp
L Diode:BAT85 D16
U 1 1 61E10245
P 2650 6650
F 0 "D16" H 2650 6850 50  0000 C CNN
F 1 "BAT85" H 2650 6775 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 6475 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 6650 50  0001 C CNN
	1    2650 6650
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 6650 2900 6950
Wire Wire Line
	2800 6650 2900 6650
Wire Wire Line
	2000 6650 1900 6650
Wire Wire Line
	2300 6650 2400 6650
Wire Wire Line
	2400 6650 2400 6950
Connection ~ 2400 6650
$Comp
L Diode:BAT85 D8
U 1 1 61E10239
P 2150 6650
F 0 "D8" H 2200 6850 50  0000 R CNN
F 1 "BAT85" H 2250 6775 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 6475 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 6650 50  0001 C CNN
	1    2150 6650
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R24
U 1 1 61E10233
P 2650 6950
F 0 "R24" V 2825 6875 50  0000 L CNN
F 1 "22 kOhm" V 2750 6800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 6950 50  0001 C CNN
F 3 "~" H 2650 6950 50  0001 C CNN
	1    2650 6950
	0    1    1    0   
$EndComp
$Comp
L pspice:R R15
U 1 1 61E1022D
P 2150 6250
F 0 "R15" V 1975 6200 50  0000 L CNN
F 1 "22 kOhm" V 2050 6100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 6250 50  0001 C CNN
F 3 "~" H 2150 6250 50  0001 C CNN
	1    2150 6250
	0    1    1    0   
$EndComp
Wire Wire Line
	1900 5950 1900 6250
Connection ~ 2400 6250
Wire Wire Line
	2800 5950 2900 5950
Wire Wire Line
	2000 5950 1900 5950
Wire Wire Line
	2300 5950 2400 5950
Wire Wire Line
	2400 5950 2400 6250
Connection ~ 2400 5950
Wire Wire Line
	2500 5950 2400 5950
$Comp
L pspice:R R23
U 1 1 61E1021F
P 2650 6250
F 0 "R23" V 2475 6175 50  0000 L CNN
F 1 "22 kOhm" V 2550 6100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 6250 50  0001 C CNN
F 3 "~" H 2650 6250 50  0001 C CNN
	1    2650 6250
	0    1    1    0   
$EndComp
$Comp
L pspice:R R7
U 1 1 61E10219
P 1650 6400
F 0 "R7" V 1825 6350 50  0000 L CNN
F 1 "100 kOhm" V 1750 6200 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 6400 50  0001 C CNN
F 3 "~" H 1650 6400 50  0001 C CNN
	1    1650 6400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1800 5300 1900 5300
Connection ~ 1400 4300
Wire Wire Line
	1300 4300 1400 4300
Wire Wire Line
	1400 4300 1400 4650
Connection ~ 1900 5300
Text GLabel 1800 5300 0    50   Input ~ 0
GND
Text GLabel 2500 4850 2    50   Input ~ 0
GPIO34
Wire Notes Line
	700  4000 700  5600
Text Notes 750  4100 0    50   ~ 0
Crash Trigger Input
Wire Wire Line
	1900 5000 1900 5300
$Comp
L Connector:AudioJack3 J3
U 1 1 61DC57C7
P 1200 4750
F 0 "J3" H 1150 4425 50  0000 C CNN
F 1 "Crash" H 1150 4500 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 1200 4750 50  0001 C CNN
F 3 "~" H 1200 4750 50  0001 C CNN
	1    1200 4750
	1    0    0    -1  
$EndComp
Connection ~ 2900 4300
$Comp
L Diode:BAT85 D13
U 1 1 61DC57BF
P 2650 4300
F 0 "D13" H 2650 4100 50  0000 C CNN
F 1 "BAT85" H 2650 4200 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 4125 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 4300 50  0001 C CNN
	1    2650 4300
	-1   0    0    1   
$EndComp
Text GLabel 3000 4300 2    50   Input ~ 0
3V3
Connection ~ 2900 5000
Connection ~ 2900 4600
Wire Wire Line
	2900 4600 2900 5000
Text GLabel 1300 4300 0    50   Input ~ 0
GND
Wire Wire Line
	1400 4300 1900 4300
Connection ~ 1900 4300
Connection ~ 2400 4850
Wire Wire Line
	1900 4850 2400 4850
Connection ~ 2400 4750
Wire Wire Line
	1900 4750 2400 4750
Wire Wire Line
	2400 4750 2400 4600
Wire Wire Line
	2500 4750 2400 4750
Wire Wire Line
	2400 4850 2400 5000
Wire Wire Line
	2500 4850 2400 4850
Wire Wire Line
	2500 5000 2400 5000
Text GLabel 2500 4750 2    50   Input ~ 0
GPIO14
$Comp
L Diode:BAT85 D5
U 1 1 61DC57A8
P 2150 4300
F 0 "D5" H 2200 4100 50  0000 R CNN
F 1 "BAT85" H 2250 4200 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 4125 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 4300 50  0001 C CNN
	1    2150 4300
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 4300 2900 4600
$Comp
L pspice:R R6
U 1 1 61DC57A1
P 1650 4850
F 0 "R6" V 1475 4800 50  0000 L CNN
F 1 "100 kOhm" V 1550 4650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 4850 50  0001 C CNN
F 3 "~" H 1650 4850 50  0001 C CNN
	1    1650 4850
	0    -1   -1   0   
$EndComp
$Comp
L pspice:R R14
U 1 1 61DC579B
P 2150 5300
F 0 "R14" V 2325 5225 50  0000 L CNN
F 1 "22 kOhm" V 2250 5150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 5300 50  0001 C CNN
F 3 "~" H 2150 5300 50  0001 C CNN
	1    2150 5300
	0    1    1    0   
$EndComp
Connection ~ 2400 5300
$Comp
L Diode:BAT85 D14
U 1 1 61DC5794
P 2650 5000
F 0 "D14" H 2650 5200 50  0000 C CNN
F 1 "BAT85" H 2650 5125 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2650 4825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2650 5000 50  0001 C CNN
	1    2650 5000
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 5000 2900 5300
Wire Wire Line
	2800 5000 2900 5000
Wire Wire Line
	2000 5000 1900 5000
Wire Wire Line
	2300 5000 2400 5000
Wire Wire Line
	2400 5000 2400 5300
Connection ~ 2400 5000
$Comp
L Diode:BAT85 D6
U 1 1 61DC5788
P 2150 5000
F 0 "D6" H 2200 5200 50  0000 R CNN
F 1 "BAT85" H 2250 5125 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2150 4825 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2150 5000 50  0001 C CNN
	1    2150 5000
	-1   0    0    1   
$EndComp
$Comp
L pspice:R R22
U 1 1 61DC5782
P 2650 5300
F 0 "R22" V 2825 5225 50  0000 L CNN
F 1 "22 kOhm" V 2750 5150 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 5300 50  0001 C CNN
F 3 "~" H 2650 5300 50  0001 C CNN
	1    2650 5300
	0    1    1    0   
$EndComp
$Comp
L pspice:R R13
U 1 1 61DC577C
P 2150 4600
F 0 "R13" V 1975 4550 50  0000 L CNN
F 1 "22 kOhm" V 2050 4450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2150 4600 50  0001 C CNN
F 3 "~" H 2150 4600 50  0001 C CNN
	1    2150 4600
	0    1    1    0   
$EndComp
Wire Wire Line
	1900 4300 1900 4600
Connection ~ 2400 4600
Wire Wire Line
	2800 4300 2900 4300
Wire Wire Line
	2000 4300 1900 4300
Wire Wire Line
	2300 4300 2400 4300
Wire Wire Line
	2400 4300 2400 4600
Connection ~ 2400 4300
Wire Wire Line
	2500 4300 2400 4300
$Comp
L pspice:R R21
U 1 1 61DC576E
P 2650 4600
F 0 "R21" V 2475 4525 50  0000 L CNN
F 1 "22 kOhm" V 2550 4450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 2650 4600 50  0001 C CNN
F 3 "~" H 2650 4600 50  0001 C CNN
	1    2650 4600
	0    1    1    0   
$EndComp
$Comp
L pspice:R R5
U 1 1 61DC5768
P 1650 4750
F 0 "R5" V 1825 4700 50  0000 L CNN
F 1 "100 kOhm" V 1750 4550 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" H 1650 4750 50  0001 C CNN
F 3 "~" H 1650 4750 50  0001 C CNN
	1    1650 4750
	0    -1   -1   0   
$EndComp
Wire Notes Line
	3350 5650 3350 7250
Wire Notes Line
	3350 7250 5950 7250
Text Notes 3400 5750 0    50   ~ 0
Tom3 Trigger Input
Wire Notes Line
	5950 5650 3350 5650
Text GLabel 9575 3625 2    50   Input ~ 0
KICK-SLEEVE
Text GLabel 9575 3725 2    50   Input ~ 0
TOM1-SLEEVE
Text GLabel 9575 3825 2    50   Input ~ 0
SNARE-SLEEVE
Text GLabel 9575 3925 2    50   Input ~ 0
HI-HAT-SLEEVE&TOM2-SLEEVE
Text GLabel 9575 4025 2    50   Input ~ 0
TOM3-SLEEVE
Text GLabel 9575 4125 2    50   Input ~ 0
HI-HAT-CONTROL-SLEEVE
Text GLabel 9575 4225 2    50   Input ~ 0
RIDE-SLEEVE&RIDE-BELL_SLEEVE
Text GLabel 9575 4325 2    50   Input ~ 0
CRASH1-SLEEVE
Text GLabel 9375 3625 0    50   Input ~ 0
GND
Wire Wire Line
	9575 3625 9475 3625
Wire Wire Line
	9475 3625 9475 3725
Wire Wire Line
	9475 4325 9575 4325
Wire Wire Line
	9575 4225 9475 4225
Connection ~ 9475 4225
Wire Wire Line
	9475 4225 9475 4325
Wire Wire Line
	9575 4125 9475 4125
Connection ~ 9475 4125
Wire Wire Line
	9475 4125 9475 4225
Wire Wire Line
	9575 4025 9475 4025
Connection ~ 9475 4025
Wire Wire Line
	9475 4025 9475 4125
Wire Wire Line
	9575 3925 9475 3925
Connection ~ 9475 3925
Wire Wire Line
	9475 3925 9475 4025
Wire Wire Line
	9575 3825 9475 3825
Connection ~ 9475 3825
Wire Wire Line
	9475 3825 9475 3925
Wire Wire Line
	9575 3725 9475 3725
Connection ~ 9475 3725
Wire Wire Line
	9475 3725 9475 3825
Wire Wire Line
	9375 3625 9475 3625
Connection ~ 9475 3625
Wire Wire Line
	1900 1000 1800 1000
Connection ~ 1900 1000
Text GLabel 1400 1550 0    50   Input ~ 0
SNARE-TIP
Text GLabel 1400 1450 0    50   Input ~ 0
SNARE-RING
NoConn ~ 8275 3475
NoConn ~ 8275 3575
$EndSCHEMATC
