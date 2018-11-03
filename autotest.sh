#!/bin/bash

release_build="./build/release/flyr"
example_json="./examples/pcap.json"
retval=0

check_checksum() {
	output=$(sha1sum $1)
	if [ $2 == ${output:0:41} ]; then
		echo "  -- [+] SUCCESS - $1 == $2"
	else
		echo "  -- [!] FAILURE - $1 != $2"
		retval=1
	fi
}

# Run flyr against the examples pcap.json
$release_build -f $example_json

# Validate checksum of all 8 generated fuzz files
echo "[*] Validating SHA1 hashes for the generated inputs..."
check_checksum "./00000000-lolwut.pcap" "9ce694eada8c843063cdc3ce367140604cad158f"
check_checksum "./00000001-lolwut.pcap" "5648f74204d1f4e37d2666249a266e1ac2b3fe46"
check_checksum "./00000002-lolwut.pcap" "98bfe145fd597b6351cfb5a4f85ee656f7d21837"
check_checksum "./00000003-lolwut.pcap" "c9f1c1cbe5a421dd4f5a2688cf94e7f218c8047d"
check_checksum "./00000004-lolwut.pcap" "cc986293e88430862c6b7f54be04dfbecdf037de"
check_checksum "./00000005-lolwut.pcap" "c9fabae50caedb8a24cd14e54f3d8c7dd8d667a3"
check_checksum "./00000006-lolwut.pcap" "a8262c39a35efda828c442764c1f4d4bbca3f178"
check_checksum "./00000007-lolwut.pcap" "dbf948735f8011e6d118d62f50ac32cbf15a4c7b"

# Delete the generated fuzz files
echo "[*] Cleaning up..."
rm *lolwut.pcap

if [ $retval == 1 ]; then
	echo "[!] Test failed :("
else
	echo "[+] Test Succeeded :)"
fi
exit $retval