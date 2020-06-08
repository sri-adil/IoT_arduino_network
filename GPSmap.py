import gmplot
import serial


ser = serial.Serial('/dev/ttyUSB0')
ser.flushInput()
count = 0
while True:
        ser_bytes = ser.readline()
        string = str(ser_bytes,'utf-8')
        string = string[:-2]
        string_array = string.split(';')
        if(string[0]== '$' and string[1] == '$'):
            print("position = ")
            print("latitude =" + string_array[1][1:-1])
            print("longitude =" + string_array[2][1:])
            latitude = float(string_array[1][1:-1])
            longitude = float(string_array[2][1:])
            lats, longs = zip(*[(latitude, longitude)])
            gmap3 = gmplot.GoogleMapPlotter(17.3616, 78.4747, 13)
            gmap3.scatter(lats, longs , 'black',size = 100, marker = False )
            gmap3.apikey = "AIzaSyCdb_2VvZV2p3ofKDcbQuEHa1SlwlNUKtw"
            gmap3.draw("scatter.html")

        else:
            print(string_array)
