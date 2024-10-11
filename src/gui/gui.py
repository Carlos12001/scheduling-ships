import socket
import tkinter as tk
import logging
from PIL import Image, ImageTk

# Configure logging level to only show critical errors
logging.basicConfig(level=logging.CRITICAL)

# Global variables
client_socket = None
canal = []
wait_right_boats = []
wait_left_boats = []
direction = ""
yellow_light = False
emergency = False
real_time = False

# Load images
file_directory = ""  # Set your file directory here

# Boat images
boat_images = {
    1: Image.open(file_directory + "images/normal.gif"),
    2: Image.open(file_directory + "images/fish.gif"),
    3: Image.open(file_directory + "images/police.gif")
}

# Background images
sea_image = Image.open(file_directory + "images/sea.gif")
asphalt_image = Image.new('RGB', (100, 100), color='gray')  # Placeholder for asphalt

# Resize images as needed
def resize_images(new_width, new_height):
    global boat_images, sea_image, asphalt_image
    for key in boat_images:
        boat_images[key] = boat_images[key].resize((new_width, new_height), Image.ANTIALIAS)
    sea_image = sea_image.resize((new_width, new_height), Image.ANTIALIAS)
    asphalt_image = asphalt_image.resize((new_width, new_height), Image.ANTIALIAS)

def process_message(message):
    global canal, direction, yellow_light, emergency, wait_left_boats, wait_right_boats

    lines = message.strip().split('\n')
    for line in lines:
        if line.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in line.split(':')[1].strip().split()]
        elif line.startswith('Direction:'):
            direction = line.split(':')[1].strip()
        elif line.startswith('Yellow Light:'):
            yellow_light = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Emergency:'):
            emergency = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Left:'):
            wait_left_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Right:'):
            wait_right_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]

    update_canvas()  # Update the GUI with new data

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
    global client_socket
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
        except BlockingIOError:
            pass
        except ConnectionResetError as e:
            logging.warning("Connection closed by the server: %s", e)
            close_socket()
        except Exception as e:
            logging.error("Unexpected error: %s", e)
            close_socket()

    else:
        logging.info("No client connected, attempting to reconnect...")
        connect_to_server()
    
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

def on_closing():
    close_socket()
    root.destroy()
    exit()

# Update the canvas based on the current data
def update_canvas():
    canvas.delete("all")  # Clear the canvas
    canvas_width = canvas.winfo_width()
    canvas_height = canvas.winfo_height()

    # Determine the size of each tile
    canal_length = len(canal)
    tile_width = canvas_width // (canal_length + 2)  # +2 for the sides
    tile_height = canvas_height // 5  # Adjust as needed

    resize_images(tile_width, tile_height)

    # Convert PIL images to ImageTk
    tk_boat_images = {key: ImageTk.PhotoImage(img) for key, img in boat_images.items()}
    tk_sea_image = ImageTk.PhotoImage(sea_image)
    tk_asphalt_image = ImageTk.PhotoImage(asphalt_image)

    # Draw the asphalt (top and bottom)
    for i in range(canal_length + 2):
        x = i * tile_width
        canvas.create_image(x, 0, anchor='nw', image=tk_asphalt_image)
        canvas.create_image(x, canvas_height - tile_height, anchor='nw', image=tk_asphalt_image)

    # Draw the waiting boats on the left
    for idx, boat_type in enumerate(wait_left_boats):
        if boat_type in tk_boat_images:
            y = (idx + 1) * tile_height
            canvas.create_image(0, y, anchor='nw', image=tk_boat_images[boat_type])

    # Draw the waiting boats on the right
    for idx, boat_type in enumerate(wait_right_boats):
        if boat_type in tk_boat_images:
            y = (idx + 1) * tile_height
            x = canvas_width - tile_width
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])

    # Draw the canal and boats
    for idx, boat_type in enumerate(canal):
        x = (idx + 1) * tile_width  # Offset by 1 to account for sides
        y = tile_height  # Start drawing from the second row
        if boat_type == -1:
            # Draw sea
            canvas.create_image(x, y, anchor='nw', image=tk_sea_image)
        elif boat_type in tk_boat_images:
            # Draw boat on sea
            canvas.create_image(x, y, anchor='nw', image=tk_sea_image)
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])

    # Keep a reference to the images to prevent garbage collection
    canvas.tk_boat_images = tk_boat_images
    canvas.tk_sea_image = tk_sea_image
    canvas.tk_asphalt_image = tk_asphalt_image

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

# Properly close the window and the socket
root.protocol("WM_DELETE_WINDOW", on_closing)

# Attempt to connect to the server and continuously receive data
connect_to_server()
receive_data()

# Start the main loop of tkinter
root.mainloop()
