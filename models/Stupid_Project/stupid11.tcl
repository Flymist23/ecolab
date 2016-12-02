#!stupid11
use_namespace stupidModel

proc simulate {} { 
    uplevel #0 {
      set running 1
         for {} {$running} { } {
	    grow
	    moveBugs
#	    draw .world.canvas
#	    bugsizes::clear
#	     histogram bugsizes [bugsizes]	     
	     set maxsize [max_bugsize]
#	    .statusbar configure -text "t:[tstep] max size:$maxsize"
	     if {$maxsize>100} {set running 0}
	 }
    }
}

set toroidal 1
set nx 200
set ny 200
set mv_dist 4
set max_food_production 0.1
setup $nx $ny $mv_dist $toroidal $max_food_production

set nbugs 4000
set max_food_consumption 1
addBugs $nbugs $max_food_consumption


# size of a grid cell in pixels
set scale 5
stupidModel.scale 5 

#GUI
#toplevel .world
#canvas .world.canvas -background black -width [expr $nx*$scale] \
#    -height [expr $ny*$scale]
#pack .world.canvas
#
#draw .world.canvas
#
#bind .world.canvas <Button-1> {obj_browser [probe %x %y cell].*}
#bind .world.canvas <Button-3> {obj_browser [probe %x %y bug].*}

#histogram bugsizes -title "Bug sizes"

simulate

puts stdout [array get cmd_times]
