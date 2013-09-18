yaws
====

Yet another Weather Station

Intro
-----
This project is DIY Yet another Weather Station. Based on Raspberry PI and Arduino wireless nodes with RFM12b chip. I try to make codebase modular so wireless nodes could be disabled, and you can add your own sensors to station. But my setup is:

 1. Raspberry PI with Raspbian Wheezy + compiled [linux kernel module for RFM12b chip](http://example.net/)(github.com/gkaindl/rfm12b-linux). Also I attach BMP085 to Raspberry PI via I2C.

 2. Two [moteino](http://lowpowerlab.com/moteino/) (Arduino with RFM12B chip) node, also you can use jeenode or comaptible nodes. One moteino I use for outdoor sensor node, another for indoor. To each node I connected DHT22 sensor for temperature and humidity measure.

 3. Also I have VPS with debian on [DigitacOcean], this server I using for logging and serving meteo content to web clients, such as desktop, iPhone.


Structure
---------

 * weather.py -- running on Raspberry PI. It recieve data from remote nodes and local sensors and push it to remote server via HTTP.
 * app.py -- running on VPS or on another PC, or on localhost (nevermind), it recieive meteo data from weather station and store it in MongoDB, and serve it to clients.
 * arduino -- code for indoor and outdoor nodes.