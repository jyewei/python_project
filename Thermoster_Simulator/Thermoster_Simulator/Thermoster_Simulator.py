import time
import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext
from tkinter import *
from tkinter.ttk import Combobox
import threading
import serial.tools.list_ports
from serial import PARITY_ODD, PARITY_EVEN
import datetime
from PIL import ImageTk, Image
import os

ports_list = list(serial.tools.list_ports.comports())
# 设置下拉框的值
select_P = {''}
for i in range(len(ports_list)):
    select_P.add(str(ports_list[i]))
select_E = {"9600"}
select_D = {"8"}
select_A = {"PARITY_ODD"}
select_S = {"1"}
data_share = []
T = None
flag = threading.Event()
ser_a = None
T1 = None
flag1 = threading.Event()
T2 = None
flag2 = threading.Event()
data_share = [[1000, 11], [1100, 2], [1200, 8], [1300, 8], [1400, 8], [1500, 8], [2000, 2], [2100, 8], [2200, 8],
              [2300, 8], [2400, 8], [3000, 2], [3100, 8], [3200, 8], [3300, 8], [3400, 8], [4000, 2], [4100, 8],
              [4200, 8], [4300, 8], [4400, 8], [5000, 2], [5100, 8], [5200, 8], [5300, 8], [5400, 8], [6000, 2],
              [6100, 8], [6200, 8], [6300, 8], [6400, 8], [7000, 2], [7100, 8], [7200, 8], [7300, 8], [7400, 8],
              [8000, 2], [8100, 8], [8200, 8], [8300, 8], [8400, 8], [9000, 2], [9100, 8], [9200, 8], [9300, 8],
              [9400, 8], [10000, 2], [10100, 8], [10200, 8], [10300, 8], [10400, 8], [11000, 2], [11100, 8], [11200, 8],
              [11300, 8], [11400, 8], [12000, 2], [12100, 8], [12200, 8], [12300, 8], [12400, 8], [13000, 2],
              [13100, 8], [13200, 8], [13300, 8], [13400, 8], [14000, 2], [14100, 8], [14200, 8], [14300, 8],
              [14400, 8], [15000, 2], [15100, 8], [15200, 8], [15300, 8], [15400, 8], [16000, 2], [16100, 8],
              [16200, 8], [16300, 8], [16400, 8], [17000, 2], [17100, 8], [17200, 8], [17300, 8], [17400, 8],
              [18000, 2], [18100, 8], [18200, 8], [18300, 8], [18400, 8], [19000, 2], [19100, 8], [19200, 8],
              [19300, 8], [19400, 8], [20000, 2], [20100, 8], [20200, 8], [20300, 8], [20400, 8], [21000, 2],
              [21100, 8], [21200, 8], [21300, 8], [21400, 8], [22000, 2], [22100, 8], [22200, 8], [22300, 8],
              [22400, 8], [23000, 2], [23100, 8], [23200, 8], [23300, 8], [23400, 8], [24000, 2], [24100, 8],
              [24200, 8], [24300, 8], [24400, 8], [25000, 2], [25100, 8], [25200, 8], [25300, 8], [25400, 8],
              [26000, 2], [26100, 8], [26200, 8], [26300, 8], [26400, 8], [27000, 2], [27100, 8], [27200, 8],
              [27300, 8], [27400, 8], [28000, 2], [28100, 8], [28200, 8], [28300, 8], [28400, 8], [29000, 2],
              [29100, 8], [29200, 8], [29300, 8], [29400, 8], [30000, 2], [30100, 8], [30200, 8], [30300, 8],
              [30400, 8], [31000, 2], [31100, 8], [31200, 8], [31300, 8], [31400, 8], [32000, 2], [32100, 8],
              [32200, 8], [32300, 8], [32400, 8]]
result = [[item[0], i] for i, item in enumerate(data_share)]
data_Thermoster = []
data_Broadcast = []
for i in range(64):
    data_Thermoster.append([])
    data_Broadcast.append([])
root = tk.Tk()
root.title("Thermoster_Simulator  Ver:1.3")
root.geometry("1030x950")
root.resizable(False, False)
icon_path = "logo.ico"
style = ttk.Style()
# 创建一个样式，命名为Bold.TButton
style.configure('Bold.TButton', font=('TkDefaultFont', 12, 'bold'))
if os.path.exists(icon_path):
    root.iconbitmap(icon_path)


class ModuleStateTab(tk.Frame):
    def __init__(self, master, modulestate_name):  # 选项卡UI设置
        super().__init__(master)
        self.modulestate_name = modulestate_name
        self.T = None
        self.flag = threading.Event()
        self.position_frame = ttk.Labelframe(self, text="单元故障")

        self.canvas = tk.Canvas(self.position_frame)
        self.canvas.place(relx=0, rely=0, relwidth=1)

        # 创建32个指示灯
        self.lights = []
        self.texts = []
        for i in range(32):
            col = i // 3  # 计算列数
            row = i % 3  # 计算行数
            x = 28 + col * 45  # 调整x坐标的值，使第二列灯向右移动
            y = 10 + row * 26
            light = self.canvas.create_oval(x, y, x + 15, y + 16, outline="SystemButtonFace",
                                            fill="light grey")  # 使用 create_oval 创建圆形
            self.lights.append(light)

            # 在灯的右侧添加文本
            text_x = x + 17  # 文本的x坐标，将文本调整到灯的右侧
            text_y = y + 8  # 文本的y坐标，使其垂直居中
            if 31 - i < 10:
                text = self.canvas.create_text(text_x, text_y, text="0" + str(31 - i) + "b", anchor=tk.W,
                                               fill="black")
            else:
                text = self.canvas.create_text(text_x, text_y, text=str(31 - i) + "b", anchor=tk.W,
                                               fill="black")

        self.position_frame.place(relx=0, rely=0, relwidth=1, relheight=0.25)

        self.cycle_frame = ttk.Labelframe(self, text="Cycle状态")
        self.cycle_frame.place(relx=0, rely=0.24, relwidth=1, relheight=0.77)

        self.cycle_tabs = ttk.Notebook(self.cycle_frame)

        for j in range(1, 5):
            cycle_state_tab = CycleStateTab(self.cycle_tabs, str(j + 4 * (int(self.modulestate_name) - 1)))
            self.cycle_tabs.add(cycle_state_tab, text='制冷循环' + str(j))

        self.cycle_tabs.place(relx=0, rely=0, relwidth=1, relheight=1)
        self.start()

    def start(self):
        if self.T is None or not self.T.is_alive():
            self.flag.set()
            self.T = threading.Thread(target=self.update_data)
            self.T.setDaemon(True)
            self.T.start()

    def update_data(self):

        local = (int(self.modulestate_name) - 1) * 5 + 1
        while True:
            time.sleep(1)
            if len(data_share[local]) > 2:
                for i in range(4):
                    binary_string = bin(data_share[local][7 + i])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
                    for j in range(8):
                        if binary_string[j] == "1":
                            self.canvas.itemconfig(self.lights[i * 8 + j], fill="pink")

                        else:
                            self.canvas.itemconfig(self.lights[i * 8 + j], fill="light grey")


