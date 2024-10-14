import socket
import tkinter as tk
import logging
import signal
import sys
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
    root.after(1, receive_data)

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
    global canal, direction, yellow_light, wait_left_boats, wait_right_boats, real_time

    lines = message.strip().split('\n')
    for line in lines:
        if line.startswith('Canal:'):
            canal = [int(x) if x != '-1' else -1 for x in line.split(':')[1].strip().split()]
        elif line.startswith('Direction:'):
            direction = line.split(':')[1].strip()
        elif line.startswith('Yellow Light:'):
            yellow_light = line.split(':')[1].strip().lower() == 'true'
        elif line.startswith('Left:'):
            wait_left_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('Right:'):
            wait_right_boats = [int(x) for x in line.split(':')[1].strip().strip('[]').split()]
        elif line.startswith('TiempoReal:'):
            real_time = line.split(':')[1].strip().lower() == 'true'

    

def on_closing():
    close_socket()
    root.destroy()
    exit()

# --- GUI Code ---

global file_directory
global boat_images, sea_image, asphalt_image
global direction_image, yellow_light_image_false, yellow_light_image_true, real_time_image_false, real_time_image_true
global MAX_VISIBLE_BOATS, left_start_index, right_start_index

# Constants
MAX_VISIBLE_BOATS = 6

# Indices for scrolling
left_start_index = 0
right_start_index = 0

# Load images
file_directory = ""  # Set your file directory here

def import_images():
    global boat_images, asphalt_image, sea_image, direction_image, yellow_light_image_false, yellow_light_image_true, real_time_image_false, real_time_image_true
    # Boat images
    boat_images = {
        1: Image.open(file_directory + "images/normal.png"),
        2: Image.open(file_directory + "images/fish.png"),
        3: Image.open(file_directory + "images/police.png")
    }

    # Background images
    sea_image = Image.open(file_directory + "images/sea.png")
    asphalt_image = Image.new('RGB', (100, 100), color='gray')  # Placeholder for gray tile

    ## Flags images
    direction_image = {
        "Left": Image.open(file_directory + "images/left.png"),
        "Right": Image.open(file_directory + "images/right.png")
    } 

    yellow_light_image_false = Image.open(file_directory + "images/turn_off.png")
    yellow_light_image_true = Image.open(file_directory + "images/turn_on.png")

    real_time_image_false = Image.open(file_directory + "images/x.png")
    real_time_image_true = Image.open(file_directory + "images/check.png")



