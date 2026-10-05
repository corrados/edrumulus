// Copyright (c) 2020-2026 Volker Fischer
// SPDX-License-Identifier: GPL-2.0-or-later

// ESP32 default pin definition ("-1" means that this channel is unused):
// For older prototypes or custom implementations, simply change the GPIO numbers in the table below
// to match your hardware (note that the GPIO assignment of Prototype 2 is the same as Prototype 4).
// clang-format off
// analog pins setup:               snare | kick | hi-hat | hi-hat-ctrl | crash | tom1 | ride | tom2 | tom3 | crash2
static int analog_pins4[]         = { 36,    33,     32,       25,         34,     39,    27,    12,    15,     4 };
static int analog_pins_rimshot4[] = { 35,    -1,     26,       -1,         14,     -1,    13,    -1,    -1,     0 };
// clang-format on

// if you want to use less number of pads, simply adjust number_pads4 value
// const int number_pads4 = sizeof(analog_pins4) / sizeof(int); // example: use all inputs defined in analog_pins4
const int number_pads4 = 9; // example: do not use crash2 and shrink number of pads from 10 to 9
// const int number_pads4 = 1; // example: just one single pad

#include "edrumulus.h"

#ifdef USE_MIDI
#  ifdef ESP_PLATFORM
#    include <MIDI.h>
#    ifdef USE_TINYUSB
#      include <Adafruit_TinyUSB.h>
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
#    else
MIDI_CREATE_DEFAULT_INSTANCE();
#    endif
#    define MYMIDI MIDI
#    define MIDI_CONTROL_CHANGE_TYPE midi::ControlChange
#    define MIDI_SEND_AFTER_TOUCH sendAfterTouch
#    define MIDI_SERIAL 38400
#    ifdef USE_BLE_MIDI
#      include <BLE2902.h>
#      include <BLEDevice.h>
#      include <BLEServer.h>
#      include <BLEUtils.h>
#    endif
#  endif
#  ifdef TEENSYDUINO
#    define MYMIDI usbMIDI
#    define MIDI_CONTROL_CHANGE_TYPE usbMIDI.ControlChange
#    define MIDI_SEND_AFTER_TOUCH sendAfterTouchPoly
#  endif
#endif

// local variables and defines
Edrumulus edrumulus;
const int midi_channel      = 10;           // default for edrums is 10
const int hihat_pad_idx     = 2;            // this definition should not be changed
const int hihatctrl_pad_idx = 3;            // this definition should not be changed
int       number_pads       = number_pads4; // initialization value, may be overwritten by get_prototype_pins()
int       status_LED_pin    = 0;            // initialization value, will be set in get_prototype_pins()
bool      is_status_LED_on  = false;        // initialization value
int       selected_pad      = 0;            // initialization value

void confirm_setting(const int controller, const int value, const bool send_all);
void preset_settings();
void write_all_settings();
void read_settings();

#ifdef USE_BLE_MIDI
#  define MIDI_SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#  define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

BLECharacteristic* p_ble_midi_characteristic = nullptr;
bool               ble_midi_device_connected = false;

class BleMidiServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer* p_server)
  {
    ble_midi_device_connected = true;
  }

  void onDisconnect(BLEServer* p_server)
  {
    ble_midi_device_connected = false;
    BLEDevice::startAdvertising();
  }
};

class BleMidiCharacteristicCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic* p_characteristic)
  {
    std::string rx_value = p_characteristic->getValue();
    if (rx_value.empty())
    {
      return;
    }

    int     byte_index     = 0;
    uint8_t running_status = 0;

    while (byte_index < static_cast<int>(rx_value.size()))
    {
      const uint8_t byte_value = static_cast<uint8_t>(rx_value[byte_index]);

      if (byte_value == 0x80)
      {
        byte_index++;
        if (byte_index >= static_cast<int>(rx_value.size()))
        {
          break;
        }
        const uint8_t timestamp_byte = static_cast<uint8_t>(rx_value[byte_index]);
        if ((timestamp_byte & 0x80) == 0)
        {
          byte_index--;
          break;
        }
        byte_index++;
        continue;
      }

      if ((byte_value & 0x80) != 0)
      {
        running_status = byte_value;
        byte_index++;
      }

      if (byte_index >= static_cast<int>(rx_value.size()))
      {
        break;
      }

      const uint8_t data_byte1    = static_cast<uint8_t>(rx_value[byte_index++]);
      uint8_t       data_byte2    = 0;
      const uint8_t status_nibble = running_status >> 4;

      if (status_nibble != 0xC && status_nibble != 0xD)
      {
        if (byte_index >= static_cast<int>(rx_value.size()))
        {
          break;
        }
        data_byte2 = static_cast<uint8_t>(rx_value[byte_index++]);
      }

      const int controller = data_byte1;
      const int value      = data_byte2;
      if (running_status == 0xB0)
      {
        if (controller == 102)
        {
          edrumulus.set_pad_type(selected_pad, static_cast<Pad::Epadtype>(value));
          edrumulus.write_setting(selected_pad, 0, value);
          confirm_setting(controller, value, true);
        }
        else if (controller == 103)
        {
          edrumulus.set_velocity_threshold(selected_pad, value);
          edrumulus.write_setting(selected_pad, 1, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 104)
        {
          edrumulus.set_velocity_sensitivity(selected_pad, value);
          edrumulus.write_setting(selected_pad, 2, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 105)
        {
          edrumulus.set_pos_threshold(selected_pad, value);
          edrumulus.write_setting(selected_pad, 3, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 106)
        {
          edrumulus.set_pos_sensitivity(selected_pad, value);
          edrumulus.write_setting(selected_pad, 4, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 107)
        {
          edrumulus.set_rim_shot_threshold(selected_pad, value);
          edrumulus.write_setting(selected_pad, 5, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 108 && value < MAX_NUM_PADS)
        {
          selected_pad = value;
          confirm_setting(controller, value, true);
        }
        else if (controller == 109)
        {
          edrumulus.set_curve(selected_pad, static_cast<Pad::Ecurvetype>(value));
          edrumulus.write_setting(selected_pad, 6, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 110)
        {
          edrumulus.set_spike_cancel_level(value);
          edrumulus.write_setting(number_pads, 0, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 111)
        {
          switch (value)
          {
            case 0:
              edrumulus.set_rim_shot_is_used(selected_pad, false);
              edrumulus.write_setting(selected_pad, 7, false);
              edrumulus.set_pos_sense_is_used(selected_pad, false);
              edrumulus.write_setting(selected_pad, 8, false);
              break;
            case 1:
              edrumulus.set_rim_shot_is_used(selected_pad, true);
              edrumulus.write_setting(selected_pad, 7, true);
              edrumulus.set_pos_sense_is_used(selected_pad, false);
              edrumulus.write_setting(selected_pad, 8, false);
              break;
            case 2:
              edrumulus.set_rim_shot_is_used(selected_pad, false);
              edrumulus.write_setting(selected_pad, 7, false);
              edrumulus.set_pos_sense_is_used(selected_pad, true);
              edrumulus.write_setting(selected_pad, 8, true);
              break;
            case 3:
              edrumulus.set_rim_shot_is_used(selected_pad, true);
              edrumulus.write_setting(selected_pad, 7, true);
              edrumulus.set_pos_sense_is_used(selected_pad, true);
              edrumulus.write_setting(selected_pad, 8, true);
              break;
          }
          confirm_setting(controller, value, false);
        }
        else if (controller == 112)
        {
          edrumulus.set_midi_note_norm(selected_pad, value);
          edrumulus.write_setting(selected_pad, 9, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 113)
        {
          edrumulus.set_midi_note_rim(selected_pad, value);
          edrumulus.write_setting(selected_pad, 10, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 114)
        {
          edrumulus.set_cancellation(selected_pad, value);
          edrumulus.write_setting(selected_pad, 11, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 115)
        {
          preset_settings();
          write_all_settings();
          confirm_setting(controller, value, false);
        }
        else if (controller == 116)
        {
          edrumulus.set_midi_note_open_norm(selected_pad, value);
          edrumulus.write_setting(selected_pad, 12, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 117)
        {
          edrumulus.set_midi_note_open_rim(selected_pad, value);
          edrumulus.write_setting(selected_pad, 13, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 118)
        {
          edrumulus.set_mask_time(selected_pad, value);
          edrumulus.write_setting(selected_pad, 14, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 119)
        {
          edrumulus.set_rim_shot_boost(selected_pad, value);
          edrumulus.write_setting(selected_pad, 15, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 120)
        {
          edrumulus.set_coupled_pad_idx(selected_pad, value);
          edrumulus.write_setting(selected_pad, 16, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 121)
        {
          edrumulus.set_rim_pos_threshold(selected_pad, value);
          edrumulus.write_setting(selected_pad, 17, value);
          confirm_setting(controller, value, false);
        }
        else if (controller == 122)
        {
          edrumulus.set_rim_pos_sensitivity(selected_pad, value);
          edrumulus.write_setting(selected_pad, 18, value);
          confirm_setting(controller, value, false);
        }
      }
    }
  }
};

void ble_midi_setup()
{
  BLEDevice::init("Edrumulus");

  BLEServer* p_server = BLEDevice::createServer();
  p_server->setCallbacks(new BleMidiServerCallbacks());

  BLEService* p_service     = p_server->createService(BLEUUID(MIDI_SERVICE_UUID));
  p_ble_midi_characteristic = p_service->createCharacteristic(
      BLEUUID(MIDI_CHARACTERISTIC_UUID),
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE_NR);
  p_ble_midi_characteristic->addDescriptor(new BLE2902());
  p_ble_midi_characteristic->setCallbacks(new BleMidiCharacteristicCallbacks());

  p_service->start();
  p_server->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
  p_server->getAdvertising()->start();
}

void ble_midi_send_packet(const uint8_t* packet, const int length)
{
  if (ble_midi_device_connected && p_ble_midi_characteristic != nullptr)
  {
    p_ble_midi_characteristic->setValue(const_cast<uint8_t*>(packet), static_cast<uint16_t>(length));
    p_ble_midi_characteristic->notify();
  }
}

void ble_midi_send_note_on(const int note, const int velocity, const int channel)
{
  uint8_t packet[5];
  packet[0] = 0x80;
  packet[1] = 0x80;
  packet[2] = static_cast<uint8_t>(0x90 | (channel & 0x0F));
  packet[3] = static_cast<uint8_t>(note & 0x7F);
  packet[4] = static_cast<uint8_t>(velocity & 0x7F);
  ble_midi_send_packet(packet, 5);
}

void ble_midi_send_note_off(const int note, const int velocity, const int channel)
{
  uint8_t packet[5];
  packet[0] = 0x80;
  packet[1] = 0x80;
  packet[2] = static_cast<uint8_t>(0x80 | (channel & 0x0F));
  packet[3] = static_cast<uint8_t>(note & 0x7F);
  packet[4] = static_cast<uint8_t>(velocity & 0x7F);
  ble_midi_send_packet(packet, 5);
}

void ble_midi_send_control_change(const int controller, const int value, const int channel)
{
  uint8_t packet[5];
  packet[0] = 0x80;
  packet[1] = 0x80;
  packet[2] = static_cast<uint8_t>(0xB0 | (channel & 0x0F));
  packet[3] = static_cast<uint8_t>(controller & 0x7F);
  packet[4] = static_cast<uint8_t>(value & 0x7F);
  ble_midi_send_packet(packet, 5);
}

void ble_midi_send_after_touch(const int note, const int pressure, const int channel)
{
  uint8_t packet[5];
  packet[0] = 0x80;
  packet[1] = 0x80;
  packet[2] = static_cast<uint8_t>(0xA0 | (channel & 0x0F));
  packet[3] = static_cast<uint8_t>(note & 0x7F);
  packet[4] = static_cast<uint8_t>(pressure & 0x7F);
  ble_midi_send_packet(packet, 5);
}
#endif

void preset_settings()
{
  // default MIDI note assignments
  edrumulus.set_midi_notes(0, 38, 40); // snare
  edrumulus.set_midi_notes(1, 36, 36); // kick
  edrumulus.set_midi_notes(hihat_pad_idx, 22 /*42*/, 22);
  edrumulus.set_midi_notes_open(hihat_pad_idx, 26 /*46*/, 26);
  edrumulus.set_midi_notes(hihatctrl_pad_idx, 44, 44); // Hi-Hat pedal hit
  edrumulus.set_midi_notes(4, 49, 55);                 // crash
  edrumulus.set_midi_notes(5, 48, 50);                 // tom 1
  edrumulus.set_midi_notes(6, 51, 53 /*59*/);          // ride (edge: 59, bell: 53)
  edrumulus.set_midi_notes(7, 45, 47);                 // tom 2
  edrumulus.set_midi_notes(8, 43, 58);                 // tom 3
  edrumulus.set_midi_notes(9, 57, 52);                 // crash 2

  // default drum kit setup
  edrumulus.set_pad_type(0, Pad::PD8);  // snare
  edrumulus.set_pad_type(1, Pad::KD7);  // kick
  edrumulus.set_pad_type(2, Pad::PD6);  // Hi-Hat
  edrumulus.set_pad_type(3, Pad::FD8);  // Hi-Hat-ctrl
  edrumulus.set_pad_type(4, Pad::CY6);  // crash
  edrumulus.set_pad_type(5, Pad::TP80); // tom 1
  edrumulus.set_pad_type(6, Pad::CY8);  // ride
  edrumulus.set_pad_type(7, Pad::TP80); // tom 2
  edrumulus.set_pad_type(8, Pad::TP80); // tom 3
  edrumulus.set_pad_type(9, Pad::CY6);  // crash 2
}

void setup()
{
  // get the pin-to-pad assignments
  int*      analog_pins         = analog_pins4;         // initialize with the default setup
  int*      analog_pins_rimshot = analog_pins_rimshot4; // initialize with the default setup
  const int prototype           = Edrumulus_hardware::get_prototype_pins(
      &analog_pins, &analog_pins_rimshot, &number_pads, &status_LED_pin);

  // initialize GPIO port for status LED and set it to on during setup
  pinMode(status_LED_pin, OUTPUT);
  digitalWrite(status_LED_pin, HIGH);

#if defined(USE_SERIAL_DEBUG_PLOTTING) && defined(ESP_PLATFORM)
  number_pads = min(number_pads, 7); // only max. 7 pads are supported for ESP32 serial debug plotting
#endif
#ifdef USE_MIDI
  MYMIDI.begin();
#endif
#ifdef MIDI_SERIAL
  if (prototype == 5)
  {
    Serial.begin(115200); // faster communication on prototype 5
  }
  else
  {
    Serial.begin(MIDI_SERIAL);
  }
#else
  Serial.begin(115200);
#endif

  edrumulus.setup(number_pads, analog_pins, analog_pins_rimshot);
  digitalWrite(status_LED_pin, LOW); // set board LED to low right after setup is done
#ifdef USE_BLE_MIDI
  ble_midi_setup();
#endif
#ifdef ESP_PLATFORM
  preset_settings(); // for ESP32, the load/save of settings is not supported, preset instead
#else
  read_settings();
#endif
}

void loop()
{
  // this function is blocking at the system sampling rate
  edrumulus.process();

  // status LED handling
  if (edrumulus.get_status_is_overload() || edrumulus.get_status_is_error())
  {
    if (!is_status_LED_on)
    {
      digitalWrite(status_LED_pin, HIGH);
      is_status_LED_on = true;
#ifdef USE_MIDI
      if (edrumulus.get_status_is_error())
      {
        const int dc_offset_error_channel = edrumulus.get_status_dc_offset_error_channel();
        if (dc_offset_error_channel >= 0)
        {
          // > 63 means DC offset error and pad/input index is coded in one value
          MYMIDI.sendNoteOff(125, 64 + dc_offset_error_channel, 1);
#  ifdef USE_BLE_MIDI
          if (ble_midi_device_connected)
          {
            ble_midi_send_note_off(125, 64 + dc_offset_error_channel, 1);
          }
#  endif
        }
        else
        {
          // 1 means to set error state
          MYMIDI.sendNoteOff(125, 1, 1);
#  ifdef USE_BLE_MIDI
          if (ble_midi_device_connected)
          {
            ble_midi_send_note_off(125, 1, 1);
          }
#  endif
        }
      }
#endif
    }
  }
  else
  {
    if (is_status_LED_on)
    {
      digitalWrite(status_LED_pin, LOW);
      is_status_LED_on = false;
#ifdef USE_MIDI
      MYMIDI.sendNoteOff(125, 0, 1); // 0 means that all errors are cleared
#  ifdef USE_BLE_MIDI
      if (ble_midi_device_connected)
      {
        ble_midi_send_note_off(125, 0, 1);
      }
#  endif
#endif
    }
  }

#ifdef USE_MIDI
  // load indicator
  if (const int load = edrumulus.get_load_indicator(); load >= 0)
  {
    MYMIDI.sendNoteOff(124, load, 1);
  }

  // send MIDI note to drum synthesizer
  for (int pad_idx = 0; pad_idx < number_pads; pad_idx++)
  {
    if (edrumulus.get_peak_found(pad_idx))
    {
      // get current MIDI note and velocity (maybe note will be overwritten later on)
      const int midi_velocity = edrumulus.get_midi_velocity(pad_idx);
      int       midi_note     = edrumulus.get_midi_note(pad_idx);

      // send midi positional control message if positional sensing is enabled for the current pad
      if (edrumulus.get_pos_sense_is_used(pad_idx))
      {
        const int midi_pos = edrumulus.get_midi_pos(pad_idx);
        MYMIDI.sendControlChange(16, midi_pos, midi_channel); // positional sensing
#  ifdef USE_BLE_MIDI
        if (ble_midi_device_connected)
        {
          ble_midi_send_control_change(16, midi_pos, midi_channel);
        }
#  endif
      }

      // send Hi-Hat control message right before each Hi-Hat pad hit
      if (pad_idx == hihat_pad_idx)
      {
        const int  midi_ctrl_ch    = edrumulus.get_midi_ctrl_ch(hihatctrl_pad_idx);
        const int  midi_ctrl_value = edrumulus.get_midi_ctrl_value(hihatctrl_pad_idx);
        const bool hi_hat_is_open  = edrumulus.get_midi_ctrl_is_open(hihatctrl_pad_idx);
        MYMIDI.sendControlChange(midi_ctrl_ch, midi_ctrl_value, midi_channel);

        // if Hi-Hat is open, overwrite MIDI note
        if (hi_hat_is_open)
        {
          midi_note = edrumulus.get_midi_note_open(pad_idx);
        }
      }

      MYMIDI.sendNoteOn(midi_note, midi_velocity, midi_channel); // (note, velocity, channel)
      MYMIDI.sendNoteOff(midi_note, 0, midi_channel);            // we need a note off
#  ifdef USE_BLE_MIDI
      if (ble_midi_device_connected)
      {
        ble_midi_send_note_on(midi_note, midi_velocity, midi_channel);
        ble_midi_send_note_off(midi_note, 0, midi_channel);
      }
#  endif
    }

    if (edrumulus.get_control_found(pad_idx))
    {
      const int midi_ctrl_ch    = edrumulus.get_midi_ctrl_ch(pad_idx);
      const int midi_ctrl_value = edrumulus.get_midi_ctrl_value(pad_idx);
      MYMIDI.sendControlChange(midi_ctrl_ch, midi_ctrl_value, midi_channel);
#  ifdef USE_BLE_MIDI
      if (ble_midi_device_connected)
      {
        ble_midi_send_control_change(midi_ctrl_ch, midi_ctrl_value, midi_channel);
      }
#  endif
    }

    if (edrumulus.get_choke_on_found(pad_idx))
    {
      // special case: if MIDI note open rim is set to zero, we use NoteOn instead of aftertouch
      // for cymbal choke (#85), where the MIDI note for NoteOn is defined by MIDI note open norm
      if (edrumulus.get_midi_note_open_rim(pad_idx) == 0)
      {
        // special case: if grabbed edge found, we send a MIDI NoteOn
        const int midi_choke_noteon = edrumulus.get_midi_note_open_norm(pad_idx);
        MYMIDI.sendNoteOn(midi_choke_noteon, 127, midi_channel);
        MYMIDI.sendNoteOff(midi_choke_noteon, 0, midi_channel); // we need a note off
#  ifdef USE_BLE_MIDI
        if (ble_midi_device_connected)
        {
          ble_midi_send_note_on(midi_choke_noteon, 127, midi_channel);
          ble_midi_send_note_off(midi_choke_noteon, 0, midi_channel);
        }
#  endif
      }
      else
      {
        // if grabbed edge found, polyphonic aftertouch at 127 is transmitted for all notes of the pad
        MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_norm(pad_idx), 127, midi_channel);
        MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_rim(pad_idx), 127, midi_channel);
        MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_open_norm(pad_idx), 127, midi_channel);
        MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_open_rim(pad_idx), 127, midi_channel);
#  ifdef USE_BLE_MIDI
        if (ble_midi_device_connected)
        {
          ble_midi_send_after_touch(edrumulus.get_midi_note_norm(pad_idx), 127, midi_channel);
          ble_midi_send_after_touch(edrumulus.get_midi_note_rim(pad_idx), 127, midi_channel);
          ble_midi_send_after_touch(edrumulus.get_midi_note_open_norm(pad_idx), 127, midi_channel);
          ble_midi_send_after_touch(edrumulus.get_midi_note_open_rim(pad_idx), 127, midi_channel);
        }
#  endif
      }
    }
    else if (edrumulus.get_choke_off_found(pad_idx))
    {
      // if released edge found, polyphonic aftertouch at 0 is transmitted for all notes of the pad
      MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_norm(pad_idx), 0, midi_channel);
      MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_rim(pad_idx), 0, midi_channel);
      MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_open_norm(pad_idx), 0, midi_channel);
      MYMIDI.MIDI_SEND_AFTER_TOUCH(edrumulus.get_midi_note_open_rim(pad_idx), 0, midi_channel);
#  ifdef USE_BLE_MIDI
      if (ble_midi_device_connected)
      {
        ble_midi_send_after_touch(edrumulus.get_midi_note_norm(pad_idx), 0, midi_channel);
        ble_midi_send_after_touch(edrumulus.get_midi_note_rim(pad_idx), 0, midi_channel);
        ble_midi_send_after_touch(edrumulus.get_midi_note_open_norm(pad_idx), 0, midi_channel);
        ble_midi_send_after_touch(edrumulus.get_midi_note_open_rim(pad_idx), 0, midi_channel);
      }
#  endif
    }
  }

  // receiving MIDI messages to change the pad settings: edrumuluscontrol.m -> loopMIDI -> Hairless MIDI
  if (MYMIDI.read(midi_channel))
  {
    if (MYMIDI.getType() == MIDI_CONTROL_CHANGE_TYPE)
    {
      const int controller = MYMIDI.getData1();
      const int value      = MYMIDI.getData2();

      // controller 102: pad type
      if (controller == 102)
      {
        edrumulus.set_pad_type(selected_pad, static_cast<Pad::Epadtype>(value));
        edrumulus.write_setting(selected_pad, 0, value);

        // on a pad type change, return all parameters of the selected pad
        confirm_setting(controller, value, true);
      }

      // controller 103: threshold
      if (controller == 103)
      {
        edrumulus.set_velocity_threshold(selected_pad, value);
        edrumulus.write_setting(selected_pad, 1, value);
        confirm_setting(controller, value, false);
      }

      // controller 104: sensitivity
      if (controller == 104)
      {
        edrumulus.set_velocity_sensitivity(selected_pad, value);
        edrumulus.write_setting(selected_pad, 2, value);
        confirm_setting(controller, value, false);
      }

      // controller 105: positional sensing threshold
      if (controller == 105)
      {
        edrumulus.set_pos_threshold(selected_pad, value);
        edrumulus.write_setting(selected_pad, 3, value);
        confirm_setting(controller, value, false);
      }

      // controller 106: positional sensing sensitivity
      if (controller == 106)
      {
        edrumulus.set_pos_sensitivity(selected_pad, value);
        edrumulus.write_setting(selected_pad, 4, value);
        confirm_setting(controller, value, false);
      }

      // controller 107: rim shot threshold
      if (controller == 107)
      {
        edrumulus.set_rim_shot_threshold(selected_pad, value);
        edrumulus.write_setting(selected_pad, 5, value);
        confirm_setting(controller, value, false);
      }

      // controller 108: select pad
      if ((controller == 108) && (value < MAX_NUM_PADS))
      {
        selected_pad = value;

        // on a pad selection, return all parameters of the selected pad
        confirm_setting(controller, value, true);
      }

      // controller 109: MIDI curve type
      if (controller == 109)
      {
        edrumulus.set_curve(selected_pad, static_cast<Pad::Ecurvetype>(value));
        edrumulus.write_setting(selected_pad, 6, value);
        confirm_setting(controller, value, false);
      }

      // controller 110: spike cancellation level
      if (controller == 110)
      {
        edrumulus.set_spike_cancel_level(value);
        edrumulus.write_setting(number_pads, 0, value);
        confirm_setting(controller, value, false);
      }

      // controller 111: enable/disable rim shot and positional sensing support
      if (controller == 111)
      {
        edrumulus.set_rim_shot_is_used(selected_pad, (value % 2) != 0);        // 0 1 0 1
        edrumulus.write_setting(selected_pad, 7, (value % 2) != 0);            // 0 1 0 1
        edrumulus.set_pos_sense_is_used(selected_pad, ((value / 2) % 2) != 0); // 0 0 1 1
        edrumulus.write_setting(selected_pad, 8, ((value / 2) % 2) != 0);      // 0 0 1 1
        confirm_setting(controller, value, false);
      }

      // controller 112: normal MIDI note
      if (controller == 112)
      {
        edrumulus.set_midi_note_norm(selected_pad, value);
        edrumulus.write_setting(selected_pad, 9, value);
        confirm_setting(controller, value, false);
      }

      // controller 113: MIDI note for rim
      if (controller == 113)
      {
        edrumulus.set_midi_note_rim(selected_pad, value);
        edrumulus.write_setting(selected_pad, 10, value);
        confirm_setting(controller, value, false);
      }

      // controller 114: cross talk cancellation
      if (controller == 114)
      {
        edrumulus.set_cancellation(selected_pad, value);
        edrumulus.write_setting(selected_pad, 11, value);
        confirm_setting(controller, value, false);
      }

      // controller 115: apply preset settings and store these to the EEPROM
      if (controller == 115)
      {
        preset_settings();
        write_all_settings();
        confirm_setting(controller, value, false);
      }

      // controller 116: normal MIDI note open (Hi-Hat)
      if (controller == 116)
      {
        edrumulus.set_midi_note_open_norm(selected_pad, value);
        edrumulus.write_setting(selected_pad, 12, value);
        confirm_setting(controller, value, false);
      }

      // controller 117: MIDI note open (Hi-Hat) for rim
      if (controller == 117)
      {
        edrumulus.set_midi_note_open_rim(selected_pad, value);
        edrumulus.write_setting(selected_pad, 13, value);
        confirm_setting(controller, value, false);
      }

      // controller 118: mask time
      if (controller == 118)
      {
        edrumulus.set_mask_time(selected_pad, value);
        edrumulus.write_setting(selected_pad, 14, value);
        confirm_setting(controller, value, false);
      }

      // controller 119: rim shot boost
      if (controller == 119)
      {
        edrumulus.set_rim_shot_boost(selected_pad, value);
        edrumulus.write_setting(selected_pad, 15, value);
        confirm_setting(controller, value, false);
      }

      // controller 120: pad coupling
      if (controller == 120)
      {
        edrumulus.set_coupled_pad_idx(selected_pad, value);
        edrumulus.write_setting(selected_pad, 16, value);
        confirm_setting(controller, value, false);
      }

      // controller 121: rim positional sensing threshold
      if (controller == 121)
      {
        edrumulus.set_rim_pos_threshold(selected_pad, value);
        edrumulus.write_setting(selected_pad, 17, value);
        confirm_setting(controller, value, false);
      }

      // controller 122: rim positional sensing sensitivity
      if (controller == 122)
      {
        edrumulus.set_rim_pos_sensitivity(selected_pad, value);
        edrumulus.write_setting(selected_pad, 18, value);
        confirm_setting(controller, value, false);
      }

      // controller 123: load indicator
      if (controller == 123)
      {
        edrumulus.set_enable_load_indicator(value);
        confirm_setting(controller, value, false);
      }
    }
  }
#endif
}

#ifdef USE_MIDI
// give feedback to the controller GUI via MIDI Note Off
void confirm_setting(const int  controller,
                     const int  value,
                     const bool send_all)
{
  if (send_all)
  {
    // return all parameters of the selected pad
    MYMIDI.sendNoteOff(102, static_cast<int>(edrumulus.get_pad_type(selected_pad)), 1);
    MYMIDI.sendNoteOff(103, edrumulus.get_velocity_threshold(selected_pad), 1);
    MYMIDI.sendNoteOff(104, edrumulus.get_velocity_sensitivity(selected_pad), 1);
    MYMIDI.sendNoteOff(105, edrumulus.get_pos_threshold(selected_pad), 1);
    MYMIDI.sendNoteOff(106, edrumulus.get_pos_sensitivity(selected_pad), 1);
    MYMIDI.sendNoteOff(107, edrumulus.get_rim_shot_threshold(selected_pad), 1);
    MYMIDI.sendNoteOff(108, selected_pad, 1);
    MYMIDI.sendNoteOff(109, static_cast<int>(edrumulus.get_curve(selected_pad)), 1);
    MYMIDI.sendNoteOff(110, edrumulus.get_spike_cancel_level(), 1);
    MYMIDI.sendNoteOff(111, edrumulus.get_rim_shot_is_used(selected_pad) + 2 * edrumulus.get_pos_sense_is_used(selected_pad), 1);
    MYMIDI.sendNoteOff(112, edrumulus.get_midi_note_norm(selected_pad), 1);
    MYMIDI.sendNoteOff(113, edrumulus.get_midi_note_rim(selected_pad), 1);
    MYMIDI.sendNoteOff(114, edrumulus.get_cancellation(selected_pad), 1);
    MYMIDI.sendNoteOff(116, edrumulus.get_midi_note_open_norm(selected_pad), 1);
    MYMIDI.sendNoteOff(117, edrumulus.get_midi_note_open_rim(selected_pad), 1);
    MYMIDI.sendNoteOff(118, edrumulus.get_mask_time(selected_pad), 1);
    MYMIDI.sendNoteOff(119, edrumulus.get_rim_shot_boost(selected_pad), 1);
    MYMIDI.sendNoteOff(120, edrumulus.get_coupled_pad_idx(selected_pad), 1);
    MYMIDI.sendNoteOff(121, edrumulus.get_rim_pos_threshold(selected_pad), 1);
    MYMIDI.sendNoteOff(122, edrumulus.get_rim_pos_sensitivity(selected_pad), 1);
    // NOTE: 123 is load indicator setting which is not stored
    // NOTE: 124 is reserved for load indicator
    // NOTE: 125 is reserved for error message
    MYMIDI.sendNoteOff(126, VERSION_MINOR, 1);
    MYMIDI.sendNoteOff(127, VERSION_MAJOR, 1);
  }
  else
  {
    // return only the given parameter
    MYMIDI.sendNoteOff(controller, value, 1); // can be checked, e.g., in the log file
  }
#  ifdef USE_BLE_MIDI
  if (ble_midi_device_connected)
  {
    if (send_all)
    {
      ble_midi_send_control_change(102, static_cast<int>(edrumulus.get_pad_type(selected_pad)), midi_channel);
      ble_midi_send_control_change(103, edrumulus.get_velocity_threshold(selected_pad), midi_channel);
      ble_midi_send_control_change(104, edrumulus.get_velocity_sensitivity(selected_pad), midi_channel);
      ble_midi_send_control_change(105, edrumulus.get_pos_threshold(selected_pad), midi_channel);
      ble_midi_send_control_change(106, edrumulus.get_pos_sensitivity(selected_pad), midi_channel);
      ble_midi_send_control_change(107, edrumulus.get_rim_shot_threshold(selected_pad), midi_channel);
      ble_midi_send_control_change(108, selected_pad, midi_channel);
      ble_midi_send_control_change(109, static_cast<int>(edrumulus.get_curve(selected_pad)), midi_channel);
      ble_midi_send_control_change(110, edrumulus.get_spike_cancel_level(), midi_channel);
      ble_midi_send_control_change(111, edrumulus.get_rim_shot_is_used(selected_pad) + 2 * edrumulus.get_pos_sense_is_used(selected_pad), midi_channel);
      ble_midi_send_control_change(112, edrumulus.get_midi_note_norm(selected_pad), midi_channel);
      ble_midi_send_control_change(113, edrumulus.get_midi_note_rim(selected_pad), midi_channel);
      ble_midi_send_control_change(114, edrumulus.get_cancellation(selected_pad), midi_channel);
      ble_midi_send_control_change(116, edrumulus.get_midi_note_open_norm(selected_pad), midi_channel);
      ble_midi_send_control_change(117, edrumulus.get_midi_note_open_rim(selected_pad), midi_channel);
      ble_midi_send_control_change(118, edrumulus.get_mask_time(selected_pad), midi_channel);
      ble_midi_send_control_change(119, edrumulus.get_rim_shot_boost(selected_pad), midi_channel);
      ble_midi_send_control_change(120, edrumulus.get_coupled_pad_idx(selected_pad), midi_channel);
      ble_midi_send_control_change(121, edrumulus.get_rim_pos_threshold(selected_pad), midi_channel);
      ble_midi_send_control_change(122, edrumulus.get_rim_pos_sensitivity(selected_pad), midi_channel);
      ble_midi_send_note_off(126, VERSION_MINOR, midi_channel);
      ble_midi_send_note_off(127, VERSION_MAJOR, midi_channel);
    }
    else
    {
      ble_midi_send_control_change(controller, value, midi_channel);
    }
  }
#  endif
}
#endif

void read_settings()
{
  for (int i = 0; i < number_pads; i++)
  {
    // NOTE that it is important that set_pad_type() is called first because it resets all other parameters
    edrumulus.set_pad_type(i, static_cast<Pad::Epadtype>(edrumulus.read_setting(i, 0)));
    edrumulus.set_velocity_threshold(i, edrumulus.read_setting(i, 1));
    edrumulus.set_velocity_sensitivity(i, edrumulus.read_setting(i, 2));
    edrumulus.set_pos_threshold(i, edrumulus.read_setting(i, 3));
    edrumulus.set_pos_sensitivity(i, edrumulus.read_setting(i, 4));
    edrumulus.set_rim_shot_threshold(i, edrumulus.read_setting(i, 5));
    edrumulus.set_curve(i, static_cast<Pad::Ecurvetype>(edrumulus.read_setting(i, 6)));
    edrumulus.set_rim_shot_is_used(i, edrumulus.read_setting(i, 7));
    edrumulus.set_pos_sense_is_used(i, edrumulus.read_setting(i, 8));
    edrumulus.set_midi_note_norm(i, edrumulus.read_setting(i, 9));
    edrumulus.set_midi_note_rim(i, edrumulus.read_setting(i, 10));
    edrumulus.set_cancellation(i, edrumulus.read_setting(i, 11));
    edrumulus.set_midi_note_open_norm(i, edrumulus.read_setting(i, 12));
    edrumulus.set_midi_note_open_rim(i, edrumulus.read_setting(i, 13));
    edrumulus.set_mask_time(i, edrumulus.read_setting(i, 14));
    edrumulus.set_rim_shot_boost(i, edrumulus.read_setting(i, 15));
    edrumulus.set_coupled_pad_idx(i, edrumulus.read_setting(i, 16));
    edrumulus.set_rim_pos_threshold(i, edrumulus.read_setting(i, 17));
    edrumulus.set_rim_pos_sensitivity(i, edrumulus.read_setting(i, 18));
  }
  edrumulus.set_spike_cancel_level(edrumulus.read_setting(number_pads, 0));
}

void write_all_settings()
{
  for (int i = 0; i < number_pads; i++)
  {
    edrumulus.write_setting(i, 0, edrumulus.get_pad_type(i));
    edrumulus.write_setting(i, 1, edrumulus.get_velocity_threshold(i));
    edrumulus.write_setting(i, 2, edrumulus.get_velocity_sensitivity(i));
    edrumulus.write_setting(i, 3, edrumulus.get_pos_threshold(i));
    edrumulus.write_setting(i, 4, edrumulus.get_pos_sensitivity(i));
    edrumulus.write_setting(i, 5, edrumulus.get_rim_shot_threshold(i));
    edrumulus.write_setting(i, 6, edrumulus.get_curve(i));
    edrumulus.write_setting(i, 7, edrumulus.get_rim_shot_is_used(i));
    edrumulus.write_setting(i, 8, edrumulus.get_pos_sense_is_used(i));
    edrumulus.write_setting(i, 9, edrumulus.get_midi_note_norm(i));
    edrumulus.write_setting(i, 10, edrumulus.get_midi_note_rim(i));
    edrumulus.write_setting(i, 11, edrumulus.get_cancellation(i));
    edrumulus.write_setting(i, 12, edrumulus.get_midi_note_open_norm(i));
    edrumulus.write_setting(i, 13, edrumulus.get_midi_note_open_rim(i));
    edrumulus.write_setting(i, 14, edrumulus.get_mask_time(i));
    edrumulus.write_setting(i, 15, edrumulus.get_rim_shot_boost(i));
    edrumulus.write_setting(i, 16, edrumulus.get_coupled_pad_idx(i));
    edrumulus.write_setting(i, 17, edrumulus.get_rim_pos_threshold(i));
    edrumulus.write_setting(i, 18, edrumulus.get_rim_pos_sensitivity(i));
  }
  edrumulus.write_setting(number_pads, 0, edrumulus.get_spike_cancel_level());
}
