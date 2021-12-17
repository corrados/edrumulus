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
P 2050 3400
F 0 "J?" H 2032 3725 50  0000 C CNN
F 1 "AudioJack3" H 2032 3634 50  0000 C CNN
F 2 "edrumulus:Jack_6.35mm_Neutrik_NMJ6HCD2_Horizontal" H 2050 3400 50  0001 C CNN
F 3 "~" H 2050 3400 50  0001 C CNN
	1    2050 3400
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R3
U 1 1 61BE1FA0
P 3250 3150
F 0 "R3" V 3350 3100 50  0000 L CNN
F 1 "100 kOhm" V 3450 3000 50  0000 L CNN
F 2 "" H 3250 3150 50  0001 C CNN
F 3 "~" H 3250 3150 50  0001 C CNN
	1    3250 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 4100 9900 4100
Wire Wire Line
	6800 2500 7550 2500
$Comp
L pspice:R R?
U 1 1 61BF93AD
P 4600 1850
F 0 "R?" V 4800 1800 50  0000 L CNN
F 1 "22 kOhm" V 4700 1700 50  0000 L CNN
F 2 "" H 4600 1850 50  0001 C CNN
F 3 "~" H 4600 1850 50  0001 C CNN
	1    4600 1850
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT85 D?
U 1 1 61BF93B4
P 4300 2350
F 0 "D?" H 4350 2550 50  0000 R CNN
F 1 "BAT85" H 4400 2450 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4300 2175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4300 2350 50  0001 C CNN
	1    4300 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	4300 2000 4300 2100
Connection ~ 4300 2100
Wire Wire Line
	4300 2100 4600 2100
Wire Wire Line
	4300 2200 4300 2100
Wire Wire Line
	4300 2500 4300 2600
Wire Wire Line
	4300 1700 4300 1600
Wire Wire Line
	4300 1600 4600 1600
$Comp
L Diode:BAT85 D?
U 1 1 61BF93CA
P 4300 1850
F 0 "D?" H 4300 2066 50  0000 C CNN
F 1 "BAT85" H 4300 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 4300 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 4300 1850 50  0001 C CNN
	1    4300 1850
	0    1    1    0   
$EndComp
Connection ~ 4600 2100
Wire Wire Line
	4300 2600 4600 2600
$Comp
L pspice:R R?
U 1 1 61BF93BB
P 4600 2350
F 0 "R?" V 4800 2300 50  0000 L CNN
F 1 "22 kOhm" V 4700 2200 50  0000 L CNN
F 2 "" H 4600 2350 50  0001 C CNN
F 3 "~" H 4600 2350 50  0001 C CNN
	1    4600 2350
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 61BFFA7F
P 2250 1850
F 0 "R?" V 2450 1800 50  0000 L CNN
F 1 "22 kOhm" V 2350 1700 50  0000 L CNN
F 2 "" H 2250 1850 50  0001 C CNN
F 3 "~" H 2250 1850 50  0001 C CNN
	1    2250 1850
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT85 D?
U 1 1 61BFFA85
P 1950 2350
F 0 "D?" H 2000 2550 50  0000 R CNN
F 1 "BAT85" H 2050 2450 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 1950 2175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 1950 2350 50  0001 C CNN
	1    1950 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 2000 1950 2100
Connection ~ 1950 2100
Wire Wire Line
	1950 2100 2250 2100
Wire Wire Line
	1950 2200 1950 2100
Wire Wire Line
	1950 2500 1950 2600
Wire Wire Line
	1950 1700 1950 1600
Wire Wire Line
	1950 1600 2250 1600
$Comp
L Diode:BAT85 D?
U 1 1 61BFFA92
P 1950 1850
F 0 "D?" H 1950 2066 50  0000 C CNN
F 1 "BAT85" H 1950 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 1950 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 1950 1850 50  0001 C CNN
	1    1950 1850
	0    1    1    0   
$EndComp
Connection ~ 2250 2100
Wire Wire Line
	1950 2600 2250 2600
$Comp
L pspice:R R?
U 1 1 61BFFA9A
P 2250 2350
F 0 "R?" V 2450 2300 50  0000 L CNN
F 1 "22 kOhm" V 2350 2200 50  0000 L CNN
F 2 "" H 2250 2350 50  0001 C CNN
F 3 "~" H 2250 2350 50  0001 C CNN
	1    2250 2350
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 61C04939
P 3800 1850
F 0 "R?" V 4000 1800 50  0000 L CNN
F 1 "22 kOhm" V 3900 1700 50  0000 L CNN
F 2 "" H 3800 1850 50  0001 C CNN
F 3 "~" H 3800 1850 50  0001 C CNN
	1    3800 1850
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT85 D?
U 1 1 61C0493F
P 3500 2350
F 0 "D?" H 3550 2550 50  0000 R CNN
F 1 "BAT85" H 3600 2450 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3500 2175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3500 2350 50  0001 C CNN
	1    3500 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 2000 3500 2100
Connection ~ 3500 2100
Wire Wire Line
	3500 2100 3800 2100
Wire Wire Line
	3500 2200 3500 2100
