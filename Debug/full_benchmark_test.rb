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

# Files
outputFolder = "./results/" + inputFolder
resultsFile = "./results/#{inputFolder} #{Time.now.iso8601}.txt"

# Test every file and save the result
FileUtils.mkdir_p( File.dirname( resultsFile ) )
File.open( resultsFile, 'w' ) do |file|
	Dir.glob( inputFolder + "/*.dl" ) do |input|
		puts "Resolving " + input
		# Timeout to 2': if it doesn't complete, terminate it
		begin
			Timeout::timeout( 120 ) do
				file.write `ruby ./benchmark_compare.rb #{os} #{input}`
			end
		rescue Timeout::Error
			puts "Timed out."
			FileUtils.rm input
		end
	end
end
