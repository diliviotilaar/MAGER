import tkinter as tk
from datetime import datetime
import serial
import sqlite3

ser = serial.Serial('COM4', 9600)
ser.readline()
conn = sqlite3.connect("smartroom.db")
c = conn.cursor()
root = tk.Tk()
root.title("Smart Room")
root.geometry("800x400")
root.configure(bg="white")

current_time = datetime.now().strftime('%H:%M:%S')
current_date = datetime.now().strftime('%Y-%m-%d')

#Title
title = tk.Label(root, text="Smart Room", bg="white", font=(20))
title.place(x=350, y=100)

#Clock
clock = tk.Label(root, text="", bg="white")
def update_clock():
    current_time = datetime.now().strftime("%H:%M:%S")
    clock.config(text=current_time)
    root.after(1000, update_clock)
clock.place(x=385, y=50)

#Automatic Switch
selected_option = tk.StringVar(value="Auto")
def auto_command():
    command = selected_option.get()
    if command == "Auto":
        ser.write(b"3,1\n")
        print("Automatic-mode")
    else:
        ser.write(b"3,0\n")
        print("Manual-mode")
automatic = tk.Radiobutton(root, text="Automatic", variable=selected_option, value="Auto", bg="white", command=auto_command)
manual = tk.Radiobutton(root, text="Manual", variable=selected_option, value="Manual", bg="white", command=auto_command)
automatic.place(x=0, y=350)
manual.place(x=0, y=370)

#Light
def l_on_button():
    n = selected_option.get()
    if n == "Manual":
        data_to_send = "1,1"
        ser.write(data_to_send.encode("utf-8"))
        print("Lights On")
    else:
        print("Auto-mode please switch if you want to manualy turn on the lights")

def l_off_button():
    n = selected_option.get()
    if n == "Manual":
        data_to_send = "1,0"
        ser.write(data_to_send.encode())
        print("Lights Off")
    else:
        print("Auto-mode please switch if you want to manualy turn off the lights")
    print(ser.readline().decode().strip())
    
light = tk.Label(root, text="light", bg="white", borderwidth=0.5, relief="solid", width=10, height=3, anchor="n")
light_on = tk.Button(root, text="On", bg="white", borderwidth=0.5, relief="solid", command=l_on_button)
light_off = tk.Button(root, text="Off", bg="white", borderwidth=0.5, relief="solid", command=l_off_button)
light.place(x=70,y=200)
light_on.place(x=79, y=220)
light_off.place(x=105, y=220)

#AC
def ac_on_button():
    n = selected_option.get()
    if n == "Manual":
        data_to_send = "0,1"
        ser.write(data_to_send.encode())
        print("AC On")
    else:
        print("Auto-mode please switch if you want to manualy turn on the AC")

def ac_off_button():
    n = selected_option.get()
    if n == "Manual":
        data_to_send = "0,0"
        ser.write(data_to_send.encode())
        print("AC Off")
    else:
        print("Auto-mode please switch if you want to manualy turn off the AC")

ac = tk.Label(root, text="AC", bg="white", borderwidth=0.5, relief="solid", width=10, height=3, anchor="n")
ac_on = tk.Button(root, text="On", bg="white", borderwidth=0.5, relief="solid", command=ac_on_button)
ac_off = tk.Button(root, text="Off", bg="white", borderwidth=0.5, relief="solid", command=ac_off_button)
ac.place(x=350, y=200)
ac_on.place(x=359, y=220)
ac_off.place(x=386, y=220)

#RGB 
def enter_button():
    r = red_scale.get()
    g = green_scale.get()
    b = blue_scale.get()
    data_to_send = f"2,{r},{g},{b}"
    ser.write(data_to_send.encode())
    print(f"red:{r}, green:{g}, blue:{b}")
rgb = tk.Label(root, text="AC", bg="white", borderwidth=0.5, relief="solid", width=10, height=3, anchor="n")

red_scale = tk.Scale(root, from_=0, to=255, orient="horizontal", label="Red")
green_scale = tk.Scale(root, from_=0, to=255, orient="horizontal", label="Green")
blue_scale = tk.Scale(root, from_=0, to=255, orient="horizontal", label="Blue")
enter = tk.Button(root, text="Enter", bg="white", borderwidth=0.5, relief="solid", command=enter_button)

rgb.place()
red_scale.place(x=600, y=120)
green_scale.place(x=600, y=180)
blue_scale.place(x=600, y=240)
enter.place(x=600, y=300)


#Temprature
# data= (19,1)
temperature = tk.Label(root, text="", bg="white")
def update_temp():
    unit = "\u00b0C"
    # current_temp= data[0]
    data_from_arduino = ser.readline().decode().strip()
    person, temp, ac, led = data_from_arduino.split(",")
    query = "INSERT into room1 (Date, Time, Person, Temperature, AC, LED) VALUES (?,?,?,?,?,?)"
    c.execute(query, (current_date,current_time,person,temp,ac,led))
    
    conn.commit()
    try:
        current_temp = float(temp)
        temperature.config(text=f"Temperature: {current_temp}{unit}")
    except ValueError:
        temperature.config(text=f"Temperature: {current_temp}{unit}")
    except TypeError:
        temperature.config(text=f"Temperature: {current_temp}{unit}")
    root.after(5000, update_temp)
temperature.place(x=690, y=370)
print(ser.readline().decode().strip())

root.after(0, update_temp)
root.after(0, update_clock)
root.mainloop()