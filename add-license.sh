LicenseFile="./LICENSE"

LicenseFirstLine=$(head -n 1 $LicenseFile)

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

LicenseText="$(make_license_text)"

function prepend_license_to_file() {
	[ ! -f $1 ] && return 0  # skip nonexistent files
	Target=$1
	if [ $# -eq 2 ]; then
		Target=$2
	fi
	Temp=".temp-license-catfile"
	if [ $(grep -c "$LicenseFirstLine" $1) -eq 0 ]; then
		echo -e "$LicenseText\n" > $Temp
		cat $1 >> $Temp
		cat $Temp > $Target
		rm $Temp
	fi

}

# if an arg is given, write license to just one file
if [ $# -gt 0 ]; then
	for var in "$@"
	do
		prepend_license_to_file $var
	done
	exit 0
fi

echo -n "Prepend license to all *.c and *.h files in src? (y/n) "
read Confirm

case $Confirm in
	y | Y)
		break
		;;
	*)
		exit 1
esac

for file in $(find src | grep "\.[ch]$")
do
	prepend_license_to_file $file
done
