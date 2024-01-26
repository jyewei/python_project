import tkinter as tk
from tkinter import *
from tkinter import ttk
from datetime import datetime
from tkinter import Canvas
import serial.tools.list_ports
import serial
from tkinter import messagebox, scrolledtext
import json, serial, crcmod, time, struct
import pandas as pd
import openpyxl
from tkinter import filedialog


# ---------------------------------------------------------------------------------------------------
# Add custom
class CustomLabel(tk.Label):
    def __init__(self, master=None, **kwargs):
        super().__init__(master, **kwargs)
        self.num_of_var = 0
        self.flag = 0


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.checkboxes = []
        self.title('QA上位机服务调试软件 V1.0')
        #self.state('zoom')
        self.geometry("1200x630")
        self.rowconfigure([0, 1, 2, 3], minsize=20)
        self.counter = 0
        self.com_flag = 0
        self.read_state_flag = 0
        self.maxpage = 0
        self.dataflow_stop_flag = 0
        self.runtime_flag = 0
        self.maxwindow_falg = 0
        self.air_Num = 0
        self.recv_data_history = {}
        self.current_time_history = {}
        self.current_button = []
        self.buttenNum = 0
        self.data_all = []
        self.pagenum = 0
        self.window_show_dataflow = None
        self.labels = [None] * 300
        self.texts = [None] * 300
        self.label = [None] * 300
        self.Entry = [None] * 300
        self.entry_commom = [None] * 34
        self.entry_sys = [None] * 42
        self.entry_unit = [None] * 22
        self.entry_cycle1 = [None] * 31
        self.entry_cycle2 = [None] * 31
        self.entry_cycle3= [None] * 31
        self.entry_cycle4 = [None] * 31
        self.entry_cycle1_comp = [None] * 26
        self.entry_cycle2_comp = [None] * 26
        self.entry_cycle3_comp = [None] * 26
        self.entry_cycle4_comp = [None] * 26
        self.entry_cycle1_fan = [None] * 22
        self.entry_cycle2_fan = [None] * 22
        self.entry_cycle3_fan = [None] * 22
        self.entry_cycle4_fan = [None] * 22
        self.entry_cycle1_fan2 = [None] * 22
        self.entry_cycle2_fan2 = [None] * 22
        self.entry_cycle3_fan2 = [None] * 22
        self.entry_cycle4_fan2 = [None] * 22
        self.entry_cycle1_EXV = [None] * 28
        self.entry_cycle2_EXV = [None] * 28
        self.entry_cycle3_EXV = [None] * 28
        self.entry_cycle4_EXV = [None] * 28

        # 数据链接---------------------------------------------------------------------------
        self.sys_num = 34
        self.unit_num = 42
        self.cycle_num = 31
        self.comp_num = 22
        self.fan_num = 11
        self.exv_num = 25
        self.cycle_num_total = self.cycle_num + self.comp_num + self.fan_num * 2 + self.exv_num
        self.sys_start = 6
        self.unit_start = self.sys_start + self.sys_num * 2
        self.cycle_start = self.unit_start + self.unit_num * 2
        self.comp_start = self.cycle_start + self.cycle_num * 2
        self.fan1_start = self.comp_start + self.comp_num * 2
        self.fan2_start = self.fan1_start + self.fan_num * 2
        self.exv_start = self.fan2_start + self.fan_num * 2

        # Define connection settings

        try:
            with open('Defaut_settings.txt', 'r') as file:
                lines = file.readlines()
                self.df_com = lines[0].strip()
                self.df_buad= lines[1].strip()
                self.df_parity = lines[2].strip()
                self.df_scanrate = lines[3].strip()
        except FileNotFoundError:
            messagebox.showerror("Error", "Can't find file 'Defaut_settings.txt'")

        # Define first row
        self.F11=tk.Frame(master=self)
        self.F11.grid(column=0, row=0,sticky="w")
        self.F12 = tk.Frame(master=self)
        self.F12.grid(column=1, row=0)
        self.F111 = tk.Frame(self.F11)
        self.F111.grid(column=0, row=1,sticky="w")
        self.F112 = tk.Frame(self.F11)
        self.F112.grid(column=0, row=2,sticky="w",padx=5)
        self.F1121=tk.Frame(self.F112)
        self.F1121.grid(column=0, row=0, sticky="w", padx=5)
        self.F1122 = tk.Frame(self.F112)
        self.F1122.grid(column=1, row=0, sticky="n", padx=5)
        self.F1 = tk.Frame(self.F111)
        self.F1.grid(column=0, row=0, sticky="w", padx=5, pady=0)
        style_frame1_lable = ttk.Style()
        style_frame1_lable.configure("Custom1.TLabelframe.Label", foreground="black", background="orange")
        self.f1 = ttk.LabelFrame(self.F1, text="Communicate setting", style="Custom1.TLabelframe")
        self.f1.grid(column=0, row=0, sticky="w", padx=5, pady=5)
        # Place component on connection settiogs
        self.bt2 = tk.Button(self.f1, text='Connect', width=15, height=1, command=self.connect_stop)
        self.bt2.grid(column=3, row=1, padx=5)

        self.state = Label(self.f1, text='Not Connected')
        self.state.grid(column=3, row=2)
        self.port = Label(self.f1, text='Port')
        self.port_box = ttk.Combobox(self.f1, width=10, state="readonly")
        ports_list = list(serial.tools.list_ports.comports())
        port_values = [port.device for port in ports_list]
        self.port_box['values'] = port_values
        self.port_box.set(self.df_com)
        self.port.grid(column=0, row=0, sticky="w")
        self.port_box.grid(column=1, row=0, padx=10)

        self.buad = Label(self.f1, text='Buad')
        self.buad_box = ttk.Combobox(self.f1, width=10, state="readonly")
        self.buad_box['value'] = ('9600', '115200','38400')
        self.buad_box.set(self.df_buad)
        self.buad.grid(column=0, row=1, sticky="w")
        self.buad_box.grid(column=1, row=1, padx=10)

        self.parity = Label(self.f1, text='Parity')
        self.parity_box = ttk.Combobox(self.f1, width=10, state="readonly")
        self.parity_box['value'] = ('None', 'Odd', 'Even')
        self.parity_box.set(self.df_parity)
        self.parity.grid(column=0, row=2, sticky="w")
        self.parity_box.grid(column=1, row=2, padx=10)

        # Define data save settings
        style_frame2_lable = ttk.Style()
        style_frame2_lable.configure("Custom2.TLabelframe.Label", foreground="black", background="brown")
        self.f2 = ttk.LabelFrame(self.F1, text="Data settings", style="Custom2.TLabelframe")
        self.f2.grid(column=1, row=0, sticky="n", padx=5, pady=5)

        self.bt_save_data = tk.Button(self.f2, text='Save data', width=10, height=1, command=self.run_time)
        self.bt_save_data.grid(column=0, row=0, pady=0)

        self.lb_run_time = Label(self.f2, text='Run time')
        self.lb_run_time.grid(column=0, row=1, pady=1)

        self.entry_run_time = Entry(self.f2, width=11)
        self.entry_run_time.grid(column=0, row=2, pady=4, sticky="w")

        self.bt_add_mechine = tk.Button(self.f2, text='Save as default', width=13, height=1, command=self.save_default)
        self.bt_add_mechine.grid(column=1, row=1, padx=7)

        self.bt_communication = tk.Button(self.f2, text='Show dataflow', width=13, height=1, command=self.show_dataflow)
        self.bt_communication.grid(column=1, row=0, padx=7)

        self.bt1 = tk.Button(self.f2, text='Set Watch items', width=13, height=1,
                             command=self.show_watch_items)
        #self.bt1.grid(column=1, row=2, padx=5)

        self.f1_1 = tk.Frame(self.f1)
        self.f1_1.grid(column=3, row=0, padx=10, pady=4, sticky="n")

        self.entry_reflash = Entry(self.f1_1, width=5)
        self.entry_reflash.grid(column=1, row=0, padx=5, pady=4, sticky="w")

        self.lb_reflash = Label(self.f1_1, text='Scan rate(ms): ')
        self.lb_reflash.grid(column=0, row=0, pady=1)
        self.entry_reflash.insert(0, self.df_scanrate)


        self.checkbox_var=[None]*6
        for i in range(6):
            self.checkbox_var[i] = tk.BooleanVar(value=True)
            self.checkbox = tk.Checkbutton(self.F1122, text=self.labelname_checkbox(i), variable=self.checkbox_var[i])
            if i==0:
                self.checkbox.grid(row=i, column=0, sticky="w", pady=10)
            else:
                self.checkbox.grid(row=i, column=0, sticky="w", pady=5)

         # Version info
        self.brand = Label(self.F1122, text='JCH WX',font=20 ,foreground="blue")
        self.brand.grid(column=0, row=6, pady=10)
        self.Name = Label(self.F1122, text='QA_MONITOR', font=15, foreground="blue")
        self.Name.grid(column=0, row=7, pady=10)
        self.version = Label(self.F1122, text='Version 1.0',font=20,foreground="blue")
        self.version.grid(column=0, row=8, pady=10)

        style_Do_lable = ttk.Style()
        style_Do_lable.configure("Do.TLabelframe.Label", foreground="black", background="yellow")
        self.F112_0 = ttk.LabelFrame(self.F1121, text="DoParam", style="Do.TLabelframe")
        self.F112_0.grid(column=0, row=0, sticky="n", padx=5, pady=5)

        self.F112_1 = tk.Frame(self.F112_0)
        self.F112_1.grid(column=0, row=0)
        self.F112_2 = tk.Frame(self.F112_0)
        self.F112_2.grid(column=1, row=0)
        self.F112_3 = tk.Frame(self.F112_0)
        self.F112_3.grid(column=2, row=0)
        self.F112_4 = tk.Frame(self.F112_0)
        self.F112_4.grid(column=3, row=0)

        self.Stauts_Do1 = [None] * 32
        self.canvas_Do1 = tk.Canvas(self.F112_1, width=60, height=380)
        self.canvas_Do1.grid(column=0, row=0,)
        # 创建圆点和Label
        for i in range(16):
            # 绘制圆点
            x = 30
            radius = 8

            y = 20 + i * 23
            self.canvas_Do1.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                          fill="gray")
            self.Stauts_Do1[i] = ttk.Label(self.F112_2)
            self.Stauts_Do1[i].grid(column=1, row=i, pady=1, stick='w')

        for i in range(16):
            self.Stauts_Do1[i].config(text=self.labelname_Do(i))

        self.Stauts_Do2 = [None] * 32
        self.canvas_Do2 = tk.Canvas(self.F112_3, width=60, height=380)
        self.canvas_Do2.grid(column=0, row=0, )
        # 创建圆点和Label
        for i in range(16):
            # 绘制圆点
            x = 30
            radius = 8

            y = 20 + i * 23
            self.canvas_Do2.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                        fill="gray")
            self.Stauts_Do2[i] = ttk.Label(self.F112_4)
            self.Stauts_Do2[i].grid(column=1, row=i, pady=1, stick='w')

        for i in range(16):
            self.Stauts_Do2[i].config(text=self.labelname_Do(i+16))

        # Define second row--------Notebook
        self.F2 = tk.Frame(self.F12)
        self.F2.grid(column=0, row=2, sticky="w", padx=5, pady=0)

        self.notebook = ttk.Notebook(self.F2, width=610, height=600)
        self.notebook.grid(column=0, row=0, sticky="n", padx=5, pady=5)

        self.tab_common = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_common, text='SysParam')

        self.tab_sys = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_sys, text='UnitParam')

        self.tab_cycle1 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_cycle1, text='Cycle 1')

        self.cycle1_f1=ttk.Frame(self.tab_cycle1)
        self.cycle1_f1.grid(column=0, row=0)
        self.cycle1_f2 = ttk.Frame(self.tab_cycle1)
        self.cycle1_f2.grid(column=0, row=1)
        self.notebook_cycle1 = ttk.Notebook(self.cycle1_f2, width=590, height=300)
        self.notebook_cycle1.grid(column=0, row=0, sticky="n", padx=5, pady=5)
        self.tab_cycle1_comp = ttk.Frame(self.notebook_cycle1)
        self.notebook_cycle1.add(self.tab_cycle1_comp, text='Comp')
        self.tab_cycle1_fan = ttk.Frame(self.notebook_cycle1)
        self.notebook_cycle1.add(self.tab_cycle1_fan, text='Fan 1')
        self.tab_cycle1_fan2 = ttk.Frame(self.notebook_cycle1)
        self.notebook_cycle1.add(self.tab_cycle1_fan2, text='Fan 2')
        self.tab_cycle1_EXV = ttk.Frame(self.notebook_cycle1)
        self.notebook_cycle1.add(self.tab_cycle1_EXV, text='EXV')

        self.tab_cycle2 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_cycle2, text='Cycle 2')

        self.cycle2_f1 = ttk.Frame(self.tab_cycle2)
        self.cycle2_f1.grid(column=0, row=0)
        self.cycle2_f2 = ttk.Frame(self.tab_cycle2)
        self.cycle2_f2.grid(column=0, row=1)
        self.notebook_cycle2 = ttk.Notebook(self.cycle2_f2, width=590, height=300)
        self.notebook_cycle2.grid(column=0, row=0, sticky="n", padx=5, pady=5)
        self.tab_cycle2_comp = ttk.Frame(self.notebook_cycle2)
        self.notebook_cycle2.add(self.tab_cycle2_comp, text='Comp')
        self.tab_cycle2_fan = ttk.Frame(self.notebook_cycle2)
        self.notebook_cycle2.add(self.tab_cycle2_fan, text='Fan 1')
        self.tab_cycle2_fan2 = ttk.Frame(self.notebook_cycle2)
        self.notebook_cycle2.add(self.tab_cycle2_fan2, text='Fan 2')
        self.tab_cycle2_EXV = ttk.Frame(self.notebook_cycle2)
        self.notebook_cycle2.add(self.tab_cycle2_EXV, text='EXV')

        self.tab_cycle3 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_cycle3, text='Cycle 3')

        self.cycle3_f1 = ttk.Frame(self.tab_cycle3)
        self.cycle3_f1.grid(column=0, row=0)
        self.cycle3_f2 = ttk.Frame(self.tab_cycle3)
        self.cycle3_f2.grid(column=0, row=1)
        self.notebook_cycle3 = ttk.Notebook(self.cycle3_f2, width=590, height=300)
        self.notebook_cycle3.grid(column=0, row=0, sticky="n", padx=5, pady=5)
        self.tab_cycle3_comp = ttk.Frame(self.notebook_cycle3)
        self.notebook_cycle3.add(self.tab_cycle3_comp, text='Comp')
        self.tab_cycle3_fan = ttk.Frame(self.notebook_cycle3)
        self.notebook_cycle3.add(self.tab_cycle3_fan, text='Fan 1')
        self.tab_cycle3_fan2 = ttk.Frame(self.notebook_cycle3)
        self.notebook_cycle3.add(self.tab_cycle3_fan2, text='Fan 2')
        self.tab_cycle3_EXV = ttk.Frame(self.notebook_cycle3)
        self.notebook_cycle3.add(self.tab_cycle3_EXV, text='EXV')

        self.tab_cycle4 = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_cycle4, text='Cycle 4')

        self.cycle4_f1 = ttk.Frame(self.tab_cycle4)
        self.cycle4_f1.grid(column=0, row=0)
        self.cycle4_f2 = ttk.Frame(self.tab_cycle4)
        self.cycle4_f2.grid(column=0, row=1)
        self.notebook_cycle4 = ttk.Notebook(self.cycle4_f2, width=590, height=300)
        self.notebook_cycle4.grid(column=0, row=0, sticky="n", padx=5, pady=5)
        self.tab_cycle4_comp = ttk.Frame(self.notebook_cycle4)
        self.notebook_cycle4.add(self.tab_cycle4_comp, text='Comp')
        self.tab_cycle4_fan = ttk.Frame(self.notebook_cycle4)
        self.notebook_cycle4.add(self.tab_cycle4_fan, text='Fan 1')
        self.tab_cycle4_fan2 = ttk.Frame(self.notebook_cycle4)
        self.notebook_cycle4.add(self.tab_cycle4_fan2, text='Fan 2')
        self.tab_cycle4_EXV = ttk.Frame(self.notebook_cycle4)
        self.notebook_cycle4.add(self.tab_cycle4_EXV, text='EXV')
        # Define watch items
        style_frame4_lable = ttk.Style()
        style_frame4_lable.configure("Custom4.TLabelframe.Label", foreground="black", background="green")
        self.f4 = ttk.LabelFrame(self.F2, text="Data1", style="Custom4.TLabelframe")
        self.f4.grid(column=0, row=1, sticky="n", padx=5, pady=5)

        # Define third row
        self.F3 = tk.Frame(self.F111)
        self.F3.grid(column=0, row=1, sticky="w", padx=5, pady=0)

        # Define fault info module
        style_fault_label = ttk.Style()
        style_fault_label.configure("Fault.TLabelframe.Label", foreground="black", background="light green")
        self.f_fault = ttk.LabelFrame(self.F3, text="Change State", style="Fault.TLabelframe")
        self.f_fault.grid(column=3, row=3, sticky="n", padx=5)

        self.f_fault1 = tk.Frame(self.f_fault)
        self.f_fault1.grid(column=0, row=0)

        self.last_state = tk.Button(self.f_fault1, text='Last', height=1, command=self.Last_state)
        self.next_state = tk.Button(self.f_fault1, text='Next', height=1, command=self.Next_state)
        self.read_state = tk.Button(self.f_fault1, text='Read state', height=1, command=self.select_file)
        self.entry_showall = tk.Entry(self.f_fault1 , width=7)
        self.entry_showrows =tk.Entry(self.f_fault1, width=7)
        self.last_state.grid(column=2, row=0, padx=5)
        self.next_state.grid(column=3, row=0, padx=5)
        self.read_state.grid(column=4, row=0, padx=5)
        self.entry_showall.grid(column=6, row=0, padx=5)
        self.entry_showall.config(state=tk.DISABLED)
        self.entry_showrows.grid(column=5, row=0, padx=1)

        label_commom = tk.Label(self.f_fault1, text='当前系统时间')
        label_commom.grid(row=0, column=0, padx=3, pady=3, sticky="w")
        self.entry_time = tk.Entry(self.f_fault1, width=20)
        self.entry_time.grid(row=0, column=1, padx=3, pady=3, sticky="w")
        self.entry_time.config(state=tk.DISABLED)

        # label name
        for i in range(34):
            label_name = self.labelname_comm(i)
            label_commom = tk.Label(self.tab_common, text=label_name)
            label_commom.grid(row=i//3, column=(i%3)*2, padx=1, pady=1,sticky="w")
            self.entry_commom[i] = tk.Entry(self.tab_common, width=8)
            self.entry_commom[i].grid(row=i//3 , column=(i%3)*2+1, padx=1, pady=1)
            self.entry_commom[i].config(state=tk.DISABLED)

        for i in range(42):
            label_name = self.labelname_sys(i)
            label_sys = tk.Label(self.tab_sys, text=label_name)
            label_sys.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")
            # if (i == 24 or i==21 or i==34 ) :
            #     label_sys.config(font=("Arial", 7))
            self.entry_sys[i] = tk.Entry(self.tab_sys, width=8)
            self.entry_sys[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_sys[i].config(state=tk.DISABLED)

        for i in range(31):
            label_name = self.labelname_cycle(i)
            label_commom = tk.Label(self.cycle1_f1, text=label_name)
            label_commom.grid(row=i//3, column=(i%3)*2, padx=1, pady=1,sticky="w")
            self.entry_cycle1[i] = tk.Entry(self.cycle1_f1, width=8)
            self.entry_cycle1[i].grid(row=i//3 , column=(i%3)*2+1, padx=1, pady=1)
            self.entry_cycle1[i].config(state=tk.DISABLED)

        for i in range(31):
            label_name = self.labelname_cycle(i)
            label_commom = tk.Label(self.cycle2_f1, text=label_name)
            label_commom.grid(row=i//3, column=(i%3)*2, padx=1, pady=1,sticky="w")
            self.entry_cycle2[i] = tk.Entry(self.cycle2_f1, width=8)
            self.entry_cycle2[i].grid(row=i//3 , column=(i%3)*2+1, padx=1, pady=1)
            self.entry_cycle2[i].config(state=tk.DISABLED)

        for i in range(31):
            label_name = self.labelname_cycle(i)
            label_commom = tk.Label(self.cycle3_f1, text=label_name)
            label_commom.grid(row=i//3, column=(i%3)*2, padx=1, pady=1,sticky="w")
            self.entry_cycle3[i] = tk.Entry(self.cycle3_f1, width=8)
            self.entry_cycle3[i].grid(row=i//3 , column=(i%3)*2+1, padx=1, pady=1)
            self.entry_cycle3[i].config(state=tk.DISABLED)

        for i in range(31):
            label_name = self.labelname_cycle(i)
            label_commom = tk.Label(self.cycle4_f1, text=label_name)
            label_commom.grid(row=i//3, column=(i%3)*2, padx=1, pady=1,sticky="w")
            self.entry_cycle4[i] = tk.Entry(self.cycle4_f1, width=8)
            self.entry_cycle4[i].grid(row=i//3 , column=(i%3)*2+1, padx=1, pady=1)
            self.entry_cycle4[i].config(state=tk.DISABLED)

        # Cycle 1-------------------------------------------------------------------------------------------------------
        self.comp1_f1 = tk.Frame(self.tab_cycle1_comp)
        self.comp1_f1.grid(row=0, column=0)
        self.comp1_f2 = tk.Frame(self.tab_cycle1_comp)
        self.comp1_f2.grid(row=1, column=0)
        self.comp1_f3 = tk.Frame(self.tab_cycle1_comp)
        self.comp1_f3.grid(row=2, column=0, padx=2,sticky="w")
        for i in range(22):
            label_name = self.labelname_comp1(i)
            label_comp1 = tk.Label(self.comp1_f1, text=label_name)
            if ((i % 3) * 2)==0:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle1_comp[i] = tk.Entry(self.comp1_f1, width=8)
            self.entry_cycle1_comp[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle1_comp[i].config(state=tk.DISABLED)

        self.Stauts_labels = [None] * 32
        self.canvas_comp1 = tk.Canvas(self.comp1_f2, width=500, height=50)
        self.canvas_comp1.grid(column=0, row=0)
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8

            x = 43 + i * 90
            self.canvas_comp1.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
            self.Stauts_labels[i] = ttk.Label(self.comp1_f3)
        self.Stauts_labels[0].grid(column=0, row=0,padx=15 ,pady=1,stick='w')
        self.Stauts_labels[1].grid(column=1, row=0, padx=22, pady=1, stick='w')
        self.Stauts_labels[2].grid(column=2, row=0, padx=0, pady=1, stick='w')
        self.Stauts_labels[3].grid(column=3, row=0, padx=5, pady=1, stick='w')
        self.Stauts_labels[4].grid(column=4, row=0, padx=12, pady=1, stick='w')
        for i in range(5):
            self.Stauts_labels[i].config(text=self.labelname_sys1(i))

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle1_fan, text=label_name)
            if((i % 3) * 2)==0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle1_fan[i] = tk.Entry(self.tab_cycle1_fan, width=8)
            self.entry_cycle1_fan[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle1_fan[i].config(state=tk.DISABLED)

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle1_fan2, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle1_fan2[i] = tk.Entry(self.tab_cycle1_fan2, width=8)
            self.entry_cycle1_fan2[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle1_fan2[i].config(state=tk.DISABLED)

        for i in range(25):
            if i<9:
                label_name = self.labelname_EXVsub(i)
            else:
                label_name = self.labelname_EXVmain(i-9)
            label_EXV = tk.Label(self.tab_cycle1_EXV, text=label_name)
            if ((i % 3) * 2) == 0:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")
            self.entry_cycle1_EXV[i] = tk.Entry(self.tab_cycle1_EXV, width=8)
            self.entry_cycle1_EXV[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle1_EXV[i].config(state=tk.DISABLED)

        # Cycle 2-------------------------------------------------------------------------------------------------------
        self.comp2_f1 = tk.Frame(self.tab_cycle2_comp)
        self.comp2_f1.grid(row=0, column=0)
        self.comp2_f2 = tk.Frame(self.tab_cycle2_comp)
        self.comp2_f2.grid(row=1, column=0)
        self.comp2_f3 = tk.Frame(self.tab_cycle2_comp)
        self.comp2_f3.grid(row=2, column=0, padx=2, sticky="w")
        for i in range(22):
            label_name = self.labelname_comp1(i)
            label_comp1 = tk.Label(self.comp2_f1, text=label_name)
            if ((i % 3) * 2)==0:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle2_comp[i] = tk.Entry(self.comp2_f1, width=8)
            self.entry_cycle2_comp[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle2_comp[i].config(state=tk.DISABLED)

        self.Stauts_labels = [None] * 32
        self.canvas_comp2 = tk.Canvas(self.comp2_f2, width=500, height=50)
        self.canvas_comp2.grid(column=0, row=0)
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8

            x = 43 + i * 90
            self.canvas_comp2.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
            self.Stauts_labels[i] = ttk.Label(self.comp2_f3)
        self.Stauts_labels[0].grid(column=0, row=0,padx=15 ,pady=1,stick='w')
        self.Stauts_labels[1].grid(column=1, row=0, padx=22, pady=1, stick='w')
        self.Stauts_labels[2].grid(column=2, row=0, padx=0, pady=1, stick='w')
        self.Stauts_labels[3].grid(column=3, row=0, padx=5, pady=1, stick='w')
        self.Stauts_labels[4].grid(column=4, row=0, padx=12, pady=1, stick='w')
        for i in range(5):
            self.Stauts_labels[i].config(text=self.labelname_sys1(i))

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle2_fan, text=label_name)
            if((i % 3) * 2)==0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle2_fan[i] = tk.Entry(self.tab_cycle2_fan, width=8)
            self.entry_cycle2_fan[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle2_fan[i].config(state=tk.DISABLED)

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle2_fan2, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle2_fan2[i] = tk.Entry(self.tab_cycle2_fan2, width=8)
            self.entry_cycle2_fan2[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle2_fan2[i].config(state=tk.DISABLED)

        for i in range(25):
            if i<9:
                label_name = self.labelname_EXVsub(i)
            else:
                label_name = self.labelname_EXVmain(i-9)

            label_EXV = tk.Label(self.tab_cycle2_EXV, text=label_name)
            if ((i % 3) * 2) == 0:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")
            self.entry_cycle2_EXV[i] = tk.Entry(self.tab_cycle2_EXV, width=8)
            self.entry_cycle2_EXV[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle2_EXV[i].config(state=tk.DISABLED)

    #Cycle 3------------------------------------------------------------------------------------------------------------
        self.comp3_f1 = tk.Frame(self.tab_cycle3_comp)
        self.comp3_f1.grid(row=0, column=0)
        self.comp3_f2 = tk.Frame(self.tab_cycle3_comp)
        self.comp3_f2.grid(row=1, column=0)
        self.comp3_f3 = tk.Frame(self.tab_cycle3_comp)
        self.comp3_f3.grid(row=2, column=0, padx=2, sticky="w")
        for i in range(22):
            label_name = self.labelname_comp1(i)
            label_comp1 = tk.Label(self.comp3_f1, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle3_comp[i] = tk.Entry(self.comp3_f1, width=8)
            self.entry_cycle3_comp[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle3_comp[i].config(state=tk.DISABLED)

        self.Stauts_labels = [None] * 32
        self.canvas_comp3 = tk.Canvas(self.comp3_f2, width=500, height=50)
        self.canvas_comp3.grid(column=0, row=0)
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8

            x = 43 + i * 90
            self.canvas_comp3.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                          fill="gray")
            self.Stauts_labels[i] = ttk.Label(self.comp3_f3)
        self.Stauts_labels[0].grid(column=0, row=0, padx=15, pady=1, stick='w')
        self.Stauts_labels[1].grid(column=1, row=0, padx=22, pady=1, stick='w')
        self.Stauts_labels[2].grid(column=2, row=0, padx=0, pady=1, stick='w')
        self.Stauts_labels[3].grid(column=3, row=0, padx=5, pady=1, stick='w')
        self.Stauts_labels[4].grid(column=4, row=0, padx=12, pady=1, stick='w')
        for i in range(5):
            self.Stauts_labels[i].config(text=self.labelname_sys1(i))

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle3_fan, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle3_fan[i] = tk.Entry(self.tab_cycle3_fan, width=8)
            self.entry_cycle3_fan[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle3_fan[i].config(state=tk.DISABLED)

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle3_fan2, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle3_fan2[i] = tk.Entry(self.tab_cycle3_fan2, width=8)
            self.entry_cycle3_fan2[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle3_fan2[i].config(state=tk.DISABLED)

        for i in range(25):
            if i < 9:
                label_name = self.labelname_EXVsub(i)
            else:
                label_name = self.labelname_EXVmain(i - 9)
            label_EXV = tk.Label(self.tab_cycle3_EXV, text=label_name)
            if ((i % 3) * 2) == 0:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")
            self.entry_cycle3_EXV[i] = tk.Entry(self.tab_cycle3_EXV, width=8)
            self.entry_cycle3_EXV[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle3_EXV[i].config(state=tk.DISABLED)

        #Cycle 4--------------------------------------------------------------------------------------------------------
        self.comp4_f1 = tk.Frame(self.tab_cycle4_comp)
        self.comp4_f1.grid(row=0, column=0)
        self.comp4_f2 = tk.Frame(self.tab_cycle4_comp)
        self.comp4_f2.grid(row=1, column=0)
        self.comp4_f3 = tk.Frame(self.tab_cycle4_comp)
        self.comp4_f3.grid(row=2, column=0, padx=2, sticky="w")
        for i in range(22):
            label_name = self.labelname_comp1(i)
            label_comp1 = tk.Label(self.comp4_f1, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp1.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle4_comp[i] = tk.Entry(self.comp4_f1, width=8)
            self.entry_cycle4_comp[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle4_comp[i].config(state=tk.DISABLED)

        self.Stauts_labels = [None] * 32
        self.canvas_comp4 = tk.Canvas(self.comp4_f2, width=500, height=50)
        self.canvas_comp4.grid(column=0, row=0)
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8

            x = 43 + i * 90
            self.canvas_comp4.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                          fill="gray")
            self.Stauts_labels[i] = ttk.Label(self.comp4_f3)
        self.Stauts_labels[0].grid(column=0, row=0, padx=15, pady=1, stick='w')
        self.Stauts_labels[1].grid(column=1, row=0, padx=22, pady=1, stick='w')
        self.Stauts_labels[2].grid(column=2, row=0, padx=0, pady=1, stick='w')
        self.Stauts_labels[3].grid(column=3, row=0, padx=5, pady=1, stick='w')
        self.Stauts_labels[4].grid(column=4, row=0, padx=12, pady=1, stick='w')
        for i in range(5):
            self.Stauts_labels[i].config(text=self.labelname_sys1(i))

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle4_fan, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle4_fan[i] = tk.Entry(self.tab_cycle4_fan, width=8)
            self.entry_cycle4_fan[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle4_fan[i].config(state=tk.DISABLED)

        for i in range(11):
            label_name = self.labelname_fan(i)
            label_comp2 = tk.Label(self.tab_cycle4_fan2, text=label_name)
            if ((i % 3) * 2) == 0:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_comp2.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")

            self.entry_cycle4_fan2[i] = tk.Entry(self.tab_cycle4_fan2, width=8)
            self.entry_cycle4_fan2[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle4_fan2[i].config(state=tk.DISABLED)

        for i in range(25):
            if i < 9:
                label_name = self.labelname_EXVsub(i)
            else:
                label_name = self.labelname_EXVmain(i - 9)
            label_EXV = tk.Label(self.tab_cycle4_EXV, text=label_name)
            if ((i % 3) * 2) == 0:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=27, pady=1, sticky="w")
            else:
                label_EXV.grid(row=i // 3, column=(i % 3) * 2, padx=1, pady=1, sticky="w")
            self.entry_cycle4_EXV[i] = tk.Entry(self.tab_cycle4_EXV, width=8)
            self.entry_cycle4_EXV[i].grid(row=i // 3, column=(i % 3) * 2 + 1, padx=1, pady=1)
            self.entry_cycle4_EXV[i].config(state=tk.DISABLED)

    def connect_stop(self):

        if self.com_flag == 0:
            # 配置串口参数
            self.current_time_history = {}
            self.n = 0
            port = self.port_box.get()  # Serial
            baudrate = self.buad_box.get()  # Baudrate
            bytesize = serial.EIGHTBITS
            parity = serial.PARITY_ODD
            stopbits = serial.STOPBITS_ONE
            '''
            if self.bytesize_box.get() == 8:  # Bytesize
                bytesize = serial.EIGHTBITS
            if self.bytesize_box.get() == 7:
                bytesize = serial.SEVENBITS
            if self.bytesize_box.get() == 6:
                bytesize = serial.SIXBITS
            if self.bytesize_box.get() == 5:
                bytesize = serial.FIVEBITS
            '''
            if self.parity_box.get() == 'Odd':
                parity = serial.PARITY_ODD  # Parity
            if self.parity_box.get() == 'Even':
                parity = serial.PARITY_EVEN
            if self.parity_box.get() == 'None':
                parity = serial.PARITY_NONE
            '''
            if self.stopbit_box.get() == 1:
                stopbits = serial.STOPBITS_ONE  # Stopbits
            if self.stopbit_box.get() == 1.5:
                stopbits = serial.STOPBITS_ONE_POINT_FIVE  # Stopbits
            if self.stopbit_box.get() == 2:
                stopbits = serial.STOPBITS_TWO  # Stopbits
            '''

            self.ser = serial.Serial(port=port, baudrate=baudrate, bytesize=bytesize, parity=parity, stopbits=stopbits,
                                     timeout=0.3)
            self.ser.close()
            self.ser.open()
            if self.ser.is_open:
                self.bt2['text'] = 'Disconnect'
                self.com_flag = 1
                self.state.config(text='Connecting', foreground="green")

        else:
            self.ser.close()
            self.bt2['text'] = 'Connect'
            self.state.config(text='Not connected', foreground="black")
            self.com_flag = 0
        self.reflashcycle()

    def Last_state(self):
        pad = [0, 0, 0, 0, 0, 0]
        self.recv_data=[]
        if self.maxpage != 0 :
            if self.read_state_flag == 1:
                if self.com_flag == 1 or self.runtime_flag == 1:
                    messagebox.showerror("Error", "Please disconnect the connection and stop save before checking history state")
                elif self.pagenum == 1:
                    messagebox.showinfo("Error", "Already show the last data")
                else:
                    self.pagenum = self.pagenum - 1
                    self.recv_data_int = self.state_data.iloc[self.pagenum-1]
                    self.recv_data_time = str(self.recv_data_int.iloc[:1].values)
                    self.recv_data_time = self.recv_data_time[12:]
                    self.recv_data_int = self.recv_data_int.iloc[1:]
                    for i in self.recv_data_int:
                        # 使用struct.pack将整数转换为两个字节
                        bytes_ = struct.pack('H', int(i))
                        swapped_bytes = bytearray([bytes_[1], bytes_[0]])
                        # 将两个字节添加到字节数组中
                        self.recv_data += swapped_bytes
                    self.recv_data = pad + self.recv_data
                    self.entry_showrows.delete(0, END)
                    self.entry_showrows.insert(0, self.pagenum)
                    self.insert_data()
            else:
                messagebox.showerror("Error", "Please read data before change state")
        else:
            messagebox.showerror("Error", "No data had been record")

    def Next_state(self):
        pad = [0, 0, 0, 0, 0, 0]
        self.recv_data=[]
        if self.maxpage != 0:
            if self.read_state_flag == 1:
                if self.com_flag == 1  or self.runtime_flag == 1:
                    messagebox.showerror("Error", "Please disconnect the connection and stop save before checking history state")
                elif self.pagenum==self.maxpage:
                    messagebox.showinfo("Error", "Already the newest data")
                else:
                    self.pagenum=self.pagenum+1
                    self.recv_data_int = self.state_data.iloc[self.pagenum-1]
                    self.recv_data_time = str(self.recv_data_int.iloc[:1].values)
                    self.recv_data_time = self.recv_data_time[12:]
                    self.recv_data_int = self.recv_data_int.iloc[1:]
                    for i in self.recv_data_int:
                        # 使用struct.pack将整数转换为两个字节
                        bytes_ = struct.pack('H', int(i))
                        swapped_bytes = bytearray([bytes_[1], bytes_[0]])
                        # 将两个字节添加到字节数组中
                        self.recv_data += swapped_bytes
                    self.recv_data = pad + self.recv_data
                    self.entry_showrows.delete(0, END)
                    self.entry_showrows.insert(0, self.pagenum)
                    self.insert_data()
            else:
                messagebox.showerror("Error", "Please read data before change state")
        else:
            messagebox.showerror("Error", "No data had been record")

    def reflashcycle(self):
        temp1=[]
        self.read_state_flag = 0
        if self.com_flag == 1:
            slaveadd = 2
            startreg = 1999
            self.regnums = 476
            send_data = self.mmodbus03or04(slaveadd, startreg, self.regnums)
            self.ser.write(send_data)
            if self.window_show_dataflow is not None:
                if self.window_show_dataflow.winfo_exists() and self.dataflow_stop_flag == 0:
                    if self.var_showtimestamp.get() == 1:
                        self.current_time = datetime.now()
                        self.scroll_text.insert(tk.END, self.current_time)
                        self.scroll_text.insert(tk.END, "\n")
                    self.scroll_text.insert(tk.END, "Send data    : ")
                    self.scroll_text.insert(tk.END, self.insert_space(send_data.hex()))
                    self.scroll_text.insert(tk.END, "\n")
                    self.scroll_text.yview_moveto(1.0)

            self.recv_data = self.ser.read(self.regnums * 2 + 8)
            if len(self.recv_data) > 0:
                if self.window_show_dataflow is not None:
                    if self.window_show_dataflow.winfo_exists() and self.dataflow_stop_flag == 0:
                        if self.var_showtimestamp.get() == 1:
                            self.current_time = datetime.now()
                            self.scroll_text.insert(tk.END, self.current_time)
                            self.scroll_text.insert(tk.END, "\n")
                        self.scroll_text.insert(tk.END, "Recived data : ")
                        self.scroll_text.insert(tk.END, self.insert_space(self.recv_data.hex()))
                        self.scroll_text.insert(tk.END, "\n")
                        self.scroll_text.yview_moveto(1.0)

                if len(self.recv_data) > 50:
                    self.insert_data()

                    # for i in range(170):
                    #     if self.labels[i] is not None:
                    #         self.texts[i].config(state=tk.NORMAL)
                    #         self.texts[i].delete(0, END)
                    # # Brec=self.recv_data.to_bytes(regnums * 2 + 8, 'big')
                    # if self.labels[0] is not None:
                    #     self.current_time = datetime.now()
                    #     self.texts[0].insert(0, self.current_time)
                    # if self.texts[1] is not None:
                    #     self.texts[1].insert(0, self.recv_data[2])
                    # if self.texts[2] is not None:
                    #     self.texts[2].insert(0, self.recv_data[3])
                    # if self.texts[3] is not None:
                    #     self.texts[3].insert(0, self.recv_data[5])
                    # if self.texts[4] is not None:
                    #     self.texts[4].insert(0, self.int2float(self.recv_data[37], self.recv_data[38], self.recv_data[39],
                    #                                            self.recv_data[40]))
                    # if self.texts[5] is not None:
                    #     self.texts[5].insert(0, self.int2float(self.recv_data[41], self.recv_data[42], self.recv_data[43],
                    #                                            self.recv_data[44]))
                    # if self.texts[6] is not None:
                    #     self.texts[6].insert(0, self.int2float(self.recv_data[45], self.recv_data[46], self.recv_data[47],
                    #                                            self.recv_data[48]))
                    # if self.texts[7] is not None:
                    #     self.texts[7].insert(0, self.int2float(self.recv_data[49], self.recv_data[50], self.recv_data[51],
                    #                                            self.recv_data[52]))
                    # for i in range(170):
                    #     if self.labels[i] is not None:
                    #         self.texts[i].config(state=tk.DISABLED)

                    if self.runtime_flag == 1:
                        series = {}
                        self.end_time = time.time()
                        self.totle_time = round(self.end_time - self.start_time, 2)
                        self.entry_run_time.delete(0, END)
                        self.entry_run_time.insert(0, self.totle_time)
                        series[0] = pd.Series(self.current_time)
                        temp1.append(series[0][0])
                        for i in range(476):
                            series[i + 1] = pd.Series((self.recv_data[i * 2 + self.sys_start] << 8) | self.recv_data[
                    i * 2 + self.sys_start + 1])
                            temp1.append(series[i+1][0])
                        self.data_all.loc[len(self.data_all)] = temp1


            #loop
            self.reflash_rate = self.entry_reflash.get()
            self.after(self.reflash_rate, self.reflashcycle)  # Reflash rate
        else:
            return

    def show_watch_items(self):
        # 创建弹窗
        window = tk.Toplevel(self)
        window.title("Set Watch items")
        self.checkboxes = []
        pl = []
        fix=0
        notebookw = ttk.Notebook(window)
        notebookw.grid(column=0, row=0, sticky="n", padx=5, pady=5)

        tab_commonw = ttk.Frame(notebookw)
        notebookw.add(tab_commonw, text='Common')

        tab_sysw = ttk.Frame(notebookw)
        notebookw.add(tab_sysw, text='System contrl')

        tab_unitw = ttk.Frame(notebookw)
        notebookw.add(tab_unitw, text='Unit contrl')

        tab_cyclew = ttk.Frame(notebookw)
        notebookw.add(tab_cyclew, text='Cycle contrl')

        tab_comp1w = ttk.Frame(notebookw)
        notebookw.add(tab_comp1w, text='Comp power')

        tab_comp2w = ttk.Frame(notebookw)
        notebookw.add(tab_comp2w, text='Comp contrl')

        tab_exvw = ttk.Frame(notebookw)
        notebookw.add(tab_exvw, text='EXV1/2/3')

        tab_fanw = ttk.Frame(notebookw)
        notebookw.add(tab_fanw, text='Comp fan')

        for i in range(170):
            if i<13:
                pl=tab_commonw
                text_name = self.labelname_comm(i + 1)
            elif i>12 and i<30:
                fix=-1
                pl=tab_sysw
                text_name = self.labelname_sys(i + 1-13)
            elif i>29 and i<52:
                pl=tab_unitw
                fix=0
                text_name = self.labelname_unit(i + 1-30)
            elif i > 51 and i < 83:
                pl=tab_cyclew
                fix=-1
                text_name = self.labelname_cycle(i + 1-52)
            elif i > 82 and i < 109:
                pl=tab_comp1w
                fix=-2
                text_name = self.labelname_comp1(i + 1-83)
            elif i > 108 and i < 131:
                pl=tab_comp2w
                fix=-1
                text_name = self.labelname_comp2(i + 1-109)
            elif i > 130 and i < 159:
                pl=tab_exvw
                fix=-2
                text_name = self.labelname_exv(i + 1-131)
            elif i > 158 and i < 170:
                pl = tab_fanw
                fix=0
                text_name = self.labelname_fan(i + 1-159)
            var = tk.BooleanVar()
            checkbox = tk.Checkbutton(pl, text=text_name, variable=var)
            checkbox.grid(row=(i+fix)//3 ,column=(i+fix)% 3, sticky="w",padx=20)
            checkbox.var = var
            checkbox.var.set(self.labels[i] is not None)
            self.checkboxes.append(checkbox)
        self.load_history()

        # 定义确认按钮的点击事件
        def confirm():
            #Custom_window = tk.Toplevel(self)
            #Custom_window.title("Custom watch window")
            for j in range(2):  # adjust format (???)
                self.counter = 0
                for i, checkbox in enumerate(self.checkboxes):
                    var = checkbox.var
                    if var.get():
                        if self.labels[i] is None:
                            if i < 13:
                                label_name = self.labelname_comm(i + 1)
                            elif i > 12 and i < 30:
                                label_name = self.labelname_sys(i + 1 - 13)
                            elif i > 29 and i < 52:
                                label_name = self.labelname_unit(i + 1 - 30)
                            elif i > 51 and i < 83:
                                label_name = self.labelname_cycle(i + 1 - 52)
                            elif i > 82 and i < 109:
                                label_name = self.labelname_comp1(i + 1 - 83)
                            elif i > 108 and i < 131:
                                label_name = self.labelname_comp2(i + 1 - 109)
                            elif i > 130 and i < 159:
                                label_name = self.labelname_exv(i + 1 - 131)
                            elif i > 158 and i < 170:
                                label_name = self.labelname_fan(i + 1 - 159)
                            self.labels[i] = CustomLabel(self.tab_custom, text=label_name, fg='black')
                            self.labels[i].num_of_var = self.counter
                            self.texts[i] = tk.Entry(self.tab_custom, width=8)
                            self.texts[i].insert(0, 0)
                            self.texts[i].config(state=tk.DISABLED)
                            self.counter += 2

                            self.labels[i].grid(row=self.labels[i].num_of_var // 6,
                                                column=self.labels[i].num_of_var % 6, padx=10, sticky="w")
                            self.texts[i].grid(row=self.labels[i].num_of_var // 6, padx=5, column=self.
                                               labels[i].num_of_var % 6 + 1)

                        if self.labels[i] is not None:
                            self.labels[i].num_of_var = self.counter
                            self.counter += 2
                            self.labels[i].grid(row=self.labels[i].num_of_var // 6,
                                                column=self.labels[i].num_of_var % 6, padx=10, sticky="w")
                            self.texts[i].grid(row=self.labels[i].num_of_var // 6,
                                               column=self.labels[i].num_of_var % 6 + 1)

                    else:
                        if self.labels[i] is not None:
                            self.labels[i].grid_forget()
                            self.labels[i] = None
                            self.texts[i].grid_forget()
                            self.texts[i] = None

            self.save_history()
            window.destroy()

        confirm_button = tk.Button(window, text="Confirm", width=10, command=confirm)
        confirm_button.grid(row=1, column=0, pady=10,sticky="n")

    def save_history(self):
        history = [checkbox.var.get() for checkbox in self.checkboxes]
        with open("history.json", "w") as file:
            json.dump(history, file)

    def load_history(self):
        try:
            with open("history.json", "r") as file:
                history = json.load(file)
                for i, checkbox in enumerate(self.checkboxes):
                    checkbox.var.set(history[i])
        except FileNotFoundError:
            pass

    def save_history_data(self):
        history_data = self.scroll_text.get(1.0, END)
        with open("history_data.json", "w") as file:
            json.dump(history_data, file, indent=4)

    def insert_data(self):
        Do_result = (self.recv_data[130] << 8) | self.recv_data[131]
        Do_result = self.decompose_binary(int(Do_result))
        for i in range(16):
            # 绘制圆点
            x = 30
            radius = 8
            y = 20 + i * 23
            if Do_result[i] == 1:
                self.canvas_Do1.create_oval(x - radius, y - radius, x + radius, y + radius,
                                            outline="SystemButtonFace",
                                            fill="green")
            else:
                self.canvas_Do1.create_oval(x - radius, y - radius, x + radius, y + radius,
                                            outline="SystemButtonFace",
                                            fill="gray")


        Do_result = (self.recv_data[128] << 8) | self.recv_data[129]
        Do_result = self.decompose_binary(int(Do_result))
        for i in range(16):
            # 绘制圆点
            x = 30
            radius = 8
            y = 20 + i * 23
            if Do_result[i] == 1:
                self.canvas_Do2.create_oval(x - radius, y - radius, x + radius, y + radius,
                                            outline="SystemButtonFace",
                                            fill="green")
            else:
                self.canvas_Do2.create_oval(x - radius, y - radius, x + radius, y + radius,
                                            outline="SystemButtonFace",
                                            fill="gray")

        comp_result = (self.recv_data[262] << 8) | self.recv_data[263]
        comp_result = self.decompose_binary(int(comp_result))
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8
            x = 43 + i * 90
            if comp_result[i]==1:
                self.canvas_comp1.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="green")
            else:
                self.canvas_comp1.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                              fill="gray")

        comp_result = (self.recv_data[462] << 8) | self.recv_data[463]
        comp_result = self.decompose_binary(int(comp_result))
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8
            x = 43 + i * 90
            if comp_result[i] == 1:
                self.canvas_comp2.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="green")
            else:
                self.canvas_comp2.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="gray")

        comp_result = (self.recv_data[662] << 8) | self.recv_data[663]
        comp_result = self.decompose_binary(int(comp_result))
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8
            x = 43 + i * 90
            if comp_result[i] == 1:
                self.canvas_comp3.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="green")
            else:
                self.canvas_comp3.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="gray")

        comp_result = (self.recv_data[862] << 8) | self.recv_data[863]
        comp_result = self.decompose_binary(int(comp_result))
        for i in range(5):
            # 绘制圆点
            y = 30
            radius = 8
            x = 43 + i * 90
            if comp_result[i] == 1:
                self.canvas_comp4.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="green")
            else:
                self.canvas_comp4.create_oval(x - radius, y - radius, x + radius, y + radius,
                                              outline="SystemButtonFace",
                                              fill="gray")

        if self.read_state_flag == 0:
            self.current_time = datetime.now()
            self.entry_time.config(state=tk.NORMAL)
            self.entry_time.delete(0, END)
            self.entry_time.insert(0, self.current_time)
            self.entry_time.config(state=tk.DISABLED)
        else:
            self.current_time = self.recv_data_time
            self.entry_time.config(state=tk.NORMAL)
            self.entry_time.delete(0, END)
            self.entry_time.insert(0, self.current_time)
            self.entry_time.config(state=tk.DISABLED)

        if self.checkbox_var[0].get():
            for i in range(self.sys_num):
                self.entry_commom[i].config(state=tk.NORMAL)
                self.entry_commom[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.sys_start] << 8) | self.recv_data[
                    i * 2 + self.sys_start + 1]
                self.entry_commom[i].insert(0, result)
                self.entry_commom[i].config(state=tk.DISABLED)

        if self.checkbox_var[1].get():
            for i in range(self.unit_num):
                self.entry_sys[i].config(state=tk.NORMAL)
                self.entry_sys[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.unit_start] << 8) | self.recv_data[
                    i * 2 + self.unit_start + 1]
                self.entry_sys[i].insert(0, result)
                self.entry_sys[i].config(state=tk.DISABLED)

        # Cycle 1-------------------------------------------------------------------------------------------
        if self.checkbox_var[2].get():
            for i in range(self.cycle_num):
                self.entry_cycle1[i].config(state=tk.NORMAL)
                self.entry_cycle1[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.cycle_start] << 8) | self.recv_data[
                    i * 2 + self.cycle_start + 1]
                self.entry_cycle1[i].insert(0, result)
                self.entry_cycle1[i].config(state=tk.DISABLED)

            for i in range(self.comp_num):
                self.entry_cycle1_comp[i].config(state=tk.NORMAL)
                self.entry_cycle1_comp[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.comp_start] << 8) | self.recv_data[
                    i * 2 + self.comp_start + 1]
                self.entry_cycle1_comp[i].insert(0, result)
                self.entry_cycle1_comp[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle1_fan[i].config(state=tk.NORMAL)
                self.entry_cycle1_fan[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan1_start] << 8) | self.recv_data[
                    i * 2 + self.fan1_start + 1]
                self.entry_cycle1_fan[i].insert(0, result)
                self.entry_cycle1_fan[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle1_fan2[i].config(state=tk.NORMAL)
                self.entry_cycle1_fan2[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan2_start] << 8) | self.recv_data[
                    i * 2 + self.fan2_start + 1]
                self.entry_cycle1_fan2[i].insert(0, result)
                self.entry_cycle1_fan2[i].config(state=tk.DISABLED)

            for i in range(self.exv_num):
                self.entry_cycle1_EXV[i].config(state=tk.NORMAL)
                self.entry_cycle1_EXV[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.exv_start] << 8) | self.recv_data[
                    i * 2 + self.exv_start + 1]
                self.entry_cycle1_EXV[i].insert(0, result)
                self.entry_cycle1_EXV[i].config(state=tk.DISABLED)

        # Cycle 2-------------------------------------------------------------------------------------------
        if self.checkbox_var[3].get():
            for i in range(self.cycle_num):
                self.entry_cycle2[i].config(state=tk.NORMAL)
                self.entry_cycle2[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.cycle_start + self.cycle_num_total * 2] << 8) | \
                         self.recv_data[
                             i * 2 + self.cycle_start + self.cycle_num_total * 2 + 1]
                self.entry_cycle2[i].insert(0, result)
                self.entry_cycle2[i].config(state=tk.DISABLED)

            for i in range(self.comp_num):
                self.entry_cycle2_comp[i].config(state=tk.NORMAL)
                self.entry_cycle2_comp[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.comp_start + self.cycle_num_total * 2] << 8) | \
                         self.recv_data[
                             i * 2 + self.comp_start + 1 + self.cycle_num_total * 2]
                self.entry_cycle2_comp[i].insert(0, result)
                self.entry_cycle2_comp[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle2_fan[i].config(state=tk.NORMAL)
                self.entry_cycle2_fan[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan1_start + self.cycle_num_total * 2] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan1_start + 1 + self.cycle_num_total * 2]
                self.entry_cycle2_fan[i].insert(0, result)
                self.entry_cycle2_fan[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle2_fan2[i].config(state=tk.NORMAL)
                self.entry_cycle2_fan2[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan2_start + self.cycle_num_total * 2] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan2_start + 1 + self.cycle_num_total * 2]
                self.entry_cycle2_fan2[i].insert(0, result)
                self.entry_cycle2_fan2[i].config(state=tk.DISABLED)

            for i in range(self.exv_num):
                self.entry_cycle2_EXV[i].config(state=tk.NORMAL)
                self.entry_cycle2_EXV[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.exv_start + self.cycle_num_total * 2] << 8) | \
                         self.recv_data[
                             i * 2 + self.exv_start + 1 + self.cycle_num_total * 2]
                self.entry_cycle2_EXV[i].insert(0, result)
                self.entry_cycle2_EXV[i].config(state=tk.DISABLED)

        # Cycle 3-------------------------------------------------------------------------------------------
        if self.checkbox_var[4].get():
            for i in range(self.cycle_num):
                self.entry_cycle3[i].config(state=tk.NORMAL)
                self.entry_cycle3[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.cycle_start + self.cycle_num_total * 4] << 8) | \
                         self.recv_data[
                             i * 2 + self.cycle_start + 1 + self.cycle_num_total * 4]
                self.entry_cycle3[i].insert(0, result)
                self.entry_cycle3[i].config(state=tk.DISABLED)

            for i in range(self.comp_num):
                self.entry_cycle3_comp[i].config(state=tk.NORMAL)
                self.entry_cycle3_comp[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.comp_start + self.cycle_num_total * 4] << 8) | \
                         self.recv_data[
                             i * 2 + self.comp_start + 1 + self.cycle_num_total * 4]
                self.entry_cycle3_comp[i].insert(0, result)
                self.entry_cycle3_comp[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle3_fan[i].config(state=tk.NORMAL)
                self.entry_cycle3_fan[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan1_start + self.cycle_num_total * 4] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan1_start + 1 + self.cycle_num_total * 4]
                self.entry_cycle3_fan[i].insert(0, result)
                self.entry_cycle3_fan[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle3_fan2[i].config(state=tk.NORMAL)
                self.entry_cycle3_fan2[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan2_start + self.cycle_num_total * 4] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan2_start + 1 + self.cycle_num_total * 4]
                self.entry_cycle3_fan2[i].insert(0, result)
                self.entry_cycle3_fan2[i].config(state=tk.DISABLED)

            for i in range(self.exv_num):
                self.entry_cycle3_EXV[i].config(state=tk.NORMAL)
                self.entry_cycle3_EXV[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.exv_start + self.cycle_num_total * 4] << 8) | \
                         self.recv_data[
                             i * 2 + self.exv_start + 1 + self.cycle_num_total * 4]
                self.entry_cycle3_EXV[i].insert(0, result)
                self.entry_cycle3_EXV[i].config(state=tk.DISABLED)

            # Cycle 4-------------------------------------------------------------------------------------------
        if self.checkbox_var[5].get():
            for i in range(self.cycle_num):
                self.entry_cycle4[i].config(state=tk.NORMAL)
                self.entry_cycle4[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.cycle_start + self.cycle_num_total * 6] << 8) | \
                         self.recv_data[
                             i * 2 + self.cycle_start + 1 + self.cycle_num_total * 6]
                self.entry_cycle4[i].insert(0, result)
                self.entry_cycle4[i].config(state=tk.DISABLED)

            for i in range(self.comp_num):
                self.entry_cycle4_comp[i].config(state=tk.NORMAL)
                self.entry_cycle4_comp[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.comp_start + self.cycle_num_total * 6] << 8) | \
                         self.recv_data[
                             i * 2 + self.comp_start + 1 + self.cycle_num_total * 6]
                self.entry_cycle4_comp[i].insert(0, result)
                self.entry_cycle4_comp[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle4_fan[i].config(state=tk.NORMAL)
                self.entry_cycle4_fan[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan1_start + self.cycle_num_total * 6] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan1_start + 1 + self.cycle_num_total * 6]
                self.entry_cycle4_fan[i].insert(0, result)
                self.entry_cycle4_fan[i].config(state=tk.DISABLED)

            for i in range(self.fan_num):
                self.entry_cycle4_fan2[i].config(state=tk.NORMAL)
                self.entry_cycle4_fan2[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.fan2_start + self.cycle_num_total * 6] << 8) | \
                         self.recv_data[
                             i * 2 + self.fan2_start + 1 + self.cycle_num_total * 6]
                self.entry_cycle4_fan2[i].insert(0, result)
                self.entry_cycle4_fan2[i].config(state=tk.DISABLED)

            for i in range(self.exv_num):
                self.entry_cycle4_EXV[i].config(state=tk.NORMAL)
                self.entry_cycle4_EXV[i].delete(0, END)
                result = (self.recv_data[i * 2 + self.exv_start + self.cycle_num_total * 6] << 8) | \
                         self.recv_data[
                             i * 2 + self.exv_start + 1 + self.cycle_num_total * 6]
                self.entry_cycle4_EXV[i].insert(0, result)
                self.entry_cycle4_EXV[i].config(state=tk.DISABLED)

    def save_default(self):
        self.df_com = self.port_box.get()
        self.df_buad = self.buad_box.get()
        self.df_parity = self.parity_box.get()
        self.df_scanrate = self.entry_reflash.get()
        try:
            with open('Defaut_settings.txt', 'w') as file:
                file.write(f"{self.df_com }\n{self.df_buad}\n{self.df_parity}\n{self.df_scanrate}")
        except FileNotFoundError:
            messagebox.showerror("Error", "Can't find file 'Defaut_settings.txt'")

    def crc16(self, verify_data):
        if not verify_data:
            return
        crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)
        return crc16(verify_data)

    def checkcrc(self, data):
        if not data:
            return False
        if len(data) <= 2:
            return False
        nocrcdata = data[:-2]
        oldcrc16 = data[-2:]
        oldcrclist = list(oldcrc16)
        crcres = self.crc16(nocrcdata)
        crc16byts = crcres.to_bytes(2, byteorder="little", signed=False)
        # print("CRC16:", crc16byts.hex())
        crclist = list(crc16byts)
        if oldcrclist[0] != crclist[0] or oldcrclist[1] != crclist[1]:
            return False
        return True

    # Modbus-RTU协议的03或04读取保存或输入寄存器功能主-》从命令帧
    def mmodbus03or04(self, add, startregadd, regnum, funcode=38):
        if add < 0 or add > 0xFF or startregadd < 0 or startregadd > 0xFFFF or regnum < 1 or regnum > 476:
            print("Error: parameter error")
            return
        if funcode != 38:
            print("Error: parameter error")
            return
        sendbytes = add.to_bytes(1, byteorder="big", signed=False)
        sendbytes = sendbytes + funcode.to_bytes(1, byteorder="big", signed=False) + startregadd.to_bytes(2,
                                                                                                          byteorder="big",
                                                                                                          signed=False) + \
                    regnum.to_bytes(4, byteorder="big", signed=False)
        crcres = self.crc16(sendbytes)
        crc16bytes = crcres.to_bytes(2, byteorder="little", signed=False)
        sendbytes = sendbytes + crc16bytes
        return sendbytes

    def insert_space(self, hex_string):
        return ' '.join(hex_string[i:i + 2] for i in range(0, len(hex_string), 2))

    def set_air_num(self, button):
        if self.current_button is not None:
            self.current_button.config(relief=tk.RAISED)
        button.config(relief=tk.SUNKEN)
        self.current_button = button

    def int2float(self, num1, num2, num3, num4):
        binary_str = bin(num3)[2:].zfill(8) + bin(num4)[2:].zfill(8) + bin(num1)[2:].zfill(8) + bin(num2)[2:].zfill(8)
        float_data = struct.unpack('!f', bytes.fromhex(hex(int(binary_str, 2))[2:].zfill(8)))[0]
        return float_data

    def int2string(self, num1, num2, num3, num4):
        binary_str = bin(num3)[2:].zfill(8) + bin(num4)[2:].zfill(8) + bin(num1)[2:].zfill(8) + bin(num2)[2:].zfill(8)
        binary_str = int(binary_str, 2).to_bytes(4, 'big')
        char_data = binary_str.decode('utf-8')
        print(char_data)
        return char_data

    def run_time(self):
        if self.com_flag == 1:
            if self.runtime_flag == 0:
                self.bt_save_data['text'] = 'Stop'
                self.start_time = time.time()
                self.runtime_flag = 1
                self.data_all = pd.DataFrame({
                    "System_time":[],
                    "Date_time": [], "Mode_run": [], "Switch_state": [], "run_state": [], "Run_time": [],
                    "Unit_counter": [], "Capacity_meet": [], "Tw_sys_in": [], "Tw_sys_out_sensor": [],
                    "Tw_sys_out_fix": [], "Tw_sys_out": [], "Tw": [], "Tw_set": [], "DTw": [], "sys_alarm_state_h": [],
                    "sys_alarm_state_l": [], "Dcor": [], "time_set": [], "cap_period": [], "cap_N": [],
                    "cap_cool_delta_Ts": [], "cap_heat_delta_Ts": [], "delta_Tw_h_set": [], "Max_T_Tw_in": [],
                    "Max_T_Tw_out": [], "cap_delta_Tw": [], "cap_PI": [], "F_total_rating": [], "delta_F_total": [],
                    "delta_F_up_total": [], "delta_F_down_total": [], "antifreeze_run_time": [],
                    "antifreeze_stop_time": [], "reserved1": [],
                    "series_type": [], "tier_type": [], "capacity": [], "refri_type": [], "pump_type": [],
                    "power_on_restart": [], "esp_type": [], "main_romNo": [], "sub_romNo": [], "address": [],
                    "work_type": [], "cycle_counter": [], "switch_set": [], "switch_rt": [], "Tw_in_sensor": [],
                    "Tw_in_fix": [], "Tw_in": [], "Tw_out_sensor": [], "Tw_out_fix": [], "Tw_out": [], "Tams": [],
                    "capacity_percent": [],"pump_current_run_time": [],"pump_stop_time": [],"chassis_heat_run_time": [],
                    "chassis_heat_stop_time": [],"Ethtime": [],"main_DO_state_h": [],"main_DO_state_l": [],
                    "sub_DO_state_h": [],"sub_DO_state_l": [],"alarm_state_h": [], "alarm_state_l": [],
                    "cap_cycle_opt_cnt": [],"cap_cycle_run_cnt": [], "cap_cycle_close_able_cnt": [], "cap_Fi_rating": [],
                    "cap_Fi_run": [], "cap_delta_Fi_up": [], "cap_delta_Fi_down": [],"cap_freq_set": [], "reserved": [],


                    #Cycle 1
                    "Cycle 1 address": [], "Cycle 1 power_on_time": [], "Cycle 1 total_run_time": [], "Cycle 1 Td": [],
                    "Cycle 1 Ts": [], "Cycle 1 Tf": [], "Cycle 1 Tcoil": [], "Cycle 1 Tsub_in": [],
                    "Cycle 1 Tsub_out": [], "Cycle 1 Pd": [], "Cycle 1 Ps": [], "Cycle 1 Tc": [], "Cycle 1 Tevp": [],
                    "Cycle 1 Iinv1": [], "Cycle 1 Iinv2": [], "Cycle 1 SH_Td": [], "Cycle 1 SH_TS_act": [],
                    "Cycle 1 SH_TS_Target": [], "Cycle 1 SH_sub": [], "Cycle 1 DO_state": [],
                    "Cycle 1 defrost_type": [], "Cycle 1 cool_total_run_time": [], "Cycle 1 heat_total_run_time": [],
                    "Cycle 1 defrost_run_time": [], "Cycle 1 oil_acc_run_time": [], "Cycle 1 oil_current_run_time": [],
                    "Cycle 1 alarm_state_h": [], "Cycle 1 alarm_state_l": [], "Cycle 1 cap_delta_Fi_up": [],
                    "Cycle 1 cap_delta_Fi_down": [], "Cycle 1 cap_freq_set": [], "Cycle 1 comp romNo": [],
                    "Cycle 1 type": [], "Cycle 1 current_run_time": [], "Cycle 1 freq_cool_rate": [],
                    "Cycle 1 freq_heat_rate": [], "Cycle 1 freq_target": [], "Cycle 1 freq_rt": [],
                    "Cycle 1 freq_step": [], "Cycle 1 freq_convert": [], "Cycle 1 Fimax": [], "Cycle 1 Fimax0": [],
                    "Cycle 1 Fimax1": [], "Cycle 1 Fimax2": [], "Cycle 1 Fimax3": [], "Cycle 1 Fimax4": [],
                    "Cycle 1 Fimin": [], "Cycle 1 Fimin1": [], "Cycle 1 Fimin2": [], "Cycle 1 Fimin3": [],
                    "Cycle 1 prot_src": [], "Cycle 1 prot_state": [], "Cycle 1 retreat": [], "Cycle 1f1 romNo": [],
                    "Cycle 1f1 adjust_period": [], "Cycle 1f1 fan_speed": [], "Cycle 1f1 Fomax": [], "Cycle 1f1 Fomin": [],
                    "Cycle 1f1 state_A": [], "Cycle 1f1 rpm_set": [], "Cycle 1f1 rpm_rt": [], "Cycle 1f1 Pdset": [],
                    "Cycle 1f1 Psfo": [], "Cycle 1f1 fan_I": [], "Cycle 1f2 romNo": [], "Cycle 1f2 adjust_period": [],
                    "Cycle 1f2 fan_speed": [], "Cycle 1f2 Fomax": [], "Cycle 1f2 Fomin": [], "Cycle 1f2 state_A": [],
                    "Cycle 1f2 rpm_set": [], "Cycle 1f2 rpm_rt": [], "Cycle 1f2 Pdset": [], "Cycle 1f2 Psfo": [],
                    "Cycle 1f2 fan_I": [], "Cycle 1 sub_adjust_period": [], "Cycle 1 sub_adjust_speed": [],
                    "Cycle 1 sub_rt_step": [], "Cycle 1 sub_delta_OP": [], "Cycle 1 sub_delta_OP_fix": [],
                    "Cycle 1 sub_OP_N": [], "Cycle 1 sub_OP_X": [], "Cycle 1 sub_OP_Y": [], "Cycle 1 sub_OP_Z": [],
                    "Cycle 1 adjust_period": [], "Cycle 1 adjust_speed": [], "Cycle 1 exv1_rt_step": [],
                    "Cycle 1 exv2_rt_step": [], "Cycle 1 OP": [], "Cycle 1 delta_OP": [], "Cycle 1 delta_OP_fix": [],
                    "Cycle 1 delta_OP1": [], "Cycle 1 delta_OP1_fix": [], "Cycle 1 OP1_N": [], "Cycle 1 OP1_X": [],
                    "Cycle 1 OP1_Y": [], "Cycle 1 delta_OP2": [], "Cycle 1 delta_OP2_fix": [], "Cycle 1 delta_OP3": [],
                    "Cycle 1 delta_OP3_fix": [],

                    # Cycle 2
                    "Cycle 2 address": [], "Cycle 2 power_on_time": [], "Cycle 2 total_run_time": [], "Cycle 2 Td": [],
                    "Cycle 2 Ts": [], "Cycle 2 Tf": [], "Cycle 2 Tcoil": [], "Cycle 2 Tsub_in": [],
                    "Cycle 2 Tsub_out": [], "Cycle 2 Pd": [], "Cycle 2 Ps": [], "Cycle 2 Tc": [], "Cycle 2 Tevp": [],
                    "Cycle 2 Iinv1": [], "Cycle 2 Iinv2": [], "Cycle 2 SH_Td": [], "Cycle 2 SH_TS_act": [],
                    "Cycle 2 SH_TS_Target": [], "Cycle 2 SH_sub": [], "Cycle 2 DO_state": [],
                    "Cycle 2 defrost_type": [], "Cycle 2 cool_total_run_time": [], "Cycle 2 heat_total_run_time": [],
                    "Cycle 2 defrost_run_time": [], "Cycle 2 oil_acc_run_time": [], "Cycle 2 oil_current_run_time": [],
                    "Cycle 2 alarm_state_h": [], "Cycle 2 alarm_state_l": [], "Cycle 2 cap_delta_Fi_up": [],
                    "Cycle 2 cap_delta_Fi_down": [], "Cycle 2 cap_freq_set": [], "Cycle 2 comp romNo": [],
                    "Cycle 2 type": [], "Cycle 2 current_run_time": [], "Cycle 2 freq_cool_rate": [],
                    "Cycle 2 freq_heat_rate": [], "Cycle 2 freq_target": [], "Cycle 2 freq_rt": [],
                    "Cycle 2 freq_step": [], "Cycle 2 freq_convert": [], "Cycle 2 Fimax": [], "Cycle 2 Fimax0": [],
                    "Cycle 2 Fimax1": [], "Cycle 2 Fimax2": [], "Cycle 2 Fimax3": [], "Cycle 2 Fimax4": [],
                    "Cycle 2 Fimin": [], "Cycle 2 Fimin1": [], "Cycle 2 Fimin2": [], "Cycle 2 Fimin3": [],
                    "Cycle 2 prot_src": [], "Cycle 2 prot_state": [], "Cycle 2 retreat": [], "Cycle 2f1 romNo": [],
                    "Cycle 2f1 adjust_period": [], "Cycle 2f1 fan_speed": [], "Cycle 2f1 Fomax": [], "Cycle 2f1 Fomin": [],
                    "Cycle 2f1 state_A": [], "Cycle 2f1 rpm_set": [], "Cycle 2f1 rpm_rt": [], "Cycle 2f1 Pdset": [],
                    "Cycle 2f1 Psfo": [], "Cycle 2f1 fan_I": [], "Cycle 2f2 romNo": [], "Cycle 2f2 adjust_period": [],
                    "Cycle 2f2 fan_speed": [], "Cycle 2f2 Fomax": [], "Cycle 2f2 Fomin": [], "Cycle 2f2 state_A": [],
                    "Cycle 2f2 rpm_set": [], "Cycle 2f2 rpm_rt": [], "Cycle 2f2 Pdset": [], "Cycle 2f2 Psfo": [],
                    "Cycle 2f2 fan_I": [], "Cycle 2 sub_adjust_period": [], "Cycle 2 sub_adjust_speed": [],
                    "Cycle 2 sub_rt_step": [], "Cycle 2 sub_delta_OP": [], "Cycle 2 sub_delta_OP_fix": [],
                    "Cycle 2 sub_OP_N": [], "Cycle 2 sub_OP_X": [], "Cycle 2 sub_OP_Y": [], "Cycle 2 sub_OP_Z": [],
                    "Cycle 2 adjust_period": [], "Cycle 2 adjust_speed": [], "Cycle 2 exv1_rt_step": [],
                    "Cycle 2 exv2_rt_step": [], "Cycle 2 OP": [], "Cycle 2 delta_OP": [], "Cycle 2 delta_OP_fix": [],
                    "Cycle 2 delta_OP1": [], "Cycle 2 delta_OP1_fix": [], "Cycle 2 OP1_N": [], "Cycle 2 OP1_X": [],
                    "Cycle 2 OP1_Y": [], "Cycle 2 delta_OP2": [], "Cycle 2 delta_OP2_fix": [], "Cycle 2 delta_OP3": [],
                    "Cycle 2 delta_OP3_fix": [],

                    # Cycle 3
                    "Cycle 3 address": [], "Cycle 3 power_on_time": [], "Cycle 3 total_run_time": [], "Cycle 3 Td": [],
                    "Cycle 3 Ts": [], "Cycle 3 Tf": [], "Cycle 3 Tcoil": [], "Cycle 3 Tsub_in": [],
                    "Cycle 3 Tsub_out": [], "Cycle 3 Pd": [], "Cycle 3 Ps": [], "Cycle 3 Tc": [], "Cycle 3 Tevp": [],
                    "Cycle 3 Iinv1": [], "Cycle 3 Iinv2": [], "Cycle 3 SH_Td": [], "Cycle 3 SH_TS_act": [],
                    "Cycle 3 SH_TS_Target": [], "Cycle 3 SH_sub": [], "Cycle 3 DO_state": [],
                    "Cycle 3 defrost_type": [], "Cycle 3 cool_total_run_time": [], "Cycle 3 heat_total_run_time": [],
                    "Cycle 3 defrost_run_time": [], "Cycle 3 oil_acc_run_time": [], "Cycle 3 oil_current_run_time": [],
                    "Cycle 3 alarm_state_h": [], "Cycle 3 alarm_state_l": [], "Cycle 3 cap_delta_Fi_up": [],
                    "Cycle 3 cap_delta_Fi_down": [], "Cycle 3 cap_freq_set": [], "Cycle 3 comp romNo": [],
                    "Cycle 3 type": [], "Cycle 3 current_run_time": [], "Cycle 3 freq_cool_rate": [],
                    "Cycle 3 freq_heat_rate": [], "Cycle 3 freq_target": [], "Cycle 3 freq_rt": [],
                    "Cycle 3 freq_step": [], "Cycle 3 freq_convert": [], "Cycle 3 Fimax": [], "Cycle 3 Fimax0": [],
                    "Cycle 3 Fimax1": [], "Cycle 3 Fimax2": [], "Cycle 3 Fimax3": [], "Cycle 3 Fimax4": [],
                    "Cycle 3 Fimin": [], "Cycle 3 Fimin1": [], "Cycle 3 Fimin2": [], "Cycle 3 Fimin3": [],
                    "Cycle 3 prot_src": [], "Cycle 3 prot_state": [], "Cycle 3 retreat": [], "Cycle 3f1 romNo": [],
                    "Cycle 3f1 adjust_period": [], "Cycle 3f1 fan_speed": [], "Cycle 3f1 Fomax": [], "Cycle 3f1 Fomin": [],
                    "Cycle 3f1 state_A": [], "Cycle 3f1 rpm_set": [], "Cycle 3f1 rpm_rt": [], "Cycle 3f1 Pdset": [],
                    "Cycle 3f1 Psfo": [], "Cycle 3f1 fan_I": [], "Cycle 3f2 romNo": [], "Cycle 3f2 adjust_period": [],
                    "Cycle 3f2 fan_speed": [], "Cycle 3f2 Fomax": [], "Cycle 3f2 Fomin": [], "Cycle 3f2 state_A": [],
                    "Cycle 3f2 rpm_set": [], "Cycle 3f2 rpm_rt": [], "Cycle 3f2 Pdset": [], "Cycle 3f2 Psfo": [],
                    "Cycle 3f2 fan_I": [], "Cycle 3 sub_adjust_period": [], "Cycle 3 sub_adjust_speed": [],
                    "Cycle 3 sub_rt_step": [], "Cycle 3 sub_delta_OP": [], "Cycle 3 sub_delta_OP_fix": [],
                    "Cycle 3 sub_OP_N": [], "Cycle 3 sub_OP_X": [], "Cycle 3 sub_OP_Y": [], "Cycle 3 sub_OP_Z": [],
                    "Cycle 3 adjust_period": [], "Cycle 3 adjust_speed": [], "Cycle 3 exv1_rt_step": [],
                    "Cycle 3 exv2_rt_step": [], "Cycle 3 OP": [], "Cycle 3 delta_OP": [], "Cycle 3 delta_OP_fix": [],
                    "Cycle 3 delta_OP1": [], "Cycle 3 delta_OP1_fix": [], "Cycle 3 OP1_N": [], "Cycle 3 OP1_X": [],
                    "Cycle 3 OP1_Y": [], "Cycle 3 delta_OP2": [], "Cycle 3 delta_OP2_fix": [], "Cycle 3 delta_OP3": [],
                    "Cycle 3 delta_OP3_fix": [],

                    # Cycle 4
                    "Cycle 4 address": [], "Cycle 4 power_on_time": [], "Cycle 4 total_run_time": [], "Cycle 4 Td": [],
                    "Cycle 4 Ts": [], "Cycle 4 Tf": [], "Cycle 4 Tcoil": [], "Cycle 4 Tsub_in": [],
                    "Cycle 4 Tsub_out": [], "Cycle 4 Pd": [], "Cycle 4 Ps": [], "Cycle 4 Tc": [], "Cycle 4 Tevp": [],
                    "Cycle 4 Iinv1": [], "Cycle 4 Iinv2": [], "Cycle 4 SH_Td": [], "Cycle 4 SH_TS_act": [],
                    "Cycle 4 SH_TS_Target": [], "Cycle 4 SH_sub": [], "Cycle 4 DO_state": [],
                    "Cycle 4 defrost_type": [], "Cycle 4 cool_total_run_time": [], "Cycle 4 heat_total_run_time": [],
                    "Cycle 4 defrost_run_time": [], "Cycle 4 oil_acc_run_time": [], "Cycle 4 oil_current_run_time": [],
                    "Cycle 4 alarm_state_h": [], "Cycle 4 alarm_state_l": [], "Cycle 4 cap_delta_Fi_up": [],
                    "Cycle 4 cap_delta_Fi_down": [], "Cycle 4 cap_freq_set": [], "Cycle 4 comp romNo": [],
                    "Cycle 4 type": [], "Cycle 4 current_run_time": [], "Cycle 4 freq_cool_rate": [],
                    "Cycle 4 freq_heat_rate": [], "Cycle 4 freq_target": [], "Cycle 4 freq_rt": [],
                    "Cycle 4 freq_step": [], "Cycle 4 freq_convert": [], "Cycle 4 Fimax": [], "Cycle 4 Fimax0": [],
                    "Cycle 4 Fimax1": [], "Cycle 4 Fimax2": [], "Cycle 4 Fimax3": [], "Cycle 4 Fimax4": [],
                    "Cycle 4 Fimin": [], "Cycle 4 Fimin1": [], "Cycle 4 Fimin2": [], "Cycle 4 Fimin3": [],
                    "Cycle 4 prot_src": [], "Cycle 4 prot_state": [], "Cycle 4 retreat": [], "Cycle 4f1 romNo": [],
                    "Cycle 4f1 adjust_period": [], "Cycle 4f1 fan_speed": [], "Cycle 4f1 Fomax": [], "Cycle 4f1 Fomin": [],
                    "Cycle 4f1 state_A": [], "Cycle 4f1 rpm_set": [], "Cycle 4f1 rpm_rt": [], "Cycle 4f1 Pdset": [],
                    "Cycle 4f1 Psfo": [], "Cycle 4f1 fan_I": [], "Cycle 4f2 romNo": [], "Cycle 4f2 adjust_period": [],
                    "Cycle 4f2 fan_speed": [], "Cycle 4f2 Fomax": [], "Cycle 4f2 Fomin": [], "Cycle 4f2 state_A": [],
                    "Cycle 4f2 rpm_set": [], "Cycle 4f2 rpm_rt": [], "Cycle 4f2 Pdset": [], "Cycle 4f2 Psfo": [],
                    "Cycle 4f2 fan_I": [], "Cycle 4 sub_adjust_period": [], "Cycle 4 sub_adjust_speed": [],
                    "Cycle 4 sub_rt_step": [], "Cycle 4 sub_delta_OP": [], "Cycle 4 sub_delta_OP_fix": [],
                    "Cycle 4 sub_OP_N": [], "Cycle 4 sub_OP_X": [], "Cycle 4 sub_OP_Y": [], "Cycle 4 sub_OP_Z": [],
                    "Cycle 4 adjust_period": [], "Cycle 4 adjust_speed": [], "Cycle 4 exv1_rt_step": [],
                    "Cycle 4 exv2_rt_step": [], "Cycle 4 OP": [], "Cycle 4 delta_OP": [], "Cycle 4 delta_OP_fix": [],
                    "Cycle 4 delta_OP1": [], "Cycle 4 delta_OP1_fix": [], "Cycle 4 OP1_N": [], "Cycle 4 OP1_X": [],
                    "Cycle 4 OP1_Y": [], "Cycle 4 delta_OP2": [], "Cycle 4 delta_OP2_fix": [], "Cycle 4 delta_OP3": [],
                    "Cycle 4 delta_OP3_fix": []

                })
            else:
                self.bt_save_data['text'] = 'Save data'
                self.runtime_flag = 0
                self.filename = str(time.asctime(time.localtime(time.time())))
                self.filename = re.sub(r"\s+", "", self.filename)
                self.filename = self.filename.replace(":", "")
                self.filename = self.filename + ".xlsx"
                self.data_all.to_excel(self.filename, index=False, engine='openpyxl')
        else:
            messagebox.showinfo('Bad News', 'No data can be recorded, please check the connection.')

    def Cleartext(self):
        self.scroll_text.delete(1.0, END)

    def dataflow_stop(self):
        self.dataflow_stop_flag = 1

    def dataflow_continue(self):
        self.dataflow_stop_flag = 0

    def show_dataflow(self):
        self.window_show_dataflow = tk.Toplevel(self)
        self.window_show_dataflow.geometry("900x350")
        self.window_show_dataflow.title("Communication Traffic")

        self.f_showdata = tk.Frame(self.window_show_dataflow)
        self.f_showdata.grid(column=0, row=0)

        self.f_showdata_1 = tk.Frame(self.window_show_dataflow)
        self.f_showdata_1.grid(column=0, row=0, sticky='w', pady=7)
        self.f_showdata_2 = tk.Frame(self.window_show_dataflow)
        self.f_showdata_2.grid(column=0, row=1)

        self.bt_stop = tk.Button(self.f_showdata_1, text='Stop', width=10, command=self.dataflow_stop)
        self.bt_clear = tk.Button(self.f_showdata_1, text='Clear', width=10, command=self.Cleartext)
        self.bt_save = tk.Button(self.f_showdata_1, text='Save', width=10, command=self.save_history_data)
        self.bt_continue = tk.Button(self.f_showdata_1, text='Contiune', width=10, command=self.dataflow_continue)
        self.bt_maxwindow= tk.Button(self.f_showdata_1, text='Maximize', width=10, command=self.maximize_window)

        self.var_stoponerror = IntVar()
        self.var_showtimestamp = IntVar()
        self.cb_stoponerror = tk.Checkbutton(self.f_showdata_1, text="Stop on Error", variable=self.var_stoponerror)
        self.cb_showtimestamp = tk.Checkbutton(self.f_showdata_1, text="show time stamp",
                                               variable=self.var_showtimestamp)

        self.bt_stop.grid(column=0, row=0, sticky='w', padx=5)
        self.bt_continue.grid(column=1, row=0, sticky='w', padx=5)
        self.bt_clear.grid(column=2, row=0, sticky='w', padx=5)
        self.bt_save.grid(column=3, row=0, sticky='w', padx=5)
        self.bt_maxwindow.grid(column=4, row=0, sticky='w', padx=5)
        self.cb_stoponerror.grid(column=5, row=0, sticky='w', padx=5)
        self.cb_showtimestamp.grid(column=6, row=0, sticky='w', padx=5)

        self.scroll_text = scrolledtext.ScrolledText(self.f_showdata_2, width=125, height=22)
        self.scroll_text.grid(column=0, row=0)

    def maximize_window(self):
        if self.maxwindow_falg==0:
            self.bt_maxwindow['text'] = 'Minimize'
            self.window_show_dataflow.state('zoomed')
            self.scroll_text.configure(width=180, height=46)
            self.maxwindow_falg=1
        else:
            self.bt_maxwindow['text']='Maximize'
            self.window_show_dataflow.state('normal')
            self.scroll_text.configure(width=125, height=22)
            self.maxwindow_falg = 0

    def show_fault_info(self):
        # 创建弹窗
        window_show_fault_info = tk.Toplevel(self)
        window_show_fault_info.geometry("1075x380")
        window_show_fault_info.title("Fault info")

        # Define watch items
        style_comp_lable = ttk.Style()
        style_comp_lable.configure("comp.TLabelframe.Label", foreground="black", background="green")
        self.f_comp = ttk.LabelFrame(window_show_fault_info, text="Comp", style="comp.TLabelframe")
        self.f_comp.grid(column=0, row=0, sticky="n", padx=5, pady=5)

        self.f_comp1 = tk.Frame(self.f_comp)
        self.f_comp1.grid(column=0, row=0)
        self.f_comp2 = tk.Frame(self.f_comp)
        self.f_comp2.grid(column=1, row=0)
        self.f_comp3 = tk.Frame(self.f_comp)
        self.f_comp3.grid(column=2, row=0)
        self.f_comp4 = tk.Frame(self.f_comp)
        self.f_comp4.grid(column=3, row=0)

        self.fault_labels = [None] * 24
        self.fault_code_labels = [None] * 24
        canvas_comp1 = tk.Canvas(self.f_comp1, width=20, height=250)
        canvas_comp1.grid(column=0, row=0)
        canvas_comp2 = tk.Canvas(self.f_comp3, width=20, height=250)
        canvas_comp2.grid(column=0, row=0, padx=5)
        # 创建圆点和Label
        for i in range(24):
            # 绘制圆点
            x = 10
            radius = 7
            if i < 12:
                y = 11 + i * 21
                canvas_comp1.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fault_labels[i] = ttk.Label(self.f_comp2)
                self.fault_labels[i].grid(column=1, row=i, stick='w')

                self.fault_code_labels[i] = ttk.Label(self.f_comp2)
                self.fault_code_labels[i].grid(column=0, row=i, stick='w', padx=2)
            else:
                y = 11 + (i - 12) * 21
                canvas_comp2.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fault_labels[i] = ttk.Label(self.f_comp4)
                self.fault_labels[i].grid(column=1, row=i, stick='w')

                self.fault_code_labels[i] = ttk.Label(self.f_comp4)
                self.fault_code_labels[i].grid(column=0, row=i, stick='w', padx=2)

        self.fault_labels[0].config(text='过流与过载')
        self.fault_labels[1].config(text='转速异常')
        self.fault_labels[2].config(text='MCU复位')
        self.fault_labels[3].config(text='电源输入异常')
        self.fault_labels[4].config(text='电流传感器故障')
        self.fault_labels[5].config(text='过温')
        self.fault_labels[6].config(text='预充电故障')
        self.fault_labels[7].config(text='过电压')
        self.fault_labels[8].config(text='欠电压')
        self.fault_labels[9].config(text='ISPM设定错误')
        self.fault_labels[10].config(text='排气压力高故障')
        self.fault_labels[11].config(text='瞬时过流')
        self.fault_labels[12].config(text='功率模块发热')
        self.fault_labels[13].config(text='IPM故障')
        self.fault_labels[14].config(text='对地短路')
        self.fault_labels[15].config(text='脱调检出')
        self.fault_labels[16].config(text='转速指令错误')
        self.fault_labels[17].config(text='预充电失败')
        self.fault_labels[18].config(text='继电器未闭合')
        self.fault_labels[19].config(text='63H')
        self.fault_labels[20].config(text='63H电路异常')
        self.fault_labels[21].config(text='63H提前复位')
        self.fault_labels[22].config(text='电源输入瞬时跌落')
        self.fault_labels[23].config(text='输入缺相')

        self.fault_code_labels[0].config(text='/')
        self.fault_code_labels[1].config(text='/')
        self.fault_code_labels[2].config(text='11')
        self.fault_code_labels[3].config(text='/')
        self.fault_code_labels[4].config(text='8')
        self.fault_code_labels[5].config(text='3')
        self.fault_code_labels[6].config(text='26')
        self.fault_code_labels[7].config(text='5')
        self.fault_code_labels[8].config(text='6')
        self.fault_code_labels[9].config(text='/')
        self.fault_code_labels[10].config(text='/')
        self.fault_code_labels[11].config(text='2')
        self.fault_code_labels[12].config(text='4')
        self.fault_code_labels[13].config(text='1')
        self.fault_code_labels[14].config(text='12')
        self.fault_code_labels[15].config(text='21')
        self.fault_code_labels[16].config(text='/')
        self.fault_code_labels[17].config(text='/')
        self.fault_code_labels[18].config(text='/')
        self.fault_code_labels[19].config(text='18')
        self.fault_code_labels[20].config(text='/')
        self.fault_code_labels[21].config(text='20')
        self.fault_code_labels[22].config(text='/')
        self.fault_code_labels[23].config(text='13')

        style_fan1_lable = ttk.Style()
        style_fan1_lable.configure("fan1.TLabelframe.Label", foreground="black", background="yellow")
        self.f_fan1 = ttk.LabelFrame(window_show_fault_info, text="Fan1", style="fan1.TLabelframe")
        self.f_fan1.grid(column=1, row=0, sticky="w", padx=5, pady=5)

        self.f_fan11 = tk.Frame(self.f_fan1)
        self.f_fan11.grid(column=0, row=0)
        self.f_fan12 = tk.Frame(self.f_fan1)
        self.f_fan12.grid(column=1, row=0)
        self.f_fan13 = tk.Frame(self.f_fan1)
        self.f_fan13.grid(column=2, row=0)
        self.f_fan14 = tk.Frame(self.f_fan1)
        self.f_fan14.grid(column=3, row=0)

        self.fan1_labels = [None] * 32
        self.fan1_code_labels = [None] * 32
        canvas_fan11 = tk.Canvas(self.f_fan11, width=20, height=330)
        canvas_fan11.grid(column=0, row=0)
        canvas_fan12 = tk.Canvas(self.f_fan13, width=20, height=330)
        canvas_fan12.grid(column=0, row=0, padx=5)

        for i in range(32):
            # 绘制圆点
            x = 10
            radius = 7
            if i < 16:
                y = 10 + i * 21
                canvas_fan11.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fan1_labels[i] = ttk.Label(self.f_fan12)
                self.fan1_labels[i].grid(column=1, row=i, stick='w')

                self.fan1_code_labels[i] = ttk.Label(self.f_fan12)
                self.fan1_code_labels[i].grid(column=0, row=i, stick='w', padx=2)
            else:
                y = 10 + (i - 16) * 21
                canvas_fan12.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fan1_labels[i] = ttk.Label(self.f_fan14)
                self.fan1_labels[i].grid(column=1, row=i, stick='w')

                self.fan1_code_labels[i] = ttk.Label(self.f_fan14)
                self.fan1_code_labels[i].grid(column=0, row=i, stick='w', padx=2)

        self.fan1_labels[0].config(text='FOCx/AP PCx过流')
        self.fan1_labels[1].config(text='FOCx驱动失败')
        self.fan1_labels[2].config(text='驱动内部故障')
        self.fan1_labels[3].config(text='输入缺相故障')
        self.fan1_labels[4].config(text='电流传感器故障')
        self.fan1_labels[5].config(text='FOCx/AP FCx过温')
        self.fan1_labels[6].config(text='预充电故障')
        self.fan1_labels[7].config(text='母线过压故障')
        self.fan1_labels[8].config(text='母线欠压故障')
        self.fan1_labels[9].config(text='散热器过温故障')
        self.fan1_labels[10].config(text='交流输入过流（硬件+软件）')
        self.fan1_labels[11].config(text='电流传感器校验失败')
        self.fan1_labels[12].config(text='驱动器或电机型号错误')
        self.fan1_labels[13].config(text='FOCx/AP FCx温度传感器故障')
        self.fan1_labels[14].config(text='排气压力高故障')
        self.fan1_labels[15].config(text='通讯故障')
        self.fan1_labels[16].config(text='FOCx/AP FCx软件过流')
        self.fan1_labels[17].config(text='FOCx/AP FCx过载')
        self.fan1_labels[18].config(text='MCU内部比较器A保护')
        self.fan1_labels[19].config(text='MCU内部比较器B保护')
        self.fan1_labels[20].config(text='MCU内部比较器C保护')
        self.fan1_labels[21].config(text='FOCx转速异常')
        self.fan1_labels[22].config(text='FOCx输出缺相')
        self.fan1_labels[23].config(text='MCU RAM')
        self.fan1_labels[24].config(text='MCU Register')
        self.fan1_labels[25].config(text='MCU 时钟')
        self.fan1_labels[26].config(text='MCU FLASH')
        self.fan1_labels[27].config(text='EEPROM校验')
        self.fan1_labels[28].config(text='参考电压异常')
        self.fan1_labels[29].config(text='软件保护模块异常')
        self.fan1_labels[30].config(text='交流输入软件过流')
        self.fan1_labels[31].config(text='交流输入过载')

        self.fan1_code_labels[0].config(text='/')
        self.fan1_code_labels[1].config(text='/')
        self.fan1_code_labels[2].config(text='/')
        self.fan1_code_labels[3].config(text='/')
        self.fan1_code_labels[4].config(text='/')
        self.fan1_code_labels[5].config(text='/')
        self.fan1_code_labels[6].config(text='/')
        self.fan1_code_labels[7].config(text='/')
        self.fan1_code_labels[8].config(text='/')
        self.fan1_code_labels[9].config(text='/')
        self.fan1_code_labels[10].config(text='/')
        self.fan1_code_labels[11].config(text='/')
        self.fan1_code_labels[12].config(text='/')
        self.fan1_code_labels[13].config(text='/')
        self.fan1_code_labels[14].config(text='/')
        self.fan1_code_labels[15].config(text='/')
        self.fan1_code_labels[16].config(text='/')
        self.fan1_code_labels[17].config(text='/')
        self.fan1_code_labels[18].config(text='/')
        self.fan1_code_labels[19].config(text='/')
        self.fan1_code_labels[20].config(text='/')
        self.fan1_code_labels[21].config(text='/')
        self.fan1_code_labels[22].config(text='/')
        self.fan1_code_labels[23].config(text='/')
        self.fan1_code_labels[24].config(text='/')
        self.fan1_code_labels[25].config(text='/')
        self.fan1_code_labels[26].config(text='/')
        self.fan1_code_labels[27].config(text='/')
        self.fan1_code_labels[28].config(text='/')
        self.fan1_code_labels[29].config(text='/')
        self.fan1_code_labels[30].config(text='/')
        self.fan1_code_labels[31].config(text='/')

        style_fan2_lable = ttk.Style()
        style_fan2_lable.configure("fan2.TLabelframe.Label", foreground="black", background="red")
        self.f_fan2 = ttk.LabelFrame(window_show_fault_info, text="Fan2", style="fan2.TLabelframe")
        self.f_fan2.grid(column=2, row=0, sticky="w", padx=5, pady=5)

        self.f_fan21 = tk.Frame(self.f_fan2)
        self.f_fan21.grid(column=0, row=0)
        self.f_fan22 = tk.Frame(self.f_fan2)
        self.f_fan22.grid(column=1, row=0)
        self.f_fan23 = tk.Frame(self.f_fan2)
        self.f_fan23.grid(column=2, row=0)
        self.f_fan24 = tk.Frame(self.f_fan2)
        self.f_fan24.grid(column=3, row=0)

        self.fan2_labels = [None] * 32
        self.fan2_code_labels = [None] * 32
        canvas_fan21 = tk.Canvas(self.f_fan21, width=20, height=330)
        canvas_fan21.grid(column=0, row=0)
        canvas_fan22 = tk.Canvas(self.f_fan23, width=20, height=330)
        canvas_fan22.grid(column=0, row=0, padx=5)

        for i in range(32):
            # 绘制圆点
            x = 10
            radius = 7
            if i < 16:
                y = 10 + i * 21
                canvas_fan21.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fan2_labels[i] = ttk.Label(self.f_fan22)
                self.fan2_labels[i].grid(column=1, row=i, stick='w')

                self.fan2_code_labels[i] = ttk.Label(self.f_fan22)
                self.fan2_code_labels[i].grid(column=0, row=i, stick='w', padx=2)
            else:
                y = 10 + (i - 16) * 21
                canvas_fan22.create_oval(x - radius, y - radius, x + radius, y + radius, outline="SystemButtonFace",
                                         fill="gray")
                self.fan2_labels[i] = ttk.Label(self.f_fan24)
                self.fan2_labels[i].grid(column=1, row=i, stick='w')

                self.fan2_code_labels[i] = ttk.Label(self.f_fan24)
                self.fan2_code_labels[i].grid(column=0, row=i, stick='w', padx=2)

        self.fan2_labels[0].config(text='FOCx/AP PCx过流')
        self.fan2_labels[1].config(text='FOCx驱动失败')
        self.fan2_labels[2].config(text='驱动内部故障')
        self.fan2_labels[3].config(text='输入缺相故障')
        self.fan2_labels[4].config(text='电流传感器故障')
        self.fan2_labels[5].config(text='FOCx/AP FCx过温')
        self.fan2_labels[6].config(text='预充电故障')
        self.fan2_labels[7].config(text='母线过压故障')
        self.fan2_labels[8].config(text='母线欠压故障')
        self.fan2_labels[9].config(text='散热器过温故障')
        self.fan2_labels[10].config(text='交流输入过流（硬件+软件）')
        self.fan2_labels[11].config(text='电流传感器校验失败')
        self.fan2_labels[12].config(text='驱动器或电机型号错误')
        self.fan2_labels[13].config(text='FOCx/AP FCx温度传感器故障')
        self.fan2_labels[14].config(text='排气压力高故障')
        self.fan2_labels[15].config(text='通讯故障')
        self.fan2_labels[16].config(text='FOCx/AP FCx软件过流')
        self.fan2_labels[17].config(text='FOCx/AP FCx过载')
        self.fan2_labels[18].config(text='MCU内部比较器A保护')
        self.fan2_labels[19].config(text='MCU内部比较器B保护')
        self.fan2_labels[20].config(text='MCU内部比较器C保护')
        self.fan2_labels[21].config(text='FOCx转速异常')
        self.fan2_labels[22].config(text='FOCx输出缺相')
        self.fan2_labels[23].config(text='MCU RAM')
        self.fan2_labels[24].config(text='MCU Register')
        self.fan2_labels[25].config(text='MCU 时钟')
        self.fan2_labels[26].config(text='MCU FLASH')
        self.fan2_labels[27].config(text='EEPROM校验')
        self.fan2_labels[28].config(text='参考电压异常')
        self.fan2_labels[29].config(text='软件保护模块异常')
        self.fan2_labels[30].config(text='交流输入软件过流')
        self.fan2_labels[31].config(text='交流输入过载')

        self.fan2_code_labels[0].config(text='/')
        self.fan2_code_labels[1].config(text='/')
        self.fan2_code_labels[2].config(text='/')
        self.fan2_code_labels[3].config(text='/')
        self.fan2_code_labels[4].config(text='/')
        self.fan2_code_labels[5].config(text='/')
        self.fan2_code_labels[6].config(text='/')
        self.fan2_code_labels[7].config(text='/')
        self.fan2_code_labels[8].config(text='/')
        self.fan2_code_labels[9].config(text='/')
        self.fan2_code_labels[10].config(text='/')
        self.fan2_code_labels[11].config(text='/')
        self.fan2_code_labels[12].config(text='/')
        self.fan2_code_labels[13].config(text='/')
        self.fan2_code_labels[14].config(text='/')
        self.fan2_code_labels[15].config(text='/')
        self.fan2_code_labels[16].config(text='/')
        self.fan2_code_labels[17].config(text='/')
        self.fan2_code_labels[18].config(text='/')
        self.fan2_code_labels[19].config(text='/')
        self.fan2_code_labels[20].config(text='/')
        self.fan2_code_labels[21].config(text='/')
        self.fan2_code_labels[22].config(text='/')
        self.fan2_code_labels[23].config(text='/')
        self.fan2_code_labels[24].config(text='/')
        self.fan2_code_labels[25].config(text='/')
        self.fan2_code_labels[26].config(text='/')
        self.fan2_code_labels[27].config(text='/')
        self.fan2_code_labels[28].config(text='/')
        self.fan2_code_labels[29].config(text='/')
        self.fan2_code_labels[30].config(text='/')
        self.fan2_code_labels[31].config(text='/')

    def labelname_comm(self, value):
        options = {
            0: "Date_time", 1: "Mode_run", 2: "Switch_state", 3: "run_state",
            4: "Run_time", 5: "Unit_counter", 6: "Capacity_meet", 7: "Tw_sys_in",
            8: "Tw_sys_out_sensor", 9: "Tw_sys_out_fix", 10: "Tw_sys_out", 11: "Tw",
            12: "Tw_set", 13: "DTw", 14: "alarm_state_h", 15: "alarm_state_l",
            16: "Dcor", 17: "time_set",
            18: "cap_period", 19: "cap_N", 20: "cap_cool_delta_Ts",
            21: "cap_heat_delta_Ts", 22: "delta_Tw_h_set", 23: "Max_T_Tw_in",
            24: "Max_T_Tw_out", 25: "cap_delta_Tw", 26: "cap_PI",
            27: "F_total_rating", 28: "delta_F_total", 29: "delta_F_up_total",
            30: "delta_F_down_total", 31: "antifreeze_run_time", 32: "antifreeze_stop_time", 33: "reserved"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_sys(self, value):
        options = {
            0: "series_type", 1: "tier_type", 2: "capacity", 3: "refri_type",
            4: "pump_type", 5: "power_on_restart", 6: "esp_type", 7: "main_romNo",
            8: "sub_romNo", 9: "address", 10: "work_type", 11: "cycle_counter",
            12: "switch_set", 13: "switch_rt", 14: "Tw_in_sensor", 15: "Tw_in_fix",
            16: "Tw_in", 17: "Tw_out_sensor",
            18: "Tw_out_fix", 19: "Tw_out", 20: "Tams",
            21: "capacity_percent", 22: "pump_current_run_time", 23: "pump_stop_time",
            24: "chassis_heat_run_time", 25: "chassis_heat_stop_time", 26: "Ethtime",
            27: "main_DO_state_h", 28: "main_DO_state_l", 29: "sub_DO_state_h",
            30: "sub_DO_state_l", 31: "alarm_state_h", 32: "alarm_state_l", 33: "cap_cycle_opt_cnt",
            34: "cap_cycle_run_cnt", 35: "cap_cycle_close_able_cnt", 36: "cap_Fi_rating", 37: "cap_Fi_run",
            38: "cap_delta_Fi_up", 39: "cap_delta_Fi_down", 40: "cap_freq_set", 41: "reserved"

        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_sys1(self, value):
        options = {
            0: "ratio_state", 1: "Pd_state", 2: "current_state", 3: "inv_temp_state",
            4: "Td_state"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_cycle(self, value):
        options = {
            0: "address", 1: "power_on_time", 2: "total_run_time", 3: "Td",
            4: "Ts", 5: "Tf", 6: "Tcoil", 7: "Tsub_in",
            8: "Tsub_out", 9: "Pd", 10: "Ps", 11: "Tc",
            12: "Tevp", 13: "Iinv1", 14: "Iinv2", 15: "SH_Td",
            16: "SH_TS_act", 17: "SH_TS_Target",
            18: "SH_sub", 19: "DO_state", 20: "defrost_type",
            21: "cool_total_run_time", 22: "heat_total_run_time", 23: "defrost_run_time",
            24: "oil_acc_run_time", 25: "oil_current_run_time", 26: "alarm_state_h",
            27: "alarm_state_l", 28: "cap_delta_Fi_up", 29: "cap_delta_Fi_down",
            30: "cap_freq_set"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_comp1(self, value):
        options = {
            0: "romNo", 1: "type", 2: "current_run_time", 3: "freq_cool_rate",
            4: "freq_heat_rate", 5: "freq_target", 6: "freq_rt", 7: "freq_step",
            8: "freq_convert", 9: "Fimax", 10: "Fimax0", 11: "Fimax1",
            12: "Fimax2", 13: "Fimax3", 14: "Fimax4", 15: "Fimin",
            16: "Fimin1", 17: "Fimin2",
            18: "Fimin3", 19: "prot_src", 20: "prot_state",
            21: "retreat"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_fan(self, value):
        options = {
            0: "romNo", 1: "adjust_period", 2: "fan_speed", 3: "Fomax",
            4: "Fomin", 5: "state_A", 6: "rpm_set", 7: "rpm_rt",
            8: "Pdset", 9: "Psfo", 10: "fan_I"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_EXVmain(self, value):
        options = {
            0: "adjust_period", 1: "adjust_speed", 2: "exv1_rt_step", 3: "exv2_rt_step",
            4: "OP", 5: "delta_OP", 6: "delta_OP_fix", 7: "delta_OP1",
            8: "delta_OP1_fix", 9: "OP1_N", 10: "OP1_X",11:"OP1_Y",
            12: "delta_OP2",13: "delta_OP2_fix",14: "delta_OP3",15:"delta_OP3_fix"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_EXVsub(self, value):
        options = {
            0: "sub_adjust_period", 1: "sub_adjust_speed", 2: "sub_rt_step", 3: "sub_delta_OP",
            4: "sub_delta_OP_fix", 5: "sub_OP_N", 6: "sub_OP_X", 7: "sub_OP_Y",
            8: "sub_OP_Z"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_checkbox(self, value):
        options = {
            0: "SysParam", 1: "UnitParam", 2: "Cycle 1", 3: "Cycle 2",
            4: "Cycle 3", 5: "Cycle 4"
        }
        result = options.get(value, "Invalid option")
        return result

    def labelname_Do(self, value):
        options = {
            0: "Do1:四通阀1", 1: "Do2:四通阀2", 2: "Do3:压缩机曲轴电加热带1", 3: "Do4:压缩机曲轴电加热带2",
            4: "Do5:EVI电磁阀1", 5: "Do6:回油电磁阀1", 6: "Do7:EVI电磁阀2", 7: "Do8:回油电磁阀2",
            8: "Do9:水泵", 9: "Do10:辅助水电电加热", 10: "Do11:底盘电加热带1", 11: "Do12:底盘电加热带2",
            12: "Do13:故障输出", 13: "Do14:水阀", 14: "Do15:板换电加热", 15: "Do16:热水水阀",
            16: "Do17:空调BPHE电磁阀1", 17: "Do18:空调BPHE电磁阀2", 18: "Do19:coil支路电磁阀1", 19: "Do20:coil之路电磁阀2",
            20: "Do21:融霜电磁阀1", 21: "Do22:融霜电磁阀2", 22: "Do23:防冻电加热2", 23: "Do24:系统热水水泵控制",
            24: "Do25:保留", 25: "Do26:保留", 26: "Do27:保留", 27: "Do28:保留",
            28: "Do29:保留", 29: "Do30:保留", 30: "Do31:保留", 31: "Do32:保留"
        }
        result = options.get(value, "Invalid option")
        return result

    def decompose_binary(self,num):
        binary = bin(num)[2:].zfill(16)  # 将整数转换为16位的二进制数，并填充前导零
        binary_array = [int(bit) for bit in binary[::-1]]  # 将二进制数的每一位倒序存储在数组中
        return binary_array

    def select_file(self):
        if self.com_flag == 1 or self.runtime_flag == 1:
            messagebox.showerror("Error",
                                 "Please disconnect the connection and stop save before checking history state")
        else:
            self.read_state_flag=1
            self.recv_data=[]
            pad=[0,0,0,0,0,0]
            file_path = filedialog.askopenfilename(initialdir="./", title="Select an Excel File",
                                                   filetypes=(("Excel Files", "*.xlsx"), ("All Files", "*.*")))
            if file_path:
                file_name = file_path.split("/")[-1]
                file_name = file_name[:-5]
                try:
                    self.state_data =  pd.read_excel("".join([file_name, ".xlsx"]),engine='openpyxl')
                except FileNotFoundError:
                    messagebox.showerror("File can't find", "Please check the file name")
                self.maxpage=len(self.state_data)
                self.pagenum=self.maxpage
                self.recv_data_int=self.state_data.iloc[-1]
                self.recv_data_time=str(self.recv_data_int.iloc[:1].values)
                self.recv_data_time= self.recv_data_time[12:]
                self.recv_data_int=self.recv_data_int.iloc[1:]
                for i in self.recv_data_int:
                    # 使用struct.pack将整数转换为两个字节
                    bytes_ = struct.pack('H', int(i))
                    swapped_bytes = bytearray([bytes_[1], bytes_[0]])
                    # 将两个字节添加到字节数组中
                    self.recv_data+= swapped_bytes
                self.recv_data =pad +self.recv_data
                self.entry_showall.config(state=tk.NORMAL)
                self.entry_showall.delete(0, END)
                self.entry_showall.insert(0, self.maxpage)
                self.entry_showall.config(state=tk.DISABLED)
                self.entry_showrows.delete(0, END)
                self.entry_showrows.insert(0, self.maxpage)
                self.insert_data()
                #messagebox.showinfo("Lucky", "Read data successfully")

app = App()
app.mainloop()
