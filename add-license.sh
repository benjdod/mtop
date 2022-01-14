LicenseFile="./LICENSE"


function make_license_text() {
	CurrentLine=0

	while IFS= read -r LINE
	do
		if [ $CurrentLine -eq 0 ]; then
			echo -n "/* $LINE"
			CurrentLine=1
		else
			echo ""
			echo -n " * $LINE"
		fi
	done < "$LicenseFile"

	echo " */"
}

# echo -e -n "$(cat LICENSE)\n\n$(cat ./draw.c)
