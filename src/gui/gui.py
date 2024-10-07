import tkinter as tk

class MyApplication(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Tkinter POO Ejemplo")
        self.geometry("300x200")   


        self.button = tk.Button(self, text="Clic me", command=self.update_label)
        self.button.pack(pady=20)
        
        self.label = tk.Label(self, text="Hola, ¡presiona el botón!")
        self.label.pack(pady=20)
    
    def update_label(self):
        self.label.config(text="¡Gracias por hacer clic!")


if __name__ == "__main__":
    app = MyApplication()
    app.mainloop()