class CycleStateTab(tk.Frame):
    def __init__(self, master, cyclestate_name):  # 选项卡UI设置
        super().__init__(master)
        self.T = None
        self.flag = threading.Event()
        self.cyclestate_name = cyclestate_name
        self.canvas = tk.Canvas(self)
        self.canvas.place(relx=0.008, rely=0.04, relwidth=1, relheight=1)
        # 创建128个指示灯
        self.lights = []
        self.texts = []
        for i in range(128):
            col = i // 11  # 计算列数
            row = i % 11  # 计算行数
            x = 6 + col * 45  # 调整x坐标的值，使第二列灯向右移动
            y = 4 + row * 26
            light = self.canvas.create_oval(x, y, x + 15, y + 16, outline="SystemButtonFace",
                                            fill="light grey")  # 使用 create_oval 创建圆形
            self.lights.append(light)

            # 在灯的右侧添加文本
            text_x = x + 17  # 文本的x坐标，将文本调整到灯的右侧
            text_y = y + 8  # 文本的y坐标，使其垂直居中
            if (i < 32):
                if 31 - i < 10:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(31 - i) + "C", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(31 - i) + "d", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(31 - i) + "E", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(31 - i) + "F", anchor=tk.W,
                                                       fill="black")
                else:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text=str(31 - i) + "C", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text=str(31 - i) + "d", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text=str(31 - i) + "E", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text=str(31 - i) + "F", anchor=tk.W,
                                                       fill="black")


            elif i < 64:
                if 63 - i < 10:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(63 - i) + "h", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(63 - i) + "n", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(63 - i) + "r", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(63 - i) + "y", anchor=tk.W,
                                                       fill="black")
                else:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text=str(63 - i) + "h", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text=str(63 - i) + "n", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text=str(63 - i) + "r", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text=str(63 - i) + "y", anchor=tk.W,
                                                       fill="black")

            elif i < 96:
                if 95 - i < 10:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(95 - i) + "G", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(95 - i) + "L", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(95 - i) + "H", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(95 - i) + "T", anchor=tk.W,
                                                       fill="black")
                else:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text=str(95 - i) + "G", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text=str(95 - i) + "L", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text=str(95 - i) + "H", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text=str(95 - i) + "T", anchor=tk.W,
                                                       fill="black")


            else:
                if 127 - i < 10:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(127 - i) + "J", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(127 - i) + "S", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(127 - i) + "P", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text="0" + str(127 - i) + "U", anchor=tk.W,
                                                       fill="black")
                else:
                    if int(self.cyclestate_name) % 4 == 1:
                        text = self.canvas.create_text(text_x, text_y, text=str(127 - i) + "J", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 2:
                        text = self.canvas.create_text(text_x, text_y, text=str(127 - i) + "S", anchor=tk.W,
                                                       fill="black")
                    elif int(self.cyclestate_name) % 4 == 3:
                        text = self.canvas.create_text(text_x, text_y, text=str(127 - i) + "P", anchor=tk.W,
                                                       fill="black")
                    else:
                        text = self.canvas.create_text(text_x, text_y, text=str(127 - i) + "U", anchor=tk.W,
                                                       fill="black")

                self.texts.append(text)
        self.start()

    def start(self):
        if self.T is None or not self.T.is_alive():
            self.flag.set()
            self.T = threading.Thread(target=self.update_data)
            self.T.setDaemon(True)
            self.T.start()

    def update_data(self):
        local = int(self.cyclestate_name) % 4
        local1 = ((int(self.cyclestate_name) - 1) // 4) * 5 + 1
        while True:
            time.sleep(1)
            if local == 0:
                if len(data_share[local1 + 4]) > 2:
                    for i in range(16):

                        binary_string = bin(data_share[local1 + 4][i + 7])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
                        for j in range(8):
                            if binary_string[j] == "1":
                                self.canvas.itemconfig(self.lights[j + 8 * i], fill="pink")
                            else:
                                self.canvas.itemconfig(self.lights[j + 8 * i], fill="light grey")
            else:
                if len(data_share[local1 + local]) > 2:
                    for i in range(16):

                        binary_string = bin(data_share[local1 + local][i + 7])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
                        for j in range(8):
                            if binary_string[j] == "1":
                                self.canvas.itemconfig(self.lights[j + 8 * i], fill="pink")
                            else:
                                self.canvas.itemconfig(self.lights[j + 8 * i], fill="light grey")


class ITCTab(tk.Frame):
    def __init__(self, master, ITC_name):  # 选项卡UI设置
        super().__init__(master)
        self.T = None
        self.flag = threading.Event()
        self.ITC_name = ITC_name
        # 创建设定1的Labelframe
        self.settings_frame = ttk.Labelframe(self, text='设定1')
        self.settings_frame.place(relx=0, rely=0, relwidth=0.2, relheight=1)
        self.checkboxes1 = []
        self.checkboxvars1 = []
        # 创建地暖、除湿、制热、制冷和运行五个checkbox
        for i in range(5):
            self.checkbox_var = tk.BooleanVar()
            self.checkboxvars1.append(self.checkbox_var)
            self.checkbox = ttk.Checkbutton(self.settings_frame, variable=self.checkbox_var)
            self.checkbox.grid(row=i % 3, column=i // 3, padx=2, pady=2)
            self.checkboxes1.append(self.checkbox)
        self.checkboxes1[0].config(text='地暖')
        self.checkboxes1[1].config(text='除湿')
        self.checkboxes1[2].config(text='制热')
        self.checkboxes1[3].config(text='制冷')
        self.checkboxes1[4].config(text='运行')
        self.checkboxes1[2]['command'] = self.toggle_checkbox
        self.checkboxes1[3]['command'] = self.toggle_checkbox

        # 创建状态1的Labelframe
        self.status_frame = ttk.Labelframe(self, text='状态1')
        self.status_frame.place(relx=0.2, rely=0, relwidth=0.32, relheight=1)
        self.checkboxes2 = []
        self.checkboxvars2 = []
        # 创建风机故障、水阀故障、循环泵开、水阀开、待机、停止和运行七个checkbox
        for i in range(7):
            self.checkbox_var = tk.BooleanVar()
            self.checkboxvars2.append(self.checkbox_var)
            self.checkbox = ttk.Checkbutton(self.status_frame, variable=self.checkbox_var)
            self.checkbox.grid(row=i % 3, column=i // 3, padx=2, pady=2)
            self.checkboxes2.append(self.checkbox)
        self.checkboxes2[0].config(text='风机故障')
        self.checkboxes2[1].config(text='水阀故障')
        self.checkboxes2[2].config(text='循环泵开')
        self.checkboxes2[3].config(text='水阀')
        self.checkboxes2[4].config(text='待机')
        self.checkboxes2[5].config(text='停止')
        self.checkboxes2[6].config(text='运行')
        self.checkboxes2[4]['command'] = self.toggle_checkbox
        self.checkboxes2[5]['command'] = self.toggle_checkbox
        self.checkboxes2[6]['command'] = self.toggle_checkbox

        # 创建用户设定的Labelframe
        self.user_settings_frame = ttk.Labelframe(self, text='用户设定')
        self.user_settings_frame.place(relx=0.52, rely=0, relwidth=0.5, relheight=1)

        # 创建FCU风扇速度的下拉框
        self.fan_speed_var = tk.StringVar()
        self.fan_speed_combobox = ttk.Combobox(self.user_settings_frame, textvariable=self.fan_speed_var,
                                               values=['0:停止', '1:微风', '2:低速', '3:中速', '4:高速', '5:超高速'], width=5)
        self.fan_label = ttk.Label(self.user_settings_frame, text='风扇速度')
        self.fan_label.grid(row=0, column=0)
        self.fan_speed_combobox.grid(row=1, column=0)
        self.fan_speed_combobox.current(0)
        self.address = ttk.Label(self.user_settings_frame, text='设备地址')
        self.address.grid(row=2, column=0)
        self.entry_address = ttk.Entry(self.user_settings_frame, width=6)
        self.entry_address.insert(0, str(self.ITC_name))
        self.entry_address.config(state='readonly')
        self.entry_address.grid(row=3, column=0)
        self.entries1 = []
        self.labels1 = []
        self.entry_frame = ttk.Frame(self.user_settings_frame)
        self.entry_frame.place(relx=0.215, rely=-0.05)
        for i in range(7):
            col = i // 4  # 计算列数
            row = i % 4  # 计算行数

            # 创建可编辑的 Label
            label = ttk.Label(self.entry_frame, text="保留", justify='left')
            label.grid(row=row, column=2 * col, padx=0, pady=0, sticky='w')
            self.labels1.append(label)

            # 创建只读的 Entry，并应用自定义样式
            self.entry = ttk.Entry(self.entry_frame, width=4)
            self.entry.grid(row=row, column=2 * col + 1, padx=0, pady=6)
            self.entry.config(validate="key")
            self.entry.config(validatecommand=(self.entry.register(lambda text: len(text) <= 5), "%P"))
            self.entries1.append(self.entry)
        self.labels1[0].config(text='制冷设温/℃')
        self.labels1[1].config(text='室内环温/℃')
        self.labels1[2].config(text='入口水温/℃')
        self.labels1[3].config(text='出口水温/℃')
        self.labels1[4].config(text='出风口温/℃')
        self.labels1[5].config(text='除湿设温/℃')
        self.labels1[6].config(text='制热设温/℃')
        temp = [0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        data_Broadcast[int(self.ITC_name) - 1] = temp
        self.start()

    def start(self):
        if self.T is None or not self.T.is_alive():
            self.flag.set()
            self.T = threading.Thread(target=self.update_data)
            self.T.setDaemon(True)
            self.T.start()

    def update_data(self):
        data_send = [int(self.ITC_name), 0x03, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, int(self.ITC_name), 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0]
        data_checkboxes1 = ["0"] * 5
        data_checkboxes2 = ["0"] * 7
        while True:
            time.sleep(0.3)
            for i in range(5):
                data_checkboxes1[i] = "0" if not self.checkboxvars1[i].get() else "1"
            temp = "000" + "".join(data_checkboxes1)
            data_send[4] = int(temp, 2)
            for i in range(7):
                data_checkboxes2[i] = "0" if not self.checkboxvars2[i].get() else "1"
            temp = "".join(data_checkboxes2[:2]) + "000000"
            data_send[7] = int(temp, 2)
            temp = "000" + "".join(data_checkboxes2[2:])
            data_send[8] = int(temp, 2)
            value = self.fan_speed_combobox.get()
            data_send[24] = int(value[0])
            for i in range(7):
                high, low = self.convert_to_hex(self.entries1[i].get())
                if i <= 4:
                    data_send[13 + i * 2] = (int(high))
                    data_send[14 + i * 2] = (int(low))
                else:
                    data_send[15 + i * 2] = (int(high))
                    data_send[16 + i * 2] = (int(low))
            data_Thermoster[int(self.ITC_name) - 1] = data_send

    def convert_to_hex(self, text):
        if text.isdigit() or (text.startswith('-') and text[1:].isdigit()):
            num = int(text)
            if num < 0:
                num += 65536
            hex_num = hex(num)[2:].zfill(4)
            high = int(hex_num[:2], 16)
            low = int(hex_num[2:], 16)
            return high, low
        elif '.' in text:
            num = float(text)

            if num < 0:
                num = int(num * 10)
                num += 65536
            else:
                num = int(num * 10)
            hex_num = hex(num)[2:].zfill(4)
            high = int(hex_num[:2], 16)
            low = int(hex_num[2:], 16)
            return high, low
        elif '0x' in text:
            num = int(text, 16)
            hex_num = hex(num)[2:].zfill(4)  # 将整数转换为4位16进制数，并用0填充
            high = int(hex_num[:2], 16)  # 将高两位从十六进制转换为十进制
            low = int(hex_num[2:], 16)  # 将低两位从十六进制转换为十进制
            return high, low
        else:
            high = 0
            low = 0
            return high, low

    def toggle_checkbox(self):

        if self.checkboxvars1[2].get():
            self.checkboxvars1[3].set(False)
            self.checkboxes1[3]['state'] = 'disabled'

        elif self.checkboxvars1[3].get():
            self.checkboxvars1[2].set(False)
            self.checkboxes1[2]['state'] = 'disabled'
        else:
            self.checkboxes1[2]['state'] = 'normal'
            self.checkboxes1[3]['state'] = 'normal'

        if self.checkboxvars2[4].get():
            self.checkboxvars2[5].set(False)
            self.checkboxes2[5]['state'] = 'disabled'
            self.checkboxvars2[6].set(False)
            self.checkboxes2[6]['state'] = 'disabled'

        elif self.checkboxvars2[5].get():
            self.checkboxvars2[4].set(False)
            self.checkboxes2[4]['state'] = 'disabled'
            self.checkboxvars2[6].set(False)
            self.checkboxes2[6]['state'] = 'disabled'

        elif self.checkboxvars2[6].get():
            self.checkboxvars2[4].set(False)
            self.checkboxes2[4]['state'] = 'disabled'
            self.checkboxvars2[5].set(False)
            self.checkboxes2[5]['state'] = 'disabled'

        else:
            self.checkboxes2[4]['state'] = 'normal'
            self.checkboxes2[5]['state'] = 'normal'
            self.checkboxes2[6]['state'] = 'normal'


module_frame = ttk.Labelframe(root, text="模块机状态")
module_frame.place(relx=0.447, rely=0.26, relwidth=0.55, relheight=0.54)

# 创建一个Notebook用于容纳所有选项卡
notebook = ttk.Notebook(module_frame)
notebook.pack(padx=0, pady=0, fill='both', expand=True)
ser_lock = threading.Lock()

frame_com = ttk.LabelFrame(root, text="串口设置:")
label_P = ttk.Label(frame_com, text='串 口:', font=('宋体', 10))
label_P.grid(row=0, column=0, padx=1.5)
com_P = Combobox(frame_com)
com_P.grid(row=0, column=1, padx=1.5)
com_P['value'] = list(select_P)
com_P.current(1)
label_E = ttk.Label(frame_com, text='波特率:', font=('宋体', 10))
label_E.grid(row=1, column=0, padx=1.5)
com_E = Combobox(frame_com)
com_E.grid(row=1, column=1, padx=1.5)
com_E['value'] = list(select_E)
com_E.current(0)
label_D = ttk.Label(frame_com, text='数据位:', font=('宋体', 10))
label_D.grid(row=2, column=0, padx=1.5)
com_D = Combobox(frame_com)
com_D.grid(row=2, column=1, padx=1.5)
com_D['value'] = list(select_D)
com_D.current(0)
com_A = Combobox(frame_com)
com_A.grid(row=3, column=1, padx=1.5)
label_A = ttk.Label(frame_com, text='校 验:', font=('宋体', 10))
label_A.grid(row=3, column=0, padx=1.5)
com_A['value'] = list(select_A)
com_A.current(0)
com_S = Combobox(frame_com)
com_S.grid(row=4, column=1, padx=1.5)
label_S = ttk.Label(frame_com, text='停止位:', font=('宋体', 10))
label_S.grid(row=4, column=0, padx=1.5)
com_S['value'] = list(select_S)
com_S.current(0)
light_uart = tk.Canvas(frame_com, width=20, height=20)
light_uart.place(relx=0.85, rely=0.816)
my_light = light_uart.create_oval(2, 2, 18, 18, outline="SystemButtonFace")
light_uart.itemconfig(my_light, fill="red")
button_con_com = ttk.Button(frame_com, text="打开串口", width=10, command=lambda: On_Serial(com_P.get(), com_E.get(),
                                                                                        com_S.get(), com_D.get(),
                                                                                        com_A.get()))
button_con_com.grid(row=5, column=1, padx=1.5, pady=8)

frame_com.place(relx=0.0, rely=0.00, relwidth=0.24, relheight=0.185)


def fresh_com():
    ports_list = list(serial.tools.list_ports.comports())
    for i in range(len(ports_list)):
        select_P.add(str(ports_list[i]))
    com_P['value'] = list(select_P)
    com_P.current(1)


button_fresh = ttk.Button(frame_com, text="刷新", width=8, command=fresh_com)
button_fresh.grid(row=5, column=0, padx=1.5, pady=8)

try:
    # Try to open the image file
    image = Image.open("jci1.png")

    # Create a PhotoImage object from the image
    photo = ImageTk.PhotoImage(image)

    # Create a label and set the image to it
    label = tk.Label(root, image=photo)
    label.place(relx=0.24, rely=0.036)

except FileNotFoundError:
    # Handle the case when the image file is not found
    pass

frame_txt = ttk.LabelFrame(root, text='数据流')
Data_txt = scrolledtext.ScrolledText(frame_txt, bg="white", fg="black", wrap="none", undo=False)
Data_txt.place(relx=0, rely=0, relwidth=1, relheight=1)
frame_txt.place(relx=0.0, rely=0.185, relwidth=0.445, relheight=0.815)
Data_txt.tag_configure("bold", font=("宋体", 11))
Data_txt.tag_configure("normal", font=("宋体", 11))
Data_txt.tag_configure("red", foreground="#E30420")
Data_txt.tag_configure("blue", foreground="#5239C4")
Data_txt.tag_configure("green", foreground="#0080CC")
Data_txt.tag_configure("black", foreground="#040404")


def clear_text():
    Data_txt.delete('1.0', tk.END)


clear_button = ttk.Button(root, text="清空数据流", width=10, command=clear_text)
clear_button.place(relx=0.34, rely=0.166)
clear_button.lift()


def On_Serial(port, bps, stopbits, bytesize, parity1):
    global ser_a, T
    if ser_a is None:
        parity = None
        if parity1 == 'PARITY_ODD':
            parity = serial.PARITY_ODD
        elif parity1 == 'PARITY_EVEN':
            parity = serial.PARITY_EVEN

        stopbits = float(stopbits)  # 将stopbits参数从字符串转换为浮点数或整数

        bytesize = int(bytesize)

        ser_a = serial.Serial(port[0] + port[1] + port[2] + port[3] + port[4], bps, stopbits=stopbits,
                              bytesize=bytesize, timeout=0, parity=parity)

        light_uart.itemconfig(my_light, fill="yellow")
        button_con_com.configure(text="关闭串口")
        start()
        toggle_serial_components('disabled')
    elif ser_a.isOpen():
        flag.clear()
        button_con_com.configure(text="打开串口")
        light_uart.itemconfig(my_light, fill="red")
        ser_a.close()
        ser_a = None
        toggle_serial_components('normal')
        if T.is_alive():
            flag.set()
            T.join()


def start():
    global T
    if ser_a is not None and ser_a.isOpen():
        flag.set()
        light_uart.itemconfig(my_light, fill="green")
        T = threading.Thread(target=processing_data)
        T.setDaemon(True)
        T.start()


def start1():
    global T2
    flag2.set()
    T2 = threading.Thread(target=update_data)
    T2.setDaemon(True)
    T2.start()


def update_data():
    update_data = []
    update_data_last = []
    while True:
        time.sleep(3)
        if len(data_share[0]) > 2:
            binary_string = bin(data_share[0][7])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
            temp = binary_string[:4]
            decimal_number = int(temp, 2)
            update_data.append(decimal_number)
            temp = binary_string[4:6]
            decimal_number = int(temp, 2)
            update_data.append(decimal_number)
            temp = binary_string[6:]
            decimal_number = int(temp, 2)
            update_data.append(decimal_number)
            binary_string = bin(data_share[0][8])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
            for i in range(5):
                if binary_string[i + 3] == "1":
                    canvas.itemconfig(lights[i], fill="light green")
                else:
                    canvas.itemconfig(lights[i], fill="light grey")
            for i in range(13, 13 + 2 * 4, 2):
                value = (data_share[0][i] << 8) | data_share[0][i + 1]
                update_data.append(value)
            for i in range(4):
                binary_string = bin(data_share[0][21 + i])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
                if i == 0:
                    for j in range(8):
                        if binary_string[j] == "1":
                            canvas.itemconfig(lights[20 - j], fill="light blue")

                        else:
                            canvas.itemconfig(lights[20 - j], fill="light grey")

                elif i == 1:
                    for j in range(8):
                        if binary_string[j] == "1":
                            canvas.itemconfig(lights[12 - j], fill="light blue")

                        else:
                            canvas.itemconfig(lights[12 - j], fill="light grey")

                elif i == 2:
                    for j in range(8):
                        if binary_string[j] == "1":
                            canvas.itemconfig(lights[36 - j], fill="light blue")

                        else:
                            canvas.itemconfig(lights[36 - j], fill="light grey")

                else:
                    for j in range(8):
                        if binary_string[j] == "1":
                            canvas.itemconfig(lights[28 - j], fill="light blue")

                        else:
                            canvas.itemconfig(lights[28 - j], fill="light grey")

            for i in range(4):
                binary_string = bin(data_share[0][25 + i])[2:].zfill(8)  # 使用zfill()方法填充零位至八位
                for j in range(8):
                    if binary_string[j] == "1":
                        canvas1.itemconfig(lights1[i * 8 + j], fill="pink")

                    else:
                        canvas1.itemconfig(lights1[i * 8 + j], fill="light grey")

            # 更新entry
            same_indexes = [index for index, (value1, value2) in enumerate(zip(update_data, update_data_last)) if
                            value1 == value2]
            for i in range(len(update_data)):
                if i not in same_indexes:
                    if 3 <= i <= 5:
                        entries1[i].config(state="normal")  # 将状态设置为可编辑
                        entries1[i].delete(0, "end")  # 清空文本框中的内容
                        if update_data[i] > 32767:
                            entries1[i].insert(0, '-' + str((65536 - update_data[i]) // 10) + '.' + str(
                                (65536 - update_data[i]) % 10))  # 在文本框中插入数据
                        else:
                            entries1[i].insert(0,
                                               str(update_data[i] // 10) + '.' + str(update_data[i] % 10))  # 在文本框中插入数据
                        entries1[i].config(state="readonly")  # 将状态设置为只读
                    elif 1 <= i <= 2:
                        entries1[i].config(state="normal")  # 将状态设置为可编辑
                        entries1[i].delete(0, "end")  # 清空文本框中的内容
                        if update_data[i] == 0:
                            entries1[i].insert(0, "回水")  # 在文本框中插入数据
                        elif update_data[i] == 1:
                            entries1[i].insert(0, "出水")  # 在文本框中插入数据
                        elif update_data[i] == 2:
                            entries1[i].insert(0, "变出水")  # 在文本框中插入数据
                        else:
                            entries1[i].insert(0, "变回水")  # 在文本框中插入数据
                        entries1[i].config(state="readonly")  # 将状态设置为只读
                    elif i == 0 or i == 6:
                        entries1[i].config(state="normal")  # 将状态设置为可编辑
                        entries1[i].delete(0, "end")  # 清空文本框中的内容
                        entries1[i].insert(0, str(update_data[i]))  # 在文本框中插入数据
                        entries1[i].config(state="readonly")  # 将状态设置为只读
            update_data_last = update_data
            update_data = []


def processing_data():  # 数据流收发处理
    data_temp = []
    data = []
    start_fresh()
    start1()
    while ser_a != None:
        time.sleep(0.01)
        if ser_a != None:
            data_temp += ser_a.read_all()
        if len(data_temp) > 0 and data_temp[0] not in range(65):
            data_temp.remove(data_temp[0])
        elif len(data_temp) > 1 and data_temp[0] in range(65) and data_temp[1] not in [0x03, 0x10]:
            data_temp.remove(data_temp[0])
            data_temp.remove(data_temp[0])
        elif len(data_temp) > 2 and data_temp[0] in range(65) and data_temp[1] == 0x10:
            if len(data_temp) > 6:
                if len(data_temp) > (int(data_temp[6]) + 8):
                    for i in range(int(data_temp[6]) + 9):
                        data.append(data_temp[i])
                    for i in range(int(data_temp[6]) + 9):
                        data_temp.remove(data_temp[0])
                    crch, crcl = CRC16(data, int(data[6]) + 7)
                    if (crch == data[int(data[6]) + 8] and crcl == data[int(data[6]) + 7]):
                        address = data[2] << 8 | data[3]
                        for i in range(len(result)):
                            if result[i][0] == address + 1:
                                data_share[result[i][1]] = data
                        Data_txt.insert(END, str(datetime.datetime.now().strftime("%H:%M:%S")) + " "'Rx:',
                                        "bold blue")
                        for i in range(len(data)):
                            data_print = hex(data[i])
                            data_print = data_print.replace('0x', '')
                            if len(data_print) == 1:
                                Data_txt.insert(END, '0%s ' % data_print.upper(), "normal black")
                            else:
                                Data_txt.insert(END, '%s ' % data_print.upper(), "normal black")
                        Data_txt.insert(END, '\n')
                        if data[0] != 0:
                            crch, crcl = CRC16(data, 6)
                            data1 = data[:6]  # 取data的前六个元素
                            data1.extend([crch, crcl])  # 添加crch和crcl到data1中
                            ser_a.write(data1)
                            Data_txt.insert(END, str(datetime.datetime.now().strftime("%H:%M:%S")) + " "'Tx:',
                                            "bold red")
                            for i in range(len(data1)):
                                data_print = hex(data1[i])
                                data_print = data_print.replace('0x', '')
                                if len(data_print) == 1:
                                    Data_txt.insert(END, '0%s ' % data_print.upper(), "normal black")
                                else:
                                    Data_txt.insert(END, '%s ' % data_print.upper(), "normal black")
                            Data_txt.insert(END, '\n')

                    data = []
        elif len(data_temp) > 2 and data_temp[0] in range(65) and data_temp[1] in [0x03]:
            if len(data_temp) > 7:
                for i in range(8):
                    data.append(data_temp[i])
                for i in range(8):
                    data_temp.remove(data_temp[0])
                crch, crcl = CRC16(data, 6)
                if (crch == data[7] and crcl == data[6]):
                    Data_txt.insert(END, str(datetime.datetime.now().strftime("%H:%M:%S")) + " "'Rx:',
                                    "bold blue")
                    for i in range(len(data)):
                        data_print = hex(data[i])
                        data_print = data_print.replace('0x', '')
                        if len(data_print) == 1:
                            Data_txt.insert(END, '0%s ' % data_print.upper(), "normal green")
                        else:
                            Data_txt.insert(END, '%s ' % data_print.upper(), "normal green")
                    Data_txt.insert(END, '\n')
                    if data[0] == 0:
                        address = (data[2] << 8 | data[3]) + 1
                        for i in range(len(data_Broadcast)):
                            if data_Broadcast[i] != []:
                                data_Broadcast[i][2] = data[5] * 2

                                if data_Thermoster[i] != []:

                                    data_Broadcast[i] = data_Broadcast[i][:3]
                                    for number in range(data[5] * 2):
                                        data_Broadcast[i].append(data_Thermoster[i][3 + (address - 40000) * 2 + number])
                                else:
                                    temp=data_Broadcast[i].copy()
                                    data_Broadcast[i] = data_Broadcast[i][:3]
                                    for number in range(data[5] * 2):
                                        data_Broadcast[i].append(temp[3 + (address - 40000) * 2 + number])

                                crch, crcl = CRC16(data_Broadcast[i], len(data_Broadcast[i]))
                                temp = data_Broadcast[i].copy()
                                temp.append(crcl)
                                temp.append(crch)
                                ser_a.write(temp)
                                Data_txt.insert(END, str(datetime.datetime.now().strftime("%H:%M:%S")) + " "'Tx:',
                                                "bold red")
                                for j in range(len(temp)):
                                    data_print = hex(temp[j])
                                    data_print = data_print.replace('0x', '')
                                    if len(data_print) == 1:
                                        Data_txt.insert(END, '0%s ' % data_print.upper(), "normal green")
                                    else:
                                        Data_txt.insert(END, '%s ' % data_print.upper(), "normal green")
                                Data_txt.insert(END, '\n')
                                time.sleep(0.2)

                    elif data[0] > 0:
                        crch, crcl = CRC16(data_Thermoster[data[0] - 1], len(data_Thermoster[data[0] - 1]))
                        temp = data_Thermoster[data[0] - 1].copy()
                        temp.append(crcl)
                        temp.append(crch)
                        ser_a.write(temp)
                        Data_txt.insert(END, str(datetime.datetime.now().strftime("%H:%M:%S")) + " "'Tx:',
                                        "bold red")
                        for i in range(len(temp)):
                            data_print = hex(temp[i])
                            data_print = data_print.replace('0x', '')
                            if len(data_print) == 1:
                                Data_txt.insert(END, '0%s ' % data_print.upper(), "normal black")
                            else:
                                Data_txt.insert(END, '%s ' % data_print.upper(), "normal black")
                        Data_txt.insert(END, '\n')

                data = []


def fresh():
    while True:
        Data_txt.see(tk.END)
        time.sleep(5)


def start_fresh():
    global T1, flag1
    flag1.set()
    T1 = threading.Thread(target=fresh)
    T1.setDaemon(True)
    T1.start()


groupstate_frame = ttk.Labelframe(root, text="组状态信息")
canvas = tk.Canvas(groupstate_frame)
canvas.place(relx=0, rely=0, relheight=0.7, relwidth=1)
# 创建5个指示灯
lights = []
texts = []
for i in range(5):
    col = i // 8  # 计算列数
    row = i % 8  # 计算行数
    x = 6 + col * 72  # 调整x坐标的值，使第二列灯向右移动
    y = 6 + row * 26
    light = canvas.create_oval(x, y, x + 16, y + 16, outline="SystemButtonFace",
                               fill="light grey")  # 使用 create_oval 创建圆形
    lights.append(light)

    # 在灯的右侧添加文本
    text_x = x + 18  # 文本的x坐标，将文本调整到灯的右侧
    text_y = y + 8  # 文本的y坐标，使其垂直居中
    text = canvas.create_text(text_x, text_y, text="保留", anchor=tk.W, fill="black")
    texts.append(text)
canvas.itemconfig(texts[0], text="水泵运行")
canvas.itemconfig(texts[1], text="水泵循环")
canvas.itemconfig(texts[2], text="制热")
canvas.itemconfig(texts[3], text="制冷")
canvas.itemconfig(texts[4], text="运行")
line_x = 78  # 线条的x坐标，将线条调整到指示灯和entry之间
line_y1 = 0  # 线条的起始y坐标，使其与canvas顶部对齐
line_y2 = 130  # 线条的结束y坐标，使其与canvas底部对齐
canvas.create_line(line_x, line_y1, line_x, line_y2, fill="light grey")
canvas.create_line(line_x + 188, line_y1, line_x + 188, line_y2, fill="light grey")
entry_frame = ttk.Frame(groupstate_frame)
entry_frame.place(relx=0.147, rely=0, relheight=0.6, relwidth=0.328)
sysalarm_frame = ttk.Labelframe(groupstate_frame, text="系统故障")
sysalarm_frame.place(relx=0, rely=0.56, relheight=0.44, relwidth=1)
entries1 = []
labels1 = []

for i in range(7):
    col = i // 4  # 计算列数
    row = i % 4  # 计算行数

    # 创建可编辑的 Label
    label = ttk.Label(entry_frame, text="保留", justify='left')
    label.grid(row=row, column=2 * col, padx=1, pady=0, sticky='w')
    labels1.append(label)

    # 创建只读的 Entry，并应用自定义样式
    entry = ttk.Entry(entry_frame, state="readonly", width=4)
    entry.grid(row=row, column=2 * col + 1, padx=0, pady=5)
    entries1.append(entry)

labels1[0].config(text='编组编号')
labels1[1].config(text='制热控制')
labels1[2].config(text='制冷控制')
labels1[3].config(text='Twin/℃')
labels1[4].config(text='Twou/℃')
labels1[5].config(text='环温/℃')
labels1[6].config(text='连接机数')

for i in range(32):
    col = i // 5  # 计算列数
    row = i % 5  # 计算行数
    x = 269 + col * 41  # 调整x坐标的值，使第二列灯向右移动
    y = 6 + row * 26
    light = canvas.create_oval(x, y, x + 16, y + 16, outline="SystemButtonFace",
                               fill="light grey")  # 使用 create_oval 创建圆形
    lights.append(light)

    # 在灯的右侧添加文本
    text_x = x + 16  # 文本的x坐标，将文本调整到灯的右侧
    text_y = y + 7.5  # 文本的y坐标，使其垂直居中
    text = canvas.create_text(text_x, text_y, text=str(i + 1) + '号', anchor=tk.W, fill="black")
    texts.append(text)

canvas.update()  # 更新Canvas以确保文本显示

groupstate_frame.place(relx=0.447, rely=0, relwidth=0.55, relheight=0.28)
canvas1 = tk.Canvas(sysalarm_frame)
canvas1.place(relx=0, rely=0, relheight=1, relwidth=1)
lights1 = []
for i in range(32):
    col = i // 3  # 计算列数
    row = i % 3  # 计算行数
    x = 28 + col * 45  # 调整x坐标的值，使第二列灯向右移动
    y = 10 + row * 26
    light = canvas1.create_oval(x, y, x + 15, y + 16, outline="SystemButtonFace",
                                fill="light grey")  # 使用 create_oval 创建圆形
    lights1.append(light)

    # 在灯的右侧添加文本
    text_x = x + 17  # 文本的x坐标，将文本调整到灯的右侧
    text_y = y + 8  # 文本的y坐标，使其垂直居中
    if 31 - i < 10:
        text = canvas1.create_text(text_x, text_y, text="0" + str(31 - i) + "A", anchor=tk.W,
                                   fill="black")
    else:
        text = canvas1.create_text(text_x, text_y, text=str(31 - i) + "A", anchor=tk.W,
                                   fill="black")


def CRC16(pu8Msg, u16Len):
    u16Sum = 0xFFFF
    for i16Tmp1 in range(u16Len):
        u16Sum ^= pu8Msg[i16Tmp1]
        for i16Tmp2 in range(8):
            if u16Sum & 0x0001:
                u16Sum = u16Sum >> 1
                u16Sum ^= 0xA001
            else:
                u16Sum = u16Sum >> 1
    return (u16Sum >> 8), (u16Sum & 0xFF)


# 分页显示的选项卡数量
num_tabs_per_page = 4
# 总的选项卡数量
total_tabs = 32

# 计算总共需要的页数
num_pages = total_tabs // num_tabs_per_page + (total_tabs % num_tabs_per_page > 0)

# 当前页的索引
current_page = 0


def show_page(page):
    if 0 <= page <= (total_tabs // num_tabs_per_page) - 1:
        global current_page
        current_page = page
        update_tabs()


updating_tabs = False


def update_tabs():
    # 隐藏所有选项卡
    for tab in notebook.tabs():
        notebook.tab(tab, state='hidden')

    # 计算当前页的起始和结束索引
    start_index = current_page * num_tabs_per_page
    end_index = min(start_index + num_tabs_per_page, total_tabs)

    for i in range(start_index, end_index):
        tab_exists = False
        for tab in notebook.tabs():
            if notebook.tab(tab, "text") == '模块机' + str(i + 1) + '状态':
                tab_exists = True
                notebook.tab(tab, state='normal')  # 如果选项卡存在，则显示它
                break

        if not tab_exists:
            module_state_tab = ModuleStateTab(notebook, str(i + 1))
            notebook.add(module_state_tab, text='模块机' + str(i + 1) + '状态')
            notebook.tab(module_state_tab, state='normal')  # 如果选项卡不存在，则添加并显示它

    # 选中第一个可见选项卡
    visible_tabs = [tab for tab in notebook.tabs() if notebook.tab(tab, "state") != 'hidden']
    if visible_tabs:
        notebook.select(visible_tabs[0])


# 创建左箭头按钮
left_arrow_button = ttk.Button(module_frame, text="<", style='Bold.TButton',
                               command=lambda: show_page(current_page - 1) if current_page > 0 else None, width=2)
left_arrow_button.place(relx=0.88, rely=0.00)

# 创建右箭头按钮
right_arrow_button = ttk.Button(module_frame, text=">", style='Bold.TButton',
                                command=lambda: show_page(current_page + 1) if current_page < (
                                        total_tabs // num_tabs_per_page) else None, width=2)
right_arrow_button.place(relx=0.94, rely=0.00)

ITC_frame = ttk.Labelframe(root, text='智能温控器设定与状态信息')
ITC_frame.place(relx=0.447, rely=0.80, relheight=0.24, relwidth=0.55)
# 温控器部分
# 创建一个Notebook用于容纳所有选项卡
notebook1 = ttk.Notebook(ITC_frame)
notebook1.pack(padx=0, pady=0, fill='both', expand=True)
ser_lock1 = threading.Lock()
# 分页显示的选项卡数量
num_tabs_per_page1 = 8
# 总的选项卡数量
total_tabs1 = 64

# 计算总共需要的页数
num_pages1 = total_tabs1 // num_tabs_per_page1 + (total_tabs1 % num_tabs_per_page1 > 0)

# 当前页的索引
current_page1 = 0


def toggle_serial_components(state):
    com_P['state'] = state
    com_E['state'] = state
    com_D['state'] = state
    com_A['state'] = state
    com_S['state'] = state
    button_fresh['state'] = state


def show_page1(page):
    if 0 <= page <= (total_tabs1 // num_tabs_per_page1) - 1:
        global current_page1
        current_page1 = page
        update_tabs1()


updating_tabs1 = False


def update_tabs1():
    # 隐藏所有选项卡
    for tab in notebook1.tabs():
        notebook1.tab(tab, state='hidden')

    # 计算当前页的起始和结束索引
    start_index = current_page1 * num_tabs_per_page1
    end_index = min(start_index + num_tabs_per_page1, total_tabs1)

    for i in range(start_index, end_index):
        tab_exists = False
        for tab in notebook1.tabs():
            if notebook1.tab(tab, "text") == '温控器' + str(i + 1):
                tab_exists = True
                notebook1.tab(tab, state='normal')  # 如果选项卡存在，则显示它
                break

        if not tab_exists:
            ITC_tab = ITCTab(notebook1, str(i + 1))
            notebook1.add(ITC_tab, text='温控器' + str(i + 1))
            notebook1.tab(ITC_tab, state='normal')  # 如果选项卡不存在，则添加并显示它

    # 选中第一个可见选项卡
    visible_tabs = [tab for tab in notebook1.tabs() if notebook1.tab(tab, "state") != 'hidden']
    if visible_tabs:
        notebook1.select(visible_tabs[0])


# 创建左箭头按钮
left_arrow_button1 = ttk.Button(ITC_frame, text="<", style='Bold.TButton',
                                command=lambda: show_page1(current_page1 - 1) if current_page1 > 0 else None, width=2)
left_arrow_button1.place(relx=0.88, rely=0.00)

# 创建右箭头按钮
right_arrow_button1 = ttk.Button(ITC_frame, text=">", style='Bold.TButton',
                                 command=lambda: show_page1(current_page1 + 1) if current_page1 < (
                                         total_tabs1 // num_tabs_per_page1) else None, width=2)
right_arrow_button1.place(relx=0.94, rely=0.00)
# 在初始加载时显示第一页的选项卡
update_tabs()
update_tabs1()
root.mainloop()
