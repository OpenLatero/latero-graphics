add_subdirectory (pitch-mod)

set (TX_FILES
grating-axial.tx grating-axial.png
grating-dots.tx grating-dots.png
grating-radial-motion.tx grating-radial-motion.png
grating-radial-vib.tx grating-radial-vib.png
grating-vertical.tx grating-vertical.png
null.tx null.png
grating-diagonal.tx grating-diagonal.png
grating-horizontal.tx grating-horizontal.png
grating-radial.tx grating-radial.png
motion.tx motion.png
vibration-low.tx vibration-low.png
grating-combo.tx grating-combo.png
grating-textured1.tx grating-textured1.png
grating-textured2.tx grating-textured2.png
grating-textured3.tx grating-textured3.png
grating-textured4.tx grating-textured4.png
grating-textured5.tx grating-textured5.png
grid.tx grid.png
jaggies.tx jaggies.png
blend.tx blend.png
bidir.tx bidir.png
axis.tx axis.png
unidir.tx unidir.png
omnidir.tx omnidir.png
slow-vib.tx slow-vib.png
plain.tx
pattern-grid.tx pattern-grid.png)


INSTALL(FILES ${TX_FILES} DESTINATION share/laterographics/texture)