# Resize images as needed
def resize_images(new_width, new_height):
    global boat_images, asphalt_image, sea_image, direction_image, yellow_light_image_false, yellow_light_image_true, real_time_image_false, real_time_image_true
    import_images()

    for key in boat_images:
        boat_images[key] = boat_images[key].resize((new_width, new_height), Image.ANTIALIAS)
    asphalt_image = asphalt_image.resize((new_width, new_height), Image.ANTIALIAS)
    sea_image = sea_image.resize((new_width, new_height), Image.ANTIALIAS)
    for key in direction_image:
        direction_image[key] = direction_image[key].resize((new_width, new_height//2), Image.ANTIALIAS)
    yellow_light_image_false = yellow_light_image_false.resize((new_width, new_height//2), Image.ANTIALIAS)
    yellow_light_image_true = yellow_light_image_true.resize((new_width, new_height//2), Image.ANTIALIAS)
    real_time_image_false = real_time_image_false.resize((new_width, new_height//2), Image.ANTIALIAS)
    real_time_image_true = real_time_image_true.resize((new_width, new_height//2), Image.ANTIALIAS)

# Update the canvas based on the current data
def update_canvas():
    global canal, direction, yellow_light, wait_left_boats, wait_right_boats, real_time
    global boat_images, asphalt_image, sea_image
    global direction_image, yellow_light_image_false, yellow_light_image_true, real_time_image_false, real_time_image_true
    global MAX_VISIBLE_BOATS, left_start_index, right_start_index
    canvas.delete("all")  # Clear the canvas
    canvas_width = canvas.winfo_width()
    canvas_height = canvas.winfo_height()

    multiplier_font = (canvas_width*canvas_height)//(window_width*window_height)

    if len(canal) == 0:
        canvas.create_text(canvas_width // 2, canvas_height // 2, text="Waiting for information...", font=("Arial", 24*multiplier_font))
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
    tk_direction_image = ImageTk.PhotoImage(direction_image[direction])
    tk_yellow_light_image = ImageTk.PhotoImage(yellow_light_image_true if yellow_light else yellow_light_image_false)
    tk_real_time_image = ImageTk.PhotoImage(real_time_image_true if real_time else real_time_image_false)

    # Keep track of images to prevent garbage collection
    canvas.tk_boat_images = tk_boat_images
    canvas.tk_sea_image = tk_sea_image
    canvas.tk_asphalt_image = tk_asphalt_image
    canvas.tk_direction_image = tk_direction_image
    canvas.tk_yellow_light_image = tk_yellow_light_image
    canvas.tk_real_time_image = tk_real_time_image

    ####--------------------- DRAW CANAL ---------------------####
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

    ####--------------------- DRAW FLAGS ---------------------####
    size_font = 6*multiplier_font
    font_text = ("Arial", size_font, "bold")
    x = 2 * tile_width
    y = 0 * tile_height
    canvas.create_image(x,y, anchor='nw', image=tk_direction_image)
    canvas.create_text(x+tile_width//2, y+tile_height//2, text="Direccion", font=font_text)

    x = 2*tile_width
    y = (num_rows-1)*tile_height
    canvas.create_image(x, y, anchor='nw', image=tk_yellow_light_image)
    canvas.create_text(x+tile_width//2, y+tile_height//2, text="Puede Pasar", font=font_text)

    x = (num_columns-1-2)*tile_width
    y = (num_rows-1)*tile_height
    canvas.create_image(x, y, anchor='nw', image=tk_real_time_image)
    canvas.create_text(x+tile_width//2, y+tile_height//2, text="Tiempo Real", font=font_text)


    ####--------------------- DRAW WAITING LEFT BOATS ---------------------####
    font_text = ("Arial", size_font*2, "bold")
    if len(wait_left_boats) <= MAX_VISIBLE_BOATS or left_start_index > len(wait_left_boats):
        left_start_index = 0
    cout = 0
    # Draw waiting boats on the left 'mar' columns
    left_mar_columns = [0, 1]
    for i in range(left_start_index, len(wait_left_boats) + left_start_index):
        if cout >= MAX_VISIBLE_BOATS:
            break
        cout += 1
        boat_type = wait_left_boats[i]
        idx = (i-left_start_index) % MAX_VISIBLE_BOATS
        if boat_type in tk_boat_images:
            col = left_mar_columns[idx % len(left_mar_columns)]
            row = idx // len(left_mar_columns)  # Stack boats vertically if more than two
            x = col * tile_width
            y = row * tile_height
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])
            canvas.create_text(x+tile_width//16, y+tile_height//32, text=str(i), font=font_text)

    # Manage visibility of left up/down buttons
    if left_start_index > 0:
        left_up_button.place_configure(x=0, y=0)
    else:
        left_up_button.place_forget()

    if left_start_index + MAX_VISIBLE_BOATS < len(wait_left_boats):
        left_down_button.place_configure(x=0, y=canvas_height - 32)
    else:
        left_down_button.pack_forget()


    ####--------------------- DRAW WAITING RIGHT BOATS ---------------------####


    if len(wait_right_boats) <= MAX_VISIBLE_BOATS or right_start_index > len(wait_right_boats):
        right_start_index = 0
    cout = 0
    # Draw waiting boats on the right 'mar' columns
    right_mar_columns = [num_columns - 2, num_columns - 1]
    for i in range(right_start_index, len(wait_right_boats)):
        if cout >= MAX_VISIBLE_BOATS:
            break
        cout += 1
        boat_type = wait_right_boats[i]
        idx = (i-right_start_index) % MAX_VISIBLE_BOATS
        if boat_type in tk_boat_images:
            col = right_mar_columns[idx % len(right_mar_columns)]
            row = idx // len(right_mar_columns)  # Stack boats vertically if more than two
            x = col * tile_width
            y = row * tile_height
            canvas.create_image(x, y, anchor='nw', image=tk_boat_images[boat_type])
            canvas.create_text(x+tile_width//16, y+tile_height//32, text=str(i), font=font_text)

    # Manage visibility of right up/down buttons
    if right_start_index > 0:
        right_up_button.place_configure(x=canvas_width - 32, y=0)
    else:
        right_up_button.place_forget()

    if right_start_index + MAX_VISIBLE_BOATS < len(wait_right_boats):
        right_down_button.place_configure(x=canvas_width - 32, y=canvas_height - 32)
    else:
        right_down_button.place_forget()


# Scroll functions for left waiting area
def left_scroll_up():
    global left_start_index
    if left_start_index >= 2:
        left_start_index -= 2
    else:
        left_start_index = 0
    update_canvas()

def left_scroll_down():
    global MAX_VISIBLE_BOATS,left_start_index, wait_left_boats
    if left_start_index + MAX_VISIBLE_BOATS < len(wait_left_boats) - 2:
        left_start_index += 2
    else:
        left_start_index = len(wait_left_boats) - MAX_VISIBLE_BOATS
        if left_start_index < 0:
            left_start_index = 0
    update_canvas()

# Scroll functions for right waiting area
def right_scroll_up():
    global right_start_index
    if right_start_index >= 2:
        right_start_index -= 2
    else:
        right_start_index = 0
    update_canvas()

def right_scroll_down():
    global MAX_VISIBLE_BOATS,right_start_index, wait_left_boats
    if right_start_index + MAX_VISIBLE_BOATS < len(wait_right_boats) - 2:
        right_start_index += 2
    else:
        right_start_index = len(wait_right_boats) - MAX_VISIBLE_BOATS
        if right_start_index < 0:
            right_start_index = 0
    update_canvas()

# --- Handle Keyboard Interrupt ---

def handle_sigint(signum, frame):
    logging.info("Keyboard interrupt received. Closing the application...")
    on_closing()


# Bind keyboard events (optional, e.g., pressing 'Escape' to close)
def on_key_press(event):
    key = event.keysym
    logging.info(f"Key pressed: {key}")
    if key == 'Escape':
        on_closing()





# Configure the GUI
# Register the signal handler for SIGINT
signal.signal(signal.SIGINT, handle_sigint)
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


# Left waiting area
left_up_button = tk.Button(canvas, text='Up', command=left_scroll_up)
left_down_button = tk.Button(canvas, text='Down', command=left_scroll_down)

# Right waiting area
right_up_button = tk.Button(canvas, text='Up', command=right_scroll_up)
right_down_button = tk.Button(canvas, text='Down', command=right_scroll_down)

# Properly close the window and the socket
root.protocol("WM_DELETE_WINDOW", on_closing)

# Bind keyboard events
root.bind('<Key>', on_key_press)

## Import images
import_images()

# Attempt to connect to the server and continuously receive data
connect_to_server()
receive_data()

# Start the main loop of tkinter with KeyboardInterrupt handling
try:
    root.mainloop()
except KeyboardInterrupt:
    handle_sigint(None, None)
except Exception as e:
    logging.error(e)
    on_closing()