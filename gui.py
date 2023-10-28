import tkinter as tk
import serial

class CounterApp:
    def __init__(self, root, serial_port):
        self.root = root
        self.root.title("Serial Counter App")
        
        # Initialize the serial connection
        self.serial_port = serial_port

        # Create and configure the label to display the counter
        self.counter_label = tk.Label(root, text="", font=("Helvetica", 24))
        self.counter_label.pack(pady=20)

        # Create the button to quit the application
        self.quit_button = tk.Button(root, text="Quit", command=root.destroy)
        self.quit_button.pack()

        # Start the update loop to read from the serial input
        self.update_display()

    def update_display(self):
        try:
            # Read data from the serial input
            data = self.serial_port.readline().decode().strip()
            self.counter_label.config(text=data)
        except serial.SerialException as e:
            self.counter_label.config(text="Error: " + str(e))
        self.root.after(100, self.update_display)  # Update every 100 milliseconds

if __name__ == "__main__":
    try:
        ser = serial.Serial('COM4', baudrate=9600)  # Replace 'COM1' with your actual serial port
        root = tk.Tk()
        app = CounterApp(root, ser)
        root.mainloop()
    except serial.SerialException as e:
        print("Error opening the serial port: " + str(e))
