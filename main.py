import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox # Added messagebox for validation
import serial
import threading
import queue
from pathlib import Path
import requests
import base64


class ArduinoGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Arduino Data & Image Sender")

        self.serial_port = '/dev/cu.usbmodem11101' 
        self.baud_rate = 9600
        self.ser = None
        self.serial_queue = queue.Queue()
        self.latest_data = None 
        self.connect_serial()

        self.image_path = None

        self.create_widgets()

        self.running = True
        self.serial_thread = threading.Thread(target=self.read_serial, daemon=True)
        self.serial_thread.start()

        # Start queue polling
        self.poll_serial_queue()

    def create_widgets(self):
        # --- User Input Section ---
        user_frame = ttk.LabelFrame(self.root, text="User Info")
        user_frame.grid(row=0, column=0, padx=10, pady=5, sticky="ew") # Grid row 0

        user_label = ttk.Label(user_frame, text="Username:")
        user_label.pack(side=tk.LEFT, padx=(5, 2), pady=5)

        self.username_entry = ttk.Entry(user_frame, width=30) # Store entry widget
        self.username_entry.pack(side=tk.LEFT, padx=(0, 5), pady=5, fill=tk.X, expand=True)

        # --- Arduino Data Section ---
        data_frame = ttk.LabelFrame(self.root, text="Arduino Data")
        # Adjusted grid row to 1
        data_frame.grid(row=1, column=0, padx=10, pady=5, sticky="nsew")

        self.text_area = scrolledtext.ScrolledText(data_frame, wrap=tk.WORD, width=50, height=10) # Reduced height slightly
        self.text_area.pack(padx=5, pady=5, fill=tk.BOTH, expand=True)
        self.text_area.insert(tk.END, "Waiting for Arduino data...\n") # Initial message

        # --- Image Upload Section ---
        image_frame = ttk.LabelFrame(self.root, text="Image Upload")
         # Adjusted grid row to 2
        image_frame.grid(row=2, column=0, padx=10, pady=5, sticky="ew")

        self.upload_btn = ttk.Button(image_frame, text="Upload Image", command=self.upload_image)
        self.upload_btn.pack(side=tk.LEFT, padx=5, pady=5)

        self.image_label = ttk.Label(image_frame, text="No image selected")
        self.image_label.pack(side=tk.LEFT, padx=5, pady=5)

        # --- Send Button ---
         # Adjusted grid row to 3
        self.send_btn = ttk.Button(self.root, text="Send Data", command=self.send_data)
        self.send_btn.grid(row=3, column=0, padx=10, pady=10, sticky="ew")

        # Configure row/column weights for resizing
        self.root.grid_rowconfigure(1, weight=1) # Allow text area row to expand
        self.root.grid_columnconfigure(0, weight=1) # Allow column to expand

    def connect_serial(self):
        try:
            self.ser = serial.Serial(
                port=self.serial_port,
                baudrate=self.baud_rate,
                timeout=1
            )
            # Insert message only if GUI exists (avoid error during init)
            if hasattr(self, 'text_area'):
                 self.text_area.insert(tk.END, f"Connected to {self.serial_port}\n")
                 self.text_area.see(tk.END)
        except Exception as e:
             # Use messagebox for connection errors as text_area might not be ready
             messagebox.showerror("Serial Error", f"Serial connection error: {e}\nPlease check the port and connection.")
             # Optionally disable sending if connection failed
             if hasattr(self, 'send_btn'):
                 self.send_btn.config(state=tk.DISABLED)


    def upload_image(self):
        filetypes = [("Image files", "*.jpg *.jpeg *.png *.bmp")]
        path = filedialog.askopenfilename(title="Select Image", filetypes=filetypes)
        if path:
            self.image_path = path
            self.image_label.config(text=Path(path).name)

    def send_data(self):
        # --- Get Username ---
        username = self.username_entry.get().strip()
        if not username:
            messagebox.showwarning("Input Error", "Please enter a username.")
            return # Stop if no username

        # --- Check Data and Image ---
        if self.latest_data is None: # Check if data was ever received
            messagebox.showwarning("Input Error", "No Arduino data received yet!")
            return
        if not self.image_path:
            messagebox.showwarning("Input Error", "No image selected!")
            return

        try:
            # Read image file and encode as base64
            with open(self.image_path, "rb") as image_file:
                encoded_image = base64.b64encode(image_file.read()).decode('utf-8')

            # Prepare data to send
            payload = {
                'username': username, # <<< ADDED USERNAME
                'arduino_data': self.latest_data,
                'image_name': Path(self.image_path).name,
                'image_data': encoded_image
            }

            # Send to server
            api_endpoint = 'http://10.210.5.222:5000/receive' # Store URL
            self.text_area.insert(tk.END, f"Sending data for user '{username}' to {api_endpoint}...\n")
            self.text_area.see(tk.END)

            response = requests.post(api_endpoint, json=payload, timeout=10) # Added timeout
            response.raise_for_status() # Raise an exception for bad status codes (4xx or 5xx)

            # Process successful response
            self.text_area.insert(tk.END, f"Successfully sent data and image! Server response: {response.text}\n")

        except requests.exceptions.RequestException as e:
            messagebox.showerror("Network Error", f"Failed to connect or send data: {e}")
            self.text_area.insert(tk.END, f"Network Error: {e}\n")
        except Exception as e:
            messagebox.showerror("Error", f"An error occurred: {e}")
            self.text_area.insert(tk.END, f"Error sending data: {e}\n")
        finally:
             # Ensure text area scrolls to the bottom regardless of outcome
            self.text_area.see(tk.END)


    def read_serial(self):
        while self.running:
            if self.ser and self.ser.is_open:
                try:
                    if self.ser.in_waiting > 0:
                        # Read with a small timeout to handle potential incomplete lines gracefully
                        data_bytes = self.ser.readline()
                        if data_bytes: # Ensure something was actually read
                            data = data_bytes.decode('utf-8', errors='ignore').rstrip() # Ignore decoding errors
                            if data: # Put non-empty data onto the queue
                                self.serial_queue.put(data)
                except serial.SerialException as e:
                     # Handle specific serial errors like device disconnection
                     self.serial_queue.put(f"Serial communication error: {e}")
                     self.ser.close() # Attempt to close the port
                     self.ser = None # Set to None to indicate disconnection
                     # Optionally try to reconnect later or notify user prominently
                     messagebox.showerror("Serial Disconnected", f"Lost connection to Arduino: {e}")
                     break # Exit the reading loop
                except Exception as e:
                    self.serial_queue.put(f"Unexpected serial read error: {e}")
                    # Consider whether to break or continue based on error type
            else:
                # If serial is not connected, wait a bit before checking again
                # This prevents this loop from busy-waiting if connection fails initially
                import time
                time.sleep(1)


    def poll_serial_queue(self):
        try:
            while not self.serial_queue.empty():
                data = self.serial_queue.get_nowait() # Use get_nowait for safety
                self.latest_data = data # Store the most recent data point
                # Display in text area
                self.text_area.insert(tk.END, f"Received: {data}\n")
                self.text_area.see(tk.END) # Scroll to the end
        except queue.Empty:
            pass # It's okay if the queue is empty
        finally:
            # Schedule the next poll
            self.root.after(100, self.poll_serial_queue)


    def on_close(self):
        self.running = False
        # Wait briefly for the serial thread to potentially finish its current read
        if self.serial_thread.is_alive():
             self.serial_thread.join(timeout=0.5)

        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Serial port closed.") # Add print confirmation
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = ArduinoGUI(root)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()
