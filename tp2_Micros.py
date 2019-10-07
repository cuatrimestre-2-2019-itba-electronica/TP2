
from __future__ import print_function

"""
Demo entry point for Tkinter Window with OpenGL
"""

import sys, math, time
if sys.version_info[0] < 3 :
    from Tkinter import *
else:
    from tkinter import *
from OpenGL import GL, GLU
from OpenGL.GL import *
from OpenGL.GLU import *
from pyopengltk import OpenGLFrame
import serial.tools.list_ports


def decodeInformation(data):
    try:
        dataStringList = data.split(':')
        ida = int(dataStringList[0], 10)
        roll = int(dataStringList[1], 10)
        head = int(dataStringList[2], 10)
        ortientation = int(dataStringList[3], 10)
        dataIntList = [ida, roll, head, ortientation]
        return dataIntList
    except:
        print("Error en decodificacion")
        return [0,0,0,0]


vertices = (
    (1, -0.5, -1),
    (1, 0.5, -1),
    (-1, 0.5, -1),
    (-1, -0.5, -1),
    (1, -0.5, 1),
    (1, 0.5, 1),
    (-1, -0.5, 1),
    (-1, 0.5, 1),
)

edges = (
    (0, 1),
    (0, 3),
    (0, 4),
    (2, 1),
    (2, 3),
    (2, 7),
    (6, 3),
    (6, 4),
    (6, 7),
    (5, 1),
    (5, 4),
    (5, 7)
)


def Cube():
    GL.glBegin(GL.GL_LINES)
    for edge in edges:
        for vertex in edge:
            GL.glVertex3fv(vertices[vertex])
    GL.glEnd()


should_i_draw = []

board_count = 8
x_sep = 5
boards_per_row = 4
y_sep = 5

#todo: actualizar con las flechas
x_camara = 0
y_camara = 0


class Board_rotations:
    yaw = 0
    roll = 0
    pitch = 0

rotations = []

placas = []

class Placa:
    def __init__(self, gn):
        self.gn = gn
        self.rolido = 0
        self.cabeceo = 0
        self.orientacion = 0

    def update(self, r, c, o):
        self.rolido = r
        self.cabeceo = c
        self.orientacion = o
    def get_rolido(self):
        return self.rolido
    def get_cabeceo(self):
        return self.cabeceo
    def get_orientacion(self):
        return self.orientacion

class AppOgl(OpenGLFrame):

    def initgl(self):
        GL.glViewport( 0, 0, self.width, self.height)
        GL.glClearColor(0.0,1.0,1.0,0.0)
        GL.glColor3f(0.0,0.0, 0.0)
        GL.glPointSize(4.0)
#        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
#        GLU.gluOrtho3D(-5,5,-5,5)
        gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)
        glTranslatef(0.0, 0.0, -20)

        self.start = time.time()
        self.nframes = 0


    def redraw(self):

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

        for i in range(board_count):
            if should_i_draw[i] == True:
                x_coord = x_sep * (i % boards_per_row - (boards_per_row - 1)/ 2) - x_camara
                y_coord = y_sep * ((int)(i/boards_per_row) - 0.5)- y_camara

                # glTranslate(x_coord, y_coord, 0)
                # glRotate(placas[i].rolido, 1, 0, 0)
                # glRotate(placas[i].cabeceo, 0, 1, 0)
                # glRotate(placas[i].orientacion, 0, 0, 1)
                # Cube()
                # glRotate(placas[i].orientacion, 0, 0, -1)
                # glRotate(placas[i].cabeceo, 0, -1, 0)
                # glRotate(placas[i].rolido, -1, 0, 0)
                # glTranslate(-x_coord, -y_coord, 0)


                glTranslate(x_coord, y_coord, 0)
                glRotate(rotations[i].roll, 1, 0, 0)
                glRotate(rotations[i].pitch, 0, 1, 0)
                glRotate(rotations[i].yaw, 0, 0, 1)
                Cube()
                glRotate(rotations[i].yaw, 0, 0, -1)
                glRotate(rotations[i].pitch, 0, -1, 0)
                glRotate(rotations[i].roll, -1, 0, 0)
                glTranslate(-x_coord, -y_coord, 0)



        # GL.glClear(GL.GL_COLOR_BUFFER_BIT)
        # GL.glBegin(GL.GL_POINTS)
        # npt = 100
        # for i in range(npt):
        #     x = -5.0 + i*10.0/npt
        #     y = math.sin(x+ time.time())*5/2
        #     GL.glVertex2f( x, y )
        # GL.glEnd()
        #
        # GL.glFlush()
        # self.nframes+=1
        # tm = time.time() - self.start
        # print("fps",self.nframes / tm, end="\r" )


class SerialHandler:
    def __init__(self, _placas):
        self.connected = False
        self.port = 'COM9'   #'/dev/cu.usbmodemSDA9077DE5F1'
        self.baud = 9600
