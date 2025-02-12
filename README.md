# ground-control
This will be the GUI for the ground control station

## To Install
- Clone this repository
- Run pipenv install (this should create an environment and install all the dependencies)

## To run
- Type `pipenv run python new_gui.py`
- For the struct string, type `I8fI`
- Pick the output folder for the output location
- Type the com port (on Windows, try using COM3, and on Unix, try using `ls /dev | grep usb` to find the device path)
- Hit Connect Serial and Listen
