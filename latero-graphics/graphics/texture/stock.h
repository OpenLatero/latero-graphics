#pragma once

#include <filesystem>
#include <vector>

namespace latero::graphics {

/**
 * Predefined textures.
 */

const std::filesystem::path tx_path			= "/usr/local/share/laterographics/texture/";

const std::filesystem::path tx_plain			= tx_path / "plain.tx";
const std::filesystem::path tx_null			= tx_path / "null.tx";
const std::filesystem::path tx_grating_vertical		= tx_path / "grating-vertical.tx";
const std::filesystem::path tx_grating_horizontal	= tx_path / "grating-horizontal.tx";
const std::filesystem::path tx_grating_diagonal		= tx_path / "grating-diagonal.tx";
const std::filesystem::path tx_grating_radial		= tx_path / "grating-radial.tx";
const std::filesystem::path tx_grating_radial_vib	= tx_path / "grating-radial-vib.tx";
const std::filesystem::path tx_grating_radial_motion	= tx_path / "grating-radial-motion.tx";
const std::filesystem::path tx_grating_axial		= tx_path / "grating-axial.tx";
const std::filesystem::path tx_grating_dots		= tx_path / "grating-dots.tx";
const std::filesystem::path tx_vibration_low		= tx_path / "vibration-low.tx";
const std::filesystem::path tx_grating_combo		= tx_path / "grating-combo.tx";
const std::filesystem::path tx_grating_textured1	= tx_path / "grating-textured1.tx";
const std::filesystem::path tx_grating_textured2	= tx_path / "grating-textured2.tx";
const std::filesystem::path tx_grating_textured3	= tx_path / "grating-textured3.tx";
const std::filesystem::path tx_grating_textured4	= tx_path / "grating-textured4.tx";
const std::filesystem::path tx_grating_textured5	= tx_path / "grating-textured5.tx";
const std::filesystem::path tx_grid			= tx_path / "grid.tx";
const std::filesystem::path tx_blend			= tx_path / "blend.tx";
const std::filesystem::path tx_jaggies			= tx_path / "jaggies.tx";
const std::filesystem::path tx_bidir			= tx_path / "bidir.tx";
const std::filesystem::path tx_axis			= tx_path / "axis.tx";
const std::filesystem::path tx_omnidir			= tx_path / "omnidir.tx";
const std::filesystem::path tx_unidir			= tx_path / "unidir.tx";
const std::filesystem::path tx_slow_vib		= tx_path / "slow-vib.tx";
const std::filesystem::path tx_pattern_grid		= tx_path / "pattern-grid.tx";

const std::filesystem::path tx_pitchmod_linear_peak	= tx_path / "pitch-mod/linear-peak.tx";
const std::filesystem::path tx_pitchmod_linear_slope	= tx_path / "pitch-mod/linear-slope.tx";
const std::filesystem::path tx_pitchmod_axial		= tx_path / "pitch-mod/axial.tx";
const std::filesystem::path tx_pitchmod_radial_abrupt	= tx_path / "pitch-mod/radial-abrupt-slope.tx";
const std::filesystem::path tx_pitchmod_radial_ring	= tx_path / "pitch-mod/radial-ring.tx";
const std::filesystem::path tx_pitchmod_radial_slope	= tx_path / "pitch-mod/radial-slope.tx";

const std::filesystem::path tx_motion			= tx_path / "motion.tx";

const std::filesystem::path tx_icon_null		= tx_path / "null.png";
const std::filesystem::path tx_icon_linear_grating	= tx_path / "grating-vertical.png";
const std::filesystem::path tx_icon_radial_grating	= tx_path / "grating-radial.png";
const std::filesystem::path tx_icon_axial_grating	= tx_path / "grating-axial.png";
const std::filesystem::path tx_icon_combo		= tx_path / "grating-combo.png";
const std::filesystem::path tx_icon_vibration		= tx_path / "vibration-low.png";
const std::filesystem::path tx_icon_double_linear_grating = tx_path / "grating-dots.png";
const std::filesystem::path tx_icon_blend		= tx_path / "blend.png"; // todo
const std::filesystem::path tx_icon_motion		= tx_path / "motion.png";
const std::filesystem::path tx_icon_pattern_grid	= tx_path / "pattern-grid.png";

inline std::vector<std::filesystem::path> GetStockTextures()
{
	return {
		tx_null,
		tx_plain,
		tx_grating_vertical,
		tx_grating_horizontal,
		tx_grating_diagonal,
		tx_grating_radial,
		tx_grating_radial_vib,
		tx_grating_radial_motion,
		tx_grating_axial,
		tx_grating_dots,
		tx_vibration_low,
		tx_grating_combo,
		tx_grating_textured1,
		tx_grating_textured2,
		tx_grating_textured3,
		tx_grating_textured4,
		tx_grating_textured5,
		tx_grid,
		tx_jaggies,
		tx_blend,
		tx_bidir,
		tx_axis,
		tx_omnidir,
		tx_unidir,
		tx_slow_vib,
		tx_pitchmod_linear_peak,
		tx_pitchmod_linear_slope,
		tx_pitchmod_axial,
		tx_pitchmod_radial_abrupt,
		tx_pitchmod_radial_ring,
		tx_pitchmod_radial_slope,
		tx_motion,
		tx_pattern_grid,
	};
}

} // namespace
