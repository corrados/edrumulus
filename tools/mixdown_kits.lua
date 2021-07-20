
-- TEST convert Drumgizmo drum kits to Edrumulus usage by mixdown all the audio channels
-- using Ardour with lua scripting
-- see also: https://github.com/Ardour/ardour/blob/master/gtk2_ardour/luainstance.cc

ardour { ["type"] = "Snippet", name = "Mixdown Drumgizmo kits for Edrumulus" }

function factory ( params ) return function()

	local files = C.StringVector();

	files:push_back ( "/home/corrados/edrumulus/tools/DRSKit/Tom1_whisker/samples/11-Tom1_whisker.wav" )

	Editor:do_import (
		files,
		Editing.ImportDistinctChannels,
		Editing.ImportAsTrack,
		ARDOUR.SrcQuality.SrcBest,
		ARDOUR.MidiTrackNameSource.SMFTrackName,
		ARDOUR.MidiTempoMapDisposition.SMFTempoIgnore,
		-1,
		ARDOUR.PluginInfo() )


-- TODO how to export a stereo audio file...
	Editor:export_audio()


end end

