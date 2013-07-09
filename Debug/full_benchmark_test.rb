#!/usr/bin/env ruby

require "fileutils"
require "time"
require "timeout"

def time
  start = Time.now
  yield
  Time.now - start
end

# Arguments
os = ARGV[ 0 ].upcase
inputFolder = ARGV[ 1 ].sub( /\/$/, "" )
givenTime = ( ARGV[ 2 ] ? ARGV[ 2 ].to_i : 120 )

# Files
outputFolder = "./results/" + inputFolder
resultsFile = "./results/#{inputFolder} #{Time.now.iso8601}.txt"

# Test every file and save the result
FileUtils.mkdir_p( File.dirname( resultsFile ) )
File.open( resultsFile, 'w' ) do |file|
	Dir.glob( inputFolder + "/*.dl" ) do |input|
		failedTime = /slow([[:digit:]]+)_/.match( input )
		next if ( failedTime && failedTime[ 1 ].to_i >= givenTime )

		puts "Resolving " + input
		# Timeout to 2' or the passed time: if it doesn't complete, terminate it
		begin
			output = Timeout::timeout( givenTime ) do
				file.write `ruby ./benchmark_compare.rb #{os} #{input}`
			end
		rescue Timeout::Error
			puts "Timed out. Marking the file."

			# Rename the file
			newName = ""
			if ( failedTime )
				newName = File.basename( input ).sub( /slow[[:digit:]]+/, "slow#{givenTime}" )
			else
				newName = "slow#{givenTime}_" + File.basename( input )
			end
			FileUtils.mv input, File.dirname( input ) + "/#{newName}"

			# Kill the process (benchmark solver)
			`killall dlv-#{os}`
		end
	end
end
