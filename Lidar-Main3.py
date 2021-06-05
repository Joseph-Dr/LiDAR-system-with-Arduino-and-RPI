# varianta 3 de Lidar Main cu definire trhead
# 01.05.2021.
# Program principal pentru achizitie de date, citire distanta,unghi,poziitie si stocare in baza de date

#_________________________________ Importare librarii necesare_________________________________________________________

import sys                         # Librarie pentru manipulare rutina specifica
import time                        # Librarie pentru 
import serial                      #
import threading                   #
import sqlite3                     #
import busio                       #
import adafruit_bmp280             #
from MPU9250lib import *
time.sleep(1)                      # delay necessary to allow mpu9250 to settle
import RPi.GPIO as GPIO            # Librarie pentru comandarea pinilor de I/O
import board
#__________________________________Declarare variabile si constante___________________________________________________

TimpInitial=0                      # Variabila pentrustocarea valorii timpului initial
Tstart=0                           # Valorile timpului
Tstop=0
K=25                               # Constanta pentru ajustare altitudine
ParTO = bool(False)                # Fanion pentru setare timp
ff = bool(False)
En = 23                            # pin pentru encoder

#__________________________________PARAMETRIZARE GENERALA_____________________________________________________________

i2c = busio.I2C(board.SCL, board.SDA)
bmp280 = adafruit_bmp280.Adafruit_BMP280_I2C(i2c,address=0x76)    # Definirea adresei pentru senzor
bmp280.sea_level_pressure = 1013.25                               # constanta pentru calibrarea altitudini

#Informativ...
#Adresa i2c senzor altitudine 0x76
#Adresa i2c senzor giroscop   0x68

#__________________________________________PARAMETRIZARE GPIO______________________________________________________________________
#GPIO.setmode(GPIO.BOARD)
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False) 
GPIO.setup(23, GPIO.IN)            # A7 Pinul 16 intrare de start masurare ENCODER
GPIO.setup(24, GPIO.OUT)           # A8 Pinul 18 iesire pentru sincronizare
GPIO.setup(17, GPIO.OUT)           # A6 Pinul 22 Iesire pentru start rotire motor
GPIO.setup(25, GPIO.OUT)           # 0E Activeaza covertizorul logic bidirectional
GPIO.setup(27, GPIO.OUT)           # A5

#1_________________________________________Fuctie de sincronizare____________________________________________

    
conn = sqlite3.connect('DataB3.db')                        # stabilirea conectiuni cu baza de date
c = conn.cursor()
    
print ("timp de referinta 00.00")
GPIO.output(25,1)
GPIO.output(24,1)                                  # Trimite un semnal catre Arduino pentru sincronizare
TimpInitial=time.time()                                    # Registreaza timpul de baza de la raspberry
time.sleep(0.1)
GPIO.output(24,0)                                   # Irerupe semnalul pentru sincronizare

ParT0 = True                                               # Schimba starea fanionului pentru initializare
#GPIO.cleanup()         # clean up after yourself        
c.execute('INSERT INTO TimpStart(Timp) VALUES (?)',[TimpInitial])
conn.commit() # Executie instructiune

c.close()
conn.close()
#2_____________________________________________________________GPIO-1__________________________________________           

def GpioControl():                 # thread 1
    print ("START SCANARE LIDAR")
    time.sleep(0.3)                                           # Timp alocat pentru sincronizare
    GPIO.output(17,1)                                         # Trimite semnal catre contolerul de driver sub forma de tensiune
    
    time.sleep(10)                                            # Timp de citire in secunde
    print ("STOP SCANARE LIDAR")                              #
    GPIO.output(17,0)                                  # Intrerupe semnalul de comanda
    GPIO.output(25,0)
    time.sleep(0.01)
    ff=True
            
#3_____________________________________________________________gpio-2__________________________________________    
def GpioControl2():                # thread 2
    stare = bool(False)
    conn = sqlite3.connect('DataB3.db')                       # stabilirea conectiuni cu baza de date
    c = conn.cursor()
    fa = bool(False)

    while True:            # this will carry on until you hit CTRL+C  
        if GPIO.input(23)==1 and fa==False: # if port 25 == 1
            fa=True
            tstart=(time.time()-TimpInitial)                # registreaza timpul de start
            print("Start masurare")   
        if GPIO.input(23)==0 and fa==True:
            fa=False
            tstop=(time.time()-TimpInitial)
            print("Stop masurare")
            c.execute('INSERT INTO Tsincronizare(Tstart,Tstop) VALUES (?,?)',[tstart,tstop])
            conn.commit() # Executie instructiune 
  
    if ff== True:
        c.close()
        conn.close()
#4________________________________________________________Citire Giroscop_________________________________________-
def CitireGiro():                  #thread 4
    conn = sqlite3.connect('DataB3.db')                     # stabilirea conectiuni cu baza de date
    c = conn.cursor()

    while 1:
        try:
            ax,ay,az,wx,wy,wz = mpu6050_conv()              # Citire date de la accelerometru
            mx,my,mz = AK8963_conv()                        # Citire senzor AK8963 magnetometru
            TimpCurentG=(time.time()-TimpInitial)           # Registarrea timpului aferent setului de date
        except:
            continue
        
        #Inserare in baza de date
        c.execute('INSERT INTO Acc(Timp,Ax,Ay,Az,Gx,Gy,Gz,Mx,My,Mz) VALUES (?,?,?,?,?,?,?,?,?,?)',[TimpCurentG,ax,ay,az,wx,wy,wz,mx,my,mz])
        conn.commit()                                       # Executie instructiune
        time.sleep(0.01)
    if ff== True:
        c.close()
        conn.close()   
 
#5________________________________________________________Citire altitudine_______________________________________
def CitireBaro():
    conn = sqlite3.connect('DataB3.db')            # stabilirea conectiuni cu baza de date
    c = conn.cursor()
    
    while True:
        Alt=(bmp280.altitude+K)
        TimpCurentA=(time.time()-TimpInitial)      # Registarrea timpului aferent setului de date altitudine
        
        c.execute('INSERT INTO Altitudine(Altimetru,Timp) VALUES (?,?)',[Alt,TimpCurentA])
        conn.commit() # Executie instructiune
        
        #print("Pressure: %0.1f hPa" % bmp280.pressure)
        #print("Altitude = %0.2f meters" % bmp280.altitude)
        #print(bmp280.altitude)
        time.sleep(0.5) # definirea intervalului de timp
    if ff== True:
        c.close()
        conn.close()        
#6________________________________________________________Citire distanta_________________________________________
def CitireDistanta():
    conn = sqlite3.connect('DataB3.db')  # stabilirea conectiuni cu baza de date
    c = conn.cursor()
     
    if __name__ == '__main__':
        ser = serial.Serial('/dev/ttyUSB0', 250000, timeout=1)
        ser.flush()

    while True:
        if ser.in_waiting > 0:
            continut = ser.readline().decode('utf-8')
            c.execute('INSERT INTO Distanta(Mesaj) VALUES (?)',[continut])
            conn.commit() # Executie instructiune
            #print(continut) 
    if ff== True:
        c.close()
        conn.close()
    

#_______________________________________________________ FUNCTII DE START_______________________________________|
t0 = threading.Thread (target = GpioControl)          #F1
t1 = threading.Thread (target = GpioControl2)         #F2
t2 = threading.Thread (target = CitireGiro)           #F3
t3 = threading.Thread (target = CitireBaro)           #F4
t4 = threading.Thread (target = CitireDistanta)       #F5


t0.start()                  #
t1.start()                  #
t2.start()                  #
t3.start()                  #
t4.start()                  #

    