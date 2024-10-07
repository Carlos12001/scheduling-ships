import tkinter as tk
from tkinter import PhotoImage
from PIL import Image, ImageTk  # Usar PIL para redimensionar imágenes

# Función para mover las imágenes
def mover_imagen(imagen_label, velocidad, imagen, y_pos):
    coords = canvas.coords(imagen_label)
    
    # Verifica si la imagen aún existe en el canvas
    if not coords:
        return  # Si no existe, no sigue con la función

    x_actual, y_actual = coords

    # Si el barco llega a la zona de espera 2 (derecha), desaparece y reaparece en la zona de espera 1
    if x_actual >= 1400:  # Al llegar a la zona de espera 2 (derecha)
        canvas.delete(imagen_label)  # Elimina el barco del canvas (desaparece)
        # Reaparece en la posición inicial (justo fuera de la zona de espera 1)
        nuevo_barco = canvas.create_image(200, y_pos, anchor=tk.NW, image=imagen)
        # Comienza a moverse de nuevo
        canvas.after(50, mover_imagen, nuevo_barco, velocidad, imagen, y_pos)
    else:
        canvas.move(imagen_label, velocidad, 0)  # Mueve hacia la derecha
    
    # Repite el movimiento cada 50ms para mantener el ciclo
    canvas.after(50, mover_imagen, imagen_label, velocidad, imagen, y_pos)


import os
print('Get current working directory : ', os.getcwd())
image_src = os.getcwd()

# Crear la ventana principal
root = tk.Tk()
root.title("Botes sobre el canal")
root.geometry("1600x800")

# Crear un canvas para mostrar las imágenes
canvas = tk.Canvas(root, width=1600, height=800)
canvas.pack()

# Cargar la imagen de fondo (mar)
fondo_mar = Image.open( image_src+ "/GUI/mar_fondo.gif")  # Asegúrate de tener una imagen llamada 'mar_fondo.gif'
fondo_mar = fondo_mar.resize((1600, 800))  # Ajustar el tamaño al tamaño del canvas
fondo_mar_img = ImageTk.PhotoImage(fondo_mar)
canvas.create_image(0, 0, anchor=tk.NW, image=fondo_mar_img)  # Colocar la imagen del mar de fondo

# Dibujar el rectángulo (Zona de Espera 1) a la izquierda
canvas.create_rectangle(50, 100, 200, 600, outline='blue', width=3)
canvas.create_text(125, 80, text="Zona de Espera 1", font=("Arial", 16), fill="blue")

# Dibujar el rectángulo (Zona de Espera 2) a la derecha
canvas.create_rectangle(1400, 100, 1550, 600, outline='blue', width=3)
canvas.create_text(1475, 80, text="Zona de Espera 2", font=("Arial", 16), fill="blue")

# Cargar las imágenes de los botes y redimensionarlas al 40% menos
bote_img_original = Image.open(image_src+"/GUI/pesquero.gif")
bote_img_reducida = bote_img_original.resize((int(bote_img_original.width * 0.2), int(bote_img_original.height * 0.2)))
bote_img = ImageTk.PhotoImage(bote_img_reducida)

norma_img_original = Image.open(image_src+"/GUI/norma.gif")
norma_img_reducida = norma_img_original.resize((int(norma_img_original.width * 0.2), int(norma_img_original.height * 0.2)))
norma_img = ImageTk.PhotoImage(norma_img_reducida)

policia_img_original = Image.open(image_src+"/GUI/policia.gif")
policia_img_reducida = policia_img_original.resize((int(policia_img_original.width * 0.2), int(policia_img_original.height * 0.2)))
policia_img = ImageTk.PhotoImage(policia_img_reducida)

# Colocar las imágenes redimensionadas en el canvas justo después de la zona de espera 1 (coordenada 200 en X)
bote_label = canvas.create_image(200, 150, anchor=tk.NW, image=bote_img)  # Justo fuera de la Zona de Espera 1
norma_label = canvas.create_image(200, 250, anchor=tk.NW, image=norma_img)  # Justo fuera de la Zona de Espera 1
policia_label = canvas.create_image(200, 350, anchor=tk.NW, image=policia_img)  # Justo fuera de la Zona de Espera 1

# Mover las imágenes con diferentes velocidades desde la zona de espera 1 hacia la zona de espera 2
mover_imagen(bote_label, 5, bote_img, 150)   # Velocidad del bote
mover_imagen(norma_label, 3, norma_img, 250)  # Velocidad de la norma
mover_imagen(policia_label, 6, policia_img, 350) # Velocidad del policía

# Iniciar la ventana principal
root.mainloop()
