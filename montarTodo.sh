#!/bin/bash

git clone https://github.com/sisoputnfrba/parsi.git
cd parsi
sudo make install
cd ..

make

cd src

(cd Coordinador; make clean; make; ../../setIPs coordinador)&
(cd Planificador; make clean; make; ../../setIPs planificador)&
(cd ESI; cp -R ../../parsi/src/parsi .; make clean; make; ../../setIPs ESI)&
(cd Instancia; make clean; make; ../../setIPs instancia)
