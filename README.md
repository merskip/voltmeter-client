# Voltmerter

## Instalacja

### Konfiguracja Qt5

Ubuntu:
 * `sudo apt-get install cmake qt5-default libqt5serialport5-dev`
 * `echo "PATH=$PATH:/usr/include/x86_64-linux-gnu/qt5" >> ~/.bashrc`
 
### Kompilacja i uruchomienie

 * `git clone git@gitlab.com:merskip/voltmeter-client.git`
 * `cd voltmeter-client`
 * `cmake . && make`
 * `./voltmeter`
 
## Autor
Piotr Merski <<merskip@gmail.com>>