Wire Wire Line
	3500 2500 3500 2600
Wire Wire Line
	3500 1700 3500 1600
Wire Wire Line
	3500 1600 3800 1600
$Comp
L Diode:BAT85 D?
U 1 1 61C0494C
P 3500 1850
F 0 "D?" H 3500 2066 50  0000 C CNN
F 1 "BAT85" H 3500 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 3500 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 3500 1850 50  0001 C CNN
	1    3500 1850
	0    1    1    0   
$EndComp
Connection ~ 3800 2100
Wire Wire Line
	3500 2600 3800 2600
$Comp
L pspice:R R?
U 1 1 61C04954
P 3800 2350
F 0 "R?" V 4000 2300 50  0000 L CNN
F 1 "22 kOhm" V 3900 2200 50  0000 L CNN
F 2 "" H 3800 2350 50  0001 C CNN
F 3 "~" H 3800 2350 50  0001 C CNN
	1    3800 2350
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 61C0679C
P 3000 1850
F 0 "R?" V 3200 1800 50  0000 L CNN
F 1 "22 kOhm" V 3100 1700 50  0000 L CNN
F 2 "" H 3000 1850 50  0001 C CNN
F 3 "~" H 3000 1850 50  0001 C CNN
	1    3000 1850
	1    0    0    -1  
$EndComp
$Comp
L Diode:BAT85 D?
U 1 1 61C067A2
P 2700 2350
F 0 "D?" H 2750 2550 50  0000 R CNN
F 1 "BAT85" H 2800 2450 50  0000 R CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2700 2175 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2700 2350 50  0001 C CNN
	1    2700 2350
	0    1    1    0   
$EndComp
Wire Wire Line
	2700 2000 2700 2100
Connection ~ 2700 2100
Wire Wire Line
	2700 2100 3000 2100
Wire Wire Line
	2700 2200 2700 2100
Wire Wire Line
	2700 2500 2700 2600
Wire Wire Line
	2700 1700 2700 1600
Wire Wire Line
	2700 1600 3000 1600
$Comp
L Diode:BAT85 D?
U 1 1 61C067AF
P 2700 1850
F 0 "D?" H 2700 2066 50  0000 C CNN
F 1 "BAT85" H 2700 1975 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 2700 1675 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/BAT85.pdf" H 2700 1850 50  0001 C CNN
	1    2700 1850
	0    1    1    0   
$EndComp
Connection ~ 3000 2100
Wire Wire Line
	2700 2600 3000 2600
$Comp
L pspice:R R?
U 1 1 61C067B7
P 3000 2350
F 0 "R?" V 3200 2300 50  0000 L CNN
F 1 "22 kOhm" V 3100 2200 50  0000 L CNN
F 2 "" H 3000 2350 50  0001 C CNN
F 3 "~" H 3000 2350 50  0001 C CNN
	1    3000 2350
	1    0    0    -1  
$EndComp
$Comp
L pspice:R R?
U 1 1 61C30414
P 2500 3150
F 0 "R?" V 2600 3100 50  0000 L CNN
F 1 "100 kOhm" V 2700 3000 50  0000 L CNN
F 2 "" H 2500 3150 50  0001 C CNN
F 3 "~" H 2500 3150 50  0001 C CNN
	1    2500 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 2900 2500 2100
Wire Wire Line
	2500 2100 2250 2100
Wire Wire Line
	3250 2900 3250 2100
Wire Wire Line
	3250 2100 3000 2100
Wire Wire Line
	3250 3400 3250 3500
Connection ~ 2250 2600
Connection ~ 2700 2600
Wire Wire Line
	3000 2600 3500 2600
Connection ~ 3000 2600
Connection ~ 3500 2600
Wire Wire Line
	3800 2600 4300 2600
Connection ~ 3800 2600
Connection ~ 4300 2600
Wire Wire Line
	2250 1600 2700 1600
Connection ~ 2250 1600
Connection ~ 2700 1600
Wire Wire Line
	3000 1600 3500 1600
Connection ~ 3000 1600
Connection ~ 3500 1600
Wire Wire Line
	3800 1600 4300 1600
Connection ~ 3800 1600
Connection ~ 4300 1600
Wire Wire Line
	4600 1600 9900 1600
Wire Wire Line
	9900 1600 9900 4100
Connection ~ 4600 1600
Wire Wire Line
	4600 4000 4600 2600
Wire Wire Line
	4600 4000 7550 4000
Connection ~ 4600 2600
Wire Wire Line
	2500 2100 2500 800 
Wire Wire Line
	2500 800  6800 800 
Wire Wire Line
	6800 800  6800 2500
Connection ~ 2500 2100
Wire Wire Line
	2250 2600 2700 2600
Wire Wire Line
	2250 3500 3250 3500
Wire Wire Line
	2250 3400 2500 3400
Wire Wire Line
	2250 3300 2250 2600
Wire Wire Line
	7550 2800 5750 2800
Wire Wire Line
	5750 2800 5750 900 
Wire Wire Line
	5750 900  3250 900 
Wire Wire Line
	3250 900  3250 2100
Connection ~ 3250 2100
$EndSCHEMATC
