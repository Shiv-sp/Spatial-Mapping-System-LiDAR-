import threading
import math
import numpy as np
import queue
import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Class to handle reliable line-by-line reading from a serial port
class SerialLineReader:
    """
    Class to handle reading lines from the serial port.
    This avoids issues with partial reads and ensures that each line
    is fully read before processing.
    """
    def __init__(self, serial_port):
        self.buffer = bytearray()             # Buffer to store incoming bytes
        self.serial_port = serial_port        # Serial port object

    def readline(self):
        # Check if buffer already has a full line
        i = self.buffer.find(b"\n")
        if i >= 0:
            r = self.buffer[:i + 1]
            self.buffer = self.buffer[i + 1:]
            return r
        # Keep reading until a full line is found
        while True:
            i = max(1, min(2048, self.serial_port.in_waiting))  # Read available bytes (min 1, max 2048)
            data = self.serial_port.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buffer + data[:i + 1]
                self.buffer[0:] = data[i + 1:]
                return r
            else:
                self.buffer.extend(data)

# Function to read serial input from the Lidar sensor
# and push the parsed data into a thread-safe queue.
def read_serial_input(input_queue):
    serial_port = serial.Serial("COM4", 115200)  # Initialize serial connection (change COM port as needed)
    reader = SerialLineReader(serial_port)       # Create instance of SerialLineReader
    while True:
        line = reader.readline().decode().split(", ")  # Read a line, decode it, and split by delimiter
        print(line)                                   # Debug: print the raw input
        input_queue.put(line)                         # Push parsed data into the queue for processing

# Create a thread-safe queue to store data from serial reader
input_queue = queue.Queue()

# Launch a background thread to continuously read serial data
input_thread = threading.Thread(target=read_serial_input, args=(input_queue,), daemon=True)
input_thread.start()

# Store scanned 3D points
points = []

# Set scan parameters
num_slices = 1           # Number of circular slices in the scan (for multi-layer scans)
dist_per_slice = 333     # Distance between slices in mm (e.g., stepper motor moves this much)

# Continuously process data from the queue
while True:
    if input_queue.qsize() > 0:
        val = input_queue.get()
        if len(val) == 3:  # Check if data is valid (3 values: x, distance, angle)
            x, dist, angle = [float(i) for i in val]    # Convert string values to floats
            rad = math.radians(angle)                   # Convert angle to radians
            z = dist * math.cos(rad)                    # Calculate Z based on distance and angle
            y = dist * math.sin(rad)                    # Calculate Y based on distance and angle
            points.append([x, y, z])                    # Store the point as [X, Y, Z]

            # Stop collecting once one full rotation is complete
            if x >= (num_slices - 1) * dist_per_slice and angle >= 359.2:
                break

# Save all collected points to a file in XYZ format
file_name = "2DX3_Final_Scan.xyz"
with open(file_name, 'w') as f:
    f.writelines('{0} {1} {2}\n'.format(*xyz) for xyz in points)
print("Done")

# Visualization parameters
points_per_slice = 512        # Number of points per full circular slice (360 degrees)
points = np.array(points)     # Convert point list to NumPy array for easy slicing

# Set up a 3D plot using matplotlib
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Extract and rearrange coordinates for better orientation
x_vals = points[:, 0]  # X remains unchanged
y_vals = points[:, 2]  # Swap Y and Z for display purposes
z_vals = points[:, 1]

# Plot all individual points
ax.scatter(x_vals, y_vals, z_vals, color='blue', s=5, label="Points")

# Draw structured lines between points to show geometry more clearly
for x in range(num_slices):
    start_idx = x * points_per_slice
    end_idx = start_idx + points_per_slice

    # Connect points around each slice (in a circle)
    for i in range(start_idx, end_idx - 1):
        ax.plot([x_vals[i], x_vals[i + 1]],
                [y_vals[i], y_vals[i + 1]],
                [z_vals[i], z_vals[i + 1]], color='red')

    # Close the circular slice by connecting last point to first
    ax.plot([x_vals[end_idx - 1], x_vals[start_idx]], 
            [y_vals[end_idx - 1], y_vals[start_idx]], 
            [z_vals[end_idx - 1], z_vals[start_idx]], color='red')

    # Connect corresponding points between slices (for multi-layer scans)
    if x < num_slices - 1:
        for i in range(points_per_slice):
            ax.plot([x_vals[start_idx + i], x_vals[start_idx + i + points_per_slice]], 
                    [y_vals[start_idx + i], y_vals[start_idx + i + points_per_slice]], 
                    [z_vals[start_idx + i], z_vals[start_idx + i + points_per_slice]], color='green')

# Label axes and show plot
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
ax.set_title("3D Scan Visualization")
plt.legend()
plt.show()

