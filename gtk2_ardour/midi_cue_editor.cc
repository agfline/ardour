/*
 * Copyright (C) 2023 Paul Davis <paul@linuxaudiosystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "canvas/canvas.h"
#include "canvas/container.h"
#include "canvas/debug.h"
#include "canvas/scroll_group.h"
#include "canvas/rectangle.h"

#include "midi_cue_editor.h"
#include "ui_config.h"

using namespace ArdourCanvas;

MidiCueEditor::MidiCueEditor()
	: vertical_adjustment (0.0, 0.0, 10.0, 400.0)
	, horizontal_adjustment (0.0, 0.0, 1e16)
{
	build_canvas ();
}

MidiCueEditor::~MidiCueEditor ()
{
}

void
MidiCueEditor::build_canvas ()
{
	_canvas_viewport = new ArdourCanvas::GtkCanvasViewport (horizontal_adjustment, vertical_adjustment);
	_canvas = _canvas_viewport->canvas ();

	_canvas->set_background_color (UIConfiguration::instance().color ("arrange base"));
	// _canvas->use_nsglview (UIConfiguration::instance().get_nsgl_view_mode () == NSGLHiRes);

	/* scroll group for items that should not automatically scroll
	 *  (e.g verbose cursor). It shares the canvas coordinate space.
	*/
	no_scroll_group = new ArdourCanvas::Container (_canvas->root());

	ArdourCanvas::ScrollGroup* hsg;
	ArdourCanvas::ScrollGroup* hg;
	ArdourCanvas::ScrollGroup* cg;

	h_scroll_group = hg = new ArdourCanvas::ScrollGroup (_canvas->root(), ArdourCanvas::ScrollGroup::ScrollsHorizontally);
	CANVAS_DEBUG_NAME (h_scroll_group, "canvas h scroll");
	_canvas->add_scroller (*hg);

	hv_scroll_group = hsg = new ArdourCanvas::ScrollGroup (_canvas->root(),
							       ArdourCanvas::ScrollGroup::ScrollSensitivity (ArdourCanvas::ScrollGroup::ScrollsVertically|
													     ArdourCanvas::ScrollGroup::ScrollsHorizontally));
	CANVAS_DEBUG_NAME (hv_scroll_group, "canvas hv scroll");
	_canvas->add_scroller (*hsg);

	cursor_scroll_group = cg = new ArdourCanvas::ScrollGroup (_canvas->root(), ArdourCanvas::ScrollGroup::ScrollsHorizontally);
	CANVAS_DEBUG_NAME (cursor_scroll_group, "canvas cursor scroll");
	_canvas->add_scroller (*cg);

	/*a group to hold global rects like punch/loop indicators */
	global_rect_group = new ArdourCanvas::Container (hv_scroll_group);
	CANVAS_DEBUG_NAME (global_rect_group, "global rect group");

        transport_loop_range_rect = new ArdourCanvas::Rectangle (global_rect_group, ArdourCanvas::Rect (0.0, 0.0, 0.0, ArdourCanvas::COORD_MAX));
	CANVAS_DEBUG_NAME (transport_loop_range_rect, "loop rect");
	transport_loop_range_rect->hide();

	/*a group to hold time (measure) lines */
	time_line_group = new ArdourCanvas::Container (h_scroll_group);
	CANVAS_DEBUG_NAME (time_line_group, "time line group");

	_trackview_group = new ArdourCanvas::Container (hv_scroll_group);
	CANVAS_DEBUG_NAME (_trackview_group, "Canvas TrackViews");

	// used as rubberband rect
	rubberband_rect = new ArdourCanvas::Rectangle (hv_scroll_group, ArdourCanvas::Rect (0.0, 0.0, 0.0, 0.0));
	rubberband_rect->hide();
}
