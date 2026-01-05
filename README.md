README

To run this project on the NUCLEO, it is necessary to have the following drivers:

1. UGUI Available at: https://github.com/deividAlfa/ST7789-STM32-uGUI
2. LCD  Available at https://www.sitronix.com.tw/en/products/aiot-device-ddi/

We use a modified version available in the lab 6 example files 
found on https://www.dejazzer.com/teaching.html under the embedded systems course.

To set up these drivers, download them to your device and include their paths under 
C/C++ include paths found under project/properties/C/C++ build in CubeMX.

For the IOC file, enable SPI, UART1, and UART2 under connectivity. Set the baud rate of
UART1 to be 9600 Bits/s as this corresponds to the HM-10 Bluetooth Module. We use pins 
PA9 and PA10 for TX and RX respectively. For SPI, we are using PA6, PA7, and PB3 for MISO,
MOSI, and SCK respectively. 

For connections, we are using a breadboard powered with 3V3.

On the pi, first install PiOS available here: https://www.raspberrypi.com/software/.
We downloaded the 64-bit Linux OS for this project. Make sure to enable SSH and configure 
the network settings so that you can immediately get in.

If unable to get in on your network, connect the pi to a monitor and keyboard and install PiConnect
using bash. Use the following commands:

sudo apt update && sudo apt install rpi-connect
sudo reboot
rpi-connect signin. 

This will give you a link to set up the pi for remote communication.

You also require several python packages to run the Pi's script.
First run: 

sudo apt install -y python3-pip python3-dev portaudio19-dev libatlas-base-dev

this is required to install numPy and PyAudio.
Install the following:

numpy: pip3 install numpy
pyaudio: pip3 install piaudio
bleak: pip3 install bleak

enable Bluetooth on the Pi by using:

sudo systemctl enable Bluetooth
sudo systemctl start Bluetooth
scan on

This will allow you to capture the address of your HM-10, which you will need to modify audio_tuner.py.

To create the file, use:
nano audio_tuner.py

To run the file, use:
python audio_tuner.py

