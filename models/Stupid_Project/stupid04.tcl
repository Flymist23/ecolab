#!stupid04
use_namespace stupidModel

proc simulate {} { 
    uplevel #0 {
      set running 1
         for {} {$running} { } {
#	    grow
	    moveBugs
	    draw .world.canvas
	    .statusbar configure -text "t:[tstep]"
	 }
    }
}

set toroidal 1
set nx 100
set ny 100
set mv_dist 4
setup $nx $ny $mv_dist $toroidal

addBugs 100

# size of a grid cell in pixels
set scale 5
stupidModel.scale 5 

GUI
toplevel .world
canvas .world.canvas -background black -width [expr $nx*$scale] \
    -height [expr $ny*$scale]
pack .world.canvas

draw .world.canvas

bind .world.canvas <Button-1> {obj_browser [probe %x %y cell].*}
bind .world.canvas <Button-3> {obj_browser [probe %x %y bug].*}
