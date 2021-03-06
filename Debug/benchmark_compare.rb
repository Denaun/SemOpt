#!/usr/bin/env ruby

require 'fileutils'

def time
  start = Time.now
  yield
  Time.now - start
end

# Arguments
os = ARGV[ 0 ].upcase
version = ARGV[ 1 ].downcase
input = ARGV[ 2 ]

# Files
ourFile = "./solutions/our_#{version}/#{input}.txt"
benchFile = "./solutions/benchmarks/#{input}.txt"
resultsFile = "./results/#{input}.txt"

# Solve with our algorithm
our = []
res = []
ourTime = time do
	res = `./#{ARGV[ 0 ]}/SemOpt-#{version} #{input}`
end

res.each_line do |line|
	if line =~ /\{.*\}/
		our.push "{" + line.gsub( /\{|\}/, "" ).split( /\s+/ ).sort.join( " " ) + "}"
	end
end
our.sort!

FileUtils.mkdir_p( File.dirname( ourFile ) )
File.open( ourFile, 'w' ) do |file|
	file.write( our.join( "\n" ) )
end

# Check whether the benchmark file exists.
# If it doesn't exist, solve with ASPARTIX and create
bench = []
if !File.exists?( benchFile )
	res = `./ASPARTIX/dlv-#{os} ASPARTIX/prefex.dl #{input} -filter=in`

	res.each_line do |line|
		if line =~ /\{.*\}/
			bench.push "{" + line.gsub( /in\(|\).?|\{|\}/, "" ).split( /\s+/ ).sort.join( " " ) + "}"
		end
	end
	bench.sort!

	FileUtils.mkdir_p( File.dirname( benchFile ) )
	File.open( benchFile, 'w' ) do |file|
		file.write( bench.join( "\n" ) )
	end
else
	File.open( benchFile ).each_line do |line|
		bench.push line
	end
end

# Compare
puts File.basename( input ) + " in #{ourTime}s.\n"
diff = `diff #{benchFile} #{ourFile}`
if !diff.empty?
	puts "Differences:\n" + diff
end
