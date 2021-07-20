

-- TEST convert Drumgizmo drum kits to Edrumulus usage by mixdown all the audio channels
-- using Ardour with lua scripting

ardour { ["type"] = "Snippet", name = "Mixdown Drumgizmo kits for Edrumulus" }

function factory (params) return function ()

	local files = C.StringVector();

	files:push_back("/home/corrados/edrumulus/tools/DRSKit/Snare_circle_whisker/samples/1-Snare_circle_whisker.wav")

	Editor:do_import (
		files,
		Editing.ImportDistinctChannels,
		Editing.ImportAsTrack,
		ARDOUR.SrcQuality.SrcBest,
		ARDOUR.MidiTrackNameSource.SMFTrackName,
		ARDOUR.MidiTempoMapDisposition.SMFTempoIgnore,
		-1,
		ARDOUR.PluginInfo() )

end end

