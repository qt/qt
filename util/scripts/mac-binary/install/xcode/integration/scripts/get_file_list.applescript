set all_files to {}
tell application "Xcode"
	tell project 1
		repeat with f in file references
			tell f
				set fn to absolute path
				set end of all_files to fn & "
"
			end tell
		end repeat
		set all_groups to {}
		repeat with grp in groups
			set end of all_groups to grp
		end repeat
		repeat with grp in all_groups
			tell grp
				repeat with grp in groups
					set end of all_groups to grp
				end repeat
				repeat with f in file references
					tell f
						set fn to absolute path
						set end of all_files to fn & "
"
					end tell
				end repeat
			end tell
		end repeat
	end tell
end tell
all_files