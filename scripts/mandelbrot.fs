

width  = 1920
height = 1080

MIN_X = -1.5
MAX_X =  1.0
MIN_Y = -1.0
MAX_Y =  1.0

#constants for the iteration function
#  max is the number of times the equation is called.
#  divergence is the value it tests
iterate {
    max        = 100
	divergence = 2.0
}

#need to be able to parse this stuff
# Z and C are Vector2ds
equation {
    Z = 1
	begin
	    #for max or until divergence
        Z = Z * Z + C
		#other possible stuff
	end
}


#add fucntions like log(), pow(), 