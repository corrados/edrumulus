

# TEST convert Drumgizmo drum kits to Edrumulus usage by mixdown all the audio channels
# using Ardour remote controlled by Open Sound Control (OSC)

# NOTE you need to activate OSC in Ardour in Edit->Preferences->Controller

from pythonosc import udp_client

ardour = udp_client.SimpleUDPClient("127.0.0.1", 3819) # Ardour default port number



#ardour.send_message("/strip/fader", [2, 0.5])


# not yet working: import wav file
#ardour.send_message("/Common/addExistingAudioFiles", "~/edrumulus/tools/DRSKit/Snare_circle_whisker/samples/4-Snare_circle_whisker.wav")


