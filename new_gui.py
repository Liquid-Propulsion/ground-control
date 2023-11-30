import sys
from time import time
import pyqtgraph as pg
from PySide6 import QtCore, QtWidgets
import qdarkstyle

from state_management_widget import state_management_widget
from file_management_widget import file_management_widget
from custom_graph_widget import custom_graph_widget
from custom_number_display import custom_number_display
from commanding_panel import commanding_panel

#https://www.pythonguis.com/tutorials/plotting-pyqtgraph/


class GroundControlWindow(QtWidgets.QWidget):
    def __init__(self) -> None:
        super().__init__()
        self.setWindowTitle("Liquid Propulsion Ground Control")
        self.layout = QtWidgets.QGridLayout(self)
        self._thread_pool = QtCore.QThreadPool.globalInstance()

        self.setStyleSheet(qdarkstyle.load_stylesheet(qt_api='pyside6'))
        
        pg.setConfigOption('background', 'black')
         
        self.program_start_time = time()
        
        self.setup_number_displays()
        self.setup_graphs()
        self.init_widgets()

    def setup_number_displays(self):
        self.numerical_displays = []
        self.numerical_displays.append(custom_number_display(1, "Current State: Vibing"))
        self.layout.addWidget(self.numerical_displays[0], 4, 2)

    def setup_graphs(self):
        self.graphs = []

        self.graphs.append(custom_graph_widget(indexes_in_struct=(2), names=('Nitrogen Line Pressure'), start=self.program_start_time))
        self.graphs.append(custom_graph_widget(indexes_in_struct=(3), names=('Oxygen Line Pressure'), start=self.program_start_time))
        self.graphs.append(custom_graph_widget(indexes_in_struct=(4), names=('Ethanol Tank Pressure'), start=self.program_start_time))
        self.graphs.append(custom_graph_widget(indexes_in_struct=(5), names=('Igniter Chamber Pressure'), start=self.program_start_time))
       
        self.layout.addWidget(self.graphs[0], 0, 0)
        self.layout.addWidget(self.graphs[1], 0, 1)
        self.layout.addWidget(self.graphs[2], 1, 0)
        self.layout.addWidget(self.graphs[3], 1, 1)

    def init_widgets(self):
        #File input
        self.file_management_panel = file_management_widget(self.output)
        self.layout.addWidget(self.file_management_panel, 2, 0)
        
        #Text view
        self.console = QtWidgets.QTextBrowser()
        self.layout.addWidget(self.console, 2, 1, 3, 1)

        #State management
        self.state_management_panel = state_management_widget(self.output, self.file_management_panel, self._thread_pool, self.graphs, self.numerical_displays, self.program_start_time)
        self.layout.addWidget(self.state_management_panel, 3, 0)
        self.state_management_panel.signals.clear_output.connect(self.clear_console)

        #Communication output
        self.command_panel = commanding_panel()
        self.layout.addWidget(self.command_panel, 0, 2, 2, 1)
        self.command_panel.command_signal.connect(self.state_management_panel.send_command)
        self.state_management_panel.command_panel = self.command_panel

    def output(self, text):
        self.console.append(text)

    def clear_console(self):
        self.console.clear()

    def closeEvent(self, event):
        self.state_management_panel.stop_listening()
    
    # def keyPressEvent(self, event):
    #     if event.key() == QtCore.Qt.Key_Space:
    #         self.command_panel.send_command("COMMAND: ABORT")

if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    app.setStyle('Fusion')

    window = GroundControlWindow()
    window.resize(800, 600)
    window.show()
    
    sys.exit(app.exec())