#        self.ser = serial.Serial(self.port, self.baud)
        self.placas = _placas


    def read_data(self):
       try:
            if (self.ser.inWaiting() > 0):  # if incoming bytes are waiting to be read from the serial input buffer

                return self.ser.read(self.ser.inWaiting()).decode('ascii')  # read the bytes and convert from binary array to ASCII
            else:
                return ''
       except:
            print('Error de lectura')
            return ''
        #return "G2R-34C024O-72/G1R-34C24O+433"

    def update_placa(self, data):
        print(data)
        iG = data.find('G')
        iR = data.find('R')
        iC = data.find('C')
        iO = data.find('O')

        gn = int(data[iG + 1:iR])
        # self.placas[gn].rolido = int(data[iR + 1:iC])
        # self.placas[gn].cabeceo = int(data[iC + 1:iO])
        # self.placas[gn].orientacion = int(data[iO + 1:])
        rotations[gn].roll = int(data[iR + 1:iC])
        rotations[gn].pitch = int(data[iC + 1:iO])
        rotations[gn].yaw = int(data[iO + 1:])

    def update_placas(self):
        data = self.read_data()
        if(data!=''):

            if(data.find("/")!=-1):
                dataLines = data.split("/")
            else:
                dataLines = [data]
            for datas in dataLines:
                try:
                    self.update_placa(datas)
                except:
                    print('invalid')


if __name__ == '__main__':
    comunic = None
    while comunic == None:
        try:
            # esto me devuelve en ports la lista de puertos a disposicion
            ports = serial.tools.list_ports.comports()
            available_ports = []
            # itero por los puertos disponibles y guardo el nombre del puerto
            for p in ports:
                available_ports.append(p.device)

            print("Puertos disponibles: " + str(available_ports))

            comunic = serial.Serial(available_ports[0], baudrate=9600, timeout=1)
            print("Conectado!")
        except:
            comunic = None
            print("Intentando conectar ... ")

    # pygame.init()
    display = (800, 600)
    # pygame.display.set_mode(display, DOUBLEBUF | OPENGL)


    nPlacas = 6 # Numero de placas
    for i in range(nPlacas):
        p = Placa(i+1)
        placas.append(p)

    for i in range(board_count):
        should_i_draw.append(True)
        rotation = Board_rotations()
        rotation.pitch = 0
        rotation.yaw = 0
        rotation.roll = 0
        rotations.append(rotation)


    sr = SerialHandler(placas)

    window = Tk()

    def on_closing():
        global run
        run = False
        window.destroy()

    window.protocol("WM_DELETE_WINDOW", on_closing)
    window.title("TP2 Micros G3")
    window.geometry('500x500')
    frames = []

    app = AppOgl(window, width=320, height=200)
    app.pack()
    app.animate=1
    app.after(100, app.printContext)

    rolidos_str = []
    cabeceo_str = []
    orientacion_str = []

    for i in range(len(rotations)):
        mainFrame = Frame(window, bd=5, relief="groove")
        mainFrame.pack(side=TOP, fill=X)
        frame = Frame(mainFrame)
        frame.pack(side=TOP)
        frames.append(frame)




    for i in range(len(rotations)):
        lblGn = Label(frames[i], text="G" + str(i + 1))
        lblGn.grid(column=0, row=0, columnspan=3)
        lblR = Label(frames[i], text="Rolido : " + str(rotations[i].roll))
        lblR.grid(column=0, row=1)
        lblC = Label(frames[i], text="Cabeceo : " + str(rotations[i].pitch))
        lblC.grid(column=1, row=1)
        lblO = Label(frames[i], text="Orientacion : " + str(rotations[i].yaw))
        lblO.grid(column=2, row=1)

    while True:
        # try:
        #     data = comunic.readline().decode('utf-8')
        # except:
        #     data = ''
        #     print("data corrompida")
        #
        # dataString = str(data)
        # # data = "1:+000:+002:+116"
        # # os.system('cls')
        # print("Informacion recibida: " + dataString)
        # if (not (dataString == '')):
        #     dataList = decodeInformation(dataString)
        #     rotations[dataList[0]].roll = dataList[1]
        #     rotations[dataList[0]].pitch = dataList[2]
        #     rotations[dataList[0]].yaw = dataList[3]

        sr.update_placas()
        for i in range(len(rotations)):
            lblGn.config(text="G" + str(i+1))
            lblR.config(text="Rolido : " + str(rotations[i].roll))
            lblC.config(text="Cabeceo : " + str(rotations[i].pitch))
            lblO.config(text="Orientacion : " + str(rotations[i].yaw))
        # for event in pygame.event.get():
        #     if event.type == pygame.QUIT:
        #         pygame.quit()
        #         quit()

        #Tk.update_idletasks(root)
        Tk.update(window)
        #app.mainloop()
