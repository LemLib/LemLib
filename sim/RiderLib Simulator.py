import tkinter as tk
import re  # Only needs to be added once in your imports
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from threading import Thread
import time
import os
import random

FILE_PATH = "pose.txt"
HISTORY_LENGTH = 10  # configurable trail length
def make_float(num):
        num = num.replace(u'\N{MINUS SIGN}', '-')
        return float(num)

class PosePlotter:
    def __init__(self, root):
        self.root = root
        self.root.title("Live Pose Plotter")

        # Set up Matplotlib figure and axis
        self.fig, self.ax = plt.subplots(figsize=(6, 6))
        self.ax.set_xlim(-40,40)
        self.ax.set_ylim(-40,40)
        self.ax.set_xlabel("X (inches)")
        self.ax.set_ylabel("Y (inches)")
        self.ax.set_title("Robot Position (Live)")

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)

        # Control frame with buttons
        control_frame = ttk.Frame(self.root)
        control_frame.pack(side=tk.BOTTOM, fill=tk.X)

        self.gen_button = ttk.Button(control_frame, text="Generate Test Data", command=self.generate_test_data)
        self.gen_button.pack(side=tk.LEFT, padx=10, pady=5)
        self.readout_label = ttk.Label(control_frame, text="Last pose: N/A")
        self.readout_label.pack(side=tk.LEFT, padx=10)

        self.history = []

        self.running = True
        self.update_thread = Thread(target=self.update_loop, daemon=True)
        self.update_thread.start()

        self.canvas.mpl_connect("scroll_event", self.zoom)
        self.scale = 1.0

    def zoom(self, event):
        base_scale = 1.2
        scale_factor = base_scale if event.button == 'up' else 1 / base_scale

        self.scale *= scale_factor
        cur_xlim = self.ax.get_xlim()
        cur_ylim = self.ax.get_ylim()

        xdata = event.xdata
        ydata = event.ydata

        new_xlim = [xdata - (xdata - cur_xlim[0]) * scale_factor,
                    xdata + (cur_xlim[1] - xdata) * scale_factor]
        new_ylim = [ydata - (ydata - cur_ylim[0]) * scale_factor,
                    ydata + (cur_ylim[1] - ydata) * scale_factor]

        self.ax.set_xlim(new_xlim)
        self.ax.set_ylim(new_ylim)
        self.canvas.draw_idle()

    def generate_test_data(self):
        with open(FILE_PATH, "w") as f:
            x = random.uniform(-40,40)
            y = random.uniform(-40,40)
            theta = random.uniform(-180, 180)
            f.write(f"{x:.2f}, {y:.2f}, {theta:.2f}")
    
    def update_loop(self):
        while self.running:
            try:
                if os.path.exists(FILE_PATH):
                    with open(FILE_PATH,'r', encoding='utf-16-le') as f:
                        lines = f.read().splitlines()
                        if lines:
                            last_line = lines[-1].strip()
                            if last_line:
                                parts = last_line.split(",")
                                if len(parts) == 3:
                                    x_str, y_str, theta_str = parts
                                    x = float(x_str)
                                    y = float(y_str)
                                    theta = float(theta_str)

                                    self.history.append((x, y, theta))
                                    if len(self.history) > HISTORY_LENGTH:
                                        self.history.pop(0)
                                    
                                    self.plot()
                                    self.readout_label.config(
                                        text=f"Last pose: ({x:.4f}, {y:.4f}) θ={theta:.4f}°"
                                    )
            except Exception as e:
                print("Error:", e)
            time.sleep(0.1)

    def plot(self):
        self.ax.clear()
        self.ax.set_xlim(-40,40)
        self.ax.set_ylim(-40,40)
        self.ax.set_xlabel("X (inches)")
        self.ax.set_ylabel("Y (inches)")
        self.ax.set_title("Robot Position (Live)")

        if self.history:
            xs, ys, _ = zip(*self.history)
            self.ax.plot(xs, ys, 'bo-', markersize=4)
            x, y, theta = self.history[-1]
            self.ax.plot(x, y, 'ro')  # Latest point
            self.ax.annotate(f"({x:.1f}, {y:.1f}) θ={theta:.1f}°", (x, y), textcoords="offset points", xytext=(10, 10))

        self.canvas.draw_idle()

    def on_close(self):
        self.running = False
        self.root.quit()
        self.root.destroy()


if __name__ == "__main__":
    root = tk.Tk()
    app = PosePlotter(root)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()
