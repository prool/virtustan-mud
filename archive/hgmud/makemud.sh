#/bin/sh
echo Copy MUD...
mkdir ~/VMUD2
mkdir ~/VMUD2/bin
mkdir ~/VMUD2/src
cp src/* ~/VMUD2/src
cp src/.depend ~/VMUD2/src
cp homedir/* ~/VMUD2
tar cf vmud-tmp.tar lib
mv vmud-tmp.tar ~/VMUD2
cd ~/VMUD2
tar xf vmud-tmp.tar
rm vmud-tmp.tar
# cd ~/VMUD2/src
# make clean
# make depend
# make
echo MUD copied!
