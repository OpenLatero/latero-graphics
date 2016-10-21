// -----------------------------------------------------------
//
// Copyright (c) 2015 by Vincent Levesque. All Rights Reserved.
//
// This file is part of latero-graphics.
//
//    latero-graphics is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    latero-graphics is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with latero-graphics.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------

#ifndef LATERO_GRAPHICS_PLANAR_STOCK
#define LATERO_GRAPHICS_PLANAR_STOCK

namespace latero {
namespace graphics { 

/**
 * Predefined textures.
 */

const std::string tx_path 			= "/usr/local/share/laterographics/texture/";

const std::string tx_plain	 		= tx_path+"plain.tx";
const std::string tx_null	 		= tx_path+"null.tx";
const std::string tx_grating_vertical 		= tx_path+"grating-vertical.tx";
const std::string tx_grating_horizontal		= tx_path+"grating-horizontal.tx";
const std::string tx_grating_diagonal 		= tx_path+"grating-diagonal.tx";
const std::string tx_grating_radial 		= tx_path+"grating-radial.tx";
const std::string tx_grating_radial_vib 	= tx_path+"grating-radial-vib.tx";
const std::string tx_grating_radial_motion 	= tx_path+"grating-radial-motion.tx";
const std::string tx_grating_axial 		= tx_path+"grating-axial.tx";
const std::string tx_grating_dots 		= tx_path+"grating-dots.tx";
const std::string tx_vibration_low		= tx_path+"vibration-low.tx";
const std::string tx_grating_combo		= tx_path+"grating-combo.tx";
const std::string tx_grating_textured1 		= tx_path+"grating-textured1.tx";
const std::string tx_grating_textured2 		= tx_path+"grating-textured2.tx";
const std::string tx_grating_textured3 		= tx_path+"grating-textured3.tx";
const std::string tx_grating_textured4 		= tx_path+"grating-textured4.tx";
const std::string tx_grating_textured5 		= tx_path+"grating-textured5.tx";
const std::string tx_grid	 		= tx_path+"grid.tx";
const std::string tx_blend	 		= tx_path+"blend.tx";
const std::string tx_jaggies	 		= tx_path+"jaggies.tx";
const std::string tx_bidir	 		= tx_path+"bidir.tx";
const std::string tx_axis	 		= tx_path+"axis.tx";
const std::string tx_omnidir	 		= tx_path+"omnidir.tx";
const std::string tx_unidir	 		= tx_path+"unidir.tx";
const std::string tx_slow_vib	 		= tx_path+"slow-vib.tx";
const std::string tx_pattern_grid 		= tx_path+"pattern-grid.tx";

const std::string tx_pitchmod_linear_peak	= tx_path+"pitch-mod/linear-peak.tx";
const std::string tx_pitchmod_linear_slope	= tx_path+"pitch-mod/linear-slope.tx";
const std::string tx_pitchmod_axial		= tx_path+"pitch-mod/axial.tx";
const std::string tx_pitchmod_radial_abrupt	= tx_path+"pitch-mod/radial-abrupt-slope.tx";
const std::string tx_pitchmod_radial_ring	= tx_path+"pitch-mod/radial-ring.tx";
const std::string tx_pitchmod_radial_slope	= tx_path+"pitch-mod/radial-slope.tx";

const std::string tx_motion			= tx_path+"motion.tx";

const std::string tx_icon_null			= tx_path+"null.png";
const std::string tx_icon_linear_grating	= tx_path+"grating-vertical.png";
const std::string tx_icon_radial_grating 	= tx_path+"grating-radial.png";
const std::string tx_icon_axial_grating	 	= tx_path+"grating-axial.png";
const std::string tx_icon_combo			= tx_path+"grating-combo.png";
const std::string tx_icon_vibration		= tx_path+"vibration-low.png";
const std::string tx_icon_double_linear_grating	= tx_path+"grating-dots.png";
const std::string tx_icon_blend 		= tx_path+"blend.png"; // todo
const std::string tx_icon_motion 		= tx_path+"motion.png";
const std::string tx_icon_pattern_grid 		= tx_path+"pattern-grid.png";

inline std::vector<std::string> GetStockTextures()
{
	std::vector<std::string> rv;
	rv.push_back(tx_null);
	rv.push_back(tx_plain);
	rv.push_back(tx_grating_vertical);
	rv.push_back(tx_grating_horizontal);
	rv.push_back(tx_grating_diagonal);
	rv.push_back(tx_grating_radial);
	rv.push_back(tx_grating_radial_vib);
	rv.push_back(tx_grating_radial_motion);
	rv.push_back(tx_grating_axial);
	rv.push_back(tx_grating_dots);
	rv.push_back(tx_vibration_low);
	rv.push_back(tx_grating_combo);
	rv.push_back(tx_grating_textured1);
	rv.push_back(tx_grating_textured2);
	rv.push_back(tx_grating_textured3);
	rv.push_back(tx_grating_textured4);
	rv.push_back(tx_grating_textured5);
	rv.push_back(tx_grid);
	rv.push_back(tx_jaggies);
	rv.push_back(tx_blend);
	rv.push_back(tx_bidir);
	rv.push_back(tx_axis);
	rv.push_back(tx_omnidir);
	rv.push_back(tx_unidir);
	rv.push_back(tx_slow_vib);
	rv.push_back(tx_pitchmod_linear_peak);
	rv.push_back(tx_pitchmod_linear_slope);
	rv.push_back(tx_pitchmod_axial);
	rv.push_back(tx_pitchmod_radial_abrupt);
	rv.push_back(tx_pitchmod_radial_ring);
	rv.push_back(tx_pitchmod_radial_slope);
	rv.push_back(tx_motion);
	rv.push_back(tx_pattern_grid);

	return rv;
}

} // namespace graphics
} // namespace latero

#endif

