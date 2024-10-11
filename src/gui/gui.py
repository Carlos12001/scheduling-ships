import socket
import tkinter as tk
import logging
from PIL import Image, ImageTk

# --- Socket Code ---

# Configure logging level to only show critical errors
logging.basicConfig(level=logging.CRITICAL)

# Global variables for socket communication
client_socket = None

# Global variables for canal data
canal = []
wait_right_boats = []
wait_left_boats = []
direction = ""
yellow_light = False
size_wait = 0
real_time = False

def connect_to_server():
    global client_socket
    if client_socket is not None:
        return  

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.setblocking(False) 
    try:
        client_socket.connect(('127.0.0.1', 5000))
    except BlockingIOError:
        pass 
    except Exception as e:
        logging.error("Error connecting to server: %s", e)
        close_socket()

def receive_data():
    global client_socket, data_received
    if client_socket:
        try:
            data = client_socket.recv(4096)
            if data:
                s = data.decode()

                index = s.find('END_OF_MESSAGE')
                if index != -1:
                    s = s[:index + len('END_OF_MESSAGE')]
                    process_message(s)
                else:
                    logging.warning("Did not find 'END_OF_MESSAGE'")
            else:
                logging.info("Connection closed by the server")
                close_socket()
        except BlockingIOError as e:
            logging.error("Unexpected error: %s", e)
        except ConnectionResetError as e:
            logging.warning("Connection closed by the server: %s", e)
            close_socket()
        except Exception as e:
            logging.error("Unexpected error: %s", e)
            close_socket()

    else:
        logging.info("No client connected, attempting to reconnect...")
        connect_to_server()
    update_canvas()  # Update the GUI with new data
    # Retry to receive data every 100 ms
    root.after(100, receive_data)

def close_socket():
    global client_socket
    if client_socket:
        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except Exception as e:
            logging.warning("Error shutting down the socket: %s", e)
        try:
            client_socket.close()
            logging.info("Socket closed successfully.")
        except Exception as e:
            logging.warning("Error closing the socket: %s", e)
        client_socket = None

def process_message(message):
    global canal, direction, yellow_light, wait_left_boats, wait_right_boats, real_time, size_wait

    lines = message.strip().split('\n')
    for line in lines:
        if line.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in line.split(':')[1].strip().split()]
        elif line.startswith('Direction:'):
            direction = line.split(':')[1].strip()
        elif line.startswith('Yellow Light:'):
            yellow_light = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('SizeWait:'):
            size_wait =int(line.split(':')[1].strip().lower())
        elif line.startswith('Left:'):
            wait_left_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Right:'):
            wait_right_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Real Time:'):
            real_time = line.split(':')[1].strip().lower() == 'true'

    

def on_closing():
    close_socket()
    root.destroy()
    exit()

# --- GUI Code ---

# Load images
file_directory = ""  # Set your file directory here

# Boat images
boat_images = {
    1: Image.open(file_directory + "images/normal.png"),
    2: Image.open(file_directory + "images/fish.png"),
    3: Image.open(file_directory + "images/police.png")
}

# Background images
sea_image = Image.open(file_directory + "images/sea.png")
asphalt_image = Image.new('RGB', (100, 100), color='gray')  # Placeholder for gray tile




# Resize images as needed
def resize_images(new_width, new_height):
    global boat_images, asphalt_image, sea_image
    for key in boat_images:
        boat_images[key] = boat_images[key].resize((new_width, new_height), Image.ANTIALIAS)
    asphalt_image = asphalt_image.resize((new_width, new_height), Image.ANTIALIAS)
    sea_image = sea_image.resize((new_width, new_height), Image.ANTIALIAS)

# Update the canvas based on the current data
def update_canvas():
    canvas.delete("all")  # Clear the canvas
    canvas_width = canvas.winfo_width()
    canvas_height = canvas.winfo_height()

    if len(canal) == 0:
        canvas.create_text(canvas_width // 2, canvas_height // 2, text="Waiting for information...", font=("Arial", 24*(canvas_width*canvas_height)//(window_width*window_height)))
        return

    # Determine the size of each tile
    canal_length = len(canal)
    num_columns = canal_length + 4  # +4 for the 'mar' columns on each side
    num_rows = 3  # As per your layout

    tile_width = canvas_width // num_columns
    tile_height = canvas_height // num_rows

    resize_images(tile_width, tile_height)

    # Convert PIL images to ImageTk
    tk_boat_images = {key: ImageTk.PhotoImage(img) for key, img in boat_images.items()}
    tk_sea_image = ImageTk.PhotoImage(sea_image)
    tk_asphalt_image = ImageTk.PhotoImage(asphalt_image)

    # Keep track of images to prevent garbage collection
    canvas.tk_boat_images = tk_boat_images
    canvas.tk_sea_image = tk_sea_image
    canvas.tk_asphalt_image = tk_asphalt_image

    # Draw the grid
    for row in range(num_rows):
        for col in range(num_columns):
            x = col * tile_width
            y = row * tile_height
            # Determine what to draw in this cell
            if row == 0 or row == 2:
                # Top or bottom row
                if col < 2 or col >= canal_length + 2:
                    # 'mar' cells (sea)
                    canvas.create_image(x, y, anchor='nw', image=tk_sea_image)
                else:
                    # 'rel' cells (gray)
                    canvas.create_image(x, y, anchor='nw', image=tk_asphalt_image)
            elif row == 1:
                # Middle row
                if col < 2 or col >= canal_length + 2:
                    # 'mar' cells (sea)
                    canvas.create_image(x, y, anchor='nw', image=tk_sea_image)
                else:
                    # 'cnl' cells (canal - sea)
                    canvas.create_image(x, y, anchor='nw', image=tk_sea_image)
                    # Draw boat if present
                    boat_idx = col - 2  # Adjust index for canal list
                    boat_type = canal[boat_idx]
                    if boat_type != -1 and boat_type in tk_boat_images:
                        canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])

    # Draw waiting boats on the left 'mar' columns
    left_mar_columns = [0, 1]
    for idx, boat_type in enumerate(wait_left_boats):
        if boat_type in tk_boat_images:
            col = left_mar_columns[idx % len(left_mar_columns)]
            row = idx // len(left_mar_columns)  # Stack boats vertically if more than two
            x = col * tile_width
            y = row * tile_height
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])

    # Draw waiting boats on the right 'mar' columns
    right_mar_columns = [num_columns - 2, num_columns - 1]
    for idx, boat_type in enumerate(wait_right_boats):
        if boat_type in tk_boat_images:
            col = right_mar_columns[idx % len(right_mar_columns)]
            row = idx // len(right_mar_columns)  # Stack boats vertically if more than two
            x = col * tile_width
            y = row * tile_height
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])



# Configure the GUI
root = tk.Tk()
root.title("Python GUI Client")

# Set window size
window_width = 800
window_height = 600
root.geometry(f"{window_width}x{window_height}")

# Create canvas
canvas = tk.Canvas(root, width=window_width, height=window_height)
canvas.pack(fill="both", expand=True)
canvas.create_text(window_width // 2, window_height // 2, text="Waiting for information...", font=("Arial", 24))

# Properly close the window and the socket
root.protocol("WM_DELETE_WINDOW", on_closing)

# Attempt to connect to the server and continuously receive data
connect_to_server()
receive_data()

# Start the main loop of tkinter
root.mainloop()
