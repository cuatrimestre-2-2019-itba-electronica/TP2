from tkinter import *
import serial

run = True

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

class SerialHandler:
    def __init__(self, _placas):
        self.connected = False
        self.port = '/dev/cu.usbmodemSDA9077DE5F1'
        self.baud = 9600
        self.ser = serial.Serial(self.port, self.baud)

        self.placas = _placas

    def read_data(self):
       try:
            if (self.ser.inWaiting() > 0):  # if incoming bytes are waiting to be read from the serial input buffer

                return self.ser.read(self.ser.inWaiting()).decode('ascii')  # read the bytes and convert from binary array to ASCII
            else:
                return ''
       except:
            print('Error de lectura')
        #return "G2R-34C024O-72/G1R-34C24O+433"

    def update_placa(self, data):
        iG = data.find('G')
        iR = data.find('R')
        iC = data.find('C')
        iO = data.find('O')

        gn = int(data[iG + 1:iR])
        self.placas[gn].rolido = int(data[iR + 1:iC])
        self.placas[gn].cabeceo = int(data[iC + 1:iO])
        self.placas[gn].orientacion = int(data[iO + 1:])

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


def main():
    placas = []
    nPlacas = 6 # Numero de placas
    for i in range(nPlacas):
        placas.append(Placa(i+1))

    #sr = SerialHandler(placas)

    window = Tk()

    def on_closing():
        global run
        run = False
        window.destroy()

    window.protocol("WM_DELETE_WINDOW", on_closing)
    window.title("TP2 Micros G3")
    window.geometry('500x500')
    frames = []

    for i in range(len(placas)):
        mainFrame = Frame(window, bd=5, relief="groove")
        mainFrame.pack(side=TOP, fill=X)
        frame = Frame(mainFrame)
        frame.pack(side=TOP)
        frames.append(frame)

    while run:

        #sr.update_placas()

        for i in range(len(placas)):
            lblGn = Label(frames[i], text="G" + str(placas[i].gn))
            lblGn.grid(column=0, row=0, columnspan=3)
            lblR = Label(frames[i], text="Rolido : " + str(placas[i].rolido))
            lblR.grid(column=0, row=1)
            lblC = Label(frames[i], text="Cabeceo : " + str(placas[i].cabeceo))
            lblC.grid(column=1, row=1)
            lblO = Label(frames[i], text="Orientacion : " + str(placas[i].orientacion))
            lblO.grid(column=2, row=1)

        window.update()


if __name__ == '__main__':
    main()