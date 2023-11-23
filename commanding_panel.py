from PySide6 import QtCore, QtWidgets

class commanding_signals(QtCore.QObject):
    command_signal = QtCore.Signal(str)

class valve_actuation_widget(QtWidgets.QWidget):
    def __init__(self, name: str):
        super().__init__()
        self.layout = QtWidgets.QGridLayout(self)

        self.label = QtWidgets.QLabel(name)
        self.layout.addWidget(self.label, 0, 0)

        self.open_button = QtWidgets.QPushButton("Open")
        self.open_button.setStyleSheet("""
            QPushButton {
                background: green;
                color: white;
                font-size: 13px;
            }

            QPushButton:hover {
                background: #5cbd5e;
            }
        """)
        self.open_button.setCursor(QtCore.Qt.PointingHandCursor) # Change cursor when hovering over the button
        self.layout.addWidget(self.open_button, 1, 0)

        self.close_button = QtWidgets.QPushButton("Close")
        self.close_button.setStyleSheet("""
            QPushButton {
                background: red;
                color: white;
                font-size: 13px;
            }

            QPushButton:hover {
                background: #ff7a73;
            }
        """)
        self.close_button.setCursor(QtCore.Qt.PointingHandCursor) # Change cursor when hovering over the button
        self.layout.addWidget(self.close_button, 2, 0)

class commanding_panel(QtWidgets.QLabel):
    def __init__(self):
        super().__init__()
        
        self.commands = commanding_signals() # Reference MUST be kept to prevent object deletion.
        self.command_signal = self.commands.command_signal

        self.layout = QtWidgets.QGridLayout(self)

        self.setup_gui()
        self.connect_functionality()


    def setup_gui(self):
        # Nitrogen Valve Actuation
        self.nitrogen_valve = valve_actuation_widget("Nitrogen Valve")
        self.layout.addWidget(self.nitrogen_valve, 0, 0)

        # Bleed Valve Actuation
        self.bleed_valve = valve_actuation_widget("Bleed Valve")
        self.layout.addWidget(self.bleed_valve, 0, 1)

        # Ethanol Valve Actuation
        self.ethanol_valve = valve_actuation_widget("Ethanol Valve")
        self.layout.addWidget(self.ethanol_valve, 1, 0)

        # Oxygen Valve Actuation
        self.oxygen_valve = valve_actuation_widget("Oxygen Valve")
        self.layout.addWidget(self.oxygen_valve, 1, 1)

        # Empty Space
        self.layout.addWidget(QtWidgets.QLabel(""), 2, 0)

        # Sequence Control
        self.sequence_command = QtWidgets.QLineEdit()
        self.sequence_command.setPlaceholderText("Choose Sequence")
        self.layout.addWidget(self.sequence_command, 3, 0, 1, 2)
        
        self.start_sequence_button = QtWidgets.QPushButton("Start Sequence")
        self.start_sequence_button.setStyleSheet("""
            QPushButton {
                background: green;
                color: white;
                font-size: 13px;
            }

            QPushButton:hover {
                background: #5cbd5e;
            }
        """)
        self.start_sequence_button.setCursor(QtCore.Qt.PointingHandCursor)
        self.layout.addWidget(self.start_sequence_button, 4, 0, 1, 2)
        
        self.abort_sequence_button = QtWidgets.QPushButton("Abort Sequence")
        self.abort_sequence_button.setStyleSheet("""
            QPushButton {
                background: red;
                color: white;
                font-size: 13px;
            }

            QPushButton:hover {
                background: #ff7a73;
            }
        """)
        self.abort_sequence_button.setCursor(QtCore.Qt.PointingHandCursor)
        self.layout.addWidget(self.abort_sequence_button, 5, 0, 1, 2)

    def connect_functionality(self):
        # self.sequence_command.returnPressed.connect(self.send_command_and_clear_text)
        self.start_sequence_button.clicked.connect(self.send_command_and_clear_text)
        self.abort_sequence_button.clicked.connect(lambda : self.send_command("COMMAND: ABORT"))

        self.nitrogen_valve.open_button.clicked.connect(lambda : self.send_command("VALVE: nitrogen open"))
        self.nitrogen_valve.close_button.clicked.connect(lambda : self.send_command("VALVE: nitrogen close"))

        self.bleed_valve.open_button.clicked.connect(lambda : self.send_command("VALVE: bleed open"))
        self.bleed_valve.close_button.clicked.connect(lambda : self.send_command("VALVE: bleed close"))

        self.ethanol_valve.open_button.clicked.connect(lambda : self.send_command("VALVE: ethanol open"))
        self.ethanol_valve.close_button.clicked.connect(lambda : self.send_command("VALVE: ethanol close"))

        self.oxygen_valve.open_button.clicked.connect(lambda : self.send_command("VALVE: oxygen open"))
        self.oxygen_valve.close_button.clicked.connect(lambda : self.send_command("VALVE: oxygen close"))

    def open_dropmech(self):
        try:
            if(self.current[1] >= 4):
                self.send_command("DROPMECH: open")
            else:
                print("not correct mode")
        except:
            self.send_command("")

    def send_command(self, command : str):
        self.command_signal.emit(command)

    def send_command_and_clear_text(self):
        self.send_command(self.sequence_command.text())
        self.sequence_command.setText("")