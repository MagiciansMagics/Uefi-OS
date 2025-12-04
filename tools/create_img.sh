dd if=/dev/zero of=os.img bs=512 count=93750 &>/dev/null

gdisk os.img &>/dev/null <<EOF
o
Y
n



ef00
w
Y
EOF

sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop99 os.img &>/dev/null
sudo mkdosfs -F 32 /dev/loop99 &>/dev/null
sudo losetup -d /dev/loop99 &>/dev/null
