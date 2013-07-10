#!/usr/bin/env ruby

ARGV.each do |input|
	values = []
	File.open( input ).each_line do |line|
		values.push( ( /(\d(\.\d+))s/.match( line )[ 1 ] ).to_f )
	end

	max		= values.max
	min		= values.min
	mean	= values.reduce( :+ ) / values.size
	dev		= Math.sqrt( values.inject{ |sum, a| sum + ( a - mean ) ** 2 } / values.size )

	puts "File:\t#{input}"
	puts "Min:\t#{min}"
	puts "Max:\t#{max}"
	puts "Media:\t#{mean}"
	puts "Dev:\t#{dev}"
end
