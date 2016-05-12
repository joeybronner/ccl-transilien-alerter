#!/usr/bin/python
import serial
import time
import urllib2
import base64
import xml.etree.ElementTree as ET
from datetime import datetime

# Init vars
PORT          = '/dev/cu.wchusbserialfd120'
REQUEST       = urllib2.Request("http://api.transilien.com/gare/87381129/depart")
BASE64AUTH    = base64.encodestring('%s:%s' % ("tnhtn175", "cgP479kW")).replace('\n', '')
REQUEST.add_header("Authorization", "Basic %s" % BASE64AUTH)
FMT           = '%d/%m/%Y %H:%M:%S'
ARD = serial.Serial(PORT, 9600, timeout=5)

while True:
    # ----------------------------------------------------------------- #
    # ----------------------------------------------------------------- #
    #                            CALL SNCF APIs                         #
    # ----------------------------------------------------------------- #
    # ----------------------------------------------------------------- #
    # Query API SNCF
    result = urllib2.urlopen(REQUEST)
    e = ET.ElementTree(ET.fromstring(result.read()))
    root = e.getroot()

    travels = []
    for train in e.findall('.//train'):
        t_date = train.find('.//date').text
        t_numb = train.find('.//num').text
        t_term = train.find('.//term').text
        t_stat = train.find('.//etat')
        t_stat = t_stat.text.encode("utf-8") if t_stat is not None else "OK"
        o_travel = {
            'date': t_date,
            'numb': t_numb,
            'term': t_term,
            'stat': t_stat
        }
        travels.append(o_travel)
    print ("-- TRAVELS UPDATED --- : count: " + str(len(travels)))

    # ----------------------------------------------------------------- #
    # ----------------------------------------------------------------- #
    #                   TRAVEL DATA TO ARDUINO BOARD                    #
    # ----------------------------------------------------------------- #
    # ----------------------------------------------------------------- #
    # Retrieving selected travel informations
    # and send it to the Arduino board
    # Loop on travels and send each to the Arduino board
    for travel in travels[:10]:
        now = datetime.now() # Update current datetime
        t_date = travel["date"]
        t_numb = travel["numb"]
        t_term = travel["term"]
        t_stat = travel["stat"]
        t_rema = datetime.strptime(t_date + ':00', FMT)
        t_rema = str(int((t_rema-now).total_seconds()/60))
        sConc = t_rema + "_" + t_date + "_" + t_numb + "_" + t_term + "_" + t_stat
        print (sConc)
        ARD.write(sConc)
        time.sleep(5)