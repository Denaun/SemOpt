#!/usr/bin/env ruby
# Script to rename every file to contain only windows-allowed characters.

require "fileutils"

Dir.glob("**/*") do |file|
  if file[':']
    FileUtils.mv file, file.gsub(":", "-")
  end
end

